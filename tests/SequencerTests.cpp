#include "../src/Sequencer.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

int checks = 0;
void check(bool condition, const std::string& message) {
    ++checks;
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
void near(float actual, float expected, const std::string& message) {
    check(std::abs(actual - expected) < 0.001f, message);
}
void tick(metro::Sequencer& s) { s.process(0.002, true, true, true, false); }
void wait(metro::Sequencer& s, double seconds) {
    for (int i = 0; i < static_cast<int>(seconds * 48000); ++i)
        s.process(1.0 / 48000, true, true, false, false);
}

int main() {
    {
        metro::Sequencer s;
        s.settings.stages[0].pulses = 3;
        tick(s);
        check(s.tracks[0].stage == 0 && s.tracks[0].pulse == 0, "first external edge starts first stage");
        tick(s); check(s.tracks[0].stage == 0 && s.tracks[0].pulse == 1, "second pulse stays at first stage");
        tick(s); check(s.tracks[0].stage == 0 && s.tracks[0].pulse == 2, "third pulse stays at first stage");
        tick(s); check(s.tracks[0].stage == 1 && s.tracks[0].pulse == 0, "pulse count advances exactly at boundary");
        s.process(0.001, false, true, false, false);
        near(s.tracks[0].gate, 0, "stop immediately closes gate");
        tick(s); check(s.tracks[0].stage == 0, "run restarts traversal");
    }
    {
        metro::Sequencer s;
        s.settings.tracks[1].division = 3;
        s.settings.tracks[1].length = 2;
        s.settings.tracks[1].offset = 6;
        tick(s); check(s.tracks[0].stage == 0 && s.tracks[1].stage == 6, "independent offsets");
        tick(s); tick(s);
        check(s.tracks[0].stage == 2 && s.tracks[1].stage == 6, "independent clock division");
        tick(s); check(s.tracks[1].stage == 7, "divided track advances on fourth global edge");
        tick(s); tick(s); tick(s);
        check(s.tracks[1].stage == 6, "independent track length wraps");
    }
    {
        metro::Sequencer s;
        s.settings.tracks[0].reversed = true;
        s.settings.tracks[0].length = 3;
        int expected[] = {2, 1, 0, 2, 1};
        for (int stage : expected) { tick(s); check(s.tracks[0].stage == stage, "reverse traversal"); }
        s.settings.tracks[0].order = metro::Pendulum;
        s.settings.tracks[0].reversed = false;
        s.reset();
        int pendulum[] = {0, 1, 2, 1, 0, 1, 2};
        for (int stage : pendulum) { tick(s); check(s.tracks[0].stage == stage, "pendulum does not repeat endpoints"); }
        s.settings.tracks[0].length = 1;
        for (int i = 0; i < 5; ++i) { tick(s); check(s.tracks[0].stage == 0, "one-stage pendulum is bounded"); }
    }
    {
        metro::Sequencer s;
        s.settings.tracks[0].stages[0].skip = true;
        tick(s); check(s.tracks[0].stage == 1, "skip removes a stage including its pulses");
        for (auto& edit : s.settings.tracks[0].stages) edit.skip = true;
        tick(s); check(s.tracks[0].stage == -1 && s.tracks[0].gate == 0, "all-skipped track is silent and bounded");
        s.settings.tracks[0].stages[5].skip = false;
        tick(s); check(s.tracks[0].stage == 5, "all-skipped track recovers after unskip");
        check(s.tracks[1].stage == 2, "skips on track 1 do not affect track 2");
    }
    {
        metro::Sequencer s;
        s.settings.stages[0].gate = metro::Rest;
        s.settings.stages[0].pulses = 2;
        tick(s); near(s.tracks[0].gate, 0, "rest is silent");
        tick(s); check(s.tracks[0].stage == 0, "rest retains pulse duration");
        tick(s); check(s.tracks[0].stage == 1, "rest eventually advances");
    }
    {
        metro::Sequencer s;
        s.settings.stages[0].gate = metro::Single;
        s.settings.stages[0].pulses = 3;
        tick(s); near(s.tracks[0].gate, 5, "single gates on first pulse");
        wait(s, 0.07); near(s.tracks[0].gate, 0, "single closes after gate length");
        tick(s); near(s.tracks[0].gate, 0, "single does not retrigger on second pulse");
    }
    {
        metro::Sequencer s;
        s.settings.stages[0].gate = metro::Hold;
        s.settings.stages[0].pulses = 3;
        tick(s); wait(s, 0.1); near(s.tracks[0].gate, 5, "hold ignores fractional gate length");
        tick(s); near(s.tracks[0].gate, 5, "hold bridges pulses");
        s.settings.tracks[0].mute = true;
        wait(s, 0.001); near(s.tracks[0].gate, 0, "mute closes hold gate");
    }
    {
        metro::Sequencer s;
        s.settings.tracks[0].stages[0].ratchets = 4;
        s.process(1.0 / 48000, true, true, true, false);
        int rises = s.tracks[0].gate > 0 ? 1 : 0;
        bool previous = s.tracks[0].gate > 0;
        for (int i = 0; i < 12000; ++i) {
            s.process(1.0 / 48000, true, true, false, false);
            bool high = s.tracks[0].gate > 0;
            if (high && !previous) ++rises;
            previous = high;
        }
        check(rises == 4, "four ratchets emit exactly four gates, even when the clock stops");
        near(s.tracks[0].gate, 0, "non-hold gate expires when external clock stops");
    }
    {
        metro::Sequencer s;
        s.settings.tracks[0].length = 1;
        s.settings.tracks[0].stages[0].probability = 0;
        s.settings.tracks[0].stages[0].accumulation = 4;
        for (int i = 0; i < 20; ++i) { tick(s); near(s.tracks[0].gate, 0, "zero probability always suppresses gate"); }
        check(s.tracks[0].accumulator[0] == 0, "suppressed stage does not accumulate");
        s.settings.tracks[0].stages[0].probability = 100;
        tick(s); near(s.tracks[0].gate, 5, "full probability always plays");
    }
    {
        check(metro::degreeToSemitone(-1, 1) == -1, "negative scale degrees use previous octave");
        check(metro::degreeToSemitone(7, 1) == 12, "seven major degrees equal an octave");
        check(metro::degreeToSemitone(4, 1) == 7, "four major degrees equal a fifth");
        metro::Sequencer s;
        s.settings.tracks[0].length = 1;
        s.settings.tracks[0].stages[0].accumulation = 4;
        s.settings.tracks[0].accumLimit = 9;
        int expected[] = {0, 4, 8, 2, 6, 0};
        for (int degree : expected) {
            tick(s);
            near(s.tracks[0].pitch, metro::degreeToSemitone(degree, 1) / 12.f, "inclusive accumulator wrap preserves overshoot");
        }
        s.reset();
        s.settings.tracks[0].stages[0].accumulation = -4;
        for (int degree : expected) {
            tick(s);
            near(s.tracks[0].pitch, metro::degreeToSemitone(-degree, 1) / 12.f, "negative accumulation wraps below root");
        }
    }
    {
        metro::Sequencer s;
        s.settings.tracks[0].stages[0].cv = 3.2f;
        s.settings.tracks[1].stages[0].cv = -2.4f;
        tick(s);
        near(s.tracks[0].cv, 3.2f, "track 1 stage CV");
        near(s.tracks[1].cv, -2.4f, "track 2 stage CV independent");
        s.settings.tracks[0].mute = true;
        wait(s, 0.001);
        near(s.tracks[0].cv, 0, "mute zeros stage CV");
        near(s.tracks[1].cv, -2.4f, "track 1 mute preserves track 2 CV");
    }
    {
        metro::Sequencer s;
        tick(s); tick(s);
        s.process(0.00001, true, true, true, true);
        check(s.tracks[0].stage == -1, "reset wins over simultaneous external clock");
        s.process(0.0005, true, true, true, false);
        check(s.tracks[0].stage == -1, "clock ignored within reset guard window");
        wait(s, 0.002);
        tick(s); check(s.tracks[0].stage == 0, "first edge after reset starts first stage");
    }
    {
        metro::Sequencer s;
        s.process(1.0 / 48000, true, false, false, false);
        check(s.tracks[0].stage == 0 && s.tracks[0].gate == 5, "internal RUN starts immediately");
        for (int i = 0; i < 6001; ++i) s.process(1.0 / 48000, true, false, false, false);
        check(s.tracks[0].stage == 1, "120 BPM advances every 125 milliseconds");
        s.process(1.0 / 48000, true, false, false, true);
        check(s.tracks[0].stage == 0, "internal reset starts stage 1 immediately");
    }
    {
        metro::Sequencer s;
        s.settings.stages[1].pitch = 1.f / 3.f;
        s.settings.tracks[0].stages[1].slide = true;
        s.settings.tracks[0].slideTime = 1.f;
        tick(s); wait(s, 0.123); tick(s);
        float start = s.tracks[0].pitch;
        wait(s, 0.05);
        check(start < 0.01f && s.tracks[0].pitch > 0.2f && s.tracks[0].pitch < 0.8f, "slide interpolates over a pulse");
        wait(s, 0.1); near(s.tracks[0].pitch, 1.f, "slide reaches next target pitch");
    }
    {
        // Stress live changes at boundaries, including all-skipped patterns.
        metro::Sequencer s;
        for (int i = 0; i < 10000; ++i) {
            for (int t = 0; t < 2; ++t) {
                auto& c = s.settings.tracks[t];
                c.length = i % 8 + 1;
                c.offset = (i / 8 + t) % 8;
                c.order = static_cast<metro::Order>(i % 3);
                c.division = i % 64 + 1;
                for (int k = 0; k < 8; ++k) c.stages[k].skip = ((i + k) % 9 == 0) || i % 13 == 0;
            }
            tick(s);
            for (const auto& t : s.tracks) {
                check(t.stage >= -1 && t.stage < 8, "live edits never index outside eight stages");
                check(std::isfinite(t.pitch) && std::isfinite(t.gate), "outputs remain finite under live edits");
            }
        }
    }
    std::cout << "PASS: " << checks << " sequencer checks\n";
}
