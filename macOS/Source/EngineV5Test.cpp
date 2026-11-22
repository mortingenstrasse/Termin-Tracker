#include "Engine.h"
#include <cassert>
#include <algorithm>
#include <cmath>
static float energy(float *p,int n){float e=0;for(int i=0;i<n;i++){assert(std::isfinite(p[i]));e+=std::abs(p[i]);}return e;}
int main(){float l[6000],r[6000],sample[64];std::fill_n(sample,64,.5f);tm_init(48000);tm_bpm_tenths(1200);assert(tm_instrument_sample(47,sample,64,48000,60)==1);tm_select_pattern(0,16);tm_cell_at3(0,23,0,61,47,85,100,0,2,30,0,0,10);tm_play(1);tm_render(l,r,6000);int starts[4]={600,1950,3300,4650};for(int i=0;i<4;i++){assert(energy(l+starts[i],64)>1);assert(energy(l+starts[i]+64,30)==0);}assert(energy(l,600)==0);assert(energy(l+starts[1],64)!=energy(l+starts[2],64));tm_play(0);tm_render(l,r,128);tm_instrument_wave(8,8);tm_preview(51,8);tm_render(l,r,512);assert(energy(l,512)>1);}
