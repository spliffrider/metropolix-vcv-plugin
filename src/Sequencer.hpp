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
#include "Controls.hpp"
#include "SwingTiming.hpp"

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
    int pitchOverride = -1; // Chromatic anchor, C0..C5; -1 follows the slider.
    int gateOverride = -1; // -1 follows track, 0 Rest, 1..99%, 100 Hold.
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
    float gateScale = 1.f;
    float slideTime = 0.25f;
    float swing = 50.f;
    int accumLimit = 9;
    int accumNegativeLimit = 9;
    bool accumBipolar = false;
    AccumOrder accumOrder = AccumWrap;
    AccumMode accumMode = StageAccumulation;
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
    std::array<Control,2> controls;
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
    bool skipInverted = false;
    PlayOrder playOrder;
    std::array<int, 8> accumulator{};
    std::array<int, 8> accumDirection{{1,1,1,1,1,1,1,1}};
    int trackAccumulator = 0;
    int trackAccumulation = 0;
    int trackAccumDirection = 1;
    AccumMode accumMode = StageAccumulation;
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

struct LoopyRequest {
    unsigned stages = 0; // Held boundaries; zero releases the performance overlay.
    unsigned targets = 1;
    int pulses = 0; // Auto, or 1..64 foreground track pulses.
};

class Sequencer {
public:
    Settings settings;
    std::array<TrackState, 2> tracks;
    LoopyRequest loopy;
    std::array<TrackState, 2> loopTracks, playerTracks;
    std::array<bool, 2> loopPlaying{{false,false}}, playerPlaying{{false,false}};
    // The normal track remains the transport/chain clock and advances underneath.
    const TrackState& sounding(int t) const {
        if (loopy.stages && (loopy.targets & (1u<<t))) {
            if (running && loopPlaying[t]) return loopTracks[t];
            if (!running && playerPlaying[t]) return playerTracks[t];
        }
        return tracks[t];
    }
    Modulation modulation;
    ControlRoutes controls;
    std::array<float, 2> aux{};
    bool running = false;
    bool resumeOnRun = false;
    unsigned chainMutes = 0;
    float clock = 0.f;

    void seed(uint32_t value) {
        random[0].state = value ? value : 1;
        random[1].state = value ^ 0x9e3779b9u;
        if (!random[1].state) random[1].state = 1;
        loopRandom[0].state=value^0xa511e9b3u;
        loopRandom[1].state=value^0x63d83595u;
        for(auto& source:loopRandom)if(!source.state)source.state=1;
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
        loopTracks = tracks; playerTracks = tracks;
        loopPlaying.fill(false); playerPlaying.fill(false);
        loopMasks.fill(0); playerMasks.fill(0); voiceMode=0;
        phase = 0.0;
        firstTick = true;
        clockRemaining = 0.0;
        clock = 0.f;
        modulation.reset();
        for (auto& timing : swingTiming) timing.reset();
        aux.fill(0.f);
    }

    // Restart all musical state on a clock that has already been detected.
    // Keep fractional phase and measured external tempo; the current edge
    // plays the new first note, without adding or discarding a clock pulse.
    void restartAtClock() {
        double savedPhase = phase;
        reset();
        phase = savedPhase;
    }

    // One external edge is one sixteenth-note pulse in this first version.
    void process(double dt, bool run, bool external, bool edge, bool resetEdge) {
        process(dt, run, external, edge, resetEdge, [] {});
    }
    template <typename BeforeClock>
    void process(double dt, bool run, bool external, bool edge, bool resetEdge, BeforeClock beforeClock) {
        dt = clamp(dt, 1e-7, 0.1);
        controls.read(settings.controls);
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
            if (running && !resumeOnRun) reset();
        }
        if (resetEdge) {
            reset();
            resetIgnore = 0.001;
        }

        double period = external ? externalPeriod : internalPeriod;
        bool tick = false;
        if (running) {
            if (external) {
                tick = edge && resetIgnore <= 0.0;
            } else if (firstTick || phase >= period) {
                tick = true;
                phase = firstTick ? 0.0 : std::fmod(phase, period);
            }
            if (tick) {
                beforeClock();
                controls.read(settings.controls);
                // A recalled direct CTRL assignment can change internal tempo.
                // The first note's gate/slide timing uses that new tempo too.
                if (!external) period = 15.0 / clamp(settings.bpm, 0.1f, 400.f);
                modulation.tick(settings.modLanes, period);
            }
            modulation.render(settings.modLanes, dt, settings.auxInputs);
            // Pending track pulses use the same sample clock as gates/slides.
            // Reset/recall above cancels them before any new pattern is played.
            for (int t = 0; t < 2; ++t)
                if (swingTiming[t].advance(dt)) performTick(t, swingTiming[t].duration);
            if (tick) {
                firstTick = false;
                clockRemaining = 0.001;
                for (int t = 0; t < 2; ++t) {
                    auto& state = tracks[t];
                    const auto& config = settings.tracks[t];
                    bool invert=controls.values[t][CtrlSkipInvert]>0;
                    if(state.skipInverted!=invert) {
                        // Restart traversal on the next base clock, preserving
                        // accumulated offsets. No synthetic clock/gate is emitted.
                        state.skipInverted=invert;state.stage=state.position=-1;
                        state.pulse=state.divider=state.totalPulses=0;state.playOrder.reset();
                    }
                    int division = clamp(config.division, 1, 64);
                    if (state.divider == 0 || state.divider >= division) {
                        auto& timing = swingTiming[t];
                        if (timing.pending) {
                            timing.pending = false;
                            performTick(t, timing.duration);
                        }
                        float amount = swingPercent(config.swing + modulation.routes[t][ModSwing] * 5.6f);
                        if (timing.clock(period * division, amount)) performTick(t, timing.duration);
                    }
                    state.divider = (state.divider + 1) % division;
                }
            }
            for (int t = 0; t < 2; ++t) {
                render(t, dt);
                if (!requested(t)) loopPlaying[t]=false;
                if (loopPlaying[t]) { voiceMode=1; render(t,dt); voiceMode=0; }
                playerPlaying[t]=false; playerMasks[t]=0;
            }
            phase += dt;
        } else {
            for (auto& state : tracks) { state.gate = 0.f; state.cv = 0.f; }
            modulation.silence();
            // External pitch routing is available to stopped Stage Player notes.
            if(loopy.stages & 255u)modulation.render(settings.modLanes, 0.0, settings.auxInputs);
            modulation.outputs.fill(0.f);
            stagePlayer();
            clockRemaining = 0.0;
        }
        clock = running && clockRemaining > 0.0 ? 5.f : 0.f;
        clockRemaining -= dt;
        resetIgnore = std::max(0.0, resetIgnore - dt);
        for (int output = 0; output < 2; ++output) {
            int source = clamp(settings.outputSources[output], 0, 9);
            if(controls.swap && source<2)source=1-source;
            aux[output] = source < 2 ? sounding(source).cv : running ? modulation.outputs[source - 2] : 0.f;
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
    RandomSource random[2], loopRandom[2];
    std::array<float,2> playerCv{};
    int voiceMode=0;
    std::array<unsigned,2> loopMasks{}, playerMasks{};
    std::array<int,2> loopCounts{}, loopLimits{};
    TrackState& voice(int t) { return voiceMode==1?loopTracks[t]:voiceMode==2?playerTracks[t]:tracks[t]; }
    const TrackState& voice(int t) const { return voiceMode==1?loopTracks[t]:voiceMode==2?playerTracks[t]:tracks[t]; }
    RandomSource& dice(int t) { return voiceMode?loopRandom[t]:random[t]; }
    bool requested(int t) const {return (loopy.stages & 255u) && (loopy.targets & (1u<<t));}
    int lower() const {for(int s=0;s<8;++s)if(loopy.stages&(1u<<s))return s;return 0;}
    int upper() const {for(int s=7;s>=0;--s)if(loopy.stages&(1u<<s))return s;return 0;}
    bool reverse(int t) const {
        float direction=controls.values[t][CtrlDirection];
        return direction==0?settings.tracks[t].reversed:direction<0;
    }
    void stagePlayer() {
        for(int t=0;t<2;++t) {
            loopPlaying[t]=false; loopMasks[t]=0;
            if(!requested(t)) {playerPlaying[t]=false;playerMasks[t]=0;continue;}
            if(!playerPlaying[t] || playerMasks[t]!=(loopy.stages&255u)) {
                auto& state=playerTracks[t];state=tracks[t];state.stage=lower();
                state.activeAccumulation=state.accumMode==TrackAccumulation?state.trackAccumulation:state.accumulator[state.stage];
                voiceMode=2;state.pitch=state.targetPitch=stagePitch(t);voiceMode=0;
                state.cv=playerCv[t]=clamp(settings.tracks[t].stages[state.stage].cv,-5.f,5.f);
                playerMasks[t]=loopy.stages&255u;playerPlaying[t]=true;
            }
            // Freeze the note/CV until a fresh stage press; Gate Type is ignored.
            playerTracks[t].gate=muted(t)?0.f:5.f;
            playerTracks[t].cv=muted(t)?0.f:playerCv[t];
        }
    }
    void performTick(int t,double duration) {
        trackTick(t,duration);
        if(!requested(t)){loopPlaying[t]=false;loopMasks[t]=0;return;}
        voiceMode=1;
        if(!loopPlaying[t] || loopMasks[t]!=(loopy.stages&255u)) {
            loopTracks[t]=tracks[t];loopTracks[t].stage=loopTracks[t].position=-1;
            loopTracks[t].pulse=0;loopTracks[t].lastRatchetEvent=-1;
            loopCounts[t]=0;loopMasks[t]=loopy.stages&255u;
        }
        loopPlaying[t]=true;
        loopLimits[t]=clamp(loopy.pulses,0,64);
        trackTick(t,duration);voiceMode=0;
    }
    std::array<SwingTiming, 2> swingTiming;

    bool muted(int t) const {
        return (chainMutes & (1u<<t)) || (settings.tracks[t].mute != (modulation.routes[t][ModMute] >= 1.5f));
    }
    bool skipped(int t,int stage) const {
        return settings.tracks[t].stages[stage].skip != voice(t).skipInverted;
    }
    int pulses(int t,int stage) const {
        return clamp(settings.stages[stage].pulses+static_cast<int>(std::round(controls.values[t][CtrlPulseCount]*7)),1,8);
    }

    bool nextStage(int t) {
        auto& state = voice(t);
        if(voiceMode==1) {
            int lo=lower(),hi=upper();bool bounded=lo!=hi || loopy.pulses==0;
            if(state.stage<0)state.stage=reverse(t)?hi:lo;
            else if(bounded)state.stage=reverse(t)?(state.stage<=lo?hi:state.stage-1):(state.stage>=hi?lo:state.stage+1);
            else state.stage=modulo(state.stage+(reverse(t)?-1:1),8);
            state.position=state.stage;
            return true; // Loopy ignores Skip, Length, Offset and play order.
        }
        const auto& config = settings.tracks[t];
        int length = clamp(config.length, 1, 8);
        if (state.windowOffset != config.offset) {
            state.playOrder.reset();
            state.windowOffset = config.offset;
        }
        std::array<bool, 8> enabled{};
        for (int i = 0; i < length; ++i) enabled[i] = !skipped(t,modulo(i + config.offset, 8));
        float direction=controls.values[t][CtrlDirection];
        bool reversed=direction==0?config.reversed:direction<0;
        state.position = state.playOrder.next(length, config.order, reversed, enabled, dice(t));
        state.stage = state.position < 0 ? -1 : modulo(state.position + config.offset, 8);
        return state.stage >= 0;
    }

    float stagePitch(int t) const {
        const auto& state = voice(t);
        const auto& ctrl=controls.values[t];
        float slider=clamp(settings.stages[state.stage].pitch,0.f,1.f);
        if(ctrl[CtrlSliderInvert]>0)slider=1-slider;
        const int overrideNote=settings.tracks[t].stages[state.stage].pitchOverride;
        float source=overrideNote<0?slider*12.f*clamp(settings.octaves,1,4):float(clamp(overrideNote,0,60));
        int base = nearestDegree(source
            + (modulation.routes[t][ModPitchPre]+ctrl[CtrlPitchPre])*12.f, settings.scale);
        int note = degreeToSemitone(base + state.activeAccumulation, settings.scale);
        const auto& route = modulation.routes[t];
        return (note + settings.root + settings.tracks[t].transpose + std::round(ctrl[CtrlRoot]*12.f)
            + std::round((route[ModPitchPost]+ctrl[CtrlPitchPost])*12.f)) / 12.f
            + std::trunc(route[ModOctave]) + std::round(ctrl[CtrlOctave]*2.f) + route[ModPitchOffset]+ctrl[CtrlPitchOffset];
    }

    void accumulate(int t) {
        auto& state = voice(t);
        const auto& config = settings.tracks[t];
        const auto& edit = config.stages[state.stage];
        const int amount = clamp(edit.accumulation, -28, 28)*(controls.values[t][CtrlAccumPolar]<0?-1:1);
        if (state.accumMode == TrackAccumulation) {
            // Zero-amount stages inherit the sounding track offset. They must
            // not consume the next offset: p127's first C D E F pass is neutral.
            if (amount != 0) {
                state.trackAccumulation = state.trackAccumulator;
                state.trackAccumulator = advanceAccumulator(state.trackAccumulation,
                    amount, config.accumLimit, config.accumNegativeLimit,
                    config.accumBipolar, config.accumOrder, state.trackAccumDirection, dice(t));
            }
            state.activeAccumulation = state.trackAccumulation;
        } else {
            state.activeAccumulation = state.accumulator[state.stage];
            state.accumulator[state.stage] = advanceAccumulator(state.activeAccumulation,
                amount, config.accumLimit, config.accumNegativeLimit,
                config.accumBipolar, config.accumOrder, state.accumDirection[state.stage], dice(t));
        }
        state.startPitch = state.pitch;
        state.slideAge = 0.0;
        state.slideDuration = edit.slide && state.hasPitch
            ? clamp(config.slideTime, 0.f, 1.f) * state.duration : 0.0;
    }

    void trackTick(int t, double duration) {
        auto& state = voice(t);
        const auto& config = settings.tracks[t];
        if(voiceMode==1 && loopLimits[t]>0 && loopCounts[t]>=loopLimits[t]) {
            state.stage=state.position=-1;state.pulse=0;loopCounts[t]=0;
        }
        if (voiceMode==0 && config.pulseLimit > 0 && state.totalPulses >= clamp(config.pulseLimit, 1, 64)) {
            auto accumulator = state.accumulator;
            auto accumDirection = state.accumDirection;
            int trackAccumulator = state.trackAccumulator;
            int trackAccumulation = state.trackAccumulation;
            int trackAccumDirection = state.trackAccumDirection;
            float pitch = state.pitch;
            bool hasPitch = state.hasPitch;
            int divider = state.divider;
            bool skipInverted=state.skipInverted;
            state = TrackState();
            state.pitch = state.targetPitch = state.startPitch = pitch;
            state.hasPitch = hasPitch;
            state.divider = divider;
            state.skipInverted=skipInverted;
            if (!config.autoAccumReset) {
                state.accumulator = accumulator; state.accumDirection = accumDirection;
                state.trackAccumulator = trackAccumulator;
                state.trackAccumulation = trackAccumulation;
                state.trackAccumDirection = trackAccumDirection;
            }
        }
        // Apply mode edits at a track-clock boundary, keeping both banks until
        // reset. A mode switch cannot create an extra ratchet halfway through a pulse.
        if (state.accumMode != config.accumMode) {
            state.accumMode = config.accumMode;
            state.activeAccumulation = state.accumMode == TrackAccumulation
                ? state.trackAccumulation : (state.stage >= 0 ? state.accumulator[state.stage] : 0);
        }
        ++state.totalPulses;
        if(voiceMode==1)loopCounts[t]=loopLimits[t]>0?loopCounts[t]+1:0;
        // Auto sequences can run indefinitely without overflowing a counter.
        if (config.pulseLimit <= 0 || voiceMode==1) state.totalPulses = 0;
        state.duration = duration;
        state.age = 0.0;
        bool newStage = state.stage < 0 || (voiceMode==0 && (state.position >= clamp(config.length, 1, 8)
            || (state.stage >= 0 && (skipped(t,state.stage)
                || state.stage != modulo(state.position + config.offset, 8)))));
        if (!newStage) {
            ++state.pulse;
            newStage = state.pulse >= pulses(t,state.stage);
        }
        if (newStage) {
            // A length/offset edit starts a new traversal if the old position
            // is no longer part of the selected stage window.
            if (voiceMode==0 && state.stage >= 0 && state.stage != modulo(state.position + config.offset, 8))
                state.position = -1;
            if (!nextStage(t)) { state.gate = 0.f; state.cv = 0.f; return; }
            state.pulse = 0;
            state.lastRatchetEvent = -1;
            const auto& edit = config.stages[state.stage];
            state.activeAccumulation = state.accumMode == TrackAccumulation
                ? state.trackAccumulation : state.accumulator[state.stage];
            state.startPitch = state.pitch;
            state.slideAge = 0.0;
            state.slideDuration = edit.slide && state.hasPitch
                ? clamp(config.slideTime, 0.f, 1.f) * duration : 0.0;
        }
        const auto& edit = config.stages[state.stage];
        if (newStage || edit.probabilityPerPulse) {
            float probability = clamp(edit.probability + modulation.routes[t][ModProbability] * 20.f + controls.values[t][CtrlProbability]*100.f, 0.f, 100.f);
            state.allowed = probability >= 100.f || (probability > 0.f && dice(t).percent() < probability);
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
        auto& state = voice(t);
        const auto& config = settings.tracks[t];
        if (state.stage < 0) { state.gate = 0.f; state.cv = 0.f; return; }
        const auto& panel = settings.stages[state.stage];
        const auto& edit = config.stages[state.stage];
        int eventId = -1;
        // Stage Rest affects only the output, never the accumulator clock.
        // Preserve the unoverridden ratchet events for its accumulation.
        float inherited=clamp(config.gateLength + modulation.routes[t][ModGateLength]*.2f,0.f,config.gateLength>=1.f?1.f:.99f);
        // Keep pre-2.10 modulation's 1% positive minimum, then apply Scale.
        if(inherited>0.f)inherited=std::max(.01f,inherited);
        float source=edit.gateOverride<0?inherited:clamp(edit.gateOverride,0,100)*.01f;
        float gateLength=source*clamp(config.gateScale,0.f,1.f);
        float accumLength=inherited*clamp(config.gateScale,0.f,1.f);
        int ratchets = clamp(edit.ratchets + static_cast<int>(std::round(modulation.routes[t][ModRatchet] * 1.4f + controls.values[t][CtrlRatchet]*7.f)), 1, 8);
        bool accumulationGate = !muted(t) && accumLength > 0.f && state.allowed && gateHigh(
            panel.gate==Rest?Rest:accumLength>=1.f?Hold:panel.gate, state.pulse,
            state.age / state.duration, pulses(t,state.stage), clamp(config.pulseDivision, 1, 8),
            config.gateStretch, config.ratchetMode, ratchets,
            accumLength, state.duration, &eventId);
        bool gate = edit.gateOverride==0?false:
            !muted(t) && gateLength>0.f && state.allowed && gateHigh(panel.gate==Rest?Rest:gateLength>=1.f?Hold:panel.gate,
                state.pulse,state.age/state.duration,pulses(t,state.stage),clamp(config.pulseDivision,1,8),
                config.gateStretch,config.ratchetMode,ratchets,gateLength,state.duration);
        if (accumulationGate && edit.accumTrigger == AccumRatchet && eventId != state.lastRatchetEvent) {
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
