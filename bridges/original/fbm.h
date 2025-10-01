#pragma once
#include "perlin.h"
#include <vector>

std::vector<float> fbm2d(int width, int height, uint64_t seed,
                         int octaves, double lacunarity, double gain, double scale);
