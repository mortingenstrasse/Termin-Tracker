#include "Engine.h"
#include <cassert>
#include <algorithm>
#include <cmath>
static float energy(float *a,int n){float e=0;for(int i=0;i<n;i++){assert(std::isfinite(a[i]));e+=std::abs(a[i]);}return e;}
int main(){float l[6000],r[6000];static float sample[96000];std::fill_n(sample,96000,.5f);tm_init(48000);tm_bpm_tenths(1200);assert(tm_instrument_sample(47,sample,96000,48000,60));tm_select_pattern(0,16);tm_cell_at3(0,0,0,61,47,100,100,-50,0,0,0,0,0);tm_cell_at3(0,23,0,61,47,100,100,50,0,0,0,0,0);tm_play(1);tm_render(l,r,2048);assert(energy(l,2048)>10&&energy(r,2048)>10);assert(tm_meter_level(0)>.1f&&tm_meter_level(23)>.1f&&tm_meter_level(24)>.1f&&tm_meter_level(25)>.1f&&tm_meter_level(26)>.1f&&tm_meter_level(27)>.1f);
tm_track_volume(0,0);tm_render(l,r,1024);assert(energy(l+512,512)==0&&energy(r,1024)>10);tm_track_volume(0,100);tm_pattern_volume(0,1,0);tm_render(l,r,2048);assert(energy(l,2048)>10&&energy(r+512,1536)==0);tm_master(0);tm_render(l,r,1024);assert(energy(l+512,512)==0&&energy(r+512,512)==0&&tm_meter_level(24)>.05f);tm_master(100);tm_pattern_volume(0,1,100);tm_mute(23,1);tm_render(l,r,1024);assert(energy(l,1024)>1&&energy(r+512,512)==0);tm_mute(23,0);tm_play(0);for(int i=0;i<20;i++)tm_render(l,r,6000);for(int i=0;i<28;i++)assert(tm_meter_level(i)<.001f);
tm_select_pattern(1,16);tm_cell_at3(1,23,0,61,47,100,100,0,0,0,0,0,0);tm_pattern_volume(1,0,0);tm_play(1);tm_render(l,r,2048);assert(energy(l,2048)==0&&energy(r,2048)>1);tm_play(0);tm_render(l,r,128);}
