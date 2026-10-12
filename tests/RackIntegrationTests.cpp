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

int main() {
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
        source.params[Metropolix::extraParam(1, Metropolix::PULSE_DIV)].setValue(3.f);
        source.params[Metropolix::extraParam(1, Metropolix::GATE_STRETCH)].setValue(1.f);
        source.params[Metropolix::accumParam(1, Metropolix::NEGATIVE_LIMIT)].setValue(5.f);
        source.params[Metropolix::accumParam(1, Metropolix::BIPOLAR)].setValue(1.f);
        source.params[Metropolix::accumParam(1, Metropolix::ACCUM_ORDER)].setValue(metro::AccumPendulum);
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
        const auto& restoredTrack = restored.sequencer.settings.tracks[1];
        require(restoredTrack.accumNegativeLimit == 5 && restoredTrack.accumBipolar && restoredTrack.accumOrder == metro::AccumPendulum, "accumulator options reach the engine");
        require(restoredTrack.stages[4].probabilityPerPulse && restoredTrack.stages[4].accumTrigger == metro::AccumRatchet, "stage trigger options reach the engine");
        restored.params[Metropolix::trackParam(0, Metropolix::ACCUM_LIMIT)].setValue(7.f);
        restored.params[Metropolix::trackParam(0, Metropolix::ORDER)].setValue(1.f);
        json_t* legacy = json_pack("{s:i}", "schemaVersion", 1);
        restored.dataFromJson(legacy);
        json_decref(legacy);
        require(restored.sequencer.settings.tracks[0].order == metro::Forward && restored.sequencer.settings.tracks[0].reversed, "legacy Reverse order is migrated without changing playback");
        require(restored.sequencer.settings.tracks[0].accumNegativeLimit == 7, "legacy shared accumulator limit migrates to negative limit");
        json_t* malformed = json_pack("{s:i, s:i, s:i}", "selectedTrack", 100, "selectedStage", -20, "selectedRow", 77);
        restored.dataFromJson(malformed);
        json_decref(malformed);
        require(restored.selectedTrack.load() == 1 && restored.selectedStage.load() == 0 && restored.selectedRow.load() == 5, "out-of-range saved UI state is clamped");
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
    delete context.engine;
    context.engine = nullptr;
    rack::contextSet(nullptr);
    std::cout << "PASS: Rack library integration, parameter persistence, transport, and input triggers\n";
}
