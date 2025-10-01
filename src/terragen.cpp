#include <cmath>
#include <algorithm>
#include <cstdint>
#include <vector>

#include <terragen/terragen.hpp>

namespace terragen {

// ---------------- Fallback value-noise + fBm (no deps) ----------------

static inline double lerp(double a,double b,double t){ return a+(b-a)*t; }
static inline double smooth(double t){ return t*t*(3.0-2.0*t); }

static inline uint64_t mix64(uint64_t x){
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
static inline uint64_t hash_xy(uint32_t seed, int64_t x, int64_t y){
    constexpr uint64_t P1=0x9E3779B185EBCA87ULL, P2=0xC2B2AE3D27D4EB4FULL;
    return mix64(uint64_t(seed) ^ uint64_t(x)*P1 ^ uint64_t(y)*P2);
}
static inline double r01(uint64_t h){ return double(h>>11) * (1.0 / double(1ULL<<53)); }

static double value_noise(uint32_t seed,double x,double y){
    const int64_t xi=(int64_t)std::floor(x), yi=(int64_t)std::floor(y);
    const double tx=x-double(xi), ty=y-double(yi);
    const double sx=smooth(tx), sy=smooth(ty);
    const double v00=r01(hash_xy(seed,xi,yi));
    const double v10=r01(hash_xy(seed,xi+1,yi));
    const double v01=r01(hash_xy(seed,xi,yi+1));
    const double v11=r01(hash_xy(seed,xi+1,yi+1));
    return lerp(lerp(v00,v10,sx), lerp(v01,v11,sx), sy);
}

static double fbm_fallback(uint32_t seed,double x,double y,int oct,double lac,double gain){
    double a=0.5,f=1.0,sum=0.0,norm=0.0;
    for(int i=0;i<oct;++i){
        sum  += a*value_noise(seed + uint32_t(i)*1013904223u, x*f, y*f);
        norm += a;
        a *= gain;
        f *= lac;
    }
    double v=(norm>0.0)?(sum/norm):0.0;
    return std::clamp(v,0.0,1.0);
}

// ---------------- Adapters to ORIGINAL repo (optional) ----------------

#if !TERRAGENLITE_USE_FALLBACK
// Expect these headers to be available when user copies original sources.
#include <perlin.h>
#include <fbm.h>
// optional
// #include <diamond_square.h>
// #include <erosion.h>
#endif

static inline double sample_fbm_original(uint32_t seed, double x, double y, const FBMParams& p){
#if TERRAGENLITE_USE_FALLBACK
    (void)seed; (void)x; (void)y; (void)p;
    return 0.0; // won't be used when fallback path is selected
#else
    // The original fbm may provide a function operating on 2D coords or on buffers.
    // We implement a minimal point sampler using the original Perlin+fBm utilities.
    // Pseudocode (adjust to your signatures):
    //    Perlin per(seed);
    //    double amp=1, freq=1, sum=0, norm=0;
    //    for (int i=0; i<p.octaves; ++i) { sum += amp*per.noise2D(x*freq,y*freq); norm += amp; amp*=p.gain; freq*=p.lacunarity; }
    //    sum = (norm>0)? sum/norm : 0;
    //    return std::clamp(0.5*(sum+1.0), 0.0, 1.0);
    //
    // If your original exposes only buffered calls, you can replicate them for a single sample;
    // or write a tiny helper that wraps your original Perlin class here.
    extern double __terragenlite_original_fbm_point(uint32_t seed,double x,double y,
                                                    int oct,double lac,double gain);
    return std::clamp(__terragenlite_original_fbm_point(seed,x,y,p.octaves,p.lacunarity,p.gain), 0.0, 1.0);
#endif
}

// ---------------------------- API ------------------------------------

static Heightmap generate_chunk_impl(int cx,int cy,int size,uint32_t seed,const Config& cfg){
    Heightmap hm(size, std::vector<float>(size, 0.0f));

    const int    oct = cfg.fbm.octaves;
    const double lac = cfg.fbm.lacunarity;
    const double gan = cfg.fbm.gain;
    const double scl = cfg.fbm.scale;

    for(int y=0;y<size;++y){
        const int64_t wy = int64_t(cy)*size + y;
        for(int x=0;x<size;++x){
            const int64_t wx = int64_t(cx)*size + x;

            double sx, sy;
            if (cfg.sampling == Sampling::Global) {
                sx = double(wx) / scl;
                sy = double(wy) / scl;
            } else { // Sampling::Local – useful to reproduce old local-normalized outputs
                sx = double(x) / std::max(1.0, scl);
                sy = double(y) / std::max(1.0, scl);
            }

            double h=0.0;
            switch (cfg.backend) {
            case Backend::FallbackFBM:
                h = fbm_fallback(seed, sx, sy, oct, lac, gan);
                break;
            case Backend::OriginalFBM:
                h = sample_fbm_original(seed, sx, sy, cfg.fbm);
                break;
            case Backend::DiamondSquare:
                // If you wish to route here, replace with diamond-square single-point equivalent
                // or generate a full buffer once and read values.
                h = fbm_fallback(seed, sx, sy, oct, lac, gan); // placeholder
                break;
            }
            hm[y][x] = float(h);
        }
    }

    // Optional: apply erosion here if cfg.apply_erosion, invoking original functions
    return hm;
}

Heightmap generate_chunk(int chunk_x,int chunk_y,int size,uint32_t seed){
    return generate_chunk(chunk_x, chunk_y, size, seed, preset_oldrepo_like());
}

Heightmap generate_chunk(int chunk_x,int chunk_y,int size,uint32_t seed,const Config& cfg){
    return generate_chunk_impl(chunk_x, chunk_y, size, seed, cfg);
}

} // namespace terragen
