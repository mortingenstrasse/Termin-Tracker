#include "Engine.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cassert>
#include <cstdio>
#include <vector>

int main(){
    tm_init(48000);tm_bpm(120);static float pcm[48000];std::fill_n(pcm,48000,.8f);assert(tm_instrument_sample(45,pcm,48000,48000,60));tm_sample_config(45,0,0,48000,nullptr,0,500,120,3,0,0);
    tm_select_pattern(4,8);tm_cell_at3(4,0,0,61,45,100,100,0,0,0,0,0,0);tm_cell_at3(4,0,1,61,45,100,100,0,0,0,0,0,0);tm_cell_at3(4,0,2,UINT64_MAX,45,100,100,0,0,0,0,0,0);tm_play(1);
    std::vector<float> l(18000),r(18000);tm_render(l.data(),r.data(),l.size());float jump=0;for(size_t i=1;i<l.size();i++)jump=std::max(jump,std::abs(l[i]-l[i-1]));assert(jump<.025f);assert(std::abs(l[15000])<1e-6f);tm_play(0);tm_render(l.data(),r.data(),512);
    // A constant-valued short crop begins and ends at zero instead of a discontinuity.
    tm_sample_config(45,0,100,300,nullptr,0,500,120,3,0,0);tm_preview(61,45);tm_render(l.data(),r.data(),1024);float cropJump=0;for(int i=1;i<1024;i++)cropJump=std::max(cropJump,std::abs(l[i]-l[i-1]));assert(cropJump<.02f&&l[200]==0);tm_preview_stop();tm_render(l.data(),r.data(),512);
    static float high[48000],low[48000];for(int i=0;i<48000;i++){high[i]=.7f*std::sin(6.283185307*18000*i/48000);low[i]=.7f*std::sin(6.283185307*1000*i/48000);}assert(tm_instrument_sample(43,high,48000,48000,60));assert(tm_instrument_sample(44,low,48000,48000,60));auto rms=[&](int slot){tm_preview(73,slot);tm_render(l.data(),r.data(),4096);double e=0;for(int i=512;i<4096;i++)e+=l[i]*l[i];tm_preview_stop();tm_render(l.data(),r.data(),512);return std::sqrt(e/3584);};double hf=rms(43),lf=rms(44);assert(hf<lf*.35);std::printf("Octave-up 18 kHz alias suppression: %.1f dB relative to 1 kHz passband\n",20*std::log10(hf/lf));
    // 192 simultaneous granular voices with sends, pitch offsets and rapid retriggers.
    static float music[48000];for(int i=0;i<48000;i++)music[i]=.7f*std::sin(6.283185307*220*i/48000)+.2f*std::sin(6.283185307*440*i/48000);assert(tm_instrument_sample(46,music,48000,48000,60));tm_sample_config(46,5,0,48000,nullptr,0,500,80,3,0,20);uint64_t packed=0;for(int n=0;n<8;n++)packed|=uint64_t(61+n*2)<<(n*8);tm_select_pattern(5,16);for(int t=0;t<24;t++){tm_track_eq(5,t,-3,2,-2,3);for(int s=0;s<16;s++){if(s%4==0)tm_cell_at3(5,t,s,packed,46,100,100,(t%2?20:-20),4,35,5,20,0);tm_step_eq(5,t,s,-2,s%2?3:-3,2,-1);}}tm_play(1);std::array<float,256> a{},b{};for(int i=0;i<12;i++)tm_render(a.data(),b.data(),256);tm_dsp_reset();double sum=0;float peak=0;for(int i=0;i<200;i++){tm_render(a.data(),b.data(),256);sum+=tm_dsp_load();for(int j=0;j<256;j++){assert(std::isfinite(a[j])&&std::isfinite(b[j]));peak=std::max(peak,std::max(std::abs(a[j]),std::abs(b[j])));}}assert(peak<=.981f&&peak>.1f);std::printf("Anti-click max jump %.6f, crop %.6f; 192-voice granular peak %.4f; CPU mean %.1f%%, peak %.1f%%, budget misses %d/200\n",jump,cropJump,peak,sum/2000.,tm_dsp_peak()/10.,tm_dsp_overruns());
}
