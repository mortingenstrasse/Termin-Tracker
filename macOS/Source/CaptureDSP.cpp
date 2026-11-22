#include "Engine.h"
#include <atomic>
#include <algorithm>
#include <cmath>
namespace {constexpr int Cap=1000000;float pcm[Cap];std::atomic<int> written{0};}
void tm_capture_begin(){written.store(0,std::memory_order_release);}
int tm_capture_push(const float* l,const float* r,int frames){if(!l||frames<=0)return written.load();int at=written.load(std::memory_order_relaxed),n=std::min(frames,Cap-at);for(int i=0;i<n;i++){float x=r?(l[i]+r[i])*.5f:l[i];pcm[at+i]=std::isfinite(x)?std::clamp(x,-1.f,1.f):0;}written.store(at+n,std::memory_order_release);return at+n;}
int tm_capture_count(){return written.load(std::memory_order_acquire);}
int tm_capture_copy(float* out,int capacity){int n=std::min(std::max(0,capacity),tm_capture_count());if(out)std::copy_n(pcm,n,out);return n;}
void tm_capture_peaks(float* low,float* high,int columns){if(!low||!high||columns<=0)return;int n=tm_capture_count();for(int i=0;i<columns;i++){int lo=int((int64_t(i)*n)/columns),hi=std::min(n,std::max(lo+1,int((int64_t(i+1)*n)/columns)));float a=0,b=0;for(int j=lo;j<hi;j++){a=std::min(a,pcm[j]);b=std::max(b,pcm[j]);}low[i]=a;high[i]=b;}}
