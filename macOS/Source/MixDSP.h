#pragma once
#include <algorithm>
#include <array>
#include <cmath>

// Four fixed-frequency, double-precision biquads. No dynamic allocation.
class FourBandEQ {
    struct Band {std::array<double,5> c{1,0,0,0,0},target{1,0,0,0,0};double z1[2]{},z2[2]{};};
    Band bands[4];double rate=48000,alpha=.002;bool flat=true;int settle=0;
public:
    void init(double sr,double smoothing=.015){rate=sr;alpha=1-std::exp(-1/(sr*smoothing));flat=true;settle=0;for(auto &b:bands)b=Band{};}
    void configure(int bass,int a,int b,int high){flat=bass==0&&a==0&&b==0&&high==0;settle=int(rate*.18);const int gain[4]={bass,a,b,high};const double freq[4]={100,500,2500,8000};for(int i=0;i<4;i++){double A=std::pow(10.,std::clamp(gain[i],-18,18)/40.),w=6.283185307179586*std::min(freq[i],rate*.44)/rate,co=std::cos(w),si=std::sin(w),al=si/(2*.8),b0,b1,b2,a0,a1,a2;if(i==0||i==3){double x=2*std::sqrt(A)*si/std::sqrt(2.);if(i==0){b0=A*((A+1)-(A-1)*co+x);b1=2*A*((A-1)-(A+1)*co);b2=A*((A+1)-(A-1)*co-x);a0=(A+1)+(A-1)*co+x;a1=-2*((A-1)+(A+1)*co);a2=(A+1)+(A-1)*co-x;}else{b0=A*((A+1)+(A-1)*co+x);b1=-2*A*((A-1)+(A+1)*co);b2=A*((A+1)+(A-1)*co-x);a0=(A+1)-(A-1)*co+x;a1=2*((A-1)-(A+1)*co);a2=(A+1)-(A-1)*co-x;}}else{b0=1+al*A;b1=-2*co;b2=1-al*A;a0=1+al/A;a1=-2*co;a2=1-al/A;}bands[i].target={b0/a0,b1/a0,b2/a0,a1/a0,a2/a0};if(gain[i]==0)bands[i].target={1,0,0,0,0};}}
    float mono(float x){if(flat&&settle<=0)return x;if(settle>0&&--settle==0&&flat){for(auto &b:bands)b=Band{};return x;}double v=x;for(auto &b:bands){for(int i=0;i<5;i++)b.c[i]+=(b.target[i]-b.c[i])*alpha;double y=b.c[0]*v+b.z1[0];b.z1[0]=b.c[1]*v-b.c[3]*y+b.z2[0];b.z2[0]=b.c[2]*v-b.c[4]*y;if(std::abs(b.z1[0])<1e-25)b.z1[0]=0;if(std::abs(b.z2[0])<1e-25)b.z2[0]=0;v=y;}return float(v);}
    void process(float &l,float &r){if(flat&&settle<=0)return;if(settle>0&&--settle==0&&flat){for(auto &b:bands)b=Band{};return;}double v[2]={l,r};for(auto &b:bands){for(int i=0;i<5;i++)b.c[i]+=(b.target[i]-b.c[i])*alpha;for(int ch=0;ch<2;ch++){double y=b.c[0]*v[ch]+b.z1[ch];b.z1[ch]=b.c[1]*v[ch]-b.c[3]*y+b.z2[ch];b.z2[ch]=b.c[2]*v[ch]-b.c[4]*y;if(std::abs(b.z1[ch])<1e-25)b.z1[ch]=0;if(std::abs(b.z2[ch])<1e-25)b.z2[ch]=0;v[ch]=y;}}l=float(v[0]);r=float(v[1]);}
};
