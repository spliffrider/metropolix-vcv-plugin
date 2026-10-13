#pragma once
#include <array>
#include <cstdint>
#include <limits>

namespace metro {
struct ChainLink {
    int slot = 0, repeats = 1;
    unsigned mutes = 0; // bit 0 TRK 1, bit 1 TRK 2, bit 2 all MOD lanes
    bool operator==(const ChainLink& b) const { return slot==b.slot && repeats==b.repeats && mutes==b.mutes; }
};
struct PresetChain {
    std::array<ChainLink,32> links{};
    int size = 0;
    bool operator==(const PresetChain& b) const { return size==b.size && links==b.links; }
    bool insert(int at, ChainLink link = ChainLink()) {
        if(size>=32 || at<0 || at>size)return false;
        for(int i=size;i>at;--i)links[i]=links[i-1];
        links[at]=link;++size;return true;
    }
    bool erase(int at) {
        if(at<0 || at>=size)return false;
        for(int i=at;i<31 && i+1<size;++i)links[i]=links[i+1];
        links[--size]=ChainLink();return true;
    }
};
// Called before each base clock. Track divisions, skipped stages and ratchets
// never advance this counter. UI frame rate is irrelevant to playback.
struct ChainPosition {
    int link = 0, repeat = 1;
    uint32_t pulse = 0;
    bool fresh = true;
    void reset() { link=0;repeat=1;pulse=0;fresh=true; }
    bool advance(const PresetChain& chain, int total, int queue) {
        if(chain.size<=0)return false;
        int duration=total>0?total:queue;
        if(fresh || duration<=0 || pulse<static_cast<unsigned>(duration))return false;
        pulse=0;
        if(repeat<chain.links[link].repeats) { ++repeat;return false; }
        repeat=1;link=(link+1)%chain.size;return true;
    }
    void tick() { fresh=false;if(pulse<std::numeric_limits<uint32_t>::max())++pulse; }
};
} // namespace metro
