#pragma once
#include "plugin.hpp"
#include "Sequencer.hpp"
#include <atomic>

struct ModVoltageQuantity : ParamQuantity {
    int typeParam = -1;
    int destinationParam = -1;
    float getMinValue() override {
        if (module && typeParam >= 0 && destinationParam >= 0) {
            bool positive = module->params[typeParam].getValue() > .5f;
            bool gate = metro::modGateDestination(static_cast<metro::ModDestination>(static_cast<int>(module->params[destinationParam].getValue())));
            if (positive || gate) return 0.f;
        }
        return -5.f;
    }
    float getValue() override {
        // Keep numeric editing and drag origins aligned with the effective
        // voltage when a bipolar lane changes to a positive-only CV type.
        return metro::clamp(ParamQuantity::getValue(), getMinValue(), 5.f);
    }
};

struct Metropolix : Module {
    enum ParamIds {
        TEMPO, RUN, RESET, ROOT, SCALE, RANGE,
        PITCH = 6, PULSES = PITCH + 8, GATES = PULSES + 8,
        TRACK = GATES + 8,
        TRACK_STRIDE = 9 + 6 * 8,
        LEGACY_NUM_PARAMS = TRACK + 2 * TRACK_STRIDE,
        EXTRA_TRACK = LEGACY_NUM_PARAMS,
        EXTRA_STRIDE = 6,
        ACCUM_OPTIONS = EXTRA_TRACK + 2 * EXTRA_STRIDE,
        ACCUM_OPTIONS_STRIDE = 3,
        STAGE_OPTIONS = ACCUM_OPTIONS + 2 * ACCUM_OPTIONS_STRIDE,
        MOD_LANE = STAGE_OPTIONS + 2 * 8 * 2,
        MOD_STRIDE = 26,
        OUTPUT_SOURCE_A = MOD_LANE + 8 * MOD_STRIDE,
        OUTPUT_SOURCE_B,
        NUM_PARAMS
    };
    enum TrackParams { LENGTH, OFFSET, DIVISION, ORDER, TRANSPOSE, GATE_LENGTH, SLIDE_TIME, ACCUM_LIMIT, MUTE };
    enum StageParams { RATCHETS, PROBABILITY, ACCUMULATION, SLIDE, SKIP, CV };
    enum ExtraTrackParams { REVERSED, PULSE_DIV, PULSE_LIMIT, GATE_STRETCH, RATCHET_MODE, ACCUM_AUTO_RESET };
    enum AccumOptions { NEGATIVE_LIMIT, BIPOLAR, ACCUM_ORDER };
    enum StageOptions { PROB_TARGET, ACCUM_TRIGGER };
    enum ModParams { MOD_LENGTH, MOD_OFFSET, MOD_DIVISION, MOD_ORDER, MOD_REVERSED, MOD_PULSE_LIMIT,
        MOD_TYPE, MOD_DESTINATION, MOD_ASSIGNMENT, MOD_MUTE, MOD_VALUE = 10, MOD_RAMP = 18 };
    enum InputIds { CLOCK_IN, RESET_IN, NUM_INPUTS };
    enum OutputIds { PITCH_1, GATE_1, PITCH_2, GATE_2, CLOCK_OUT, A_OUT, B_OUT, NUM_OUTPUTS };
    enum LightIds { POSITION_1, POSITION_2 = POSITION_1 + 8, NUM_LIGHTS = POSITION_2 + 8 };
    static int trackParam(int t, int p) { return TRACK + t * TRACK_STRIDE + p; }
    static int stageParam(int t, int stage, int row) { return trackParam(t, 9 + row * 8 + stage); }
    static int extraParam(int t, int p) { return EXTRA_TRACK + t * EXTRA_STRIDE + p; }
    static int accumParam(int t, int p) { return ACCUM_OPTIONS + t * ACCUM_OPTIONS_STRIDE + p; }
    static int stageOption(int t, int s, int p) { return STAGE_OPTIONS + t * 16 + p * 8 + s; }
    static int modParam(int lane, int p) { return MOD_LANE + lane * MOD_STRIDE + p; }
    static std::vector<std::string> modDestinations() {
        return {"None", "Octave", "Pitch Pre", "Pitch Post", "Pitch Offset", "Gate Length", "Probability", "Ratchet", "Mute"};
    }
    static std::vector<std::string> outputSources() {
        return {"Track 1 CV", "Track 2 CV", "MOD 1", "MOD 2", "MOD 3", "MOD 4", "MOD 5", "MOD 6", "MOD 7", "MOD 8"};
    }
    static std::vector<std::string> orderLabels() {
        return {"Linear", "Ping Pong", "Pendulum", "Odd / Even", "Jump by 2", "Jump by 3", "Jump by 4", "Jump by 5",
            "Hopscotch", "Arp 3 Shift -1", "Arp 4 Shift -2", "Arp 4 Shift -1", "Converge", "Diverge",
            "Growing", "Shrinking", "Pedal Point", "Brownian", "Shuffle", "Quasi Random", "Random"};
    }

    metro::Sequencer sequencer;
    dsp::SchmittTrigger clockTrigger, resetTrigger, resetButton;
    int controlCounter = 0;
    std::atomic<int> selectedTrack{0}, selectedStage{0}, selectedRow{0};
    std::atomic<bool> expanded{false}, externalClock{false};
    std::atomic<bool> modEditor{false};
    std::atomic<int> selectedLane{0};
    std::atomic<int> modStage[8];
    std::atomic<float> modVoltage[8];
    std::atomic<int> playStage[2];
    std::atomic<int> playPulse[2];
    std::atomic<float> playPitch[2];

    Metropolix() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(TEMPO, 0.1f, 400.f, 120.f, "Tempo", " BPM")->randomizeEnabled = false;
        configSwitch(RUN, 0, 1, 0, "Transport", {"Stopped", "Running"})->randomizeEnabled = false;
        configButton(RESET, "Reset position and accumulators");
        configSwitch(ROOT, 0, 11, 0, "Root", {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"});
        configSwitch(SCALE, 0, 6, 1, "Scale", {"Chromatic", "Major", "Minor", "Dorian", "Mixolydian", "Major pent.", "Minor pent."});
        configParam(RANGE, 1, 4, 3, "Slider range", " octaves")->snapEnabled = true;
        const float notes[8] = {0, 4, 7, 12, 16, 19, 14, 7};
        for (int s = 0; s < 8; ++s) {
            std::string name = "Stage " + std::to_string(s + 1) + " ";
            configParam(PITCH + s, 0, 1, notes[s] / 36.f, name + "pitch", "%", 0, 100);
            configParam(PULSES + s, 1, 8, 1, name + "pulse count")->snapEnabled = true;
            configSwitch(GATES + s, 0, 3, metro::Multi, name + "gate type", {"Rest", "Single", "Multi", "Hold"});
        }
        for (int t = 0; t < 2; ++t) {
            playStage[t].store(-1);
            playPulse[t].store(0);
            playPitch[t].store(0.f);
            std::string name = "Track " + std::to_string(t + 1) + " ";
            configParam(trackParam(t, LENGTH), 1, 8, 8, name + "length", " stages")->snapEnabled = true;
            configParam(trackParam(t, OFFSET), 0, 7, 0, name + "offset", " stages")->snapEnabled = true;
            configParam(trackParam(t, DIVISION), 1, 64, 1, name + "clock division")->snapEnabled = true;
            configSwitch(trackParam(t, ORDER), 0, 20, 0, name + "order", orderLabels())->snapEnabled = true;
            configSwitch(extraParam(t, REVERSED), 0, 1, 0, name + "direction", {"Forward", "Reverse"})->snapEnabled = true;
            configParam(extraParam(t, PULSE_DIV), 1, 8, 1, name + "pulse-count division")->snapEnabled = true;
            configParam(extraParam(t, PULSE_LIMIT), 0, 64, 0, name + "fixed pulse length (0 = Auto)")->snapEnabled = true;
            configSwitch(extraParam(t, GATE_STRETCH), 0, 1, 0, name + "gate stretching", {"Off", "On"})->snapEnabled = true;
            configSwitch(extraParam(t, RATCHET_MODE), 0, 2, 0, name + "ratchet mode", {"Multiply", "Pulse", "Gated"})->snapEnabled = true;
            configSwitch(extraParam(t, ACCUM_AUTO_RESET), 0, 1, 0, name + "accumulator reset", {"Manual", "Auto"})->snapEnabled = true;
            configParam(accumParam(t, NEGATIVE_LIMIT), 1, 28, 9, name + "negative accumulator limit", " scale degrees")->snapEnabled = true;
            configSwitch(accumParam(t, BIPOLAR), 0, 1, 0, name + "accumulator polarity", {"Unipolar", "Bipolar"});
            configSwitch(accumParam(t, ACCUM_ORDER), 0, 3, 0, name + "accumulator order", {"Wrap", "Pendulum", "Random", "Hold"});
            configParam(trackParam(t, TRANSPOSE), -24, 24, 0, name + "transpose", " semitones")->snapEnabled = true;
            configParam(trackParam(t, GATE_LENGTH), 1, 99, 50, name + "gate length", "%");
            configParam(trackParam(t, SLIDE_TIME), 0, 100, 25, name + "slide time", "% of a pulse");
            configParam(trackParam(t, ACCUM_LIMIT), 1, 28, 9, name + "positive accumulator limit", " scale degrees")->snapEnabled = true;
            configSwitch(trackParam(t, MUTE), 0, 1, 0, name + "mute", {"Off", "On"})->randomizeEnabled = false;
            for (int s = 0; s < 8; ++s) {
                std::string prefix = name + "stage " + std::to_string(s + 1) + " ";
                configParam(stageParam(t, s, RATCHETS), 1, 8, 1, prefix + "ratchets")->snapEnabled = true;
                configParam(stageParam(t, s, PROBABILITY), 0, 100, 100, prefix + "probability", "%")->snapEnabled = true;
                configParam(stageParam(t, s, ACCUMULATION), -28, 28, 0, prefix + "accumulation", " scale degrees")->snapEnabled = true;
                configSwitch(stageParam(t, s, SLIDE), 0, 1, 0, prefix + "slide", {"Off", "On"});
                configSwitch(stageParam(t, s, SKIP), 0, 1, 0, prefix + "skip", {"Off", "On"});
                configParam(stageParam(t, s, CV), -5, 5, 0, prefix + "CV", " V");
                configSwitch(stageOption(t, s, PROB_TARGET), 0, 1, 0, prefix + "probability target", {"Stage", "Pulse"});
                configSwitch(stageOption(t, s, ACCUM_TRIGGER), 0, 2, 0, prefix + "accumulator trigger", {"Stage", "Pulse", "Ratchet"});
            }
        }
        for (int lane = 0; lane < 8; ++lane) {
            modStage[lane].store(-1);
            modVoltage[lane].store(0.f);
            std::string name = "MOD " + std::to_string(lane + 1) + " ";
            configParam(modParam(lane, MOD_LENGTH), 1, 8, 8, name + "length", " stages")->snapEnabled = true;
            configParam(modParam(lane, MOD_OFFSET), 0, 7, 0, name + "offset", " stages")->snapEnabled = true;
            configParam(modParam(lane, MOD_DIVISION), 1, 64, 1, name + "clock division")->snapEnabled = true;
            configSwitch(modParam(lane, MOD_ORDER), 0, 20, 0, name + "play order", orderLabels());
            configSwitch(modParam(lane, MOD_REVERSED), 0, 1, 0, name + "direction", {"Forward", "Reverse"});
            configParam(modParam(lane, MOD_PULSE_LIMIT), 0, 64, 0, name + "pulse length (0 = Auto)")->snapEnabled = true;
            configSwitch(modParam(lane, MOD_TYPE), 0, 2, 0, name + "CV type", {"Bipolar", "Unipolar", "Gate"});
            configSwitch(modParam(lane, MOD_DESTINATION), 0, metro::ModDestinationCount - 1, 0, name + "internal destination", modDestinations());
            configSwitch(modParam(lane, MOD_ASSIGNMENT), 0, 2, 0, name + "destination tracks", {"Track 1", "Track 2", "Both tracks"});
            configSwitch(modParam(lane, MOD_MUTE), 0, 1, 0, name + "mute", {"Off", "On"});
            for (int stage = 0; stage < 8; ++stage) {
                std::string prefix = name + "stage " + std::to_string(stage + 1) + " ";
                auto* voltage = configParam<ModVoltageQuantity>(modParam(lane, MOD_VALUE + stage), -5, 5, 0, prefix + "voltage", " V");
                voltage->typeParam = modParam(lane, MOD_TYPE);
                voltage->destinationParam = modParam(lane, MOD_DESTINATION);
                configSwitch(modParam(lane, MOD_RAMP + stage), 0, 1, 0, prefix + "transition", {"Step", "Ramp"});
            }
        }
        configSwitch(OUTPUT_SOURCE_A, 0, 9, 0, "Output A source", outputSources());
        configSwitch(OUTPUT_SOURCE_B, 0, 9, 1, "Output B source", outputSources());
        configInput(CLOCK_IN, "Clock (one pulse per sixteenth note)");
        configInput(RESET_IN, "Reset");
        configOutput(PITCH_1, "Track 1 pitch (1 V/oct)");
        configOutput(GATE_1, "Track 1 gate (5 V)");
        configOutput(PITCH_2, "Track 2 pitch (1 V/oct)");
        configOutput(GATE_2, "Track 2 gate (5 V)");
        configOutput(CLOCK_OUT, "Clock (5 V, 1 ms)");
        configOutput(A_OUT, "Assignable A: track CV or MOD lane");
        configOutput(B_OUT, "Assignable B: track CV or MOD lane");
        sequencer.seed(random::u32());
        readSettings();
    }

    int integer(int id) { return static_cast<int>(std::round(params[id].getValue())); }
    void readSettings() {
        auto& c = sequencer.settings;
        c.outputSources[0] = integer(OUTPUT_SOURCE_A);
        c.outputSources[1] = integer(OUTPUT_SOURCE_B);
        for (int lane = 0; lane < 8; ++lane) {
            auto& mod = c.modLanes[lane];
            mod.length = integer(modParam(lane, MOD_LENGTH));
            mod.offset = integer(modParam(lane, MOD_OFFSET));
            mod.division = integer(modParam(lane, MOD_DIVISION));
            mod.order = static_cast<metro::Order>(integer(modParam(lane, MOD_ORDER)));
            mod.reversed = integer(modParam(lane, MOD_REVERSED)) != 0;
            mod.pulseLimit = integer(modParam(lane, MOD_PULSE_LIMIT));
            mod.type = static_cast<metro::ModType>(integer(modParam(lane, MOD_TYPE)));
            mod.destination = static_cast<metro::ModDestination>(integer(modParam(lane, MOD_DESTINATION)));
            mod.assignment = integer(modParam(lane, MOD_ASSIGNMENT));
            mod.mute = integer(modParam(lane, MOD_MUTE)) != 0;
            for (int stage = 0; stage < 8; ++stage) {
                mod.values[stage] = params[modParam(lane, MOD_VALUE + stage)].getValue();
                mod.ramps[stage] = integer(modParam(lane, MOD_RAMP + stage)) != 0;
            }
        }
        c.bpm = params[TEMPO].getValue();
        c.root = integer(ROOT);
        c.scale = integer(SCALE);
        c.octaves = integer(RANGE);
        for (int s = 0; s < 8; ++s) {
            c.stages[s].pitch = params[PITCH + s].getValue();
            c.stages[s].pulses = integer(PULSES + s);
            c.stages[s].gate = static_cast<metro::Gate>(integer(GATES + s));
        }
        for (int t = 0; t < 2; ++t) {
            auto& track = c.tracks[t];
            track.length = integer(trackParam(t, LENGTH));
            track.offset = integer(trackParam(t, OFFSET));
            track.division = integer(trackParam(t, DIVISION));
            track.order = static_cast<metro::Order>(integer(trackParam(t, ORDER)));
            track.reversed = integer(extraParam(t, REVERSED)) != 0;
            track.pulseDivision = integer(extraParam(t, PULSE_DIV));
            track.pulseLimit = integer(extraParam(t, PULSE_LIMIT));
            track.gateStretch = integer(extraParam(t, GATE_STRETCH)) != 0;
            track.ratchetMode = static_cast<metro::RatchetMode>(integer(extraParam(t, RATCHET_MODE)));
            track.autoAccumReset = integer(extraParam(t, ACCUM_AUTO_RESET)) != 0;
            track.transpose = integer(trackParam(t, TRANSPOSE));
            track.gateLength = params[trackParam(t, GATE_LENGTH)].getValue() * 0.01f;
            track.slideTime = params[trackParam(t, SLIDE_TIME)].getValue() * 0.01f;
            track.accumLimit = integer(trackParam(t, ACCUM_LIMIT));
            track.accumNegativeLimit = integer(accumParam(t, NEGATIVE_LIMIT));
            track.accumBipolar = integer(accumParam(t, BIPOLAR)) != 0;
            track.accumOrder = static_cast<metro::AccumOrder>(integer(accumParam(t, ACCUM_ORDER)));
            track.mute = integer(trackParam(t, MUTE)) != 0;
            for (int s = 0; s < 8; ++s) {
                auto& edit = track.stages[s];
                edit.ratchets = integer(stageParam(t, s, RATCHETS));
                edit.probability = params[stageParam(t, s, PROBABILITY)].getValue();
                edit.probabilityPerPulse = integer(stageOption(t, s, PROB_TARGET)) != 0;
                edit.accumulation = integer(stageParam(t, s, ACCUMULATION));
                edit.accumTrigger = static_cast<metro::AccumTrigger>(integer(stageOption(t, s, ACCUM_TRIGGER)));
                edit.slide = integer(stageParam(t, s, SLIDE)) != 0;
                edit.skip = integer(stageParam(t, s, SKIP)) != 0;
                edit.cv = params[stageParam(t, s, CV)].getValue();
            }
        }
    }

    void process(const ProcessArgs& args) override {
        bool update = controlCounter++ % 32 == 0;
        if (update) readSettings();
        bool clockEdge = clockTrigger.process(inputs[CLOCK_IN].getVoltage(), 0.1f, 1.5f);
        bool resetEdge = resetTrigger.process(inputs[RESET_IN].getVoltage(), 0.1f, 1.5f);
        bool button = resetButton.process(params[RESET].getValue());
        bool external = inputs[CLOCK_IN].isConnected();
        sequencer.process(args.sampleTime, params[RUN].getValue() > 0.5f, external, clockEdge, resetEdge || button);
        for (int t = 0; t < 2; ++t) {
            const auto& state = sequencer.tracks[t];
            outputs[PITCH_1 + 2 * t].setVoltage(state.pitch);
            outputs[GATE_1 + 2 * t].setVoltage(state.gate);
            outputs[A_OUT + t].setVoltage(sequencer.aux[t]);
            if (update) {
                playStage[t].store(state.stage, std::memory_order_relaxed);
                playPulse[t].store(state.pulse, std::memory_order_relaxed);
                playPitch[t].store(state.pitch, std::memory_order_relaxed);
                for (int s = 0; s < 8; ++s)
                    lights[POSITION_1 + t * 8 + s].setBrightness(sequencer.running && state.stage == s ? 1.f : 0.f);
            }
        }
        outputs[CLOCK_OUT].setVoltage(sequencer.clock);
        if (update) for (int lane = 0; lane < 8; ++lane) {
            modStage[lane].store(sequencer.running ? sequencer.modulation.lanes[lane].stage : -1, std::memory_order_relaxed);
            modVoltage[lane].store(sequencer.modulation.outputs[lane], std::memory_order_relaxed);
        }
        if (update) externalClock.store(external, std::memory_order_relaxed);
        // Avoid signed counter overflow in patches left running for days.
        controlCounter &= 31;
    }

    void onReset(const ResetEvent& e) override {
        Module::onReset(e);
        sequencer.reset();
        readSettings();
    }
    json_t* dataToJson() override {
        json_t* root = json_object();
        json_object_set_new(root, "schemaVersion", json_integer(4));
        json_object_set_new(root, "modEditor", json_boolean(modEditor.load()));
        json_object_set_new(root, "selectedLane", json_integer(selectedLane.load()));
        json_object_set_new(root, "expanded", json_boolean(expanded.load()));
        json_object_set_new(root, "selectedTrack", json_integer(selectedTrack.load()));
        json_object_set_new(root, "selectedStage", json_integer(selectedStage.load()));
        json_object_set_new(root, "selectedRow", json_integer(selectedRow.load()));
        return root;
    }
    void dataFromJson(json_t* root) override {
        // Preserve the original prototype's ORDER=1 (Reverse) after ORDER=1
        // becomes Ping Pong. All existing parameter IDs remain unchanged.
        if (json_integer_value(json_object_get(root, "schemaVersion")) < 2) {
            for (int t = 0; t < 2; ++t) {
                if (integer(trackParam(t, ORDER)) == 1) {
                    params[trackParam(t, ORDER)].setValue(0.f);
                    params[extraParam(t, REVERSED)].setValue(1.f);
                }
            }
        }
        if (json_integer_value(json_object_get(root, "schemaVersion")) < 3) {
            for (int t = 0; t < 2; ++t)
                params[accumParam(t, NEGATIVE_LIMIT)].setValue(params[trackParam(t, ACCUM_LIMIT)].getValue());
        }
        expanded.store(json_is_true(json_object_get(root, "expanded")));
        modEditor.store(json_is_true(json_object_get(root, "modEditor")));
        selectedLane.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "selectedLane"))), 0, 7));
        selectedTrack.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "selectedTrack"))), 0, 1));
        selectedStage.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "selectedStage"))), 0, 7));
        selectedRow.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "selectedRow"))), 0, 5));
        sequencer.reset();
        readSettings();
    }
};
