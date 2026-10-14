#include "../src/Sequencer.hpp"
#include <iostream>
#include <cstdlib>
#include <string>

int checks = 0;
void check(bool value, const std::string& label) {
    ++checks;
    if (!value) { std::cerr << "FAIL: " << label << '\n'; std::exit(1); }
}
void near(float value, float expected, const std::string& label) { check(std::abs(value - expected) < .0001f, label); }
void tick(metro::Sequencer& seq) { seq.process(.002, true, true, true, false); }

int main() {
    using namespace metro;
    {
        // Manual p192: internal assignment determines the sounding type;
        // stored preferences only select the unassigned/output-only mode.
        for(int destination=0;destination<ModDestinationCount;++destination)
        for(int saved=0;saved<3;++saved){
            ModLane lane;lane.destination=static_cast<ModDestination>(destination);lane.type=static_cast<ModType>(saved);
            bool gate=modGateDestination(lane.destination) || (destination==ModNone && saved==ModGate);
            bool unipolar=destination==ModNone && saved==ModUnipolar;
            for(float value:{-5.f,-3.25f,0.f,1.49f,1.5f,2.25f,5.f}){
                lane.values[0]=value;float expected=gate?(value>=1.5f?5.f:0.f):(unipolar?std::max(0.f,value):value);
                near(Modulation::stageValue(lane,0),expected,"assigned MOD type follows destination without clipping or quantizing continuous values");
                check(int(lane.type)==saved && lane.values[0]==value,"effective MOD mode preserves saved type and stage data");
            }
            lane.values[0]=4;lane.ramps[0]=true;ModLaneState state;state.stage=0;state.start=-4;state.age=.25;state.duration=1;
            near(Modulation::voltage(lane,state),gate?5.f:unipolar?0.f:-2.f,"MOD ramps and limits use the destination's effective mode");
        }
    }
    {
        Sequencer seq;
        seq.settings.outputSources = {{2,3}};
        seq.settings.modLanes[0].values = {{-2,-1,0,1,2,1,0,-1}};
        seq.settings.modLanes[1].values = {{0,5,0,5,0,5,0,5}};
        seq.settings.modLanes[1].type = ModGate;
        seq.settings.modLanes[1].division = 2;
        seq.settings.stages[0].pulses = 8;
        const float a[] = {-2,-1,0,1,2,1,0,-1,-2,-1};
        const float b[] = {0,0,5,5,0,0,5,5,0,0};
        for (int i = 0; i < 10; ++i) {
            tick(seq);
            near(seq.aux[0], a[i], "A emits literal MOD voltage pattern");
            near(seq.aux[1], b[i], "B emits independent divided 0/5V gate pattern");
        }
        seq.settings.modLanes[0].mute = true;
        tick(seq);
        near(seq.aux[0], 0, "lane mute silences output");
        check(seq.modulation.lanes[0].stage == 2, "muted lane keeps its clock position");
        seq.process(.001, false, true, false, false);
        near(seq.aux[0], 0, "stop silences A"); near(seq.aux[1], 0, "stop silences B");
        seq.settings.modLanes[0].mute = false;
        tick(seq);
        near(seq.aux[0], -2, "restart resets all lanes");
    }
    {
        Modulation mod;
        std::array<ModLane,8> lanes;
        lanes[0].length = 3; lanes[0].offset = 6; lanes[0].pulseLimit = 4;
        int expected[] = {6,7,0,6,6,7,0,6};
        for (int stage : expected) { mod.tick(lanes, .125); check(mod.lanes[0].stage == stage, "MOD fixed length and offset wrap"); }
        lanes[0].reversed = true; mod.reset();
        int reverse[] = {0,7,6,0};
        for (int stage : reverse) { mod.tick(lanes, .125); check(mod.lanes[0].stage == stage, "MOD reversed order"); }
    }
    {
        Modulation mod;
        std::array<ModLane,8> lanes;
        lanes[0].values = {{-2,2,-4,5,0,0,0,0}};
        lanes[0].ramps[1] = true;
        mod.tick(lanes, 1.0); mod.render(lanes, 1.0);
        near(mod.outputs[0], -2, "first stage steps immediately");
        mod.tick(lanes, 1.0); mod.render(lanes, .5);
        near(mod.outputs[0], -2, "p139 ramp starts at previous voltage");
        mod.render(lanes, .5); near(mod.outputs[0], 0, "p139 halfway ramp voltage");
        mod.render(lanes, .01); near(mod.outputs[0], 2, "p139 ramp reaches target at stage end");
        mod.render(lanes, 1.0); near(mod.outputs[0], 2, "stalled clock holds final ramp target");
        mod.tick(lanes, 1.0); mod.render(lanes, .1); near(mod.outputs[0], -4, "step follows ramp without interpolation");
        lanes[0].type = ModUnipolar;
        mod.render(lanes,.1); near(mod.outputs[0], 0, "unipolar clamps negative stages");
        lanes[0].type = ModGate; lanes[0].values[2] = 1.49f;
        mod.render(lanes,.1); near(mod.outputs[0], 0, "gate below 1.5 V stays low");
        lanes[0].values[2] = 1.5f;
        mod.render(lanes,.1); near(mod.outputs[0], 5, "gate at 1.5 V goes high");
    }
    {
        // Pitch semantics, pp160-161: octave thresholds, quantizer placement,
        // chromatic post-quantization, and continuous voltage offset.
        Sequencer seq;
        auto& lane = seq.settings.modLanes[0]; lane.length = 1; lane.values[0] = 1.9f;
        lane.destination = ModOctave;
        tick(seq); near(seq.tracks[0].pitch, 1, "octave changes in whole volts"); near(seq.tracks[1].pitch, 0, "track 2 unaffected by track 1 assignment");
        lane.assignment = 1; seq.reset(); tick(seq);
        near(seq.tracks[0].pitch, 0, "track 1 unaffected by track 2 assignment"); near(seq.tracks[1].pitch, 1, "track 2 receives octave");
        lane.assignment = 2; lane.values[0] = -1.9f; seq.reset(); tick(seq);
        near(seq.tracks[0].pitch, -1, "negative octave uses whole-volt threshold"); near(seq.tracks[1].pitch, -1, "both tracks receive modulation");
        lane.values[0] = 1.f/12; lane.destination = ModPitchPre; seq.reset(); tick(seq);
        near(seq.tracks[0].pitch, 0, "pitch pre is quantized to C major (lower tie)");
        lane.destination = ModPitchPost; seq.reset(); tick(seq);
        near(seq.tracks[0].pitch, 1.f/12, "pitch post can produce chromatic C sharp");
        lane.destination = ModPitchOffset; lane.values[0] = .03f; seq.reset(); tick(seq);
        near(seq.tracks[0].pitch, .03f, "pitch offset is continuous");
        check(seq.settings.tracks[0].transpose == 0 && seq.settings.stages[0].pitch == 0, "routing never overwrites base settings");
        lane.destination = ModNone; seq.reset(); tick(seq);
        near(seq.tracks[0].pitch, 0, "removing destination restores programmed pitch");
    }
    {
        Sequencer seq;
        for (int i = 0; i < 2; ++i) {
            auto& lane = seq.settings.modLanes[i]; lane.length = 1;
            lane.values[0] = .6f; lane.destination = ModOctave;
        }
        tick(seq); near(seq.tracks[0].pitch, 1, "multiple lanes sum before destination quantization");
        seq.settings.modLanes[1].mute = true; seq.reset(); tick(seq);
        near(seq.tracks[0].pitch, 0, "muted lane removed from internal sum");
        for (auto& lane : seq.settings.modLanes) {
            lane.length = 1; lane.mute = false; lane.destination = ModOctave; lane.values[0] = 5;
        }
        seq.reset(); tick(seq);
        near(seq.tracks[0].pitch, 5, "summed modulation is bounded to destination voltage range");
    }
    {
        Sequencer seq;
        auto& lane = seq.settings.modLanes[0]; lane.length = 1; lane.values[0] = -5;
        lane.destination = ModProbability; tick(seq);
        near(seq.tracks[0].gate, 0, "negative probability modulation can suppress all gates");
        near(seq.tracks[1].gate, 5, "probability routing is track-specific");
        near(seq.settings.tracks[0].stages[0].probability, 100, "probability base parameter preserved");
        lane.destination = ModGateLength; seq.reset(); tick(seq);
        near(seq.tracks[0].gate, 0, "negative gate-length modulation reaches rest");
        lane.destination = ModMute; lane.values[0] = 5; seq.reset(); tick(seq);
        near(seq.tracks[0].gate, 0, "high MOD gate toggles mute on");
        seq.settings.tracks[0].mute = true; seq.reset(); tick(seq);
        near(seq.tracks[0].gate, 5, "high MOD gate toggles programmed mute off");
        lane.values[0] = 0; seq.reset(); tick(seq);
        near(seq.tracks[0].gate, 0, "low gate restores programmed mute");
    }
    {
        Sequencer seq;
        auto& lane = seq.settings.modLanes[0]; lane.length = 1;
        lane.destination = ModRatchet; lane.values[0] = 5;
        bool last = false; int rises = 0;
        for (int sample = 0; sample < 6000; ++sample) {
            seq.process(1.0/48000, true, false, false, false);
            bool gate = seq.tracks[0].gate > 0;
            if (gate && !last) ++rises;
            last = gate;
        }
        check(rises == 8, "positive ratchet modulation produces eight emitted gates per pulse");
        check(seq.settings.tracks[0].stages[0].ratchets == 1, "ratchet base parameter preserved");
    }
    {
        // Revisit each source, not just lanes 1/2. Run at different audio rates.
        for (int rate : {44100,48000,96000}) {
            Sequencer seq;
            for (int i = 0; i < 8; ++i) {
                seq.settings.modLanes[i].length = 1;
                seq.settings.modLanes[i].values[0] = (i - 4) * .5f;
            }
            for (int source = 2; source < 10; ++source) {
                seq.settings.outputSources = {{source,source}};
                seq.process(1.0/rate,true,false,false,false);
                near(seq.aux[0], (source - 6) * .5f, "every MOD source reaches A");
                near(seq.aux[1], seq.aux[0], "same lane can feed both outputs");
            }
        }
        Sequencer seq;
        seq.settings.tracks[0].stages[0].cv = -2.5;
        seq.settings.tracks[1].stages[0].cv = 3.5;
        tick(seq); near(seq.aux[0], -2.5, "old patches retain track 1 CV on A"); near(seq.aux[1], 3.5, "old patches retain track 2 CV on B");
    }
    std::cout << "PASS: " << checks << " MOD lane, routing, and output checks\n";
}
