#pragma once
#include "types.hpp"
#include <cstdint>

// Flip this to route to original sources placed under bridges/original
#ifndef TERRAGENLITE_USE_FALLBACK
#define TERRAGENLITE_USE_FALLBACK 0
#endif

namespace terragen {

struct FBMParams {
    int    octaves     = 5;
    double lacunarity  = 2.0;
    double gain        = 0.5;
    double scale       = 64.0; // larger => smoother/continental features
};

struct Config {
    Backend  backend   = Backend::FallbackFBM;
    Sampling sampling  = Sampling::Global; // Global = seamless chunks
    FBMParams fbm;      // used for FBM flavors
    bool     apply_erosion = false;
    int      erosion_iters = 0;
    double   talus_angle   = 0.0;
};

// A convenient preset that tries to mimic the old repo defaults
inline Config preset_oldrepo_like() {
    Config c;
#if TERRAGENLITE_USE_FALLBACK
    c.backend = Backend::FallbackFBM;
#else
    c.backend = Backend::OriginalFBM;
#endif
    c.sampling = Sampling::Global;
    c.fbm.octaves    = 6;
    c.fbm.lacunarity = 2.0;
    c.fbm.gain       = 0.5;
    c.fbm.scale      = 64.0;
    return c;
}

} // namespace terragen
