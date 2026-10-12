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

// Clock the complete engine and inspect emitted notes, not just its counters.
void pulse(metro::Sequencer& seq, int sampleRate=48000) {
    for(int i=0;i<sampleRate/8;++i)seq.process(1.0/sampleRate,true,true,false,false);
    seq.process(1.0/sampleRate,true,true,true,false);
}
void trackModes() {
    using namespace metro;
    for(int rate : {44100,48000,96000}) {
        Sequencer seq;
        const int base[]={0,2,4,5};
        for(int s=0;s<4;++s)seq.settings.stages[s].pitch=base[s]/36.f;
        for(int t=0;t<2;++t) {
            seq.settings.tracks[t].length=4;
            seq.settings.tracks[t].stages[0].accumulation=1;
        }
        seq.settings.tracks[1].accumMode=TrackAccumulation;
        const int stageNotes[]={0,2,4,5,2,2,4,5,4,2,4,5,5,2,4,5};
        const int trackNotes[]={0,2,4,5,2,4,5,7,4,5,7,9,5,7,9,11};
        for(int i=0;i<16;++i) {
            pulse(seq,rate);
            expect(std::abs(seq.tracks[0].pitch*12-stageNotes[i])<.0001f,"p126 literal Stage-mode phrase");
            expect(std::abs(seq.tracks[1].pitch*12-trackNotes[i])<.0001f,"p127 literal Track-mode phrase");
            expect(seq.tracks[0].gate==5 && seq.tracks[1].gate==5,"modes preserve gates and independent tracks");
        }
        seq.process(1.0/rate,true,true,false,true);
        pulse(seq,rate);
        expect(seq.tracks[1].pitch==0 && seq.tracks[1].trackAccumulation==0,"manual reset starts Track accumulation from base pitch");
    }
    for(bool automatic : {false,true}) {
        Sequencer seq;auto& track=seq.settings.tracks[0];
        track.accumMode=TrackAccumulation;track.length=4;track.pulseLimit=3;
        track.autoAccumReset=automatic;track.stages[0].accumulation=1;
        for(int i=0;i<12;++i) {
            pulse(seq);
            expect(seq.tracks[0].activeAccumulation==(automatic?0:i/3),"fixed length Manual retains and Auto clears shared accumulation");
        }
    }
    // Skipped stages consume neither their eight pulses nor their increments.
    for(int order=Forward;order<=RandomOrder;++order) {
        Sequencer seq;auto& track=seq.settings.tracks[0];
        track.accumMode=TrackAccumulation;track.order=static_cast<Order>(order);
        track.stages[0].skip=true;track.stages[0].accumulation=7;
        seq.settings.stages[0].pulses=8;
        for(int i=0;i<64;++i) {
            pulse(seq);
            expect(seq.tracks[0].stage!=0 && seq.tracks[0].activeAccumulation==0,"all orders ignore skipped accumulator source");
            expect(seq.tracks[0].pulse==0,"skipped pulse count adds no silent duration");
        }
        for(auto& edit:track.stages)edit.skip=true;
        pulse(seq);expect(seq.tracks[0].gate==0,"all skipped is silent in Track mode");
        track.order=Forward;track.stages[3].skip=false;
        pulse(seq);expect(seq.tracks[0].stage==3 && seq.tracks[0].gate==5,"Track mode recovers from all skipped");
    }
    for(int suppression=0;suppression<3;++suppression) {
        Sequencer seq;auto& track=seq.settings.tracks[0];
        track.accumMode=TrackAccumulation;track.length=2;
        track.stages[0].accumulation=1;
        if(suppression==0)seq.settings.stages[0].gate=Rest;
        if(suppression==1)track.stages[0].probability=0;
        if(suppression==2)track.mute=true;
        for(int i=0;i<8;++i)pulse(seq);
        expect(seq.tracks[0].trackAccumulator==0,"rest, rejected probability and mute do not advance Track accumulator");
    }
    {
        // Zero-amount stages with ratchets must retain the sounding offset.
        Sequencer seq;auto& track=seq.settings.tracks[0];
        track.accumMode=TrackAccumulation;track.length=2;
        track.stages[0].accumulation=1;
        track.stages[1].ratchets=4;track.stages[1].accumTrigger=AccumRatchet;
        seq.settings.stages[1].pulses=3;
        for(int i=0;i<12;++i) {
            pulse(seq);
            expect(seq.tracks[0].activeAccumulation==i/4,"zero-amount stages inherit shared offset across pulses and ratchets");
        }
        track.stages[0].skip=true;
        for(int i=0;i<8;++i){pulse(seq);expect(seq.tracks[0].activeAccumulation==2,"skipping an active source freezes existing Track transposition");}
        int pending=seq.tracks[0].trackAccumulator;
        track.accumMode=StageAccumulation;pulse(seq);
        expect(seq.tracks[0].activeAccumulation==0 && seq.tracks[0].trackAccumulator==pending,"switch to Stage retains dormant Track bank");
        track.accumMode=TrackAccumulation;pulse(seq);
        expect(seq.tracks[0].activeAccumulation==2,"switch back resumes sounding Track offset");
    }
    {
        // An explicit implementation contract where the manual lacks a mixed
        // source diagram: nonzero triggers share one play-before-advance bank.
        Sequencer seq;auto& track=seq.settings.tracks[0];
        track.accumMode=TrackAccumulation;track.length=3;track.accumBipolar=true;
        track.stages[0].accumulation=2;track.stages[2].accumulation=-1;
        const int offsets[]={0,0,2,1,1,3,2,2,4};
        for(int n:offsets){pulse(seq);expect(seq.tracks[0].activeAccumulation==n,"mixed signed sources follow shared play-before-advance policy");}
    }
    for(auto order : {AccumWrap,AccumPendulum,AccumRandom,AccumHold}) {
        Sequencer seq;auto& track=seq.settings.tracks[0];
        track.accumMode=TrackAccumulation;track.accumOrder=order;track.accumBipolar=true;
        track.accumLimit=2;track.accumNegativeLimit=3;track.length=2;
        track.stages[0].accumulation=4;
        for(int i=0;i<40;++i){pulse(seq);expect(seq.tracks[0].activeAccumulation>=-3 && seq.tracks[0].activeAccumulation<=2,"all Track orders respect asymmetric limits");}
    }
}

void randomizedSkips() {
    using namespace metro;
    // Reproducible regression scenarios prompted by tester feedback. Change
    // skip masks during playback, including at the current stage and all-skip.
    for(auto mode : {StageAccumulation,TrackAccumulation})
    for(int order=Forward;order<=RandomOrder;++order)
    for(uint32_t seed=1;seed<=16;++seed) {
        RandomSource edits;edits.state=seed*739+order*17;
        Sequencer seq;seq.seed(seed);auto& config=seq.settings.tracks[0];
        config.accumMode=mode;config.order=static_cast<Order>(order);
        config.length=1+edits.index(8);config.offset=edits.index(8);
        config.accumBipolar=true;config.accumLimit=7;config.accumNegativeLimit=5;
        for(int s=0;s<8;++s) {
            seq.settings.stages[s].pitch=edits.index(36)/36.f;
            seq.settings.stages[s].pulses=1+edits.index(8);
            config.stages[s].accumulation=edits.index(9)-4;
        }
        for(int tick=0;tick<64;++tick) {
            if(tick%8==0)for(auto& edit:config.stages)edit.skip=edits.index(2)!=0;
            if(tick==24)for(auto& edit:config.stages)edit.skip=true;
            if(tick==32)for(auto& edit:config.stages)edit.skip=false;
            const auto before=seq.tracks[0];
            for(int i=0;i<16;++i)seq.process(.001,true,true,false,false);
            seq.process(.001,true,true,true,false);
            const auto& after=seq.tracks[0];
            expect(after.stage<0 || !config.stages[after.stage].skip,"random live skips never play a disabled stage");
            expect(std::isfinite(after.pitch) && std::abs(after.pitch)<10,"random skip/accumulator interaction keeps pitch finite and bounded");
            if(tick>=24 && tick<32)expect(after.gate==0 && after.stage==-1,"random all-skip interval is silent");
            for(int s=0;s<8;++s)
                if(config.stages[s].skip)expect(after.accumulator[s]==before.accumulator[s],"live-skipped stages never advance their accumulator");
            if(after.stage<0 || after.pulse!=0 || config.stages[after.stage].accumulation==0)
                expect(after.trackAccumulator==before.trackAccumulator,"Track bank advances only on an enabled nonzero stage trigger");
            if(before.stage>=0 && !config.stages[before.stage].skip && before.pulse+1<seq.settings.stages[before.stage].pulses)
                expect(after.stage==before.stage && after.pulse==before.pulse+1,"unskipped multi-pulse stages retain their duration during live edits");
        }
    }
}

int main() {
    using namespace metro;
    trackModes();
    randomizedSkips();
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
    for (auto mode : {StageAccumulation, TrackAccumulation})
    for (int gateType = Rest; gateType <= Hold; ++gateType) {
        for (int trigger = AccumStage; trigger <= AccumRatchet; ++trigger) {
            Sequencer seq;
            auto& track = seq.settings.tracks[0];
            track.accumMode = mode;
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
