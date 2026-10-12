#pragma once

// The musical engine has no Rack or UI dependencies. All state below belongs to
// the audio thread; views communicate through Rack parameters and atomic meters.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include "PlayOrder.hpp"
#include "GateTiming.hpp"
#include "Accumulator.hpp"
#include "Modulation.hpp"

namespace metro {

template <typename T> T clamp(T v, T lo, T hi) { return std::max(lo, std::min(v, hi)); }
inline int modulo(int v, int n) { return ((v % n) + n) % n; }

struct Stage {
    float pitch = 0.f;
    int pulses = 1;
    Gate gate = Multi;
};
struct StageEdit {
    int ratchets = 1;
    float probability = 100.f;
    bool probabilityPerPulse = false;
    int accumulation = 0;
    AccumTrigger accumTrigger = AccumStage;
    bool slide = false;
    bool skip = false;
    float cv = 0.f;
};
struct Track {
    int length = 8;
    int offset = 0;
    int division = 1;
    Order order = Forward;
    bool reversed = false;
    int pulseDivision = 1;
    int pulseLimit = 0; // 0 = Auto; 1..64 = a fixed track-clock pulse length.
    bool gateStretch = false;
    RatchetMode ratchetMode = MultiplyRatchets;
    bool autoAccumReset = false;
    int transpose = 0;
    float gateLength = 0.5f;
    float slideTime = 0.25f;
    int accumLimit = 9;
    int accumNegativeLimit = 9;
    bool accumBipolar = false;
    AccumOrder accumOrder = AccumWrap;
    bool mute = false;
    std::array<StageEdit, 8> stages;
};
struct Settings {
    float bpm = 120.f;
    int root = 0;
    int scale = 1;
    int octaves = 3;
    std::array<Stage, 8> stages;
    std::array<Track, 2> tracks;
    std::array<ModLane, 8> modLanes;
    std::array<AuxInput, 3> auxInputs;
    std::array<int, 2> outputSources{{0,1}}; // 0/1 = track CV, 2..9 = MOD 1..8
};

struct Scale {
    int count;
    int notes[12];
};
inline const Scale& scaleAt(int index) {
    static const Scale scales[] = {
        {12, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},
        {7, {0, 2, 4, 5, 7, 9, 11}},
        {7, {0, 2, 3, 5, 7, 8, 10}},
        {7, {0, 2, 3, 5, 7, 9, 10}},
        {7, {0, 2, 4, 5, 7, 9, 10}},
        {5, {0, 2, 4, 7, 9}},
        {5, {0, 3, 5, 7, 10}}
    };
    return scales[clamp(index, 0, 6)];
}
inline int degreeToSemitone(int degree, int scale) {
    const Scale& s = scaleAt(scale);
    int note = modulo(degree, s.count);
    int octave = (degree - note) / s.count;
    return 12 * octave + s.notes[note];
}
inline int nearestDegree(float semitone, int scale) {
    const Scale& s = scaleAt(scale);
    int octave = static_cast<int>(std::floor(semitone / 12.f));
    int best = octave * s.count;
    float distance = 100.f;
    for (int d = (octave - 1) * s.count; d <= (octave + 2) * s.count; ++d) {
        float candidate = std::abs(semitone - degreeToSemitone(d, scale));
        if (candidate < distance) { distance = candidate; best = d; }
    }
    return best;
}

struct TrackState {
    int stage = -1;
    int position = -1;
    int pulse = 0;
    int divider = 0;
    int totalPulses = 0;
    int windowOffset = -1;
    PlayOrder playOrder;
    std::array<int, 8> accumulator{};
    std::array<int, 8> accumDirection{{1,1,1,1,1,1,1,1}};
    int activeAccumulation = 0;
    int lastRatchetEvent = -1;
    bool allowed = true;
    bool hasPitch = false;
    double age = 0.0;
    double duration = 0.125;
    float pitch = 0.f;
    float targetPitch = 0.f;
    float startPitch = 0.f;
    double slideAge = 0.0;
    double slideDuration = 0.0;
    float gate = 0.f;
    float cv = 0.f;
};

class Sequencer {
public:
    Settings settings;
    std::array<TrackState, 2> tracks;
    Modulation modulation;
    std::array<float, 2> aux{};
    bool running = false;
    float clock = 0.f;

    void seed(uint32_t value) {
        random[0].state = value ? value : 1;
        random[1].state = value ^ 0x9e3779b9u;
        if (!random[1].state) random[1].state = 1;
        modulation.seed(value);
    }

    void reset() {
        for (auto& state : tracks) {
            // CV pitch holds on reset until the next played note.
            float pitch = state.pitch;
            state = TrackState();
            state.pitch = pitch;
            state.targetPitch = pitch;
        }
        phase = 0.0;
        firstTick = true;
        clockRemaining = 0.0;
        clock = 0.f;
        modulation.reset();
        aux.fill(0.f);
    }

    // One external edge is one sixteenth-note pulse in this first version.
    void process(double dt, bool run, bool external, bool edge, bool resetEdge) {
        dt = clamp(dt, 1e-7, 0.1);
        const double internalPeriod = 15.0 / clamp(settings.bpm, 0.1f, 400.f);
        if (external != previousExternal) {
            haveExternalEdge = false;
            externalElapsed = 0.0;
            externalPeriod = internalPeriod;
            reset();
            previousExternal = external;
        }
        externalElapsed += dt;
        if (external && edge) {
            if (haveExternalEdge)
                externalPeriod = clamp(externalElapsed, 0.001, 150.0);
            externalElapsed = 0.0;
            haveExternalEdge = true;
        }
        if (run != running) {
            running = run;
            if (running) reset();
        }
        if (resetEdge) {
            reset();
            resetIgnore = 0.001;
        }

        const double period = external ? externalPeriod : internalPeriod;
        bool tick = false;
        if (running) {
            if (external) {
                tick = edge && resetIgnore <= 0.0;
            } else if (firstTick || phase >= period) {
                tick = true;
                phase = firstTick ? 0.0 : std::fmod(phase, period);
            }
            if (tick) modulation.tick(settings.modLanes, period);
            modulation.render(settings.modLanes, dt, settings.auxInputs);
            if (tick) {
                firstTick = false;
                clockRemaining = 0.001;
                for (int t = 0; t < 2; ++t) {
                    auto& state = tracks[t];
                    const auto& config = settings.tracks[t];
                    int division = clamp(config.division, 1, 64);
                    if (state.divider == 0 || state.divider >= division)
                        trackTick(t, period * division);
                    state.divider = (state.divider + 1) % division;
                }
            }
            for (int t = 0; t < 2; ++t) render(t, dt);
            phase += dt;
        } else {
            for (auto& state : tracks) { state.gate = 0.f; state.cv = 0.f; }
            modulation.silence();
            clockRemaining = 0.0;
        }
        clock = running && clockRemaining > 0.0 ? 5.f : 0.f;
        clockRemaining -= dt;
        resetIgnore = std::max(0.0, resetIgnore - dt);
        for (int output = 0; output < 2; ++output) {
            int source = clamp(settings.outputSources[output], 0, 9);
            aux[output] = !running ? 0.f : source < 2 ? tracks[source].cv : modulation.outputs[source - 2];
        }
    }

private:
    bool firstTick = true;
    bool previousExternal = false;
    bool haveExternalEdge = false;
    double phase = 0.0;
    double externalElapsed = 0.0;
    double externalPeriod = 0.125;
    double clockRemaining = 0.0;
    double resetIgnore = 0.0;
    RandomSource random[2];

    bool muted(int t) const {
        return settings.tracks[t].mute != (modulation.routes[t][ModMute] >= 1.5f);
    }

    bool nextStage(int t) {
        auto& state = tracks[t];
        const auto& config = settings.tracks[t];
        int length = clamp(config.length, 1, 8);
        if (state.windowOffset != config.offset) {
            state.playOrder.reset();
            state.windowOffset = config.offset;
        }
        std::array<bool, 8> enabled{};
        for (int i = 0; i < length; ++i) enabled[i] = !config.stages[modulo(i + config.offset, 8)].skip;
        state.position = state.playOrder.next(length, config.order, config.reversed, enabled, random[t]);
        state.stage = state.position < 0 ? -1 : modulo(state.position + config.offset, 8);
        return state.stage >= 0;
    }

    float stagePitch(int t) const {
        const auto& state = tracks[t];
        int base = nearestDegree(clamp(settings.stages[state.stage].pitch, 0.f, 1.f)
            * 12.f * clamp(settings.octaves, 1, 4) + modulation.routes[t][ModPitchPre] * 12.f, settings.scale);
        int note = degreeToSemitone(base + state.activeAccumulation, settings.scale);
        const auto& route = modulation.routes[t];
        return (note + settings.root + settings.tracks[t].transpose + std::round(route[ModPitchPost] * 12.f)) / 12.f
            + std::trunc(route[ModOctave]) + route[ModPitchOffset];
    }

    void accumulate(int t) {
        auto& state = tracks[t];
        const auto& config = settings.tracks[t];
        const auto& edit = config.stages[state.stage];
        state.activeAccumulation = state.accumulator[state.stage];
        state.accumulator[state.stage] = advanceAccumulator(state.activeAccumulation,
            clamp(edit.accumulation, -28, 28), config.accumLimit, config.accumNegativeLimit,
            config.accumBipolar, config.accumOrder, state.accumDirection[state.stage], random[t]);
        state.startPitch = state.pitch;
        state.slideAge = 0.0;
        state.slideDuration = edit.slide && state.hasPitch
            ? clamp(config.slideTime, 0.f, 1.f) * state.duration : 0.0;
    }

    void trackTick(int t, double duration) {
        auto& state = tracks[t];
        const auto& config = settings.tracks[t];
        if (config.pulseLimit > 0 && state.totalPulses >= clamp(config.pulseLimit, 1, 64)) {
            auto accumulator = state.accumulator;
            auto accumDirection = state.accumDirection;
            float pitch = state.pitch;
            bool hasPitch = state.hasPitch;
            int divider = state.divider;
            state = TrackState();
            state.pitch = state.targetPitch = state.startPitch = pitch;
            state.hasPitch = hasPitch;
            state.divider = divider;
            if (!config.autoAccumReset) { state.accumulator = accumulator; state.accumDirection = accumDirection; }
        }
        ++state.totalPulses;
        // Auto sequences can run indefinitely without overflowing a counter.
        if (config.pulseLimit <= 0) state.totalPulses = 0;
        state.duration = duration;
        state.age = 0.0;
        bool newStage = state.stage < 0 || state.position >= clamp(config.length, 1, 8)
            || (state.stage >= 0 && (config.stages[state.stage].skip
                || state.stage != modulo(state.position + config.offset, 8)));
        if (!newStage) {
            ++state.pulse;
            newStage = state.pulse >= clamp(settings.stages[state.stage].pulses, 1, 8);
        }
        if (newStage) {
            // A length/offset edit starts a new traversal if the old position
            // is no longer part of the selected stage window.
            if (state.stage >= 0 && state.stage != modulo(state.position + config.offset, 8))
                state.position = -1;
            if (!nextStage(t)) { state.gate = 0.f; state.cv = 0.f; return; }
            state.pulse = 0;
            state.lastRatchetEvent = -1;
            const auto& edit = config.stages[state.stage];
            state.activeAccumulation = state.accumulator[state.stage];
            state.startPitch = state.pitch;
            state.slideAge = 0.0;
            state.slideDuration = edit.slide && state.hasPitch
                ? clamp(config.slideTime, 0.f, 1.f) * duration : 0.0;
        }
        const auto& edit = config.stages[state.stage];
        if (newStage || edit.probabilityPerPulse) {
            float probability = clamp(edit.probability + modulation.routes[t][ModProbability] * 20.f, 0.f, 100.f);
            state.allowed = probability >= 100.f || (probability > 0.f && random[t].percent() < probability);
        }
        const Gate gate = settings.stages[state.stage].gate;
        if (state.allowed && gate != Rest && !muted(t)) {
            if ((edit.accumTrigger == AccumStage && newStage)
                || (edit.accumTrigger == AccumPulse && (newStage || gate == Multi))) accumulate(t);
        }
        if (state.allowed && settings.stages[state.stage].gate != Rest && !muted(t)) {
            state.targetPitch = stagePitch(t);
            state.hasPitch = true;
        }
    }

    void render(int t, double dt) {
        auto& state = tracks[t];
        const auto& config = settings.tracks[t];
        if (state.stage < 0) { state.gate = 0.f; state.cv = 0.f; return; }
        const auto& panel = settings.stages[state.stage];
        const auto& edit = config.stages[state.stage];
        int eventId = -1;
        float gateLength = clamp(config.gateLength + modulation.routes[t][ModGateLength] * .2f, 0.f, .99f);
        int ratchets = clamp(edit.ratchets + static_cast<int>(std::round(modulation.routes[t][ModRatchet] * 1.4f)), 1, 8);
        bool gate = !muted(t) && gateLength > 0.f && state.allowed && gateHigh(panel.gate, state.pulse,
            state.age / state.duration, clamp(panel.pulses, 1, 8), clamp(config.pulseDivision, 1, 8),
            config.gateStretch, config.ratchetMode, ratchets,
            std::max(gateLength, .01f), state.duration, &eventId);
        if (gate && edit.accumTrigger == AccumRatchet && eventId != state.lastRatchetEvent) {
            accumulate(t);
            state.lastRatchetEvent = eventId;
        }
        if (state.allowed && panel.gate != Rest && !muted(t))
            state.targetPitch = stagePitch(t);
        double mix = state.slideDuration > 0.0 ? std::min(1.0, state.slideAge / state.slideDuration) : 1.0;
        state.pitch = static_cast<float>(state.startPitch + (state.targetPitch - state.startPitch) * mix);
        state.slideAge += dt;
        state.cv = muted(t) ? 0.f : clamp(edit.cv, -5.f, 5.f);
        state.gate = gate ? 5.f : 0.f;
        state.age += dt;
    }
};
} // namespace metro
