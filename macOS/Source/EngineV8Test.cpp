#include "Engine.h"
#include <cassert>
#include <algorithm>
#include <cmath>
static float energy(float *p,int n){float e=0;for(int i=0;i<n;i++){assert(std::isfinite(p[i]));e+=std::abs(p[i]);}return e;}
int main(){float l[6000],r[6000];static float sample[48000];std::fill_n(sample,48000,.2f);std::fill(sample+24000,sample+48000,.8f);tm_init(48000);tm_bpm_tenths(1200);assert(tm_instrument_sample(47,sample,48000,48000,60));tm_sample_config(47,0,100,200,nullptr,0,500,120,2,0,0);tm_preview(61,47);tm_render(l,r,512);assert(energy(l,100)>1&&energy(l+100,412)==0);tm_preview_stop();tm_render(l,r,128);
int chops[2]={0,24000};tm_sample_config(47,4,0,48000,chops,2,500,120,2,0,0);tm_preview(25,47);tm_render(l,r,512);float e1=energy(l,512);tm_preview(26,47);tm_render(l,r,512);assert(energy(l,512)>e1*3);tm_preview_stop();tm_render(l,r,128);
tm_select_pattern(0,8);tm_cell_at3(0,0,0,25,47,100,100,0,7,2,0,0,0);tm_play(1);tm_render(l,r,512);assert(energy(l,512)>15);tm_play(0);tm_render(l,r,128);
tm_sample_config(47,5,0,48000,nullptr,0,100,10,3,0,0);tm_preview(61,47);tm_render(l,r,2048);float low=energy(l,2048);tm_sample_config(47,5,0,48000,nullptr,0,850,10,3,0,0);tm_preview(61,47);tm_render(l,r,2048);assert(energy(l,2048)>low*3);for(int i=0;i<12;i++)tm_render(l,r,6000);assert(energy(l,6000)>1);tm_preview_stop();tm_render(l,r,128);
static float impulse[64];std::fill_n(impulse,64,.7f);assert(tm_instrument_sample(46,impulse,64,48000,60));tm_select_pattern(1,16);tm_cell_at3(1,0,0,61,46,100,100,0,4,100,0,0,0);tm_play(1);tm_render(l,r,6000);assert(energy(l,64)>1&&energy(l+1000,4500)>0.1f);tm_play(0);for(int i=0;i<40;i++)tm_render(l,r,6000);
tm_select_pattern(2,16);tm_cell_at3(2,0,0,61,46,100,100,0,5,100,0,0,0);tm_play(1);tm_render(l,r,6000);tm_render(l,r,6000);tm_render(l,r,6000);tm_render(l,r,6000);assert(energy(l,64)>.1f);tm_play(0);for(int i=0;i<40;i++)tm_render(l,r,6000);
tm_sample_config(47,0,0,48000,nullptr,0,500,120,2,0,0);tm_select_pattern(3,16);tm_cell_at3(3,0,0,61,47,100,100,0,6,100,0,0,0);tm_play(1);tm_render(l,r,6000);assert(energy(l+100,100)>1&&energy(l+500,100)<.01f&&energy(l+850,100)>1);tm_play(0);tm_render(l,r,128);}
