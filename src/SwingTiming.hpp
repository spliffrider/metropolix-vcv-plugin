#pragma once
#include <algorithm>
#include <cmath>

namespace metro {
// Manual p.79: 50% is straight; the second pulse of a pair is delayed.
// Swing lives below the global clock and track divider, above stage traversal.
// Latch each pair so live CV cannot move a scheduled pulse backwards in time.
inline float swingPercent(float value) {
    return std::isfinite(value) ? std::max(50.f, std::min(78.f, value)) : 50.f;
}
inline int swingShortcut(int stage) {
    const int values[] = {50,54,58,62,66,70,75,78};
    return values[std::max(0,std::min(7,stage))];
}
struct SwingTiming {
    bool second = false, pending = false;
    double amount = .5, remaining = 0.0, duration = .125;
    void reset() { *this = SwingTiming(); }
    bool advance(double dt) {
        if (!pending) return false;
        remaining -= dt;
        if (remaining > 1e-10) return false;
        pending = false;
        return true;
    }
    // Called only for an actual (possibly divided) clock edge. Never invent
    // future external pulses if the clock input stalls. The caller drains an
    // older pending pulse first if an abrupt tempo increase overtakes it.
    bool clock(double period, float percent) {
        if (!second) {
            amount = swingPercent(percent) * .01;
            duration = period * 2.0 * amount;
            remaining = 0.0;
        } else {
            duration = period * 2.0 * (1.0 - amount);
            remaining = period * (2.0 * amount - 1.0);
        }
        second = !second;
        pending = remaining > 1e-10;
        return !pending;
    }
};
} // namespace metro
