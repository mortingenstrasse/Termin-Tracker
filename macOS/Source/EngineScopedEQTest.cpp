#include "Engine.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

int main(){
    tm_init(48000);tm_bpm(120);tm_select_pattern(0,8);
    std::vector<float> pcm(48000);for(int i=0;i<48000;i++)pcm[i]=.1f*std::sin(6.283185307179586*500*i/48000);
    assert(tm_instrument_sample(45,pcm.data(),48000,48000,60));
    tm_cell_at4(0,0,0,61,45,100,100,-50,0,0,0,0,0,16);
    tm_cell_at4(0,1,0,61,45,100,100,50,0,0,0,0,0,16);
    tm_track_eq(0,0,0,12,0,0);tm_step_eq(0,0,1,0,-12,0,0);
    std::vector<float> l(18000),r(18000);tm_play(1);tm_render(l.data(),r.data(),18000);
    auto ratio=[&](int a,int b){double le=0,re=0;for(int i=a;i<b;i++){assert(std::isfinite(l[i])&&std::isfinite(r[i]));le+=l[i]*l[i];re+=r[i]*r[i];}assert(re>1e-7);return le/re;};
    double before=ratio(3500,5800),during=ratio(9500,11800),after=ratio(15500,17800);
    std::printf("Track EQ ratio %.3f, step EQ ratio %.3f, next-step ratio %.3f\n",before,during,after);
    assert(before>12&&before<19);assert(during>.65&&during<1.45);assert(after>12&&after<19);
    tm_play(0);tm_render(l.data(),r.data(),512);
    tm_select_pattern(1,8);tm_cell_at4(1,0,0,61,45,100,100,-50,0,0,0,0,0,16);tm_cell_at4(1,1,0,61,45,100,100,50,0,0,0,0,0,16);tm_play(1);tm_render(l.data(),r.data(),6000);assert(ratio(3500,5800)>.9&&ratio(3500,5800)<1.1);
    std::puts("Track isolation, step-only EQ on held note, next-row reset and pattern isolation passed");
}
