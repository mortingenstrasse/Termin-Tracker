#include "Engine.h"
#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
int main(){tm_init(48000);tm_bpm(120);static float pcm[48000];for(int i=0;i<48000;i++)pcm[i]=i<24000?.15f:.85f;assert(tm_instrument_sample(10,pcm,48000,48000,60));std::array<float,6000> l{},r{};
    tm_sample_config_lfo(10,5,0,48000,nullptr,0,500,20,3,0,0,1,0,768,100);tm_preview(61,10);tm_render(l.data(),r.data(),6000);assert(std::abs(tm_sample_position(10)-500)<=1);double a=0;for(float x:l)a+=std::abs(x);tm_render(l.data(),r.data(),6000);assert(tm_sample_position(10)>=999);double b=0;for(float x:l)b+=std::abs(x);assert(b>a*2);tm_render(l.data(),r.data(),6000);assert(std::abs(tm_sample_position(10)-500)<=1);tm_render(l.data(),r.data(),6000);assert(tm_sample_position(10)<=1);
    tm_preview_stop();tm_render(l.data(),r.data(),6000);tm_sample_config_lfo(10,5,0,48000,nullptr,0,500,20,3,0,0,2,0,100,100);tm_preview(61,10);tm_render(l.data(),r.data(),6000);tm_render(l.data(),r.data(),6000);assert(std::abs(tm_sample_position(10)-500)<=1);tm_bpm(240);tm_render(l.data(),r.data(),6000);tm_render(l.data(),r.data(),6000);assert(tm_sample_position(10)>=999);
    tm_preview_stop();tm_render(l.data(),r.data(),6000);tm_bpm(120);tm_sample_config_lfo(10,5,0,48000,nullptr,0,500,20,3,0,0,1,0,24576,100);tm_select_pattern(0,16);tm_play(1);for(int i=0;i<64;i++)tm_render(l.data(),r.data(),6000);assert(tm_sample_position(10)>=999);for(int i=0;i<64;i++)tm_render(l.data(),r.data(),6000);assert(tm_sample_position(10)<=1);std::puts("Granular LFO: 4-step scan audible, free Hz independent of BPM, 128-step cycle crosses pattern loops passed");}
