#include "Engine.h"
#include <cassert>
#include <cmath>
#include <algorithm>
static float energy(float *p,int n){float e=0;for(int i=0;i<n;i++){assert(std::isfinite(p[i]));e+=std::abs(p[i]);}return e;}
int main(){float l[8192],r[8192],sample[4096];std::fill_n(sample,4096,.5f);tm_init(48000);tm_bpm_tenths(1200);assert(tm_instrument_sample(47,sample,4096,48000,60)==1);tm_select_pattern(0,8);tm_cell_at3(0,23,0,61,47,100,100,0,0,0,0,0,25);tm_play(1);tm_render(l,r,1500);assert(energy(l,1500)==0);tm_render(l,r,256);assert(energy(l,256)>1);tm_mute(23,1);tm_render(l,r,256);assert(energy(l+248,8)==0);tm_mute(23,0);tm_play(0);tm_render(l,r,128);tm_select_pattern(1,8);tm_cell_at3(1,12,0,61,47,100,100,0,0,0,0,0,50);tm_play(1);tm_render(l,r,3000);assert(energy(l,3000)==0);tm_render(l,r,256);assert(energy(l,256)>1);tm_play(0);tm_render(l,r,128);tm_instrument_clear(47);tm_preview(61,47);tm_render(l,r,256);assert(energy(l,256)>0);}
