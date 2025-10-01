#pragma once
#include <vector>
#include <cstdint>

struct Perlin {
  explicit Perlin(uint64_t seed);
  double noise1D(double x) const;
  double noise2D(double x, double y) const;
private:
  std::vector<int> p; // permutation table
  static double fade(double t);
  static double lerp(double a, double b, double t);
  static double grad(int hash, double x, double y=0.0, double z=0.0);
};
