#pragma once
#include <vector>
#include <cstdint>

std::vector<float> diamond_square(int n_power, uint64_t seed, float roughness);
// returns (size x size) grid flattened row-major; size = (1<<n_power) + 1
