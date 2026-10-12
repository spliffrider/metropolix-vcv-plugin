#pragma once
#include <algorithm>
#include <cstdlib>
#include "PlayOrder.hpp"

namespace metro {
enum AccumOrder { AccumWrap, AccumPendulum, AccumRandom, AccumHold };
enum AccumTrigger { AccumStage, AccumPulse, AccumRatchet };
enum AccumMode { StageAccumulation, TrackAccumulation };

// The caller plays the current value before advancing it. Limits include their
// endpoints, as in the printed sequences on manual pp.128-133.
inline int advanceAccumulator(int value, int amount, int positive, int negative,
                              bool bipolar, AccumOrder order, int& direction,
                              RandomSource& random) {
    if (amount == 0) return value;
    positive = std::max(1, std::min(28, positive));
    negative = std::max(1, std::min(28, negative));
    const int lo = bipolar || amount < 0 ? -negative : 0;
    const int hi = bipolar || amount > 0 ? positive : 0;
    value = std::max(lo, std::min(hi, value));
    int step = amount;
    if (order == AccumRandom) {
        const float chance = random.percent();
        if (chance >= 80.f) return value;
        step = (amount < 0 ? -1 : 1) * (1 + random.index(std::abs(amount)));
        if (chance >= 50.f) step = -step;
    }
    if (order == AccumHold) return std::max(lo, std::min(hi, value + step));
    if (order == AccumPendulum) {
        int next = value + step * direction;
        // At most 28 reflections for the supported +/-28 amount and >=1 span.
        while (next < lo || next > hi) {
            if (next > hi) next = hi - (next - hi);
            else next = lo + (lo - next);
            direction = -direction;
        }
        return next;
    }
    const int width = hi - lo + 1;
    return lo + ((value + step - lo) % width + width) % width;
}
} // namespace metro
