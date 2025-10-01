#include "perlin.h"
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>

Perlin::Perlin(uint64_t seed) {
  p.resize(256);
  std::iota(p.begin(), p.end(), 0);
  std::mt19937_64 rng(seed);
  std::shuffle(p.begin(), p.end(), rng);
  p.insert(p.end(), p.begin(), p.end());
}

double Perlin::fade(double t){ return t*t*t*(t*(t*6-15)+10); }
double Perlin::lerp(double a,double b,double t){ return a + t * (b - a); }
double Perlin::grad(int h,double x,double y,double z){
  int g = h & 15;
  double u = g<8?x:y, v = g<4?y:(g==12||g==14?x:z);
  return ((g&1)?-u:u) + ((g&2)?-v:v);
}

double Perlin::noise2D(double x,double y) const {
  int X = (int)floor(x) & 255;
  int Y = (int)floor(y) & 255;
  x -= floor(x); y -= floor(y);
  double u = fade(x), v = fade(y);
  int A = p[X] + Y, B = p[X+1] + Y;
  double n00 = grad(p[A], x, y);
  double n10 = grad(p[B], x-1, y);
  double n01 = grad(p[A+1], x, y-1);
  double n11 = grad(p[B+1], x-1, y-1);
  return lerp(lerp(n00, n10, u), lerp(n01, n11, u), v); // ~[-1,1]
}

double Perlin::noise1D(double x) const {
  return noise2D(x, 0.0);
}
