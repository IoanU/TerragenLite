#pragma once
#include "types.hpp"
#include "config.hpp"

namespace terragen {

/**
 * Minimal API (seamless-chunk generator).
 * Deterministic for the same (seed, chunk coords, size, cfg).
 * Output values in [0, 1].
 */
Heightmap generate_chunk(int chunk_x, int chunk_y, int size, uint32_t seed);
Heightmap generate_chunk(int chunk_x, int chunk_y, int size, uint32_t seed, const Config& cfg);

} // namespace terragen
