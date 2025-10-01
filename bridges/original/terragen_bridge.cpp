// bridges/original/terragen_bridge.cpp
#include <algorithm>
#include <cstdint>
#include <perlin.h>   // din repo-ul vechi

namespace terragen {

// Punct unic de eșantionare fBm bazat pe Perlin-ul din repo-ul vechi
double __terragenlite_original_fbm_point(uint32_t seed,
                                         double x, double y,
                                         int oct, double lac, double gain)
{
    Perlin per(seed);                // clasa din perlin.h (repo vechi)
    double amp = 1.0, freq = 1.0;
    double sum = 0.0, norm = 0.0;

    for (int i = 0; i < oct; ++i) {
        sum  += amp * per.noise2D(x * freq, y * freq);  // <- numele e de obicei noise2D
        norm += amp;
        amp  *= gain;
        freq *= lac;
    }

    double v = (norm > 0.0) ? (sum / norm) : 0.0; // ~[-1, 1]
    v = 0.5 * (v + 1.0);                          // -> [0, 1]
    if (v < 0.0) v = 0.0;
    if (v > 1.0) v = 1.0;
    return v;
}

} // namespace terragen
