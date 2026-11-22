#include "SampleDSP.h"
#include <cassert>
#include <cstdio>
int main(){
    auto energy=[](int size,int decay,int pre,int width){RealtimeFX f;f.init(48000);f.configure(size,decay,pre,35,width,80,43,65,70,true);double late=0,stereo=0,early=0;for(int i=0;i<240000;i++){float l=0,r=0;f.process(i==0?1:0,0,0,0,12000,l,r);assert(std::isfinite(l)&&std::isfinite(r));if(i<1500)early+=l*l+r*r;if(i>48000)late+=l*l+r*r;stereo+=(l-r)*(l-r);}return std::array<double,3>{early,late,stereo};};
    auto room=energy(50,10,0,100),hall=energy(90,45,28,100),mono=energy(90,45,28,0);assert(hall[1]>room[1]*10);assert(hall[0]==0);assert(mono[2]<1e-12);
    RealtimeFX f;f.init(48000);f.configure(50,10,0,50,100,0,60,0,100,true);float l=0,r=0;for(int i=0;i<=2000;i++){l=r=0;f.process(0,0,i==0?1:0,0,1000,l,r);if(i==1000)assert(l>.9&&r==0);if(i==2000)assert(r>.59&&l==0);}std::puts("FX Hall tail, predelay, width and ping-pong passed");
}
