#include "Engine.h"
#include <cassert>
#include <algorithm>
#include <cmath>
static float energy(float *p,int n){float e=0;for(int i=0;i<n;i++)e+=std::abs(p[i]);return e;}
int main(){float l[6000],r[6000];static float sample[96000];std::fill_n(sample,96000,.5f);tm_init(48000);tm_bpm_tenths(1200);assert(tm_instrument_sample(47,sample,96000,48000,60));for(int p=0;p<2;p++){tm_cell_at3(p,0,0,61,47,100,100,-50,0,0,0,0,0);tm_cell_at3(p,1,0,61,47,100,100,50,0,0,0,0,0);}tm_pattern_track_volume(0,0,0);tm_pattern_track_volume(1,0,100);tm_pattern_mute_mask(0,0);tm_pattern_mute_mask(1,1<<1);tm_song_set(0,0,1);tm_song_set(1,1,1);tm_song_count(2);tm_song_play(1);tm_play(1);tm_render(l,r,6000);assert(tm_playing_pattern()==0&&energy(l+512,5488)==0&&energy(r,6000)>10);tm_render(l,r,6000);assert(tm_playing_pattern()==1&&energy(l,6000)>10&&energy(r+512,5488)==0);tm_render(l,r,6000);assert(tm_playing_pattern()==0&&energy(l+512,5488)==0&&energy(r,6000)>10);tm_play(0);tm_render(l,r,128);assert(tm_playing_pattern()==-1);}
