#pragma once
#include <array>
#include <atomic>
#include <cstdint>

namespace metro {

enum LoadDelay { LoadInstant, LoadQueuePulses, LoadTotalPulses };

// Single producer / single consumer, fixed storage. No allocation or locks on
// the audio thread. One cell remains unused to distinguish full from empty.
template <typename T, unsigned N> class PresetMailbox {
    std::array<T, N> cells{};
    std::atomic<unsigned> readIndex{0}, writeIndex{0};
public:
    bool push(const T& value) {
        unsigned w = writeIndex.load(std::memory_order_relaxed);
        unsigned next = (w + 1) % N;
        if (next == readIndex.load(std::memory_order_acquire)) return false;
        cells[w] = value;
        writeIndex.store(next, std::memory_order_release);
        return true;
    }
    bool pop(T& value) {
        unsigned r = readIndex.load(std::memory_order_relaxed);
        if (r == writeIndex.load(std::memory_order_acquire)) return false;
        value = cells[r];
        readIndex.store((r + 1) % N, std::memory_order_release);
        return true;
    }
    // Only while the engine is excluded (Rack patch load/reset).
    void clear() { readIndex.store(0); writeIndex.store(0); }
};

// Counts base clock pulses, independent of track divisions, skips and LEN.
// Queue Pulses measures multiples since an explicit reset/recall/start. Total
// Pulses repeats a phrase; its automatic wrap does not shorten that queue grid.
struct PresetTiming {
    uint64_t queuePulse = 0;
    unsigned phrasePulse = 0;
    void reset() { queuePulse = 0; phrasePulse = 0; }
    bool totalDue(int total) const { return total > 0 && phrasePulse >= unsigned(total); }
    bool loadDue(int delay, int queue, int total) const {
        if (delay == LoadInstant) return true;
        if (delay == LoadTotalPulses && total > 0) return totalDue(total);
        return queue > 0 && queuePulse > 0 && queuePulse % unsigned(queue) == 0;
    }
    void tick() { ++queuePulse; ++phrasePulse; }
};

template <unsigned N> struct PresetSnapshot {
    std::array<float, N> values{};
    bool occupied = false;
};

} // namespace metro
