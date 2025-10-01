#include "fbm.h"
#include <cmath>

std::vector<float> fbm2d(int W,int H,uint64_t seed,int oct,double lac,double gain,double scale){
  Perlin per(seed);
  std::vector<float> out(W*H, 0.f);
  double amp=1.0, freq=1.0, norm=0.0;
  for(int o=0;o<oct;o++){
    for(int y=0;y<H;y++){
      for(int x=0;x<W;x++){
        double nx = (x/(double)W) * scale * freq;
        double ny = (y/(double)H) * scale * freq;
        out[y*W+x] += (float)(amp * per.noise2D(nx, ny));
      }
    }
    norm += amp;
    amp *= gain;
    freq *= lac;
  }
  // map roughly from [-norm, norm] to [0,1]
  for(auto &v: out) v = 0.5f * (v / (float)norm + 1.0f);
  return out;
}
