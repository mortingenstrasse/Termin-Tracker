#pragma once
#include <cmath>
#include <algorithm>
#include <array>

// Omondeu: band-limited subtractive synthesis, continuous phase and true stereo.
namespace omondeu {
constexpr int N=23;
constexpr float pi=3.14159265358979323846f;
inline constexpr std::array<int,N> defaults={1,2,70,35,75,-1,0,7,800,28,35,45,4,250,75,180,0,70,60,22,350,0,0};
// wave A/B, mix A/B/sub, sub octave, B semitone, detune cents, cutoff Hz,
// resonance, filter env, drive, attack, decay, sustain, release, mode, glide,
// width, pulse width offset, filter decay, LFO rate (tenths Hz), LFO depth.
inline float sineTable[2049]{};
inline void init(){for(int i=0;i<=2048;i++)sineTable[i]=std::sin(2*pi*i/2048);}
inline float sine(float p){float x=p*2048;int i=std::clamp(int(x),0,2047);return sineTable[i]+(sineTable[i+1]-sineTable[i])*(x-i);}
inline float blep(float p,float d){if(p<d){p/=d;return 2*p-p*p-1;}if(p>1-d){p=(p-1)/d;return p*p+2*p+1;}return 0;}
inline float wrap(float p){return p>=1?p-1:p<0?p+1:p;}
inline float osc(int w,float &p,float d,float pw){d=std::clamp(d,0.0000001f,.45f);float x=p,v=0;if(w==0)v=sine(x);else if(w==1)v=2*x-1-blep(x,d);else if(w==2)v=(x<pw?1.f:-1.f)+blep(x,d)-blep(wrap(x-pw),d);else{ // band-limited triangle via odd harmonics
    v=sine(x);if(d*3<.45f)v-=sine(x*3-std::floor(x*3))/9;if(d*5<.45f)v+=sine(x*5-std::floor(x*5))/25;v*=.81f;
}p=wrap(p+d);return v;}
inline float sat(float x){return x/(1+std::abs(x));}
struct Patch{
    std::array<int,N> p=defaults;
    float a=.7f,b=.35f,sub=.75f,subRatio=.5f,bRatio=1,detL=1,detR=1,g=.05f,k=1.5f,env=.35f,drive=1.45f,attack=.005f,decay=.005f,sustain=.75f,release=.005f,glide=.002f,width=.6f,pw=.5f,filterDecay=.001f,lfoRate=0,lfoDepth=0;
    void prepare(double sr){a=p[2]*.01f;b=p[3]*.01f;sub=p[4]*.01f;subRatio=std::exp2(float(p[5]));bRatio=std::exp2(p[6]/12.f);detL=std::exp2(-p[7]/1200.f);detR=1/detL;g=std::tan(pi*std::min(float(sr*.40),float(p[8]))/float(sr));k=2.f-1.88f*p[9]*.01f;env=p[10]*.01f;drive=1+p[11]*.035f;attack=1.f/std::max(1.,sr*p[12]*.001);decay=1.f/std::max(1.,sr*p[13]*.001);sustain=p[14]*.01f;release=1.f/std::max(1.,sr*p[15]*.001);glide=1.f-std::exp(-1.f/std::max(1.,sr*p[17]*.001));width=p[18]*.01f;pw=.5f+p[19]*.004f;filterDecay=1.f-std::exp(-4.f/std::max(1.,sr*p[20]*.001));lfoRate=p[21]*.1f/float(sr);lfoDepth=p[22]*.01f;}
};
struct Filter {float z1=0,z2=0;float process(float x,float g,float k){float a=1/(1+g*(g+k)),v1=a*(z1+g*(x-z2)),v2=z2+g*v1;z1=2*v1-z1;z2=2*v2-z2;return v2;}};
struct Voice {
    float pa=0,pbL=0,pbR=.23f,ps=0,lf=0,amp=0,fe=1,pitch=0,releaseStart=1,sideLow=0,cut=0,side=0;int stage=0;Filter left,right;
    void start(float note){pitch=note;}
    float render(const Patch &p,float target,bool off,float sr,float &s){
        if(off&&stage!=3){stage=3;releaseStart=amp;}
        if(stage==0){amp+=p.attack;if(amp>=1){amp=1;stage=1;}}
        else if(stage==1){amp=std::max(p.sustain,amp-p.decay);if(amp<=p.sustain)stage=2;}
        else if(stage==2)amp+=(p.sustain-amp)*.002f;
        else amp=std::max(0.f,amp-p.release*releaseStart);
        pitch+=(target-pitch)*p.glide;fe+=(0-fe)*p.filterDecay;
        float a=osc(p.p[0],pa,pitch,p.pw)*p.a,sub=osc(0,ps,pitch*p.subRatio,.5f)*p.sub;
        // keep sub centered, detune oscillator B only; high-pass the stereo difference
        float bl=osc(p.p[1],pbL,pitch*p.bRatio*p.detL,p.pw)*p.b,br=osc(p.p[1],pbR,pitch*p.bRatio*p.detR,p.pw)*p.b;
        float g=std::clamp(p.g*(1+fe*p.env*7+sine(lf)*p.lfoDepth*.8f),.0001f,2.8f);lf=wrap(lf+p.lfoRate);cut+=(g-cut)*.015f;
        float l=left.process(sat((a+bl)*p.drive),cut,p.k),r=right.process(sat((a+br)*p.drive),cut,p.k);
        float diff=(l-r)*.5f;sideLow+=(diff-sideLow)*std::min(.1f,2*pi*120/sr);s=(diff-sideLow)*p.width*amp*.15f;
        return ((l+r)*.5f+sub*.65f)*amp*.15f;
    }
};
}
