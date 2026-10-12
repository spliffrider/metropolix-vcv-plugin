#pragma once
#include <algorithm>
#include <cmath>

namespace metro {
enum Gate { Rest, Single, Multi, Hold };
enum RatchetMode { MultiplyRatchets, PulseRatchets, GatedRatchets };

// Phase and lengths are expressed in track clock pulses, not global pulses.
// Manual references: gate types p.24; DIV p.77; diagrams pp.94-97, 166-167.
inline bool gateHigh(Gate type, int pulse, double phase, int stagePulses,
                     int pulseDivision, bool stretch, RatchetMode mode,
                     int ratchets, float gateLength, double pulseDuration, int* eventId = nullptr) {
    if (eventId) *eventId = -1;
    if (type == Rest) return false;
    if (type == Hold) { if (eventId) *eventId = 0; return true; }
    const double position = pulse + phase;
    const int groupStart = type == Multi ? (pulse / pulseDivision) * pulseDivision : 0;
    const double groupPhase = position - groupStart;
    const double nominalLength = type == Multi ? pulseDivision : stagePulses;
    const double span = stretch ? nominalLength : 1.0;
    double interval = span / ratchets;
    double end = span;
    if (stretch && ratchets > 1 && mode != MultiplyRatchets) {
        interval = 1.0 / ratchets;
        end = mode == PulseRatchets ? 1.0 : nominalLength * gateLength;
    }
    if (groupPhase + 1e-9 >= end || groupPhase < 0.0) return false;
    // Only physical clock ticks may advance the group. A stalled input must
    // never create another group of ratchets from free-running modulo math.
    if (groupPhase >= nominalLength && stretch) return false;
    // Stretching keeps the nominal group length even when the final group is
    // partial, then truncates at one 1/96-pulse tick before the stage boundary.
    if (stretch && position >= stagePulses - 1.0 / 96.0) return false;
    const double minimum = 0.000333 / pulseDuration;
    const double high = std::min(interval * 0.99,
        std::max(minimum, interval * static_cast<double>(gateLength)));
    int ratchet = static_cast<int>(std::floor(groupPhase / interval + 1e-9));
    if (eventId) *eventId = groupStart * 64 + ratchet;
    double localPhase = groupPhase - ratchet * interval;
    return localPhase < high;
}
} // namespace metro
