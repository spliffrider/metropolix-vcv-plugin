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
#include "Scales.hpp"
#include "SlideTiming.hpp"
#include "StageCv.hpp"
#include "ClockOutput.hpp"

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
    StageCvRamp cvRamp = CvStep;
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
    int sliderOctaves = 0; // 0 follows the shared legacy range.
    bool sliderInverted = false;
    int velocitySource = 2; // 50%, 75%, 100%, or Track CV.
    int pulseSource = 0; // 0 shared switches, otherwise a uniform 1..8.
    int gateSource = 0; // Switch, Single, Single+Rest, Multi, Multi+Rest, Hold, Hold+Rest.
    bool restPitchUpdate = false;
    float gateLength = 0.5f;
    float gateScale = 1.f;
    float slideTime = 0.25f;
    SlideType slideType = TempoSlide;
    TrackCvType cvType = BipolarCv;
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
    int clockOutDivision = 1, clockOutOffset = 0;
    int root = 0;
    int scale = 1;
    int lockedScale = -1;
    std::array<UserScale,UserScaleCount> userScales;
    int octaves = 3;
    std::array<Stage, 8> stages;
    std::array<Track, 2> tracks;
    std::array<ModLane, 8> modLanes;
    std::array<AuxInput, 3> auxInputs;
    std::array<Control,2> controls;
    std::array<int, 2> outputSources{{0,1}}; // Stable IDs decoded by OutputRouting.
};

struct TrackState {
    TrackOutputEvents outputEvents;
    int stage = -1;
    int position = -1;
    int pulse = 0;
    int divider = 0;
    int totalPulses = 0;
    int windowOffset = -1;
    bool skipInverted = false;
    PlayOrder playOrder;
    std::array<int, 8> accumulator{};
    std::array<int,8> accumulatorPlayed{}; // Last emitted offset, distinct from the next counter.
    std::array<bool,8> accumulatorHasPlayed{}, accumulatorReversed{}, accumulatorInverted{};
    std::array<int,8> accumulatorPlayedDirection{{1,1,1,1,1,1,1,1}};
    std::array<int, 8> accumDirection{{1,1,1,1,1,1,1,1}};
    int trackAccumulator = 0;
    int trackAccumulation = 0;
    int trackAccumDirection = 1;
    int trackPlayedDirection = 1;
    bool trackAccumulatorHasPlayed = false, trackAccumulatorReversed = false, trackAccumulatorInverted = false;
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
    SlideType slideType = TempoSlide;
    bool slideActive = false, slideFromPrevious = false;
    float gate = 0.f;
    float cv = 0.f;
    float cvValue = 0.f, cvFrom = 0.f;
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
    bool swapped() const {
        if(controls.swap || modulation.routes[0][ModOutputSwap]>=1.5f || modulation.routes[1][ModOutputSwap]>=1.5f)return true;
        // The global external gate also works while the transport is stopped.
        for(const auto& input:settings.auxInputs)if(input.destination==ModOutputSwap && input.effectiveVoltage()>=1.5f)return true;
        return false;
    }
    int sliderOctaves(int t) const {return clamp(settings.tracks[t].sliderOctaves?settings.tracks[t].sliderOctaves:settings.octaves,1,4);}
    float sliderPosition(int t,int stage,bool orient=true) const {
        if(stage<0 || stage>7)return 0;
        float value=clamp(settings.stages[stage].pitch,0.f,1.f);
        float invert=controls.values[t][CtrlSliderInvert];
        if(orient && (settings.tracks[t].sliderInverted!=(invert>0)))value=1-value;
        return value;
    }
    float velocity(int t) const {
        const auto& state=sounding(t);int source=clamp(settings.tracks[t].velocitySource,0,3);
        const float levels[]={2.5f,3.75f,5.f};
        float base=source==3?state.cv:levels[source];
        return clamp(base+modulation.routes[t][ModVelocity]+controls.values[t][CtrlVelocity]*5.f,0.f,5.f);
    }
    float outputVoltage(int source) const {
        auto selection=outputSelection(source);int t=selection.target;
        if(outputIsTrack(selection.kind) && swapped())t=1-t;
        const auto& state=sounding(t<2?t:0);
        switch(selection.kind){
            case OutCtrl1Bipolar: return ctrlClamp(settings.controls[0].value)*5.f;
            case OutCtrl1Unipolar: return (ctrlClamp(settings.controls[0].value)+1)*2.5f;
            case OutCtrl2Bipolar: return ctrlClamp(settings.controls[1].value)*5.f;
            case OutCtrl2Unipolar: return (ctrlClamp(settings.controls[1].value)+1)*2.5f;
            case OutTrackCv: return state.cv;
            case OutFirstPulse: return tracks[t].outputEvents.values[TrackOutputEvents::First];
            case OutLastPulse: return tracks[t].outputEvents.values[TrackOutputEvents::Last];
            case OutModClock: return running?modulation.clocks[t]:0.f;
            case OutModCv: return running?modulation.outputs[t]:0.f;
            case OutNewStage: return state.outputEvents.values[TrackOutputEvents::NewStage];
            case OutReset: return resetOutput;
            case OutRoot: return (scaleRoot(t)+std::round(controls.values[t][CtrlRoot]*12.f))/12.f;
            case OutRun: return running?5.f:0.f;
            case OutSlider: return sliderPosition(t,state.stage,false)*5.f;
            case OutSliderRange: return sliderPosition(t,state.stage)*sliderOctaves(t);
            case OutVelocity: return velocity(t);
            default: return 0;
        }
    }
    int scaleIndex(int t) const {
        int index=clamp(settings.lockedScale>=0?settings.lockedScale:settings.scale,0,ScaleCount-1);
        return settings.lockedScale>=0?modulatedUserScale(index,
            modulation.routes[t][ModUserScale]+controls.values[t][CtrlUserScale]*5.f):index;
    }
    Scale currentScale(int t) const {
        int index=scaleIndex(t);return userScale(index)?scaleFromMask(settings.userScales[index-FactoryScaleCount].mask):scaleAt(index);
    }
    int scaleRoot(int t) const {
        int index=scaleIndex(t);return userScale(index)?clamp(settings.userScales[index-FactoryScaleCount].root,0,11):settings.root;
    }

    void seed(uint32_t value) {
        random[0].state = value ? value : 1;
        random[1].state = value ^ 0x9e3779b9u;
        if (!random[1].state) random[1].state = 1;
        loopRandom[0].state=value^0xa511e9b3u;
        loopRandom[1].state=value^0x63d83595u;
        for(auto& source:loopRandom)if(!source.state)source.state=1;
        modulation.seed(value);
    }

    void reset(bool manual=false) {
        pendingAccumReset.fill(false);accumResetHigh.fill(false);
        for (auto& state : tracks) {
            // CV pitch holds on reset until the next played note.
            float pitch = state.pitch;
            state = TrackState();
            state.pitch = pitch;
            state.targetPitch = pitch;
            if(manual){state.outputEvents.pulses[TrackOutputEvents::First].fire();state.outputEvents.firstDue=false;}
        }
        loopTracks = tracks; playerTracks = tracks;
        loopPlaying.fill(false); playerPlaying.fill(false);
        loopMasks.fill(0); playerMasks.fill(0); voiceMode=0;
        phase = 0.0;
        firstTick = true;
        clockOutput.reset();
        clock = 0.f;
        resetPulse.fire();
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
            if(!running){for(auto& state:tracks)state.outputEvents.clear();for(auto& state:loopTracks)state.outputEvents.clear();for(auto& state:playerTracks)state.outputEvents.clear();}
        }
        if (resetEdge) {
            reset(true);
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
            readAccumulatorReset();
            // Pending track pulses use the same sample clock as gates/slides.
            // Reset/recall above cancels them before any new pattern is played.
            for (int t = 0; t < 2; ++t)
                if (swingTiming[t].advance(dt)) performTick(t, swingTiming[t].duration);
            if (tick) {
                firstTick = false;
                clockOutput.tick(settings.clockOutDivision,settings.clockOutOffset);
                for (int t = 0; t < 2; ++t) {
                    auto& state = tracks[t];
                    const auto& config = settings.tracks[t];
                    bool invert=controls.values[t][CtrlSkipInvert]>0;
                    if(state.skipInverted!=invert) {
                        // Restart traversal on the next base clock, preserving
                        // accumulated offsets. No synthetic clock/gate is emitted.
                        state.skipInverted=invert;state.stage=state.position=-1;
                        state.pulse=state.divider=state.totalPulses=0;state.playOrder.reset();
                        state.outputEvents.firstDue=true;
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
            readAccumulatorReset(true);
            // External pitch routing is available to stopped Stage Player notes.
            if(loopy.stages & 255u)modulation.render(settings.modLanes, 0.0, settings.auxInputs);
            modulation.outputs.fill(0.f);
            stagePlayer(dt);
        }
        clock = clockOutput.render(dt,running);
        resetIgnore = std::max(0.0, resetIgnore - dt);
        resetOutput=resetPulse.render(dt);
        for(auto& state:tracks)state.outputEvents.render(dt);
        for(auto& state:loopTracks)state.outputEvents.render(dt);
        for(auto& state:playerTracks)state.outputEvents.render(dt);
        for (int output = 0; output < 2; ++output) {
            aux[output] = outputVoltage(settings.outputSources[output]);
        }
    }

private:
    bool firstTick = true;
    bool previousExternal = false;
    bool haveExternalEdge = false;
    double phase = 0.0;
    double externalElapsed = 0.0;
    double externalPeriod = 0.125;
    ClockOutput clockOutput;
    OutputPulse resetPulse;
    float resetOutput=0;
    double resetIgnore = 0.0;
    RandomSource random[2], loopRandom[2];
    std::array<float,2> playerCv{};
    int voiceMode=0;
    std::array<bool,2> pendingAccumReset{},accumResetHigh{};
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
    bool accumulationEnabled(int t) const {
        for(const auto& control:settings.controls)if(control.destination==CtrlAccumDir && control.targets(t) && controls.values[t][CtrlAccumDir]==0)return false;
        return !modulation.assigned[t][ModAccumulate] || modulation.routes[t][ModAccumulate]>=1.5f;
    }
    bool accumulationReverse(int t) const {return controls.values[t][CtrlAccumDir]<0 || modulation.routes[t][ModAccumReverse]>=1.5f;}
    AccumMode effectiveAccumMode(int t) const {return AccumMode(int(settings.tracks[t].accumMode)^(modulation.routes[t][ModAccumMode]>=1.5f));}
    void readAccumulatorReset(bool stopped=false) {
        for(int t=0;t<2;++t){float voltage=modulation.routes[t][ModAccumReset];
            if(stopped){voltage=0;for(const auto& input:settings.auxInputs)if(input.destination==ModAccumReset && (input.assignment==2 || input.assignment==t))voltage+=input.effectiveVoltage();}
            bool high=voltage>=1.5f;if(high && !accumResetHigh[t])pendingAccumReset[t]=true;accumResetHigh[t]=high;
        }
    }
    void clearAccumulator(int t) {
        for(auto* state:{&tracks[t],&loopTracks[t],&playerTracks[t]}){
            state->accumulator.fill(0);state->accumulatorPlayed.fill(0);state->accumDirection.fill(1);
            state->accumulatorHasPlayed.fill(false);state->accumulatorReversed.fill(false);state->accumulatorInverted.fill(false);state->accumulatorPlayedDirection.fill(1);
            state->trackAccumulator=state->trackAccumulation=state->activeAccumulation=0;state->trackAccumDirection=1;
            state->trackPlayedDirection=1;state->trackAccumulatorHasPlayed=state->trackAccumulatorReversed=state->trackAccumulatorInverted=false;
        }
    }
    void stagePlayer(double dt) {
        for(int t=0;t<2;++t) {
            loopPlaying[t]=false; loopMasks[t]=0;
            if(!requested(t)) {playerPlaying[t]=false;playerMasks[t]=0;continue;}
            if(!playerPlaying[t] || playerMasks[t]!=(loopy.stages&255u)) {
                auto& state=playerTracks[t];bool have=state.hasPitch || tracks[t].hasPitch;
                float from=playerPlaying[t]?state.pitch:tracks[t].pitch;
                state=tracks[t];state.stage=lower();
                state.outputEvents=TrackOutputEvents();state.outputEvents.firstDue=false;
                state.activeAccumulation=state.accumMode==TrackAccumulation?state.trackAccumulation:accumulationEnabled(t)?state.accumulator[state.stage]:state.accumulatorPlayed[state.stage];
                voiceMode=2;state.targetPitch=stagePitch(t);voiceMode=0;
                const auto& config=settings.tracks[t];
                state.slideDuration=config.slideType==AnalogSlide && config.stages[state.stage].slide && have?analogSlideTime(config.slideTime):0;
                state.pitch=state.slideDuration>0?from:state.targetPitch;state.hasPitch=true;
                state.cv=playerCv[t]=trackCvVoltage(settings.tracks[t].stages[state.stage].cv,config.cvType);
                playerMasks[t]=loopy.stages&255u;playerPlaying[t]=true;
            }
            // Freeze the note/CV until a fresh stage press; Gate Type is ignored.
            playerTracks[t].pitch=slewPitch(playerTracks[t].pitch,playerTracks[t].targetPitch,dt,playerTracks[t].slideDuration);
            playerTracks[t].gate=muted(t)?0.f:5.f;
            playerTracks[t].cv=muted(t)?0.f:playerCv[t];
        }
    }
    void performTick(int t,double duration) {
        if(pendingAccumReset[t]){clearAccumulator(t);pendingAccumReset[t]=false;}
        trackTick(t,duration);
        if(!requested(t)){loopPlaying[t]=false;loopMasks[t]=0;return;}
        voiceMode=1;
        if(!loopPlaying[t] || loopMasks[t]!=(loopy.stages&255u)) {
            loopTracks[t]=tracks[t];loopTracks[t].stage=loopTracks[t].position=-1;
            loopTracks[t].pulse=0;loopTracks[t].lastRatchetEvent=-1;
            loopTracks[t].outputEvents=TrackOutputEvents();
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
        int source=settings.tracks[t].pulseSource;
        return clamp((source?clamp(source,1,8):settings.stages[stage].pulses)+static_cast<int>(std::round(controls.values[t][CtrlPulseCount]*7)),1,8);
    }
    Gate panelGate(int t,int stage) const {
        Gate gate=settings.stages[stage].gate;
        int source=clamp(settings.tracks[t].gateSource,0,6);
        if(source && !(source%2==0 && gate==Rest))gate=source<=2?Single:source<=4?Multi:Hold;
        return settings.tracks[t].slideType==AcidSlide && settings.tracks[t].stages[stage].slide?Hold:gate;
    }
    void beginSlide(int t) {
        auto& state=voice(t);const auto& config=settings.tracks[t];
        state.startPitch=state.pitch;state.slideAge=0;
        state.slideType=config.slideType;
        state.slideActive=state.hasPitch && (config.slideType==AcidSlide?state.slideFromPrevious:config.stages[state.stage].slide);
        state.slideDuration=state.slideActive && config.slideType==TempoSlide?slideAmount(config.slideTime)*state.duration:0;
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
        float slider=sliderPosition(t,state.stage);
        const int overrideNote=settings.tracks[t].stages[state.stage].pitchOverride;
        float source=overrideNote<0?slider*12.f*sliderOctaves(t):float(clamp(overrideNote,0,60));
        Scale scale=currentScale(t);
        int base = nearestDegree(source
            + (modulation.routes[t][ModPitchPre]+ctrl[CtrlPitchPre])*12.f, scale);
        int note = degreeToSemitone(base + state.activeAccumulation, scale);
        const auto& route = modulation.routes[t];
        return (note + scaleRoot(t) + settings.tracks[t].transpose + std::round(ctrl[CtrlRoot]*12.f)
            + std::round((route[ModPitchPost]+ctrl[CtrlPitchPost])*12.f)) / 12.f
            + std::trunc(route[ModOctave]) + std::round(ctrl[CtrlOctave]*2.f) + route[ModPitchOffset]+ctrl[CtrlPitchOffset];
    }

    void accumulate(int t) {
        if(!accumulationEnabled(t))return;
        auto& state = voice(t);
        const auto& config = settings.tracks[t];
        const auto& edit = config.stages[state.stage];
        bool invert=(controls.values[t][CtrlAccumPolar]<0)!=(modulation.routes[t][ModAccumInvert]>=1.5f);
        const int amount = clamp(edit.accumulation, -28, 28)*(invert?-1:1);
        bool bipolar=config.accumBipolar!=(modulation.routes[t][ModAccumPolarity]>=1.5f);
        bool reverseAccum=accumulationReverse(t);
        const int low=bipolar || amount<0?-clamp(config.accumNegativeLimit,1,28):0;
        const int high=bipolar || amount>0?clamp(config.accumLimit,1,28):0;
        if (state.accumMode == TrackAccumulation) {
            // Zero-amount stages inherit the sounding track offset. They must
            // not consume the next offset: p127's first C D E F pass is neutral.
            if (amount != 0) {
                if(state.trackAccumulatorHasPlayed && (state.trackAccumulatorReversed!=reverseAccum || state.trackAccumulatorInverted!=invert)) {
                    state.trackAccumDirection=state.trackPlayedDirection;
                    state.trackAccumulator=advanceAccumulator(state.trackAccumulation,amount,config.accumLimit,config.accumNegativeLimit,bipolar,config.accumOrder,state.trackAccumDirection,dice(t),reverseAccum,state.trackAccumulatorInverted!=invert);
                }
                state.trackAccumulatorHasPlayed=true;state.trackAccumulatorReversed=reverseAccum;state.trackAccumulatorInverted=invert;
                state.trackPlayedDirection=state.trackAccumDirection;
                state.trackAccumulation = clamp(state.trackAccumulator,low,high);
                state.trackAccumulator = advanceAccumulator(state.trackAccumulation,
                    amount, config.accumLimit, config.accumNegativeLimit,
                    bipolar, config.accumOrder, state.trackAccumDirection, dice(t),reverseAccum);
            }
            state.activeAccumulation = state.trackAccumulation;
        } else {
            if(amount!=0 && state.accumulatorHasPlayed[state.stage] && (state.accumulatorReversed[state.stage]!=reverseAccum || state.accumulatorInverted[state.stage]!=invert)) {
                state.accumDirection[state.stage]=state.accumulatorPlayedDirection[state.stage];
                state.accumulator[state.stage]=advanceAccumulator(state.accumulatorPlayed[state.stage],amount,config.accumLimit,config.accumNegativeLimit,bipolar,config.accumOrder,state.accumDirection[state.stage],dice(t),reverseAccum,state.accumulatorInverted[state.stage]!=invert);
            }
            if(amount!=0)state.accumulatorHasPlayed[state.stage]=true;
            state.accumulatorReversed[state.stage]=reverseAccum;
            state.accumulatorInverted[state.stage]=invert;
            state.accumulatorPlayedDirection[state.stage]=state.accumDirection[state.stage];
            state.activeAccumulation = amount?clamp(state.accumulator[state.stage],low,high):state.accumulator[state.stage];
            state.accumulatorPlayed[state.stage]=state.activeAccumulation;
            state.accumulator[state.stage] = advanceAccumulator(state.activeAccumulation,
                amount, config.accumLimit, config.accumNegativeLimit,
                bipolar, config.accumOrder, state.accumDirection[state.stage], dice(t),reverseAccum);
        }
        beginSlide(t);
    }

    void trackTick(int t, double duration) {
        auto& state = voice(t);
        const auto& config = settings.tracks[t];
        int precedingStage=state.stage;
        if(voiceMode==1 && loopLimits[t]>0 && loopCounts[t]>=loopLimits[t]) {
            state.stage=state.position=-1;state.pulse=0;loopCounts[t]=0;
        }
        if (voiceMode==0 && config.pulseLimit > 0 && state.totalPulses >= clamp(config.pulseLimit, 1, 64)) {
            auto accumulator = state.accumulator;
            auto accumulatorPlayed=state.accumulatorPlayed;
            auto accumulatorHasPlayed=state.accumulatorHasPlayed, accumulatorReversed=state.accumulatorReversed, accumulatorInverted=state.accumulatorInverted;
            auto accumulatorPlayedDirection=state.accumulatorPlayedDirection;
            auto accumDirection = state.accumDirection;
            int trackAccumulator = state.trackAccumulator;
            int trackAccumulation = state.trackAccumulation;
            int trackAccumDirection = state.trackAccumDirection;
            int trackPlayedDirection=state.trackPlayedDirection;
            bool trackAccumulatorHasPlayed=state.trackAccumulatorHasPlayed,trackAccumulatorReversed=state.trackAccumulatorReversed,trackAccumulatorInverted=state.trackAccumulatorInverted;
            float pitch = state.pitch;
            float cvValue=state.cvValue;
            bool hasPitch = state.hasPitch;
            int divider = state.divider;
            bool skipInverted=state.skipInverted;
            state = TrackState();
            state.pitch = state.targetPitch = state.startPitch = pitch;
            state.hasPitch = hasPitch;
            state.cvValue=cvValue;
            state.divider = divider;
            state.skipInverted=skipInverted;
            if (!config.autoAccumReset) {
                state.accumulator = accumulator; state.accumulatorPlayed=accumulatorPlayed;state.accumDirection = accumDirection;
                state.accumulatorHasPlayed=accumulatorHasPlayed;state.accumulatorReversed=accumulatorReversed;state.accumulatorPlayedDirection=accumulatorPlayedDirection;
                state.accumulatorInverted=accumulatorInverted;
                state.trackAccumulator = trackAccumulator;
                state.trackAccumulation = trackAccumulation;
                state.trackAccumDirection = trackAccumDirection;
                state.trackPlayedDirection=trackPlayedDirection;state.trackAccumulatorHasPlayed=trackAccumulatorHasPlayed;state.trackAccumulatorReversed=trackAccumulatorReversed;
                state.trackAccumulatorInverted=trackAccumulatorInverted;
            }
        }
        if(voiceMode==0) {
            if(state.outputEvents.firstDue){state.outputEvents.pulses[TrackOutputEvents::First].fire();state.outputEvents.firstDue=false;}
            if(config.pulseLimit>0 && state.totalPulses+1>=clamp(config.pulseLimit,1,64))state.outputEvents.pulses[TrackOutputEvents::Last].fire();
        }
        // Apply mode edits at a track-clock boundary, keeping both banks until
        // reset. A mode switch cannot create an extra ratchet halfway through a pulse.
        if (state.accumMode != effectiveAccumMode(t)) {
            state.accumMode = effectiveAccumMode(t);
            state.activeAccumulation = state.accumMode == TrackAccumulation
                ? state.trackAccumulation : (state.stage >= 0 ? accumulationEnabled(t)?state.accumulator[state.stage]:state.accumulatorPlayed[state.stage] : 0);
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
            state.outputEvents.pulses[TrackOutputEvents::NewStage].fire();
            state.slideFromPrevious=precedingStage>=0 && config.stages[precedingStage].slide;
            state.pulse = 0;
            state.lastRatchetEvent = -1;
            state.activeAccumulation = state.accumMode == TrackAccumulation
                ? state.trackAccumulation : accumulationEnabled(t)?state.accumulator[state.stage]:state.accumulatorPlayed[state.stage];
            beginSlide(t);
            state.cvFrom=state.cvValue;
        }
        const auto& edit = config.stages[state.stage];
        if (newStage || edit.probabilityPerPulse) {
            float probability = clamp(edit.probability + modulation.routes[t][ModProbability] * 20.f + controls.values[t][CtrlProbability]*100.f, 0.f, 100.f);
            state.allowed = probability >= 100.f || (probability > 0.f && dice(t).percent() < probability);
        }
        const Gate gate = panelGate(t,state.stage);
        if (state.allowed && gate != Rest && !muted(t)) {
            if ((edit.accumTrigger == AccumStage && newStage)
                || (edit.accumTrigger == AccumPulse && (newStage || gate == Multi))) accumulate(t);
        }
        if (state.allowed && (gate != Rest || config.restPitchUpdate) && !muted(t)) {
            state.targetPitch = stagePitch(t);
            state.hasPitch = true;
        }
    }

    void render(int t, double dt) {
        auto& state = voice(t);
        const auto& config = settings.tracks[t];
        if (state.stage < 0) { state.gate = 0.f; state.cv = 0.f; return; }
        const auto& edit = config.stages[state.stage];
        Gate physical=panelGate(t,state.stage);
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
            physical==Rest?Rest:accumLength>=1.f?Hold:physical, state.pulse,
            state.age / state.duration, pulses(t,state.stage), clamp(config.pulseDivision, 1, 8),
            config.gateStretch, config.ratchetMode, ratchets,
            accumLength, state.duration, &eventId);
        bool gate = edit.gateOverride==0?false:
            !muted(t) && gateLength>0.f && state.allowed && gateHigh(physical==Rest?Rest:gateLength>=1.f?Hold:physical,
                state.pulse,state.age/state.duration,pulses(t,state.stage),clamp(config.pulseDivision,1,8),
                config.gateStretch,config.ratchetMode,ratchets,gateLength,state.duration);
        if (accumulationGate && edit.accumTrigger == AccumRatchet && eventId != state.lastRatchetEvent) {
            accumulate(t);
            state.lastRatchetEvent = eventId;
        }
        if (state.allowed && (physical != Rest || config.restPitchUpdate) && !muted(t))
            state.targetPitch = stagePitch(t);
        double mix = state.slideDuration > 0.0 ? std::min(1.0, state.slideAge / state.slideDuration) : 1.0;
        if(state.slideType!=TempoSlide && state.slideActive)state.pitch=slewPitch(state.pitch,state.targetPitch,dt,analogSlideTime(config.slideTime));
        else state.pitch = static_cast<float>(state.startPitch + (state.targetPitch - state.startPitch) * mix);
        state.slideAge += dt;
        state.cvValue=stageCvValue(state.cvFrom,edit.cv,config.cvType,edit.cvRamp,
            state.pulse,state.age,state.duration,pulses(t,state.stage));
        state.cv = muted(t) ? 0.f : state.cvValue;
        state.gate = gate ? 5.f : 0.f;
        state.age += dt;
    }
};
} // namespace metro
