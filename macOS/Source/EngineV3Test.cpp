#include "Engine.h"
#include <cassert>
#include <cmath>
static float energy(float *x,int n){float e=0;for(int i=0;i<n;i++){assert(std::isfinite(x[i]));e+=std::abs(x[i]);}return e;}
int main(){float l[8192],r[8192];tm_init(48000);tm_bpm(120);tm_select_pattern(0,32);tm_cell_at2(0,3,0,61ull|(65ull<<8)|(68ull<<16)|(72ull<<24),3,90,80,-50,3,25,1,10);tm_play(1);tm_render(l,r,1024);assert(energy(l,600)>1);assert(energy(l+800,224)==0);assert(energy(r,1024)==0);tm_play(0);tm_render(l,r,512);assert(tm_step()==-1);
tm_preview(61,3);tm_render(l,r,512);assert(energy(l,512)>1&&energy(r,512)>1);for(int i=0;i<5;i++)tm_render(l,r,8192);assert(energy(l,8192)==0);
tm_select_pattern(1,32);tm_cell_at(1,3,7,61,3,100,100,0,0,0);tm_start_step(7);tm_play(1);tm_render(l,r,512);assert(tm_step()==7&&energy(l,512)>1);tm_mute(3,1);tm_render(l,r,512);assert(energy(l+256,256)==0);tm_mute(3,0);tm_master(0);tm_render(l,r,512);assert(energy(l+256,256)==0);tm_master(100);tm_play(0);tm_render(l,r,512);
tm_song_set(0,2,1);tm_song_set(1,3,2);tm_song_count(2);tm_cell_at(2,3,0,61,3,100,100,0,0,0);tm_cell_at(3,3,0,73,3,100,100,0,0,0);tm_song_play(1);tm_play(1);tm_render(l,r,6000);assert(tm_song_row()==0&&tm_step()==0);tm_render(l,r,6000);assert(tm_song_row()==1&&tm_step()==0);tm_render(l,r,6000);assert(tm_song_row()==1&&tm_step()==1);tm_render(l,r,6000);assert(tm_song_row()==0&&tm_step()==0);tm_play(0);tm_render(l,r,512);assert(tm_song_row()==-1);}
