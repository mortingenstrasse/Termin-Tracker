#include "Engine.h"
#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
int main(){tm_capture_begin();float a[]={.2f,.4f,.6f,.8f},b[]={.4f,.6f,.8f,1.f},out[4]{};assert(tm_capture_push(a,b,4)==4);assert(tm_capture_copy(out,4)==4);assert(std::abs(out[0]-.3f)<.0001f&&std::abs(out[3]-.9f)<.0001f);float low[2],high[2];tm_capture_peaks(low,high,2);assert(high[0]>.49f&&high[1]>.89f);tm_capture_begin();assert(tm_capture_count()==0);
tm_init(48000);tm_bpm(120);static float pcm[48000];for(int i=0;i<48000;i++)pcm[i]=.3f;assert(tm_instrument_sample(10,pcm,48000,48000,60));int anchors[5]={90,720,310,850,190};tm_sample_config_scan(10,5,0,48000,nullptr,0,500,20,3,0,30,1,2,768,100,1,anchors);tm_preview(61,10);std::array<float,6000> l{},r{};for(int i=0;i<4;i++){tm_render(l.data(),r.data(),6000);int pos=tm_sample_position(10);bool found=false;for(int n:anchors)if(pos==n)found=true;assert(found);}tm_preview_stop();tm_render(l.data(),r.data(),512);tm_sample_config_scan(10,5,0,48000,nullptr,0,500,20,3,0,0,1,2,768,100,2,anchors);tm_preview(61,10);tm_render(l.data(),r.data(),3000);assert(std::abs(tm_sample_position(10)-405)<=1);std::puts("Capture stereo downmix, waveform peaks, marker step/morph scan passed");}
