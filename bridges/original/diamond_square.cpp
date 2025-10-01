#include "diamond_square.h"
#include <random>
#include <cmath>

static inline int idx(int x,int y,int size){ return y*size + x; }

std::vector<float> diamond_square(int n_power, uint64_t seed, float rough){
  int size = (1<<n_power) + 1;
  std::vector<float> h(size*size, 0.f);
  std::mt19937_64 rng(seed);
  std::uniform_real_distribution<float> dist(-1.f, 1.f);

  // corners
  h[idx(0,0,size)] = dist(rng);
  h[idx(size-1,0,size)] = dist(rng);
  h[idx(0,size-1,size)] = dist(rng);
  h[idx(size-1,size-1,size)] = dist(rng);

  int step = size-1;
  float scale = rough;
  while(step > 1){
    int half = step/2;

    // diamond
    for(int y=half; y<size; y+=step)
      for(int x=half; x<size; x+=step){
        float a = h[idx(x-half, y-half, size)];
        float b = h[idx(x+half, y-half, size)];
        float c = h[idx(x-half, y+half, size)];
        float d = h[idx(x+half, y+half, size)];
        h[idx(x,y,size)] = (a+b+c+d)/4.f + dist(rng)*scale;
      }

    // square
    for(int y=0; y<size; y+=half){
      int shift = ((y/half) % 2) ? 0 : half;
      for(int x=shift; x<size; x+=step){
        float sum=0; int cnt=0;
        if(x-half>=0)        { sum+=h[idx(x-half,y,size)]; cnt++; }
        if(x+half<size)      { sum+=h[idx(x+half,y,size)]; cnt++; }
        if(y-half>=0)        { sum+=h[idx(x,y-half,size)]; cnt++; }
        if(y+half<size)      { sum+=h[idx(x,y+half,size)]; cnt++; }
        h[idx(x,y,size)] = sum/cnt + dist(rng)*scale;
      }
    }

    step = half;
    scale *= 0.5f;
  }

  // normalize to [0,1]
  float mn = h[0], mx=h[0];
  for(float v: h){ if(v<mn) mn=v; if(v>mx) mx=v; }
  float inv = (mx>mn)? 1.f/(mx-mn) : 1.f;
  for(float &v: h) v = (v - mn) * inv;
  return h;
}
