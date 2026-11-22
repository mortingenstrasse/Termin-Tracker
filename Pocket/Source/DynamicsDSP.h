#pragma once
#include <algorithm>
#include <cmath>

// Stereo-linked, zero-lookahead gain control. No allocations or delay buffers.
struct DynamicsDSP {
    float gain=1.f,attack=.01f,release=.001f,threshold=.05f,exponent=.75f,minGain=.125f,makeup=1.f;
    void configure(double sr,int thresholdDB,int ratio,int attackMS,int releaseMS,int makeupDB,int depthDB){threshold=std::pow(10.f,thresholdDB/20.f);exponent=1.f-1.f/std::max(1,ratio);attack=1.f-std::exp(-1.f/float(sr*std::max(1,attackMS)*.001));release=1.f-std::exp(-1.f/float(sr*std::max(20,releaseMS)*.001));makeup=std::pow(10.f,makeupDB/20.f);minGain=std::pow(10.f,-depthDB/20.f);}
    float process(float peak,bool enabled){float target=enabled&&peak>threshold?std::max(minGain,std::pow(threshold/std::max(peak,1e-8f),exponent)):1.f;gain+=(target-gain)*(target<gain?attack:release);if(std::abs(gain-1)<1e-6f)gain=1;return gain*(enabled?makeup:1.f);}
    void reset(){gain=1.f;}
};
