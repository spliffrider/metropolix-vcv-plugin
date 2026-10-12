#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include "PlayOrder.hpp"

namespace metro {
enum ModType { ModBipolar, ModUnipolar, ModGate };
enum ModDestination { ModNone, ModOctave, ModPitchPre, ModPitchPost, ModPitchOffset,
    ModGateLength, ModProbability, ModRatchet, ModMute, ModDestinationCount };
inline bool modGateDestination(ModDestination destination) { return destination == ModMute; }
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
    std::array<std::array<float, ModDestinationCount>, 2> routes{};
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
        outputs.fill(0.f);
        for (auto& track : routes) track.fill(0.f);
    }
    static float stageValue(const ModLane& lane, int stage) {
        float value = lane.values[stage];
        if (lane.type == ModGate || modGateDestination(lane.destination)) return value >= 1.5f ? 5.f : 0.f;
        return modClamp(value, lane.type == ModUnipolar ? 0.f : -5.f, 5.f);
    }
    static float voltage(const ModLane& config, const ModLaneState& state) {
        if (state.stage < 0) return 0.f;
        float target = stageValue(config, state.stage);
        bool ramp = config.ramps[state.stage] && config.type != ModGate && !modGateDestination(config.destination);
        double mix = ramp ? modClamp(state.age / state.duration, 0.0, 1.0) : 1.0;
        float value = state.start + (target - state.start) * mix;
        return modClamp(value, config.type == ModUnipolar ? 0.f : -5.f, 5.f);
    }
    void tick(const std::array<ModLane, 8>& configs, double basePeriod) {
        const std::array<bool, 8> enabled{{true,true,true,true,true,true,true,true}};
        for (int i = 0; i < 8; ++i) {
            auto& state = lanes[i];
            const auto& config = configs[i];
            int division = modClamp(config.division, 1, 64);
            if (state.divider == 0 || state.divider >= division) {
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
        for (auto& track : routes) track.fill(0.f);
        for (int i = 0; i < 8; ++i) {
            const auto& config = configs[i];
            auto& state = lanes[i];
            state.value = voltage(config, state);
            outputs[i] = config.mute ? 0.f : state.value;
            state.age = std::min(state.age + dt, state.duration);
            if (config.mute || config.destination <= ModNone || config.destination >= ModDestinationCount) continue;
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
            for (int track = 0; track < 2; ++track)
                if (input.assignment == 2 || input.assignment == track)
                    routes[track][input.destination] += value;
        }
        for (auto& track : routes) for (auto& value : track) value = modClamp(value, -5.f, 5.f);
    }
};
} // namespace metro
