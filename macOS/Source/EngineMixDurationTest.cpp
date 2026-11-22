#include "Engine.h"
#include "MixDSP.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
int main(){
    FourBandEQ eq;eq.init(48000);eq.configure(0,12,0,0);double e=0,ref=0;for(int i=0;i<48000;i++){float x=.1f*std::sin(6.28318530718*500*i/48000),l=x,r=x;eq.process(l,r);assert(std::isfinite(l));if(i>24000){e+=l*l;ref+=x*x;}}assert(e/ref>12&&e/ref<18);
    tm_init(48000);tm_bpm(120);tm_select_pattern(0,16);tm_cell_at4(0,0,0,61,3,100,100,0,0,0,0,0,0,8);tm_play(1);std::vector<float> l(18000),r(18000);tm_render(l.data(),r.data(),18000);auto energy=[&](int a,int b){double e=0;for(int i=a;i<b;i++)e+=std::abs(l[i]);return e;};assert(energy(9000,11000)>5);assert(energy(12500,13000)<.001);tm_play(0);tm_render(l.data(),r.data(),512);
    static float pcm[48000];std::fill_n(pcm,48000,.4f);assert(tm_instrument_sample(45,pcm,48000,48000,60));tm_select_pattern(1,16);tm_cell_at4(1,0,0,61,45,100,100,0,0,0,0,0,0,8);tm_track_sends(1,0,0,0,100);tm_play(1);tm_render(l.data(),r.data(),6000);assert(energy(200,500)>1&&energy(1000,1300)<.01);tm_play(0);tm_render(l.data(),r.data(),1024);
    static float impulse[64];std::fill_n(impulse,64,.7f);assert(tm_instrument_sample(46,impulse,64,48000,60));tm_select_pattern(2,16);tm_cell_at3(2,0,0,61,46,100,100,0,0,0,0,0,0);tm_track_sends(2,0,100,0,0);tm_play(1);tm_render(l.data(),r.data(),6000);assert(energy(1000,6000)>.03);tm_play(0);for(int i=0;i<40;i++)tm_render(l.data(),r.data(),6000);
    tm_select_pattern(3,16);tm_cell_at3(3,0,0,61,46,100,100,0,0,0,0,0,0);tm_master_sends(0,100,0);tm_play(1);for(int i=0;i<4;i++)tm_render(l.data(),r.data(),6000);assert(energy(0,100)>.02);std::puts("Four-band EQ response, held note release, channel slicer/reverb, master delay passed");
}
