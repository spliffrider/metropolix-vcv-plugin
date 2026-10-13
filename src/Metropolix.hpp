#pragma once
#include "plugin.hpp"
#include "Sequencer.hpp"
#include "Presets.hpp"
#include "Chains.hpp"
#include <atomic>
#include <mutex>
#include <deque>

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

struct LoopyPulseQuantity : ParamQuantity {
    std::string getDisplayValueString() override {return getValue()<.5f?"Auto":ParamQuantity::getDisplayValueString();}
    void setDisplayValueString(std::string s) override {if(s=="Auto" || s=="auto")setValue(0);else ParamQuantity::setDisplayValueString(s);}
};
struct GateLengthQuantity : ParamQuantity {
    std::string getDisplayValueString() override { return getValue()>=100?"Hold":ParamQuantity::getDisplayValueString(); }
    std::string getUnit() override { return getValue()>=100?"":unit; }
    void setDisplayValueString(std::string s) override { if(s=="Hold" || s=="hold")setValue(100);else ParamQuantity::setDisplayValueString(s); }
};
struct GateOverrideQuantity : ParamQuantity {
    std::string getDisplayValueString() override {
        int v=int(std::round(getValue()));return v<0?"-- (track)":v==0?"Rest":std::to_string(v)+"%";
    }
    void setDisplayValueString(std::string s) override {
        if(s=="--" || s=="track")setValue(-1);
        else if(s=="Rest" || s=="rest")setValue(0);
        else if(s=="Hold" || s=="hold")setValue(100);
        else ParamQuantity::setDisplayValueString(s);
    }
};
struct PitchOverrideQuantity : ParamQuantity {
    int scaleParam=-1;
    int scale() const { return module && scaleParam>=0?int(std::round(module->params[scaleParam].getValue())):0; }
    void setValue(float v) override {
        ParamQuantity::setValue(v<-.5f?-1.f:float(metro::degreeToSemitone(metro::nearestDegree(metro::clamp(v,0.f,60.f),scale()),scale())));
    }
    std::string getDisplayValueString() override {
        if(getValue()<0)return "-- (slider)";
        static const char* names[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
        int note=metro::degreeToSemitone(metro::nearestDegree(getValue(),scale()),scale());
        return std::string(names[note%12])+std::to_string(note/12);
    }
    void setDisplayValueString(std::string s) override {
        if(s=="--" || s=="slider"){setValue(-1);return;}
        static const char* names[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
        for(int n=0;n<12;++n)for(int octave=0;octave<=5;++octave)
            if(s==std::string(names[n])+std::to_string(octave)){setValue(float(n+12*octave));return;}
        try { setValue(std::stof(s)); } catch(...) {}
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
        AUX_PARAMS = OUTPUT_SOURCE_B + 1,
        AUX_STRIDE = 3,
        ACCUM_MODE = AUX_PARAMS + 3 * AUX_STRIDE,
        CTRL_PARAMS = ACCUM_MODE + 2,
        CTRL_STRIDE = 3,
        TOTAL_PULSES = CTRL_PARAMS + 2 * CTRL_STRIDE,
        QUEUE_PULSES, LOAD_DELAY, LOAD_PITCH, LOAD_SWITCHES, LOAD_CTRL, LOAD_AUX,
        RUN_MODE,
        SWING_1, SWING_2,
        GATE_SCALE_1, GATE_SCALE_2,
        STAGE_OVERRIDES,
        LOOPY_PULSES = STAGE_OVERRIDES + 32, LOOPY_TARGET, NUM_PARAMS
    };
    enum TrackParams { LENGTH, OFFSET, DIVISION, ORDER, TRANSPOSE, GATE_LENGTH, SLIDE_TIME, ACCUM_LIMIT, MUTE };
    enum StageParams { RATCHETS, PROBABILITY, ACCUMULATION, SLIDE, SKIP, CV, PITCH_OVERRIDE, GATE_OVERRIDE };
    enum ExtraTrackParams { REVERSED, PULSE_DIV, PULSE_LIMIT, GATE_STRETCH, RATCHET_MODE, ACCUM_AUTO_RESET };
    enum AccumOptions { NEGATIVE_LIMIT, BIPOLAR, ACCUM_ORDER };
    enum StageOptions { PROB_TARGET, ACCUM_TRIGGER };
    enum ModParams { MOD_LENGTH, MOD_OFFSET, MOD_DIVISION, MOD_ORDER, MOD_REVERSED, MOD_PULSE_LIMIT,
        MOD_TYPE, MOD_DESTINATION, MOD_ASSIGNMENT, MOD_MUTE, MOD_VALUE = 10, MOD_RAMP = 18 };
    enum AuxParams { AUX_AMOUNT, AUX_DESTINATION, AUX_ASSIGNMENT };
    enum CtrlParams { CTRL_VALUE, CTRL_DESTINATION, CTRL_ASSIGNMENT };
    enum InputIds { CLOCK_IN, RESET_IN, AUX_X_IN, AUX_Y_IN, AUX_Z_IN, NUM_INPUTS };
    enum OutputIds { PITCH_1, GATE_1, PITCH_2, GATE_2, CLOCK_OUT, A_OUT, B_OUT, NUM_OUTPUTS };
    enum LightIds { POSITION_1, POSITION_2 = POSITION_1 + 8, NUM_LIGHTS = POSITION_2 + 8 };
    static int trackParam(int t, int p) { return TRACK + t * TRACK_STRIDE + p; }
    static int stageParam(int t, int stage, int row) { return row<6?trackParam(t,9+row*8+stage):STAGE_OVERRIDES+t*16+(row-PITCH_OVERRIDE)*8+stage; }
    static int gateScaleParam(int t) { return GATE_SCALE_1+t; }
    static int extraParam(int t, int p) { return EXTRA_TRACK + t * EXTRA_STRIDE + p; }
    static int accumParam(int t, int p) { return ACCUM_OPTIONS + t * ACCUM_OPTIONS_STRIDE + p; }
    static int stageOption(int t, int s, int p) { return STAGE_OPTIONS + t * 16 + p * 8 + s; }
    static int modParam(int lane, int p) { return MOD_LANE + lane * MOD_STRIDE + p; }
    static int auxParam(int input, int p) { return AUX_PARAMS + input * AUX_STRIDE + p; }
    static int accumModeParam(int t) { return ACCUM_MODE + t; }
    static int ctrlParam(int knob, int p) { return CTRL_PARAMS + knob * CTRL_STRIDE + p; }
    static int swingParam(int track) { return SWING_1 + track; }
    static std::vector<std::string> ctrlDestinations() {
        return {"None", "Accum Mode", "Accum Polar", "Tempo", "Clock Div", "Direction",
            "Gate Length", "Octave", "Pitch Offset", "Pitch Post", "Pitch Pre", "Play Order",
            "Probability", "Pulse Count", "Pulse Div", "Pulses Len", "Ratchet", "Root",
            "Skip Invert", "Slide Amount", "Slider Invert", "Stages Len", "Stages Offset", "Track Out Swap", "Swing"};
    }
    static int ctrlBaseParam(metro::CtrlDestination d, int t) {
        switch(d) {
            case metro::CtrlAccumMode: return accumModeParam(t);
            case metro::CtrlAccumPolar: return accumParam(t,BIPOLAR);
            case metro::CtrlTempo: return TEMPO;
            case metro::CtrlClockDiv: return trackParam(t,DIVISION);
            case metro::CtrlGateLength: return trackParam(t,GATE_LENGTH);
            case metro::CtrlOrder: return trackParam(t,ORDER);
            case metro::CtrlPulseDiv: return extraParam(t,PULSE_DIV);
            case metro::CtrlPulsesLength: return extraParam(t,PULSE_LIMIT);
            case metro::CtrlSlideAmount: return trackParam(t,SLIDE_TIME);
            case metro::CtrlStagesLength: return trackParam(t,LENGTH);
            case metro::CtrlStagesOffset: return trackParam(t,OFFSET);
            case metro::CtrlSwing: return swingParam(t);
            default: return -1;
        }
    }
    int ctrlOwner(int id) {
        if(id<0) return -1;
        for(int k=1;k>=0;--k) {
            auto d=static_cast<metro::CtrlDestination>(static_cast<int>(std::round(params[ctrlParam(k,CTRL_DESTINATION)].getValue())));
            int assignment=static_cast<int>(std::round(params[ctrlParam(k,CTRL_ASSIGNMENT)].getValue()));
            for(int t=0;t<2;++t)
                if((assignment==2 || assignment==t || d==metro::CtrlTempo) && ctrlBaseParam(d,t)==id) return k;
        }
        return -1;
    }
    static int accumScreenParam(int t, int field) {
        const int ids[] = {trackParam(t, ACCUM_LIMIT), accumParam(t, NEGATIVE_LIMIT),
            accumModeParam(t), accumParam(t, ACCUM_ORDER), accumParam(t, BIPOLAR), extraParam(t, ACCUM_AUTO_RESET)};
        return ids[metro::clamp(field, 0, 5)];
    }
    static std::vector<std::string> modDestinations() {
        return {"None", "Octave", "Pitch Pre", "Pitch Post", "Pitch Offset", "Gate Length", "Probability", "Ratchet", "Mute", "Swing"};
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
    std::atomic<int> selectedAux{0};
    std::atomic<int> selectedCtrl{0};
    std::atomic<float> auxVoltage[3];
    // Presentation only: the hardware panel and optional grids share all
    // existing Rack parameters. No engine parameter IDs change here.
    enum UiPage { HOME, BPM_PAGE, SCALE_PAGE, ORDER_PAGE, LENGTH_PAGE, DIV_PAGE,
        SLIDE_PAGE, GATE_PAGE, STAGE_PAGE, A_PAGE, B_PAGE, SETUP_PAGE, UNAVAILABLE_PAGE, AUX_PAGE, ACCUM_PAGE, CTRL_PAGE,
        LOAD_PAGE, SAVE_PAGE, PRESET_SETUP_PAGE, CHAIN_PAGE, SWING_PAGE, LOOPY_PAGE };
    std::atomic<int> uiPage{HOME}, uiField{0};
    std::atomic<int> uiHome{0};
    std::atomic<bool> uiStageDetail{false};
    std::atomic<bool> uiMod{false}, uiEdit{false}, uiAlt{false};
    std::atomic<bool> uiChooseFunction{false};
    std::atomic<int> modStage[8];
    std::atomic<float> modVoltage[8];
    std::atomic<int> playStage[2];
    std::atomic<int> playPulse[2];
    std::atomic<float> playPitch[2];
    std::atomic<float> playCv[2], pitchPre[2], pitchPost[2];
    std::atomic<float> displayBpm{120.f};
    double meterClockElapsed=0.0;
    bool meterClockSeen=false, meterWasExternal=false;

    // A single atomic command publishes the held stage boundaries.
    // Latching and held gestures are transient: opening a patch cannot hold gates.
    std::atomic<unsigned> loopyCommand{0}, loopyHeld{0}, loopyMuteCommands{0};
    std::atomic<bool> loopyLatched{false}, loopyMuteHeld{false}, loopyMuteUsed{false};
    void clearLoopy() {loopyCommand.store(0);loopyHeld.store(0);loopyLatched.store(false);}
    void requestLoopy(unsigned stages) {
        loopyCommand.store(stages&255u);
    }
    void pressLoopyStage(int stage) {
        unsigned mask=loopyHeld.fetch_or(1u<<stage)|(1u<<stage);requestLoopy(mask);
    }
    void releaseLoopyStage(int stage) {
        unsigned mask=loopyHeld.fetch_and(~(1u<<stage))&~(1u<<stage);
        if(!loopyLatched.load())requestLoopy(mask);
    }
    void latchLoopy() {if(loopyCommand.load()&255u)loopyLatched.store(true);}
    void muteLoopy(int index) {loopyMuteUsed.store(true);loopyMuteCommands.fetch_xor(1u<<index);}
    void applyLoopyMutes() {
        unsigned mask=loopyMuteCommands.exchange(0);
        for(int i=0;i<10;++i)if(mask&(1u<<i)) {
            int id=i<2?trackParam(i,MUTE):modParam(i-2,MOD_MUTE);
            params[id].setValue(integer(id)?0.f:1.f);
        }
        if(mask)readSettings();
    }
    Metropolix() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        auto* loop=configParam<LoopyPulseQuantity>(LOOPY_PULSES,0,64,0,"Loopy pulses");loop->snapEnabled=true;loop->randomizeEnabled=false;
        configSwitch(LOOPY_TARGET,0,1,0,"Loopy target",{"Current track","Tracks 1+2"})->randomizeEnabled=false;
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
            playCv[t].store(0.f); pitchPre[t].store(0.f); pitchPost[t].store(0.f);
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
            configSwitch(accumModeParam(t), 0, 1, 0, name + "accumulator mode", {"Stage", "Track"});
            configParam(trackParam(t, TRANSPOSE), -24, 24, 0, name + "transpose", " semitones")->snapEnabled = true;
            configParam<GateLengthQuantity>(trackParam(t, GATE_LENGTH), 1, 100, 50, name + "gate length", "%");
            configParam(gateScaleParam(t),0,100,100,name+"gate scale","%")->snapEnabled=true;
            configParam(trackParam(t, SLIDE_TIME), 0, 100, 25, name + "slide time", "% of a pulse");
            configParam(swingParam(t), 50, 78, 50, name + "swing", "%")->snapEnabled = true;
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
                auto* pitch=configParam<PitchOverrideQuantity>(stageParam(t,s,PITCH_OVERRIDE),-1,60,-1,prefix+"pitch override");
                pitch->scaleParam=SCALE; pitch->snapEnabled=true;
                configParam<GateOverrideQuantity>(stageParam(t,s,GATE_OVERRIDE),-1,100,-1,prefix+"gate override")->snapEnabled=true;
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
        for (int input = 0; input < 3; ++input) {
            std::string name = std::string("AUX ") + char('X' + input) + " ";
            auxVoltage[input].store(0.f);
            configParam(auxParam(input,AUX_AMOUNT),-1.f,1.f,0.f,name+"attenuverter","%",0,100);
            configSwitch(auxParam(input,AUX_DESTINATION),0,metro::ModDestinationCount-1,0,name+"destination",modDestinations());
            configSwitch(auxParam(input,AUX_ASSIGNMENT),0,2,0,name+"target",{"Track 1","Track 2","Both tracks"});
            configInput(AUX_X_IN+input,name+"CV / gate (channel 1; assign with the button below)");
        }
        for(int k=0;k<2;++k) {
            std::string name="CTRL "+std::to_string(k+1)+" ";
            configParam(ctrlParam(k,CTRL_VALUE),-1.f,1.f,k?-.5f:0.f,name+"position","%",0,50,50);
            configSwitch(ctrlParam(k,CTRL_DESTINATION),0,metro::CtrlDestinationCount-1,k?metro::CtrlSlideAmount:metro::CtrlGateLength,name+"destination",ctrlDestinations())->randomizeEnabled=false;
            configSwitch(ctrlParam(k,CTRL_ASSIGNMENT),0,2,2,name+"target",{"Track 1","Track 2","Both tracks"})->randomizeEnabled=false;
        }
        configurePresets();
        configInput(CLOCK_IN, "Clock (one pulse per sixteenth note)");
        configInput(RESET_IN, "Reset");
        configOutput(PITCH_1, "Track 1 pitch (1 V/oct)");
        configOutput(GATE_1, "Track 1 gate (5 V)");
        configOutput(PITCH_2, "Track 2 pitch (1 V/oct)");
        configOutput(GATE_2, "Track 2 gate (5 V)");
        configOutput(CLOCK_OUT, "Clock (5 V, 1 ms)");
        configOutput(A_OUT, "Assignable A: track CV or MOD lane");
        configOutput(B_OUT, "Assignable B: track CV or MOD lane");
        for (auto* q : paramQuantities) {
            if (dynamic_cast<SwitchQuantity*>(q)) q->snapEnabled = true;
            q->description = "Hold and drag up to increase, down to decrease. Ctrl-drag for fine adjustment. Right-click for values and reset.";
        }
        paramQuantities[RUN]->description = "Click to start or stop. Stopping silences gates and modulation; pitch holds.";
        paramQuantities[RESET]->description = "Press to restart note tracks, MOD lanes, and accumulators.";
        for (int t=0; t<2; ++t)
            paramQuantities[accumModeParam(t)]->description = "Stage changes each stage independently. Track carries accumulation across the whole phrase. Takes effect on the next track clock pulse.";
        sequencer.seed(random::u32());
        readSettings();
    }

    int integer(int id) { return static_cast<int>(std::round(params[id].getValue())); }
#include "PresetAdapter.hpp"
#include "ChainAdapter.hpp"
    void readSettings() {
        auto& c = sequencer.settings;
        // Direct CTRL destinations own the panel parameter itself. Offsets are
        // routed separately and never accumulate into saved base values.
        for(int k=0;k<2;++k) {
            auto& ctrl=c.controls[k];
            ctrl.value=metro::ctrlClamp(params[ctrlParam(k,CTRL_VALUE)].getValue());
            ctrl.destination=static_cast<metro::CtrlDestination>(metro::clamp(integer(ctrlParam(k,CTRL_DESTINATION)),0,int(metro::CtrlDestinationCount)-1));
            ctrl.assignment=metro::clamp(integer(ctrlParam(k,CTRL_ASSIGNMENT)),0,2);
            for(int t=0;t<2;++t) {
                int id=ctrlBaseParam(ctrl.destination,t);
                if(id>=0 && (ctrl.targets(t) || ctrl.destination==metro::CtrlTempo))
                    params[id].setValue(metro::ctrlDirectValue(ctrl.destination,ctrl.value));
            }
        }
        c.outputSources[0] = integer(OUTPUT_SOURCE_A);
        c.outputSources[1] = integer(OUTPUT_SOURCE_B);
        for (int input=0; input<3; ++input) {
            auto& aux = c.auxInputs[input];
            aux.amount = params[auxParam(input,AUX_AMOUNT)].getValue();
            aux.destination = static_cast<metro::ModDestination>(integer(auxParam(input,AUX_DESTINATION)));
            aux.assignment = integer(auxParam(input,AUX_ASSIGNMENT));
        }
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
            mod.mute = integer(modParam(lane, MOD_MUTE)) != 0 || (sequencer.chainMutes & 4);
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
            track.gateScale=params[gateScaleParam(t)].getValue()*.01f;
            track.slideTime = params[trackParam(t, SLIDE_TIME)].getValue() * 0.01f;
            track.swing = params[swingParam(t)].getValue();
            track.accumLimit = integer(trackParam(t, ACCUM_LIMIT));
            track.accumNegativeLimit = integer(accumParam(t, NEGATIVE_LIMIT));
            track.accumBipolar = integer(accumParam(t, BIPOLAR)) != 0;
            track.accumOrder = static_cast<metro::AccumOrder>(integer(accumParam(t, ACCUM_ORDER)));
            track.accumMode = static_cast<metro::AccumMode>(integer(accumModeParam(t)));
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
                edit.pitchOverride=integer(stageParam(t,s,PITCH_OVERRIDE));
                edit.gateOverride=integer(stageParam(t,s,GATE_OVERRIDE));
            }
        }
    }

    void process(const ProcessArgs& args) override {
        bool update = controlCounter++ % 32 == 0;
        if (update) readSettings();
        for (int input=0; input<3; ++input) {
            auto& aux = sequencer.settings.auxInputs[input];
            aux.connected = inputs[AUX_X_IN+input].isConnected();
            aux.voltage = inputs[AUX_X_IN+input].getVoltage();
            if (update) auxVoltage[input].store(aux.effectiveVoltage(),std::memory_order_relaxed);
        }
        bool clockEdge = clockTrigger.process(inputs[CLOCK_IN].getVoltage(), 0.1f, 1.5f);
        bool resetEdge = resetTrigger.process(inputs[RESET_IN].getVoltage(), 0.1f, 1.5f);
        bool button = resetButton.process(params[RESET].getValue());
        bool external = inputs[CLOCK_IN].isConnected();
        if(!external || !meterWasExternal){meterClockSeen=false;meterClockElapsed=0.0;}
        if(external) {
            meterClockElapsed+=args.sampleTime;
            if(clockEdge) {
                if(meterClockSeen)displayBpm.store(static_cast<float>(15.0/metro::clamp(meterClockElapsed,0.001,150.0)),std::memory_order_relaxed);
                meterClockElapsed=0.0;meterClockSeen=true;
            }
        }
        meterWasExternal=external;
        bool run = params[RUN].getValue() > 0.5f;
        processChainCommands(run, external, resetEdge || button);
        processPresetCommands(run, external, resetEdge || button);
        unsigned gesture=loopyCommand.load();
        sequencer.loopy.stages=gesture&255u;sequencer.loopy.targets=integer(LOOPY_TARGET)?3u:1u<<selectedTrack.load();
        sequencer.loopy.pulses=integer(LOOPY_PULSES);
        if(!run)applyLoopyMutes();
        sequencer.process(args.sampleTime, run, external, clockEdge, resetEdge || button, [this] { presetClock(); applyLoopyMutes(); });
        for (int t = 0; t < 2; ++t) {
            const auto& state = sequencer.sounding(t);
            int physical=sequencer.controls.swap?1-t:t;
            outputs[PITCH_1 + 2 * physical].setVoltage(state.pitch);
            outputs[GATE_1 + 2 * physical].setVoltage(state.gate);
            outputs[A_OUT + t].setVoltage(sequencer.aux[t]);
            if (update) {
                playStage[t].store(state.stage, std::memory_order_relaxed);
                playPulse[t].store(state.pulse, std::memory_order_relaxed);
                playPitch[t].store(state.pitch, std::memory_order_relaxed);
                playCv[t].store(state.cv, std::memory_order_relaxed);
                pitchPre[t].store(sequencer.modulation.routes[t][metro::ModPitchPre]+sequencer.controls.values[t][metro::CtrlPitchPre], std::memory_order_relaxed);
                pitchPost[t].store(sequencer.modulation.routes[t][metro::ModPitchPost]+sequencer.controls.values[t][metro::CtrlPitchPost], std::memory_order_relaxed);
                for (int s = 0; s < 8; ++s)
                    lights[POSITION_1 + t * 8 + s].setBrightness((sequencer.running || sequencer.playerPlaying[t]) && state.stage == s ? 1.f : 0.f);
            }
        }
        outputs[CLOCK_OUT].setVoltage(sequencer.clock);
        if (update) for (int lane = 0; lane < 8; ++lane) {
            modStage[lane].store(sequencer.running ? sequencer.modulation.lanes[lane].stage : -1, std::memory_order_relaxed);
            modVoltage[lane].store(sequencer.modulation.outputs[lane], std::memory_order_relaxed);
        }
        if (update) {
            externalClock.store(external, std::memory_order_relaxed);
            if(!external)displayBpm.store(params[TEMPO].getValue(),std::memory_order_relaxed);
        }
        // Avoid signed counter overflow in patches left running for days.
        controlCounter &= 31;
    }

    void onReset(const ResetEvent& e) override {
        Module::onReset(e);
        clearLoopy();loopyMuteCommands.store(0);loopyMuteHeld.store(false);
        resetPresetRuntime();
        resetChainRuntime();
        { std::lock_guard<std::mutex> lock(presetMutex);chainState.enabled=false;chainEnabled.store(false); }
        sequencer.reset();
        readSettings();
    }
    json_t* dataToJson() override {
        json_t* root = json_object();
        json_object_set_new(root, "schemaVersion", json_integer(14));
        json_object_set_new(root, "chainFormat", json_integer(1));
        json_object_set_new(root, "chains", chainsToJson());
        json_object_set_new(root, "presetFormat", json_integer(3));
        json_object_set_new(root, "presets", presetsToJson());
        json_object_set_new(root, "presetBank", json_integer(presetBankIndex.load()));
        json_object_set_new(root, "selectedCtrl", json_integer(selectedCtrl.load()));
        json_object_set_new(root, "selectedAux", json_integer(selectedAux.load()));
        json_object_set_new(root, "uiHome", json_integer(uiHome.load()));
        json_object_set_new(root, "uiStageDetail", json_boolean(uiStageDetail.load()));
        json_object_set_new(root, "uiPage", json_integer(uiPage.load()));
        json_object_set_new(root, "uiField", json_integer(uiField.load()));
        json_object_set_new(root, "uiMod", json_boolean(uiMod.load()));
        json_object_set_new(root, "uiEdit", json_boolean(uiEdit.load()));
        json_object_set_new(root, "uiChooseFunction", json_boolean(uiChooseFunction.load()));
        json_object_set_new(root, "modEditor", json_boolean(modEditor.load()));
        json_object_set_new(root, "selectedLane", json_integer(selectedLane.load()));
        json_object_set_new(root, "expanded", json_boolean(expanded.load()));
        json_object_set_new(root, "selectedTrack", json_integer(selectedTrack.load()));
        json_object_set_new(root, "selectedStage", json_integer(selectedStage.load()));
        json_object_set_new(root, "selectedRow", json_integer(selectedRow.load()));
        return root;
    }
    void dataFromJson(json_t* root) override {
        clearLoopy();loopyMuteCommands.store(0);loopyMuteHeld.store(false);
        if(json_integer_value(json_object_get(root,"schemaVersion"))<14)
            for(int id=LOOPY_PULSES;id<NUM_PARAMS;++id)params[id].setValue(paramQuantities[id]->defaultValue);
        if(json_integer_value(json_object_get(root,"schemaVersion"))<13)
            for(int id=GATE_SCALE_1;id<NUM_PARAMS;++id)params[id].setValue(paramQuantities[id]->defaultValue);
        if(json_integer_value(json_object_get(root,"schemaVersion"))<12)
            for(int t=0;t<2;++t)params[swingParam(t)].setValue(50.f);
        if(json_integer_value(json_object_get(root,"schemaVersion"))<9)
            for(int id=CTRL_PARAMS;id<TOTAL_PULSES;++id)params[id].setValue(0.f);
        if(json_integer_value(json_object_get(root,"schemaVersion"))<10)
            for(int id=TOTAL_PULSES;id<NUM_PARAMS;++id)params[id].setValue(paramQuantities[id]->defaultValue);
        presetsFromJson(root);
        if(json_integer_value(json_object_get(root,"schemaVersion"))<11)params[RUN_MODE].setValue(0);
        chainsFromJson(root);
        // Old module presets may be loaded over an already configured module.
        // Missing AUX parameters must restore the original, unassigned sound.
        if(json_integer_value(json_object_get(root,"schemaVersion"))<7)
            for(int id=AUX_PARAMS;id<ACCUM_MODE;++id)params[id].setValue(0.f);
        if(json_integer_value(json_object_get(root,"schemaVersion"))<8)
            for(int t=0;t<2;++t)params[accumModeParam(t)].setValue(0.f);
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
        selectedRow.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "selectedRow"))), 0, 7));
        selectedAux.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "selectedAux"))),0,2));
        selectedCtrl.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root,"selectedCtrl"))),0,1));
        uiPage.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "uiPage"))), 0, static_cast<int>(LOOPY_PAGE)));
        uiField.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "uiField"))), 0, uiPage.load()==PRESET_SETUP_PAGE?6:uiPage.load()==ACCUM_PAGE?5:uiPage.load()==SETUP_PAGE?3:1));
        uiHome.store(metro::clamp(static_cast<int>(json_integer_value(json_object_get(root, "uiHome"))),0,3));
        uiStageDetail.store(json_is_true(json_object_get(root,"uiStageDetail")));
        if(json_integer_value(json_object_get(root,"schemaVersion"))<6 && uiPage.load()==LENGTH_PAGE)uiField.store(0);
        uiMod.store(json_is_true(json_object_get(root, "uiMod")));
        uiEdit.store(json_is_true(json_object_get(root, "uiEdit")));
        uiChooseFunction.store(json_is_true(json_object_get(root, "uiChooseFunction")));
        uiAlt.store(false);
        sequencer.reset();
        readSettings();
    }
};
