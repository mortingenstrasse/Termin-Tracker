#include "Engine.h"
#include "OmondeuDSP.h"
#include <cstdio>
#include <cassert>
#include <vector>
constexpr int rate=48000;
double tone(const std::vector<float>& v,double hz,int start,int end){double a=0,b=0;for(int i=start;i<end;i++){double p=2*omondeu::pi*hz*i/rate;a+=v[i]*std::sin(p);b+=v[i]*std::cos(p);}return 2*std::sqrt(a*a+b*b)/(end-start);}
int main(){
    tm_init(rate);tm_instrument_wave(9,9);
    auto p=omondeu::defaults;p[2]=0;p[3]=0;p[4]=100;p[5]=-1;p[14]=100;p[15]=100;p[18]=100;
    tm_omondeu(9,p.data());tm_midi_note(45,9,127);
    std::vector<float> l(rate),r(rate);tm_render(l.data(),r.data(),rate);
    double sub=tone(l,55,12000,48000);assert(sub>.025);double side=0;for(int i=0;i<rate;i++){assert(std::isfinite(l[i]));side+=std::abs(l[i]-r[i]);}assert(side<.0001);
    tm_midi_note(45,9,0);tm_render(l.data(),r.data(),rate);assert(std::abs(l.back())<.00001);
    p=omondeu::defaults;p[8]=4000;p[18]=100;p[7]=20;tm_omondeu(9,p.data());tm_midi_note(60,9,127);
    tm_render(l.data(),r.data(),rate);double mid=0,st=0;for(int i=12000;i<rate;i++){mid+=std::pow((l[i]+r[i])*.5,2);st+=std::pow((l[i]-r[i])*.5,2);}assert(st/mid>.001);
    p[18]=0;tm_omondeu(9,p.data());tm_render(l.data(),r.data(),rate);for(int i=rate/2;i<rate;i++)assert(std::abs(l[i]-r[i])<.000001);
    tm_midi_note(60,9,0);tm_render(l.data(),r.data(),rate);
    p=omondeu::defaults;p[0]=0;p[2]=100;p[3]=p[4]=0;p[9]=0;p[10]=p[11]=0;p[8]=12000;p[14]=100;p[16]=2;p[17]=40;tm_omondeu(9,p.data());
    tm_midi_note(45,9,127);tm_render(l.data(),r.data(),rate);
    tm_midi_note(57,9,127);tm_render(l.data(),r.data(),rate);double a=tone(l,110,24000,48000),b=tone(l,220,24000,48000);assert(b>a*10&&b>.02);
    tm_midi_note(57,9,0);tm_render(l.data(),r.data(),rate);assert(tone(l,110,24000,48000)>tone(l,220,24000,48000)*10);
    tm_midi_panic();tm_render(l.data(),r.data(),rate);assert(std::abs(l.back())<.00001);
    p[16]=0;tm_omondeu(9,p.data());tm_midi_note(45,9,127);tm_midi_note(52,9,127);tm_render(l.data(),r.data(),rate);assert(tone(l,110,24000,48000)>.02&&tone(l,164.8138,24000,48000)>.02);
    std::printf("Omondeu: 55 Hz sub amplitude %.4f, mono sub centered, stereo side/mid power %.3f, width zero mono, legato note priority and poly verified\n",sub,st/mid);
    tm_midi_panic();tm_render(l.data(),r.data(),rate);
    p=omondeu::defaults;tm_omondeu(9,p.data());
    tm_cell_at4(0,0,0,36+1,9,100,100,0,0,0,0,0,0,16);
    tm_select_pattern(0,64);tm_play(1);tm_render(l.data(),r.data(),12000);
    double energy=0,difference=0;for(int i=0;i<12000;i++){energy+=l[i]*l[i];difference+=std::abs(l[i]-r[i]);}
    assert(energy>1&&difference>1);tm_play(0);tm_render(l.data(),r.data(),rate);
    uint64_t chord=0;for(int i=0;i<8;i++)chord|=uint64_t(37+i*3)<<(i*8);
    for(int t=0;t<24;t++)tm_cell_at4(0,t,0,chord,9,70,50,0,0,0,0,0,0,48);
    tm_dsp_reset();tm_play(1);int peak=0,mean=0;for(int b=0;b<200;b++){tm_render(l.data(),r.data(),256);mean+=tm_dsp_load();peak=std::max(peak,tm_dsp_load());for(int i=0;i<256;i++)assert(std::isfinite(l[i])&&std::abs(l[i])<=1);}
    std::printf("Omondeu 192 active voices, 24 tracks: callback mean %.1f%% peak %.1f%% misses %d/200\n",mean/2000.,peak/10.,tm_dsp_overruns());
    tm_play(0);tm_render(l.data(),r.data(),256);
    for(int t=1;t<24;t++)tm_cell_at4(0,t,0,0,9,70,50,0,0,0,0,0,0,0);
    tm_dsp_reset();tm_play(1);peak=mean=0;for(int b=0;b<200;b++){tm_render(l.data(),r.data(),256);mean+=tm_dsp_load();peak=std::max(peak,tm_dsp_load());}
    std::printf("Omondeu 8 active voices, 1 track: callback mean %.1f%% peak %.1f%% misses %d/200\n",mean/2000.,peak/10.,tm_dsp_overruns());
}
