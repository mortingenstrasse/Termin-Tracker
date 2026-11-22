#include "Engine.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

int main(){
    tm_init(48000);tm_bpm(120);tm_select_pattern(0,16);
    std::vector<float> key(12000,.8f),pad(60000,.2f);assert(tm_instrument_sample(45,key.data(),int(key.size()),48000,60));assert(tm_instrument_sample(46,pad.data(),int(pad.size()),48000,60));
    tm_cell_at4(0,0,1,61,45,100,100,-50,0,0,0,0,0,8);tm_cell_at4(0,1,0,61,46,100,100,50,0,0,0,0,0,40);
    tm_dynamics(0,1,1,-30,4,2,60,0,-35,6);tm_play(1);std::vector<float> l(30000),r(30000);tm_render(l.data(),r.data(),30000);
    auto mean=[](const std::vector<float>& a,int from,int to){double out=0;for(int i=from;i<to;i++){assert(std::isfinite(a[i]));out+=std::abs(a[i]);}return out/(to-from);};double before=mean(r,3000,5500),duck=mean(r,9000,11000),restored=mean(r,27000,29000);std::printf("Sidechain before %.4f, duck %.4f, recovered %.4f, key %.4f\n",before,duck,restored,mean(l,9000,11000));assert(duck/before>.47&&duck/before<.56);assert(restored/before>.85);assert(mean(l,9000,11000)>0.035);tm_play(0);tm_render(l.data(),r.data(),512);
    tm_select_pattern(1,8);std::vector<float> tone(48000);for(int i=0;i<48000;i++)tone[i]=.2f*std::sin(2*3.141592653589793*440*i/48000);assert(tm_instrument_sample(44,tone.data(),48000,48000,60));tm_sample_octave(44,1);tm_dynamics(0,0,0,-24,3,5,150,0,-30,6);tm_cell_at4(1,0,0,61,44,100,100,0,0,0,0,0,0,16);tm_play(1);tm_render(l.data(),r.data(),12000);int crossings=0;for(int i=3001;i<11000;i++)if(l[i-1]<0&&l[i]>=0)crossings++;double hz=crossings*48000./8000;std::printf("Sample octave +1: %.1f Hz\n",hz);assert(std::abs(hz-880)<12);std::puts("Sidechain duck, recovery, source isolation and sample octave passed");
}
