#include "Engine.h"
#include <array>
#include <cmath>
#include <cassert>
#include <cstdio>
int main(){
    tm_init(48000); std::array<float,512> l{},r{}; std::array<float,48000> pcm{};
    for(int i=0;i<(int)pcm.size();i++)pcm[i]=.6f*std::sin(6.2831853*261.6256*i/48000);
    assert(tm_instrument_sample(10,pcm.data(),pcm.size(),48000,60));tm_sample_config(10,5,0,pcm.size(),nullptr,0,500,100,3,0,10);
    auto block=[&]{tm_render(l.data(),r.data(),l.size()); double energy=0;for(float x:l){assert(std::isfinite(x));energy+=x*x;}return energy;};
    tm_midi_note(60,10,100);assert(block()>.0001);tm_midi_note(64,10,100);tm_midi_note(67,10,100);assert(block()>.0001);
    // Release one voice without interrupting the other two, then release all.
    tm_midi_note(60,10,0);block();assert(block()>.0001);tm_midi_note(64,10,0);tm_midi_note(67,10,0);block();block();assert(block()<1e-12);
    tm_midi_note(48,10,110);assert(block()>.0001);tm_midi_panic();block();block();assert(block()<1e-12);
    // Sequenced OFF event terminates a persistent granular voice at a row boundary.
    tm_select_pattern(0,16);tm_cell_at3(0,0,0,61,10,100,100,0,0,0,0,0,0);tm_cell_at3(0,0,1,UINT64_MAX,10,100,100,0,0,0,0,0,0);tm_bpm(120);tm_play(1);assert(block()>.0001);assert(tm_record_position()!=UINT32_MAX);for(int i=0;i<18;i++)block();assert(block()<1e-12);tm_play(0);block();assert(tm_record_position()==UINT32_MAX);
    std::puts("MIDI polyphonic granular, independent note-off, panic, sequenced OFF and transport snapshot passed");
}
