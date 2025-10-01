#include "erosion.h"
#include <algorithm>
#include <array>

void thermal_erosion(std::vector<float>& h,int W,int H,int iters,float talus){
  auto idx=[&](int x,int y){return y*W+x;};
  std::array<int,8> dx{ -1,0,1,-1,1,-1,0,1 };
  std::array<int,8> dy{ -1,-1,-1,0,0,1,1,1 };
  std::vector<float> delta(W*H,0.f);

  for(int it=0; it<iters; ++it){
    std::fill(delta.begin(), delta.end(), 0.f);
    for(int y=1;y<H-1;y++){
      for(int x=1;x<W-1;x++){
        float z = h[idx(x,y)];
        float total=0.f;
        for(int k=0;k<8;k++){
          int nx=x+dx[k], ny=y+dy[k];
          float d = z - h[idx(nx,ny)];
          if(d>talus){ float m = 0.5f*(d - talus); delta[idx(x,y)] -= m; delta[idx(nx,ny)] += m; total += m; }
        }
      }
    }
    for(int i=0;i<W*H;i++) h[i]+=delta[i];
  }
}