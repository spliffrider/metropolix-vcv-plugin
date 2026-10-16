#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <array>

namespace metro {
inline int midiValue(float v) {return std::max(0,std::min(127,int(std::lround(v*127.f))));}
inline int midiVoltage(float v,bool bipolar) {return midiValue(bipolar?(v+5.f)*.1f:v*.2f);}
inline int midiNote(float volts,int zero=60) {return std::max(0,std::min(127,int(std::lround(volts*12.f))+zero));}

// Channels are displayed as 1..16; zero disables a lane. Emit receives bytes,
// not host-owned buffers, so the same logic is tested without a MIDI device.
struct MidiVoice {
    int note=-1,channel=0; bool high=false; uint64_t event=0;
    template<class Emit> void silence(Emit emit) {
        if(note>=0)emit(0x80+channel-1,note,0,3);
        note=-1;channel=0;high=false;
    }
    template<class Emit> bool process(bool gate,int key,int ch,int velocity,uint64_t serial,bool legato,Emit emit) {
        ch=std::max(0,std::min(16,ch));key=std::max(0,std::min(127,key));
        velocity=std::max(0,std::min(127,velocity));
        gate=gate && velocity>0;
        bool change=note>=0 && (ch!=channel || key!=note || serial!=event);
        bool start=ch>0 && gate && (note<0 || !high || change);
        if(note>=0 && (!gate || !ch || change)) {
            if(start && legato && ch==channel && key!=note) {
                emit(0x90+ch-1,key,velocity,3);emit(0x80+channel-1,note,0,3);start=false;
            }else emit(0x80+channel-1,note,0,3);
            note=-1;
        }
        if(start)emit(0x90+ch-1,key,velocity,3);
        bool began=ch>0 && gate && (start || change);
        note=ch>0 && gate?key:-1;channel=ch;high=gate;event=serial;
        return began;
    }
};

// One shared MX bus, independent of a physical expander. CC102..109 or notes
// 35..42 carry its eight gates. The caller supplies each gate's MIDI value;
// modulation modes can therefore use the programmed stage's velocity/value.
struct MidiMxOutput {
    int channel=0,kind=0,mode=-1;
    std::array<MidiVoice,8> voices;
    std::array<int,8> lastCC{{-1,-1,-1,-1,-1,-1,-1,-1}};
    std::array<bool,8> previous{},suppressed{};
    std::array<uint64_t,8> events{};
    template<class Emit> void silence(Emit emit,bool suppressHeld=false){
        for(auto& voice:voices)voice.silence(emit);
        if(channel && !kind)for(int lane=0;lane<8;++lane)if(lastCC[lane]>0)emit(0xb0+channel-1,102+lane,0,3);
        lastCC.fill(-1);if(suppressHeld)suppressed=previous;
    }
    void suppressHeld(){suppressed=previous;}
    // Release through silence() before discarding a host/runtime receipt.
    void reset(){voices.fill(MidiVoice{});channel=kind=0;mode=-1;lastCC.fill(-1);previous.fill(false);suppressed.fill(false);events.fill(0);}
    template<class Emit> void process(int requestedChannel,int requestedKind,int requestedMode,const std::array<float,8>& gates,const std::array<int,8>& levels,Emit emit){
        requestedChannel=std::max(0,std::min(16,requestedChannel));requestedKind=std::max(0,std::min(1,requestedKind));
        if(channel!=requestedChannel || kind!=requestedKind || mode!=requestedMode){silence(emit);channel=requestedChannel;kind=requestedKind;mode=requestedMode;previous.fill(false);suppressed.fill(false);}
        if(!channel){previous.fill(false);return;}
        for(int lane=0;lane<8;++lane){
            bool high=std::isfinite(gates[lane]) && gates[lane]>=1.5f;
            if(!high)suppressed[lane]=false;
            if(high && !previous[lane])++events[lane];
            previous[lane]=high;bool active=high && !suppressed[lane];int value=std::max(0,std::min(127,levels[lane]));
            if(kind)voices[lane].process(active,35+lane,channel,value,events[lane],false,emit);
            else{int next=active?value:0;if(next!=lastCC[lane]){emit(0xb0+channel-1,102+lane,next,3);lastCC[lane]=next;}}
        }
    }
};

// MIDI quarter-note clock has six pulses per Rack sixteenth. Input continues
// counting while stopped. Start/Continue align phase; SPP/reset do not.
struct MidiClockInput {
    unsigned phase=0; uint64_t pending=0;
    void align(){phase=0;pending=0;}
    void pulse(){if(phase==0 && pending<1024)++pending;phase=(phase+1)%6;}
    bool edge(){if(!pending)return false;--pending;return true;}
};

struct MidiClockOutput {
    double elapsed=0, period=.125, until=0;bool seen=false;
    unsigned remaining=0;
    void align(){elapsed=until=0;remaining=0;seen=false;}
    template<class Emit> void process(double dt,bool master,bool external,double internalPeriod,bool enabled,Emit emit) {
        elapsed+=dt;
        if(!external){
            if(!seen){seen=true;until=0;}
            until-=dt;
            if(until<=1e-10){if(enabled)emit(0xf8,0,0,1);until+=internalPeriod/6;}
            return;
        }
        if(master){
            if(external && seen)period=std::max(.001,std::min(150.,elapsed));
            else if(!external || !seen)period=internalPeriod;
            elapsed=0;seen=true;remaining=5;until=period/6;
            if(enabled)emit(0xf8,0,0,1);
        }else if(remaining){
            until-=dt;
            if(until<=1e-10){if(enabled)emit(0xf8,0,0,1);--remaining;until+=period/6;}
        }
    }
};
} // namespace metro
