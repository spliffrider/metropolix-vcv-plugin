#include "../src/Sequencer.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

int assertions = 0;
void expect(bool value, const std::string& label) {
    ++assertions;
    if (!value) { std::cerr << "FAIL: " << label << '\n'; std::exit(1); }
}
void tick(metro::Sequencer& s) { s.process(0.002, true, true, true, false); }

void orderExample(metro::Order order, const std::vector<int>& printed, const char* reference) {
    for (bool reverse : {false, true}) {
        metro::Sequencer s;
        s.settings.tracks[0].order = order;
        s.settings.tracks[0].reversed = reverse;
        // Three repetitions also test each documented repeat boundary.
        for (int i = 0; i < static_cast<int>(printed.size()) * 3; ++i) {
            int expected = printed[i % printed.size()] - 1;
            if (reverse) expected = 7 - expected;
            tick(s);
            expect(s.tracks[0].stage == expected, std::string(reference) + " event " + std::to_string(i));
        }
    }
}

struct Window { double start, end; };
// Expected intervals are literal transcriptions of the manual's diagrams in
// clock-pulse units. The oracle contains no implementation gate formulas.
void gateExample(metro::Gate type, int count, int division, bool stretch, metro::RatchetMode mode,
                 int ratchets, float length, std::initializer_list<Window> expected, const char* reference) {
    for (int sample = 0; sample < count * 960; ++sample) {
        double position = (sample + 0.37) / 960.0; // sample inside cells, away from rounded boundaries
        int pulse = static_cast<int>(position);
        bool target = false;
        for (const auto& window : expected) if (position >= window.start && position < window.end) target = true;
        bool actual = metro::gateHigh(type, pulse, position - pulse, count, division, stretch, mode, ratchets, length, 0.125);
        expect(actual == target, std::string(reference) + " at pulse " + std::to_string(position));
    }
}

int main() {
    using namespace metro;
    orderExample(Forward, {1,2,3,4,5,6,7,8}, "p71 Linear");
    orderExample(PingPong, {1,2,3,4,5,6,7,8,8,7,6,5,4,3,2,1}, "p71 Ping Pong");
    orderExample(Pendulum, {1,2,3,4,5,6,7,8,7,6,5,4,3,2}, "p71 Pendulum");
    orderExample(OddEven, {1,3,5,7,2,4,6,8}, "p71 Odd/Even");
    orderExample(Jump2, {1,3,5,7}, "p71 Jump 2");
    orderExample(Jump3, {1,4,7,2,5,8,3,6}, "p71 Jump 3");
    orderExample(Jump4, {1,5}, "p72 Jump 4");
    orderExample(Jump5, {1,6,3,8,5,2,7,4}, "p72 Jump 5");
    orderExample(Hopscotch, {1,3,2,4,3,5,4,6,5,7,6,8,7,1,8,2}, "p72 Hopscotch");
    orderExample(Arp3Shift1, {1,2,3,2,3,4,3,4,5,4,5,6,5,6,7,6,7,8,7,8,1,8,1,2}, "p72 Arp 3 Shift -1");
    orderExample(Arp4Shift2, {1,2,3,4,2,3,4,5,3,4,5,6,4,5,6,7,5,6,7,8,6,7,8,1,7,8,1,2,8,1,2,3}, "p72 Arp 4 Shift -2");
    orderExample(Arp4Shift1, {1,2,3,4,3,4,5,6,5,6,7,8,7,8,1,2}, "p72 Arp 4 Shift -1");
    orderExample(Converge, {1,8,2,7,3,6,4,5}, "p72 Converge");
    orderExample(Diverge, {4,5,3,6,2,7,1,8}, "p72 Diverge");
    orderExample(Growing, {1,1,2,1,2,3,1,2,3,4,1,2,3,4,5,1,2,3,4,5,6,1,2,3,4,5,6,7,1,2,3,4,5,6,7,8}, "p73 Growing");
    orderExample(Shrinking, {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,1,2,3,4,5,6,1,2,3,4,5,1,2,3,4,1,2,3,1,2,1}, "p73 Shrinking");
    orderExample(PedalPoint, {1,2,1,3,1,4,1,5,1,6,1,7,1,8}, "p73 Pedal Point");

    gateExample(Multi, 4, 2, false, MultiplyRatchets, 2, .5f,
        {{0,.25},{.5,.75},{2,2.25},{2.5,2.75}}, "p94 Multi /2, ratchet 2, stretch off");
    gateExample(Single, 4, 1, false, MultiplyRatchets, 4, .5f,
        {{0,.125},{.25,.375},{.5,.625},{.75,.875}}, "p94 Single, ratchet 4, stretch off");
    gateExample(Multi, 4, 2, true, MultiplyRatchets, 4, .5f,
        {{0,.25},{.5,.75},{1,1.25},{1.5,1.75},{2,2.25},{2.5,2.75},{3,3.25},{3.5,3.75}}, "p95 Multiply group 5");
    gateExample(Single, 3, 1, true, MultiplyRatchets, 4, .5f,
        {{0,.375},{.75,1.125},{1.5,1.875},{2.25,2.625}}, "p95 Multiply group 9");
    gateExample(Single, 4, 1, true, PulseRatchets, 1, .5f,
        {{0,2}}, "p96 Pulse count 1 still stretches");
    gateExample(Single, 4, 1, true, PulseRatchets, 4, .5f,
        {{0,.125},{.25,.375},{.5,.625},{.75,.875}}, "p96 Pulse group 24");
    gateExample(Multi, 4, 2, true, PulseRatchets, 2, .5f,
        {{0,.25},{.5,.75},{2,2.25},{2.5,2.75}}, "p96 Pulse group 19");
    gateExample(Multi, 4, 1, true, GatedRatchets, 2, .5f,
        {{0,.25},{1,1.25},{2,2.25},{3,3.25}}, "p97 Gated group 11");
    gateExample(Single, 3, 1, true, GatedRatchets, 2, .5f,
        {{0,.25},{.5,.75},{1,1.25}}, "p97 Gated group 16");
    gateExample(Single, 4, 1, true, GatedRatchets, 4, .5f,
        {{0,.125},{.25,.375},{.5,.625},{.75,.875},{1,1.125},{1.25,1.375},{1.5,1.625},{1.75,1.875}}, "p97 Gated group 17");
    gateExample(Single, 2, 1, false, MultiplyRatchets, 1, .75f,
        {{0,.75}}, "p166 two-pulse note, stretch off");
    gateExample(Single, 2, 1, true, MultiplyRatchets, 1, .75f,
        {{0,1.5}}, "p166 two-pulse note, stretch on");
    gateExample(Multi, 4, 3, true, MultiplyRatchets, 1, .75f,
        {{0,2.25},{3,4.0-1.0/96.0}}, "p167 row 4 partial group truncates at stage boundary");

    {
        Sequencer s;
        s.settings.tracks[0].length = 3;
        s.settings.tracks[0].pulseLimit = 4;
        int pattern[] = {0,1,2,0,0,1,2,0};
        for (int stage : pattern) { tick(s); expect(s.tracks[0].stage == stage, "p75 fixed length wraps then resets"); }
        s.settings.stages[1].pulses = 4;
        s.reset();
        int truncated[] = {0,1,1,1,0,1,1,1};
        for (int stage : truncated) { tick(s); expect(s.tracks[0].stage == stage, "p75 fixed length truncates last stage"); }
    }
    {
        Sequencer s;
        s.settings.tracks[0].length = 1;
        s.settings.tracks[0].pulseLimit = 2;
        s.settings.tracks[0].stages[0].accumulation = 1;
        for (int i = 0; i < 5; ++i) { tick(s); expect(s.tracks[0].activeAccumulation == i, "p134 Manual ignores automatic reset"); }
        s.settings.tracks[0].autoAccumReset = true;
        s.reset();
        for (int i = 0; i < 6; ++i) { tick(s); expect(s.tracks[0].activeAccumulation == i % 2, "p134 Auto resets at fixed pulse length"); }
    }
    {
        Sequencer s;
        s.seed(712345);
        s.settings.tracks[0].order = Shuffle;
        int sequence[8];
        bool seen[8] = {};
        for (int i = 0; i < 8; ++i) { tick(s); sequence[i] = s.tracks[0].stage; seen[sequence[i]] = true; }
        for (bool present : seen) expect(present, "p73 Shuffle visits every stage");
        for (int i = 0; i < 24; ++i) { tick(s); expect(s.tracks[0].stage == sequence[i % 8], "p73 Shuffle repeats same permutation"); }
        s.reset();
        bool changed = false;
        for (int i = 0; i < 8; ++i) { tick(s); changed |= s.tracks[0].stage != sequence[i]; }
        expect(changed, "p73 reset reshuffles (fixed test seed)");
        s.settings.tracks[0].order = QuasiRandom;
        s.reset();
        int previous = -1;
        for (int i = 0; i < 2000; ++i) { tick(s); expect(s.tracks[0].stage != previous, "p73 Quasi Random avoids consecutive repeats"); previous = s.tracks[0].stage; }
    }
    {
        // Statistical contract is from p73, independent of the RNG algorithm.
        Sequencer s;
        s.seed(92173);
        s.settings.tracks[0].order = Brownian;
        tick(s);
        int previous = s.tracks[0].stage, forward = 0, back = 0, stay = 0;
        for (int i = 0; i < 30000; ++i) {
            tick(s);
            int delta = modulo(s.tracks[0].stage - previous, 8);
            if (delta == 1) ++forward; else if (delta == 7) ++back; else if (delta == 0) ++stay;
            else expect(false, "p73 Brownian only moves one stage");
            previous = s.tracks[0].stage;
        }
        expect(forward > 14500 && forward < 15500, "p73 Brownian roughly 50% forward");
        expect(back > 7100 && back < 7900 && stay > 7100 && stay < 7900, "p73 Brownian roughly 25% reverse and stay");
    }
    {
        for (int order = 0; order <= 20; ++order) for (int length = 1; length <= 8; ++length) {
            Sequencer s;
            s.settings.tracks[0].order = static_cast<Order>(order);
            s.settings.tracks[0].length = length;
            s.settings.tracks[0].offset = 7;
            for (int i = 0; i < 100; ++i) {
                tick(s);
                expect(modulo(s.tracks[0].stage - 7, 8) < length, "every order stays within its stage window");
            }
            for (auto& stage : s.settings.tracks[0].stages) stage.skip = true;
            tick(s);
            expect(s.tracks[0].stage == -1 && s.tracks[0].gate == 0, "every order handles all stages skipped");
        }
    }
    std::cout << "PASS: " << assertions << " manual-derived and boundary assertions\n";
}
