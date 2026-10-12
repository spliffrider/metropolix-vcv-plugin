#include "../src/Sequencer.hpp"
#include <iostream>
#include <initializer_list>
#include <cstdlib>

static int checks = 0;
void expect(bool result, const char* message) {
    ++checks;
    if (!result) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
void diagram(metro::AccumOrder order, bool bipolar, int amount, int positive, int negative,
             std::initializer_list<int> pitches, const char* name) {
    metro::RandomSource random;
    int value = 0, direction = 1;
    for (int expected : pitches) {
        expect(value == expected, name);
        value = metro::advanceAccumulator(value, amount, positive, negative, bipolar, order, direction, random);
    }
}

int main() {
    using namespace metro;
    diagram(AccumWrap, false, 4, 9, 5, {0,4,8,2,6,0,4}, "p128 unipolar wrap diagram");
    diagram(AccumWrap, false, -4, 5, 9, {0,-4,-8,-2,-6,0,-4}, "p128 negative wrap uses negative limit");
    diagram(AccumWrap, true, 4, 9, 5, {0,4,8,-3,1,5,9,-2,2,6,-5,-1,3,7,-4,0}, "p129 bipolar wrap diagram");
    diagram(AccumPendulum, false, 4, 9, 5, {0,4,8,6,2,2,6,8,4,0}, "p130 unipolar pendulum diagram");
    diagram(AccumPendulum, true, 4, 9, 5, {0,4,8,6,2,-2,-4,0,4,8}, "p131 bipolar pendulum diagram");
    diagram(AccumHold, true, 4, 9, 5, {0,4,8,9,9,9}, "p133 hold remains at positive limit");
    diagram(AccumHold, true, -4, 9, 5, {0,-4,-5,-5,-5}, "p133 hold remains at negative limit");
    {
        RandomSource random;
        random.state = 721739;
        int positive = 0, negative = 0, stay = 0;
        for (int i = 0; i < 50000; ++i) {
            int direction = 1;
            int next = advanceAccumulator(0, 4, 28, 28, true, AccumRandom, direction, random);
            expect(next >= -4 && next <= 4, "p131 random step never exceeds amount");
            if (next > 0) ++positive; else if (next < 0) ++negative; else ++stay;
        }
        expect(positive > 24200 && positive < 25800, "p131 random 50% forward");
        expect(negative > 14300 && negative < 15700, "p131 random 30% backward");
        expect(stay > 9400 && stay < 10600, "p131 random 20% unchanged");
    }
    // Literal gate/trigger matrix from p102, sampled through the complete engine.
    // 48 kHz, 120 BPM, two pulses per stage and four ratchets per pulse.
    for (int gateType = Rest; gateType <= Hold; ++gateType) {
        for (int trigger = AccumStage; trigger <= AccumRatchet; ++trigger) {
            Sequencer seq;
            auto& track = seq.settings.tracks[0];
            track.length = 1;
            track.accumLimit = 28;
            track.stages[0].accumulation = 1;
            track.stages[0].accumTrigger = static_cast<AccumTrigger>(trigger);
            track.stages[0].ratchets = 4;
            seq.settings.stages[0].pulses = 2;
            seq.settings.stages[0].gate = static_cast<Gate>(gateType);
            for (int sample = 0; sample < 36000; ++sample) {
                seq.process(1.0/48000, true, false, false, false);
                if (sample % 1500 != 10) continue;
                int event = sample / 1500;
                int stage = event / 8;
                int within = event % 8;
                int expected = 0;
                if (gateType == Hold) expected = stage;
                if (gateType == Single) expected = trigger == AccumRatchet ? stage * 4 + std::min(3, within) : stage;
                if (gateType == Multi) expected = trigger == AccumRatchet ? event : trigger == AccumPulse ? event / 4 : stage;
                expect(seq.tracks[0].activeAccumulation == expected, "p102 gate and trigger matrix");
            }
        }
    }
    for (int trigger = AccumStage; trigger <= AccumRatchet; ++trigger) {
        Sequencer seq;
        seq.settings.tracks[0].length = 1;
        auto& edit = seq.settings.tracks[0].stages[0];
        edit.accumTrigger = static_cast<AccumTrigger>(trigger);
        edit.accumulation = 1;
        edit.ratchets = 4;
        edit.probability = 0;
        edit.probabilityPerPulse = true;
        for (int i = 0; i < 30000; ++i) seq.process(1.0/48000, true, false, false, false);
        expect(seq.tracks[0].accumulator[0] == 0 && seq.tracks[0].gate == 0, "p102 rejected pulses do not accumulate");
    }
    // Stage probability makes one decision for the whole stage. Pulse target
    // must also produce partially played stages (fixed seed, wide bounds).
    for (bool perPulse : {false, true}) {
        Sequencer seq;
        seq.seed(275438);
        seq.settings.tracks[0].length = 1;
        seq.settings.stages[0].pulses = 4;
        seq.settings.tracks[0].stages[0].probability = 50;
        seq.settings.tracks[0].stages[0].probabilityPerPulse = perPulse;
        int mixed = 0, played = 0;
        for (int sample = 0; sample < 2400000; ++sample) {
            seq.process(1.0/48000, true, false, false, false);
            if (sample % 6000 == 10) {
                played += seq.tracks[0].allowed ? 1 : 0;
                if ((sample / 6000) % 4 == 3) { if (played > 0 && played < 4) ++mixed; played = 0; }
            }
        }
        expect(perPulse ? mixed > 60 : mixed == 0, "p98 probability target controls stage versus pulse decisions");
    }
    for (int order = AccumWrap; order <= AccumHold; ++order) {
        for (int amount = -28; amount <= 28; ++amount) {
            RandomSource random;
            int value = 0, direction = 1;
            for (int i = 0; i < 100; ++i) {
                value = advanceAccumulator(value, amount, 1, 1, true, static_cast<AccumOrder>(order), direction, random);
                expect(value >= -1 && value <= 1, "large amounts stay within narrow limits");
            }
        }
    }
    std::cout << "PASS: " << checks << " accumulator and probability assertions\n";
}
