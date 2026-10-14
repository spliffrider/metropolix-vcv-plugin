#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include "PlayOrder.hpp"
#include "OutputRouting.hpp"

namespace metro {
enum ModType { ModBipolar, ModUnipolar, ModGate };
enum ModDestination { ModNone, ModOctave, ModPitchPre, ModPitchPost, ModPitchOffset,
    ModGateLength, ModProbability, ModRatchet, ModMute, ModSwing, ModUserScale, ModVelocity, ModOutputSwap,
    ModAccumulate, ModAccumReverse, ModAccumInvert, ModAccumMode, ModAccumPolarity, ModAccumReset,
    ModTempo, ModClockDiv, ModDirection, ModGateScale, ModLoopy, ModLoopyPulses, ModLoopyOffset,
    ModOrder, ModPulseCount, ModPulseDiv, ModPulsesLength, ModReset, ModRoot, ModSkipInvert,
    ModSlide, ModSlideAmount, ModSliderInvert, ModSliderRange, ModStagesLength, ModStagesOffset, ModDestinationCount };
inline bool modGateDestination(ModDestination destination) {
    switch(destination){
        case ModMute: case ModOutputSwap: case ModAccumulate: case ModAccumReverse: case ModAccumInvert:
        case ModAccumMode: case ModAccumPolarity: case ModAccumReset: case ModDirection: case ModLoopy:
        case ModReset: case ModSkipInvert: case ModSlide: case ModSliderInvert: return true;
        default: return false;
    }
}
inline bool modGlobalDestination(ModDestination destination){return destination==ModOutputSwap || destination==ModTempo || destination==ModLoopy || destination==ModLoopyPulses || destination==ModLoopyOffset;}
// Manual p192: an internal destination fixes its lane to Bipolar or Gate.
// Keep the saved type for unassigned lanes (including lanes sent to A/B).
inline ModType effectiveModType(ModType saved, ModDestination destination) {
    if(destination<=ModNone || destination>=ModDestinationCount)return saved;
    return modGateDestination(destination)?ModGate:ModBipolar;
}
template <class T> T modClamp(T v, T lo, T hi) { return std::max(lo, std::min(hi, v)); }

// AUX pp.154-156: continuous CV is attenuverted; a gate destination uses
// logical inversion to the left of noon. Unpatched gate controls can be
// operated manually (CW = high, CCW = low). Unpatched continuous CV is zero.
struct AuxInput {
    float voltage = 0.f, amount = 0.f;
    bool connected = false;
    ModDestination destination = ModNone;
    int assignment = 0;

    float effectiveVoltage() const {
        if (!std::isfinite(amount) || (connected && !std::isfinite(voltage))) return 0.f;
        float gain = modClamp(amount, -1.f, 1.f);
        if (gain == 0.f) return 0.f;
        if (modGateDestination(destination)) {
            bool high = !connected || voltage >= 1.5f;
            if (gain < 0.f) high = !high;
            return high ? 5.f : 0.f;
        }
        return connected ? modClamp(voltage * gain, -5.f, 5.f) : 0.f;
    }
};

struct ModLane {
    int length = 8, offset = 0, division = 1, pulseLimit = 0;
    Order order = Forward;
    bool reversed = false, mute = false;
    ModType type = ModBipolar;
    ModDestination destination = ModNone;
    int assignment = 0; // 0 = track 1, 1 = track 2, 2 = both
    std::array<float, 8> values{};
    std::array<bool, 8> ramps{};
};
struct ModLaneState {
    PlayOrder order;
    int stage = -1, divider = 0, count = 0, offset = -1;
    double age = 0.0, duration = .125;
    float start = 0.f, value = 0.f;
};

// Independent control sequences: pp.137-145. Base note parameters are never
// changed by modulation. Routes are recomputed from current lane voltages.
struct Modulation {
    std::array<ModLaneState, 8> lanes;
    std::array<float, 8> outputs{};
    std::array<OutputPulse,8> clockPulses;
    std::array<float,8> clocks{};
    std::array<std::array<float, ModDestinationCount>, 2> routes{};
    std::array<std::array<bool,ModDestinationCount>,2> assigned{};
    std::array<float,ModDestinationCount> globalRoutes{}; // Each source once, even when targeting both tracks.
    std::array<RandomSource, 8> random;

    void seed(uint32_t value) {
        for (int i = 0; i < 8; ++i) {
            random[i].state = value ^ (0x85ebca6bu * (i + 1));
            if (!random[i].state) random[i].state = i + 1;
        }
    }
    void reset() {
        for (auto& lane : lanes) lane = ModLaneState();
        silence();
    }
    void silence() {
        globalRoutes.fill(0.f);
        outputs.fill(0.f);
        clocks.fill(0.f);for(auto& pulse:clockPulses)pulse.clear();
        for (auto& track : routes) track.fill(0.f);
        for (auto& track : assigned) track.fill(false);
    }
    static float stageValue(const ModLane& lane, int stage) {
        float value = lane.values[stage];
        ModType type=effectiveModType(lane.type,lane.destination);
        if (type == ModGate) return value >= 1.5f ? 5.f : 0.f;
        return modClamp(value, type == ModUnipolar ? 0.f : -5.f, 5.f);
    }
    static float voltage(const ModLane& config, const ModLaneState& state) {
        if (state.stage < 0) return 0.f;
        float target = stageValue(config, state.stage);
        ModType type=effectiveModType(config.type,config.destination);
        bool ramp = config.ramps[state.stage] && type != ModGate;
        double mix = ramp ? modClamp(state.age / state.duration, 0.0, 1.0) : 1.0;
        float value = state.start + (target - state.start) * mix;
        return modClamp(value, type == ModUnipolar ? 0.f : -5.f, 5.f);
    }
    void tick(const std::array<ModLane, 8>& configs, double basePeriod) {
        const std::array<bool, 8> enabled{{true,true,true,true,true,true,true,true}};
        for (int i = 0; i < 8; ++i) {
            auto& state = lanes[i];
            const auto& config = configs[i];
            int division = modClamp(config.division, 1, 64);
            if (state.divider == 0 || state.divider >= division) {
                clockPulses[i].fire();
                state.start = voltage(config, state);
                if ((config.pulseLimit > 0 && state.count >= modClamp(config.pulseLimit, 1, 64)) || state.offset != config.offset) {
                    state.order.reset();
                    state.count = 0;
                    state.offset = config.offset;
                }
                int position = state.order.next(modClamp(config.length, 1, 8), config.order, config.reversed, enabled, random[i]);
                state.stage = (position + modClamp(config.offset, 0, 7)) % 8;
                state.count = config.pulseLimit > 0 ? state.count + 1 : 0;
                state.age = 0.0;
                state.duration = std::max(.001, basePeriod * division);
            }
            state.divider = (state.divider + 1) % division;
        }
    }
    void render(const std::array<ModLane, 8>& configs, double dt,
                const std::array<AuxInput, 3>& auxInputs = std::array<AuxInput, 3>()) {
        globalRoutes.fill(0.f);
        for (auto& track : routes) track.fill(0.f);
        for (auto& track : assigned) track.fill(false);
        for (int i = 0; i < 8; ++i) {
            const auto& config = configs[i];
            auto& state = lanes[i];
            float clock=clockPulses[i].render(dt);clocks[i]=config.mute?0.f:clock;
            state.value = voltage(config, state);
            outputs[i] = config.mute ? 0.f : state.value;
            state.age = std::min(state.age + dt, state.duration);
            if(config.destination>ModNone && config.destination<ModDestinationCount)
                for(int t=0;t<2;++t)if(config.assignment==2 || config.assignment==t)assigned[t][config.destination]=true;
            if (config.mute || config.destination <= ModNone || config.destination >= ModDestinationCount) continue;
            globalRoutes[config.destination]+=state.value;
            for (int track = 0; track < 2; ++track) {
                if (config.assignment == 2 || config.assignment == track)
                    routes[track][config.destination] += state.value;
            }
        }
        // Sum AUX and MOD contributions before destination saturation. This
        // lets negative CV cancel a positive route even if MOD alone exceeds 5V.
        for (const auto& input : auxInputs) {
            if (input.destination <= ModNone || input.destination >= ModDestinationCount) continue;
            float value = input.effectiveVoltage();
            globalRoutes[input.destination]+=value;
            for (int track = 0; track < 2; ++track)
                if (input.assignment == 2 || input.assignment == track) {
                    assigned[track][input.destination]=true;
                    routes[track][input.destination] += value;
                }
        }
        for (auto& track : routes) for (auto& value : track) value = modClamp(value, -5.f, 5.f);
        for(auto& value:globalRoutes)value=modClamp(value,-5.f,5.f);
    }
};
} // namespace metro
