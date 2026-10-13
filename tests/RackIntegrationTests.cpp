// This executable is a minimal test host, so it constructs Rack's engine.
// The plugin itself only uses the public plugin API.
#include <engine/Engine.hpp>
#undef PRIVATE
#include "../src/Metropolix.hpp"
#include <iostream>
#include <cstdlib>

void require(bool value, const char* message) {
    if (!value) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}

#include "PresetRackTests.hpp"
#include "ChainRackTests.hpp"
#include "SwingRackTests.hpp"
#include "OverrideRackTests.hpp"
#include "LoopyRackTests.hpp"
#include "ScaleRackTests.hpp"
#include "SlideRackTests.hpp"
#include "StageCvRackTests.hpp"
#include "ClockOutputRackTests.hpp"
#include "OutputRoutingRackTests.hpp"
#include "TrackOptionsRackTests.hpp"
#include "AccumulatorRoutingRackTests.hpp"

int main() {
    static_assert(Metropolix::OUTPUT_SOURCE_A==402 && Metropolix::OUTPUT_SOURCE_B==403,"legacy parameters stay at their original IDs");
    static_assert(Metropolix::AUX_PARAMS==404 && Metropolix::ACCUM_MODE==413 && Metropolix::CTRL_PARAMS==415 && Metropolix::TOTAL_PULSES==421 && Metropolix::RUN_MODE==428 && Metropolix::SWING_1==429 && Metropolix::GATE_SCALE_1==431 && Metropolix::STAGE_OVERRIDES==433 && Metropolix::LOOPY_PULSES==465 && Metropolix::SCALE_LOCK==467 && Metropolix::SLIDE_TYPE_1==685 && Metropolix::CV_TYPE_1==687 && Metropolix::CLOCK_OUT_DIV==705 && Metropolix::OUTPUT_TRACK_OPTIONS==707 && Metropolix::NUM_PARAMS==719,"CV parameters append without changing existing IDs");
    static_assert(Metropolix::CLOCK_IN==0 && Metropolix::RESET_IN==1 && Metropolix::AUX_X_IN==2 && Metropolix::NUM_OUTPUTS==7,"legacy cable endpoints preserved");
    rack::random::init();
    rack::Context context;
    rack::contextSet(&context);
    context.engine = new rack::engine::Engine;
    {
        Metropolix source;
        source.params[Metropolix::TEMPO].setValue(141.5f);
        source.params[Metropolix::stageParam(1, 4, Metropolix::RATCHETS)].setValue(4.f);
        source.params[Metropolix::stageParam(1, 4, Metropolix::CV)].setValue(-2.75f);
        source.selectedTrack.store(1);
        source.selectedStage.store(4);
        source.selectedRow.store(Metropolix::RATCHETS);
        source.expanded.store(true);
        source.modEditor.store(true);
        source.selectedLane.store(6);
        source.uiPage.store(Metropolix::LENGTH_PAGE);
        source.uiField.store(1);
        source.uiMod.store(true);
        source.uiEdit.store(true);
        source.uiHome.store(3);
        source.uiStageDetail.store(true);
        source.selectedAux.store(2);
        for(int input=0;input<3;++input) {
            source.params[Metropolix::auxParam(input,Metropolix::AUX_AMOUNT)].setValue(-.75f+input*.5f);
            source.params[Metropolix::auxParam(input,Metropolix::AUX_DESTINATION)].setValue(metro::ModPitchPost+input);
            source.params[Metropolix::auxParam(input,Metropolix::AUX_ASSIGNMENT)].setValue(input);
        }
        source.params[Metropolix::modParam(6, Metropolix::MOD_VALUE + 3)].setValue(-2.5f);
        source.params[Metropolix::modParam(6, Metropolix::MOD_RAMP + 3)].setValue(1.f);
        source.params[Metropolix::modParam(6, Metropolix::MOD_DESTINATION)].setValue(metro::ModPitchPre);
        source.params[Metropolix::OUTPUT_SOURCE_A].setValue(8.f);
        source.params[Metropolix::extraParam(1, Metropolix::PULSE_DIV)].setValue(3.f);
        source.params[Metropolix::extraParam(1, Metropolix::GATE_STRETCH)].setValue(1.f);
        source.params[Metropolix::accumParam(1, Metropolix::NEGATIVE_LIMIT)].setValue(5.f);
        source.params[Metropolix::accumParam(1, Metropolix::BIPOLAR)].setValue(1.f);
        source.params[Metropolix::accumParam(1, Metropolix::ACCUM_ORDER)].setValue(metro::AccumPendulum);
        source.params[Metropolix::accumModeParam(1)].setValue(metro::TrackAccumulation);
        source.params[Metropolix::stageOption(1, 4, Metropolix::PROB_TARGET)].setValue(1.f);
        source.params[Metropolix::stageOption(1, 4, Metropolix::ACCUM_TRIGGER)].setValue(metro::AccumRatchet);
        json_t* params = source.paramsToJson();
        json_t* data = source.dataToJson();
        Metropolix restored;
        restored.paramsFromJson(params);
        restored.dataFromJson(data);
        json_decref(params);
        json_decref(data);
        require(restored.params[Metropolix::TEMPO].getValue() == 141.5f, "Rack saves tempo exactly");
        require(restored.params[Metropolix::stageParam(1, 4, Metropolix::RATCHETS)].getValue() == 4.f, "Rack saves track 2 stage edit");
        require(restored.params[Metropolix::stageParam(1, 4, Metropolix::CV)].getValue() == -2.75f, "Rack saves bipolar stage CV");
        require(restored.selectedTrack.load() == 1 && restored.selectedStage.load() == 4 && restored.expanded.load(), "UI selection and expanded state survive JSON round trip");
        require(restored.sequencer.settings.tracks[1].pulseDivision == 3 && restored.sequencer.settings.tracks[1].gateStretch, "extended track settings survive JSON round trip");
        for (int id = 0; id < Metropolix::NUM_PARAMS; ++id)
            require(source.params[id].getValue() == restored.params[id].getValue(), "every parameter survives Rack serialization");
        require(restored.modEditor.load() && restored.selectedLane.load() == 6, "MOD editor state persists");
        require(restored.selectedAux.load()==2 && restored.sequencer.settings.auxInputs[2].amount==.25f,"AUX selection and parameters survive serialization");
        require(restored.uiPage.load()==Metropolix::LENGTH_PAGE && restored.uiField.load()==1 && restored.uiMod.load() && restored.uiEdit.load(), "hardware UI page, field and edit target persist");
        require(restored.uiHome.load()==3 && restored.uiStageDetail.load(), "OLED home and detail selection persist");
        require(restored.sequencer.settings.modLanes[6].ramps[3] && restored.sequencer.settings.modLanes[6].values[3] == -2.5f
            && restored.sequencer.settings.outputSources[0] == 8, "MOD lane and output routing reach the engine");
        const auto& restoredTrack = restored.sequencer.settings.tracks[1];
        require(restoredTrack.accumNegativeLimit == 5 && restoredTrack.accumBipolar && restoredTrack.accumOrder == metro::AccumPendulum, "accumulator options reach the engine");
        require(restoredTrack.accumMode == metro::TrackAccumulation && restored.sequencer.settings.tracks[0].accumMode == metro::StageAccumulation, "independent accumulator modes survive save/load");
        json_t* oldMode = json_pack("{s:i}", "schemaVersion", 7);
        restored.dataFromJson(oldMode); json_decref(oldMode);
        require(restored.sequencer.settings.tracks[1].accumMode == metro::StageAccumulation, "2.4 preset loaded over Track mode restores Stage mode");
        require(restored.params[Metropolix::auxParam(2,Metropolix::AUX_AMOUNT)].getValue()==.25f, "mode migration preserves existing AUX settings");
        require(restoredTrack.stages[4].probabilityPerPulse && restoredTrack.stages[4].accumTrigger == metro::AccumRatchet, "stage trigger options reach the engine");
        restored.params[Metropolix::trackParam(0, Metropolix::ACCUM_LIMIT)].setValue(7.f);
        restored.params[Metropolix::trackParam(0, Metropolix::ORDER)].setValue(1.f);
        json_t* legacy = json_pack("{s:i}", "schemaVersion", 1);
        restored.dataFromJson(legacy);
        json_decref(legacy);
        require(restored.sequencer.settings.tracks[0].order == metro::Forward && restored.sequencer.settings.tracks[0].reversed, "legacy Reverse order is migrated without changing playback");
        require(restored.sequencer.settings.tracks[0].accumNegativeLimit == 7, "legacy shared accumulator limit migrates to negative limit");
        for(int id=Metropolix::AUX_PARAMS;id<Metropolix::TOTAL_PULSES;++id)
            require(restored.params[id].getValue()==0.f,"loading old preset over existing module clears AUX routing");
        json_t* malformed = json_pack("{s:i, s:i, s:i}", "selectedTrack", 100, "selectedStage", -20, "selectedRow", 77);
        restored.dataFromJson(malformed);
        json_decref(malformed);
        require(restored.selectedTrack.load() == 1 && restored.selectedStage.load() == 0 && restored.selectedRow.load() == 7, "out-of-range saved UI state is clamped");
        json_t* previousUi=json_pack("{s:i,s:i,s:i}","schemaVersion",5,"uiPage",Metropolix::LENGTH_PAGE,"uiField",1);
        restored.dataFromJson(previousUi);json_decref(previousUi);
        require(restored.uiField.load()==0 && restored.uiHome.load()==0, "old LEN offset selection migrates without selecting pulse length");
    }
    {
        Metropolix module;
        Module::ProcessArgs args;
        args.sampleRate = 48000.f;
        args.sampleTime = 1.f / 48000.f;
        module.params[Metropolix::RUN].setValue(1.f);
        module.process(args);
        require(module.outputs[Metropolix::GATE_1].getVoltage() == 5.f, "Rack module emits first internal gate immediately");
        for (int i = 0; i < 6100; ++i) module.process(args);
        require(module.sequencer.tracks[0].stage == 1, "Rack module advances at internal tempo");
        module.params[Metropolix::RUN].setValue(0.f);
        module.process(args);
        require(module.outputs[Metropolix::GATE_1].getVoltage() == 0.f, "Rack transport stops gates");
        module.inputs[Metropolix::CLOCK_IN].channels = 1;
        module.inputs[Metropolix::CLOCK_IN].setVoltage(0.f);
        module.params[Metropolix::RUN].setValue(1.f);
        for (int i = 0; i < 32; ++i) module.process(args);
        require(module.sequencer.tracks[0].stage == -1, "external mode waits for edge");
        module.inputs[Metropolix::CLOCK_IN].setVoltage(5.f);
        module.process(args);
        require(module.sequencer.tracks[0].stage == 0, "Rack Schmitt input detects external edge");
        for (int i = 0; i < 64; ++i) module.process(args);
        require(module.sequencer.tracks[0].stage == 0, "high clock level does not retrigger");
        module.inputs[Metropolix::RESET_IN].channels = 1;
        module.inputs[Metropolix::RESET_IN].setVoltage(0.f);
        module.inputs[Metropolix::CLOCK_IN].setVoltage(0.f);
        module.process(args);
        module.inputs[Metropolix::RESET_IN].setVoltage(5.f);
        module.inputs[Metropolix::CLOCK_IN].setVoltage(5.f);
        module.process(args);
        require(module.sequencer.tracks[0].stage == -1, "Rack reset input takes priority over clock");
    }
    {
        Metropolix module;
        Module::ProcessArgs args;
        args.sampleRate = 48000.f; args.sampleTime = 1.f / 48000;
        auto* quantity = module.getParamQuantity(Metropolix::modParam(0, Metropolix::MOD_VALUE));
        quantity->setValue(-2.5f);
        module.params[Metropolix::modParam(0, Metropolix::MOD_TYPE)].setValue(metro::ModUnipolar);
        require(quantity->getMinValue() == 0.f && quantity->getValue() == 0.f, "unipolar editor displays effective voltage and starts drags at zero");
        quantity->setDisplayValue(-1.f);
        require(module.params[Metropolix::modParam(0, Metropolix::MOD_VALUE)].getValue() == 0.f, "unipolar numeric entry cannot set negative voltage");
        module.params[Metropolix::modParam(0, Metropolix::MOD_TYPE)].setValue(metro::ModBipolar);
        module.params[Metropolix::RUN].setValue(1.f);
        module.params[Metropolix::OUTPUT_SOURCE_A].setValue(2.f);
        module.params[Metropolix::OUTPUT_SOURCE_B].setValue(9.f);
        module.params[Metropolix::modParam(0, Metropolix::MOD_VALUE)].setValue(-2.5f);
        module.params[Metropolix::modParam(7, Metropolix::MOD_VALUE)].setValue(4.5f);
        module.process(args);
        require(module.outputs[Metropolix::A_OUT].getVoltage() == -2.5f, "actual Rack output A carries MOD 1 voltage");
        require(module.outputs[Metropolix::B_OUT].getVoltage() == 4.5f, "actual Rack output B carries MOD 8 voltage");
        module.params[Metropolix::RUN].setValue(0.f); module.process(args);
        require(module.outputs[Metropolix::A_OUT].getVoltage() == 0.f && module.outputs[Metropolix::B_OUT].getVoltage() == 0.f, "Rack MOD outputs silence on stop");
    }
    {
        Metropolix module;
        Module::ProcessArgs args;args.sampleRate=48000.f;args.sampleTime=1.f/48000.f;
        module.params[Metropolix::TEMPO].setValue(135.f);
        module.params[Metropolix::stageParam(0,0,Metropolix::CV)].setValue(-2.5f);
        module.params[Metropolix::modParam(0,Metropolix::MOD_VALUE)].setValue(.5f);
        module.params[Metropolix::modParam(0,Metropolix::MOD_DESTINATION)].setValue(metro::ModPitchPre);
        module.params[Metropolix::modParam(1,Metropolix::MOD_VALUE)].setValue(-.25f);
        module.params[Metropolix::modParam(1,Metropolix::MOD_DESTINATION)].setValue(metro::ModPitchPost);
        module.params[Metropolix::RUN].setValue(1.f);
        for(int i=0;i<64;++i)module.process(args);
        require(module.displayBpm.load()==135.f, "OLED internal tempo follows parameter");
        require(module.playCv[0].load()==-2.5f && module.pitchPre[0].load()==.5f && module.pitchPost[0].load()==-.25f,
            "OLED meters expose actual track CV and pre/post routes");
        module.inputs[Metropolix::CLOCK_IN].channels=1;
        module.inputs[Metropolix::CLOCK_IN].setVoltage(5.f);module.process(args);
        module.inputs[Metropolix::CLOCK_IN].setVoltage(0.f);
        for(int i=0;i<4799;++i)module.process(args);
        module.inputs[Metropolix::CLOCK_IN].setVoltage(5.f);module.process(args);
        require(std::abs(module.displayBpm.load()-150.f)<.01f,"OLED external tempo measures a 4800-sample sixteenth-note interval");
        module.inputs[Metropolix::CLOCK_IN].channels=0;
        for(int i=0;i<32;++i)module.process(args);
        require(module.displayBpm.load()==135.f && !module.externalClock.load(),"OLED returns to internal tempo after unplugging clock");
    }
    {
        Metropolix module;Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
        auto process=[&](int count=32) { for(int i=0;i<count;++i)module.process(args); };
        module.params[Metropolix::RUN].setValue(1);
        module.params[Metropolix::auxParam(0,Metropolix::AUX_AMOUNT)].setValue(.5f);
        module.params[Metropolix::auxParam(0,Metropolix::AUX_DESTINATION)].setValue(metro::ModPitchOffset);
        module.inputs[Metropolix::AUX_X_IN].channels=2;
        module.inputs[Metropolix::AUX_X_IN].setVoltage(1.f,0);
        module.inputs[Metropolix::AUX_X_IN].setVoltage(4.f,1);
        process();
        require(module.outputs[Metropolix::PITCH_1].getVoltage()==.5f && module.outputs[Metropolix::PITCH_2].getVoltage()==0,"Rack AUX reads first channel and affects assigned output only");
        module.inputs[Metropolix::AUX_X_IN].setVoltage(-2.f);process(1);
        require(module.outputs[Metropolix::PITCH_1].getVoltage()==-1.f,"Rack AUX voltage is read every sample");
        module.inputs[Metropolix::AUX_X_IN].channels=0;process(1);
        require(module.outputs[Metropolix::PITCH_1].getVoltage()==0,"Rack cable disconnect immediately restores base pitch");
        module.params[Metropolix::auxParam(1,Metropolix::AUX_AMOUNT)].setValue(.1f);
        module.params[Metropolix::auxParam(1,Metropolix::AUX_DESTINATION)].setValue(metro::ModMute);
        module.params[Metropolix::auxParam(1,Metropolix::AUX_ASSIGNMENT)].setValue(2);
        process();
        require(module.auxVoltage[1].load()==5 && module.outputs[Metropolix::GATE_1].getVoltage()==0 && module.outputs[Metropolix::GATE_2].getVoltage()==0,"unpatched AUX knob is a manual gate for both tracks");
        module.params[Metropolix::RUN].setValue(0);process();
        require(module.auxVoltage[1].load()==5,"AUX meter remains live while transport is stopped");
        module.uiPage.store(Metropolix::AUX_PAGE);module.uiField.store(1);module.selectedAux.store(1);
        json_t* data=module.dataToJson();Metropolix restored;restored.dataFromJson(data);json_decref(data);
        require(restored.uiPage.load()==Metropolix::AUX_PAGE && restored.uiField.load()==1 && restored.selectedAux.load()==1,"AUX display state roundtrip");
        json_t* previous=json_pack("{s:i}","schemaVersion",6);module.dataFromJson(previous);json_decref(previous);
        for(int id=Metropolix::AUX_PARAMS;id<Metropolix::TOTAL_PULSES;++id)require(module.params[id].getValue()==0,"2.3.x preset clears AUX with no change to old IDs");
    }
    {
        Metropolix module;
        Module::ProcessArgs args; args.sampleRate=48000; args.sampleTime=1.f/48000;
        const int notes[]={0,2,4,5};
        for(int s=0;s<4;++s)module.params[Metropolix::PITCH+s].setValue(notes[s]/36.f);
        for(int t=0;t<2;++t) {
            module.params[Metropolix::trackParam(t,Metropolix::LENGTH)].setValue(4);
            module.params[Metropolix::stageParam(t,0,Metropolix::ACCUMULATION)].setValue(1);
        }
        module.params[Metropolix::accumModeParam(1)].setValue(metro::TrackAccumulation);
        module.params[Metropolix::RUN].setValue(1);
        module.inputs[Metropolix::CLOCK_IN].channels=1;
        const int expectedStage[]={0,2,4,5,2,2,4,5,4,2,4,5,5,2,4,5};
        const int expectedTrack[]={0,2,4,5,2,4,5,7,4,5,7,9,5,7,9,11};
        for(int pulse=0;pulse<16;++pulse) {
            module.inputs[Metropolix::CLOCK_IN].setVoltage(0);
            for(int i=0;i<6000;++i)module.process(args);
            module.inputs[Metropolix::CLOCK_IN].setVoltage(5);module.process(args);
            require(std::abs(module.outputs[Metropolix::PITCH_1].getVoltage()*12-expectedStage[pulse])<.0001f,"p126 Stage mode actual Rack output");
            require(std::abs(module.outputs[Metropolix::PITCH_2].getVoltage()*12-expectedTrack[pulse])<.0001f,"p127 Track mode actual Rack output");
            require(module.outputs[Metropolix::GATE_1].getVoltage()==5 && module.outputs[Metropolix::GATE_2].getVoltage()==5,"accumulator modes preserve actual gate outputs");
        }
        module.uiPage.store(Metropolix::ACCUM_PAGE);module.uiField.store(5);
        json_t* data=module.dataToJson();module.dataFromJson(data);json_decref(data);
        require(module.uiPage.load()==Metropolix::ACCUM_PAGE && module.uiField.load()==5,"LIM page and all six fields survive save/load");
    }
    {
        Metropolix m;
        auto assign=[&](int k,metro::CtrlDestination d,float value,int target) {
            m.params[Metropolix::ctrlParam(k,Metropolix::CTRL_DESTINATION)].setValue(d);
            m.params[Metropolix::ctrlParam(k,Metropolix::CTRL_VALUE)].setValue(value);
            m.params[Metropolix::ctrlParam(k,Metropolix::CTRL_ASSIGNMENT)].setValue(target);
            m.readSettings();
        };
        require(m.integer(Metropolix::ctrlParam(0,Metropolix::CTRL_DESTINATION))==metro::CtrlGateLength && m.integer(Metropolix::ctrlParam(1,Metropolix::CTRL_DESTINATION))==metro::CtrlSlideAmount,"new modules use hardware CTRL defaults");
        assign(0,metro::CtrlGateLength,1,0);
        require(m.params[Metropolix::trackParam(0,Metropolix::GATE_LENGTH)].getValue()==99 && m.params[Metropolix::trackParam(1,Metropolix::GATE_LENGTH)].getValue()==50,"direct CTRL changes only assigned base settings");
        require(m.ctrlOwner(Metropolix::trackParam(0,Metropolix::GATE_LENGTH))==0 && m.ctrlOwner(Metropolix::trackParam(1,Metropolix::GATE_LENGTH))==-1,"ownership follows target track");
        assign(1,metro::CtrlGateLength,-1,2);
        require(m.sequencer.settings.tracks[0].gateLength==.01f && m.ctrlOwner(Metropolix::trackParam(0,Metropolix::GATE_LENGTH))==1,"CTRL 2 wins direct collisions");
        assign(0,metro::CtrlNone,0,2);assign(1,metro::CtrlNone,0,2);
        require(m.params[Metropolix::trackParam(0,Metropolix::GATE_LENGTH)].getValue()==1 && m.ctrlOwner(Metropolix::trackParam(0,Metropolix::GATE_LENGTH))==-1,"unassigning direct control retains resulting panel value and releases lock");
        assign(0,metro::CtrlTempo,1,1);require(m.sequencer.settings.bpm==400,"tempo is global even with track 2 assignment");
        assign(0,metro::CtrlClockDiv,-1,0);assign(1,metro::CtrlStagesLength,-1,1);
        require(m.sequencer.settings.tracks[0].division==1 && m.sequencer.settings.tracks[1].length==1,"clock division and stage length reach engine");
        assign(0,metro::CtrlAccumMode,1,2);assign(1,metro::CtrlAccumPolar,0,1);
        require(m.sequencer.settings.tracks[0].accumMode==metro::TrackAccumulation && m.sequencer.settings.tracks[1].accumBipolar,"accumulator direct destinations reach engine");
        assign(0,metro::CtrlPitchOffset,.333f,0);assign(1,metro::CtrlOutputSwap,1,2);
        for(int t=0;t<2;++t)m.params[Metropolix::trackParam(t,Metropolix::GATE_LENGTH)].setValue(50);
        m.selectedCtrl.store(1);m.uiPage.store(Metropolix::CTRL_PAGE);m.uiField.store(1);
        json_t* params=m.paramsToJson();json_t* data=m.dataToJson();Metropolix restored;
        restored.paramsFromJson(params);restored.dataFromJson(data);json_decref(params);json_decref(data);
        for(int id=0;id<Metropolix::NUM_PARAMS;++id)require(m.params[id].getValue()==restored.params[id].getValue(),"CTRL patch serialization keeps every parameter");
        require(restored.selectedCtrl.load()==1 && restored.uiPage.load()==Metropolix::CTRL_PAGE && restored.uiField.load()==1,"CTRL page and selected knob survive save/load");
        Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
        restored.params[Metropolix::RUN].setValue(1);restored.process(args);
        require(restored.outputs[Metropolix::PITCH_1].getVoltage()==0 && std::abs(restored.outputs[Metropolix::PITCH_2].getVoltage()-.333f)<.0001f,"physical pitch outputs swap after per-track control");
        restored.params[Metropolix::trackParam(0,Metropolix::MUTE)].setValue(1);
        for(int i=0;i<32;++i)restored.process(args);
        require(restored.outputs[Metropolix::GATE_2].getVoltage()==0 && restored.outputs[Metropolix::GATE_1].getVoltage()==5,"physical gate outputs swap with pitch");
        m.params[Metropolix::trackParam(0,Metropolix::GATE_LENGTH)].setValue(37);
        json_t* old=json_pack("{s:i}","schemaVersion",8);m.dataFromJson(old);json_decref(old);
        require(m.sequencer.settings.controls[0].destination==metro::CtrlNone && m.sequencer.settings.controls[1].destination==metro::CtrlNone && m.params[Metropolix::trackParam(0,Metropolix::GATE_LENGTH)].getValue()==37,"2.5 patches clear CTRL without altering old panel values");
    }
    presetRackTests();
    chainRackTests();
    swingRackTests();
    overrideRackTests();
    loopyRackTests();
    scaleRackTests();
    slideRackTests();
    stageCvRackTests();
    clockOutputRackTests();
    outputRoutingRackTests();
    trackOptionsRackTests();
    accumulatorRoutingRackTests();
    delete context.engine;
    context.engine = nullptr;
    rack::contextSet(nullptr);
    std::cout << "PASS: Rack library integration, parameter persistence, transport, and input triggers\n";
}
