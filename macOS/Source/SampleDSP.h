#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

// Immutable playback settings. The UI publishes these independently from PCM.
struct PlaybackConfig {
    int mode=0,start=0,end=0,sliceCount=0;
    int slices[48]{};
    int position=500,lengthMs=120,shape=2,direction=0,spread=8,lfoMode=0,lfoShape=0,lfoSteps=128,lfoDepth=100,scanMode=0,anchors[5]{0,250,500,750,1000};
};

struct GrainState {double position=0;int phase=0,length=1,direction=1,shape=2,mode=0;};
class GrainWindows {
    std::array<std::array<float,1024>,4> table{};
public:
    void init(){for(int i=0;i<1024;i++){float x=float(i)/1023;table[0][i]=std::min(1.f,std::min(x*20.f,(1-x)*20.f));table[1][i]=1.f-std::abs(2*x-1);table[2][i]=std::max(0.f,(std::exp(-.5f*std::pow((x-.5f)*6.f,2.f))-.011109f)/.988891f);table[3][i]=.5f-.5f*std::cos(6.28318530718f*x);}}
    float value(int shape,int phase,int length)const{return table[std::clamp(shape,0,3)][std::clamp(int(float(phase)*1023.f/std::max(1,length-1)),0,1023)];}
};

// Small, original stereo Schroeder-style room and tempo-synced cross-feedback delay.
// Buffers are allocated only during initialization, never in the audio render path.
class RealtimeFX {
    struct Tap{int current=-1,prior=0;float blend=1;void set(int n){if(current<0){current=prior=n;blend=1;}else if(n!=current){prior=current;current=n;blend=0;}}float read(const std::vector<float>& b,size_t i,int n,float direct=0)const{if(n==0)return direct;size_t p=i>=size_t(n)?i-size_t(n):i+b.size()-size_t(n);return b[p];}float value(const std::vector<float>& b,size_t i,float direct=0)const{float y=read(b,i,current,direct);return blend>=1?y:read(b,i,prior,direct)*(1-blend)+y*blend;}void advance(float step){blend=std::min(1.f,blend+step);}};
    struct Comb{std::vector<float> b;size_t i=0;float low=0,fb=0;Tap tap;bool first=true;float process(float x,int length,float feedback,float damping,float smooth){tap.set(length);float y=tap.value(b,i);tap.advance(smooth);if(first){fb=feedback;first=false;}else fb+=(feedback-fb)*smooth;low+=(y-low)*(1.f-damping);b[i]=x+low*fb;i=(i+1==b.size()?0:i+1);return y;}};
    struct Allpass{std::vector<float> b;size_t i=0;float process(float x){float y=b[i],out=y-x*.5f;b[i]=x+y*.5f;i=(i+1==b.size()?0:i+1);return out;}};
    Comb comb[2][8];Allpass ap[2][3];std::vector<float> dl,dr,pre[2];size_t di=0,pi=0;float delayLowL=0,delayLowR=0;double rate=48000;int lengths[2][8]{};float feedbacks[2][8]{};int preFrames=0;float damping=.76f,width=1.f,revLevel=.55f,delayFeedback=.43f,delayDamping=.65f,delayLevel=.7f,cross=1.f;float current[7]{},smooth=.002f;bool first=true;Tap preTap,delayTap;
public:
    void init(double sr){rate=sr;smooth=float(1./std::max(1.,sr*.015));first=true;preTap=Tap{};delayTap=Tap{};for(int c=0;c<2;c++){pre[c].assign(std::max(2,int(sr*.251)),0);for(auto &v:comb[c]){v.b.assign(std::max(2,int(sr*.14)),0);v.i=0;v.low=0;v.tap=Tap{};v.first=true;}for(int i=0;i<3;i++){auto &v=ap[c][i];v.b.assign(std::max(2,int(sr*(i==0?.005:i==1?.0017:.0113)+c*13)),0);v.i=0;}}dl.assign(std::max(2,int(sr*6.01)),0);dr=dl;di=pi=0;delayLowL=delayLowR=0;configure(50,10,0,76,100,55,43,65,70,true);}
    void configure(int size,int decay,int predelay,int damp,int stereo,int level,int df,int dd,int dlv,bool pingpong){if(dl.empty())return;const double times[8]={.0239,.0297,.0337,.0371,.0391,.0411,.0437,.0473};double scale=.4+1.2*size/100.;for(int c=0;c<2;c++)for(int i=0;i<8;i++){double seconds=(times[i]+c*.0013)*scale;lengths[c][i]=std::clamp(int(rate*seconds),1,int(comb[c][i].b.size()-1));feedbacks[c][i]=float(std::min(.995,std::pow(.001,seconds/std::max(.1,decay/10.))));}preFrames=std::clamp(int(rate*predelay/1000.),0,int(pre[0].size()-1));damping=damp*.0098f;width=stereo*.01f;revLevel=level*.01f;delayFeedback=df*.01f;delayDamping=dd*.0098f;delayLevel=dlv*.01f;cross=pingpong?1.f:0.f;}
    void process(float revL,float revR,float sendL,float sendR,int delayFrames,float &l,float &r){if(dl.empty())return;float target[7]={damping,width,revLevel,delayFeedback,delayDamping,delayLevel,cross};for(int k=0;k<7;k++){if(first)current[k]=target[k];else current[k]+=(target[k]-current[k])*smooth;}first=false;preTap.set(preFrames);delayTap.set(std::clamp(delayFrames,1,int(dl.size()-1)));float input[2]={revL+.12f*revR,revR+.12f*revL},wet[2]{};for(int c=0;c<2;c++){float x=preTap.value(pre[c],pi,input[c]);pre[c][pi]=input[c];float a=0;for(int j=0;j<8;j++)a+=comb[c][j].process(x*.125f,lengths[c][j],feedbacks[c][j],current[0],smooth);for(auto &v:ap[c])a=v.process(a);wet[c]=a*current[2];}preTap.advance(smooth);pi=(pi+1==pre[0].size()?0:pi+1);float mid=(wet[0]+wet[1])*.5f,side=(wet[0]-wet[1])*.5f*current[1];float a=delayTap.value(dl,di),b=delayTap.value(dr,di);delayTap.advance(smooth);delayLowL+=(a-delayLowL)*(1-current[4]);delayLowR+=(b-delayLowR)*(1-current[4]);dl[di]=sendL+current[3]*(delayLowL*(1-current[6])+delayLowR*current[6]);dr[di]=sendR+current[3]*(delayLowR*(1-current[6])+delayLowL*current[6]);di=(di+1==dl.size()?0:di+1);l+=mid+side+a*current[5];r+=mid-side+b*current[5];}
};
