#pragma once
#include "plugin.hpp"
#include "MxProtocol.hpp"
#include "SetupTiming.hpp"
struct Mx : Module {
    enum OutputIds {GATE_1,NUM_OUTPUTS=8};
    enum LightIds {GATE_LIGHT,CONNECTED_LIGHT=8,NUM_LIGHTS};
    metro::MxMessage messages[2];
    metro::OutputDelay<8> outputDelay;
    std::int64_t lastSource=-1;
    Mx(){
        config(0,0,NUM_OUTPUTS,NUM_LIGHTS);
        for(int out=0;out<8;++out){configOutput(out,string::f("MX %d",out+1));configLight(out,string::f("MX %d gate",out+1));}
        configLight(CONNECTED_LIGHT,"Metropolix connection");
        leftExpander.producerMessage=&messages[0];leftExpander.consumerMessage=&messages[1];
    }
    void process(const ProcessArgs& args) override {
        auto* source=leftExpander.module;
        auto* message=static_cast<const metro::MxMessage*>(leftExpander.consumerMessage);
        bool connected=source && source->id>=0 && source->rightExpander.module==this && modelMetropolix && source->model==modelMetropolix && message && message->signature==metro::MxMessage::Signature && message->sourceId==source->id;
        std::int64_t id=connected?source->id:-1;
        if(id!=lastSource){for(auto& frame:outputDelay.frames)frame.fill(0);lastSource=id;}
        std::array<float,8> signals{};
        if(connected)signals=outputDelay.process(message->output,message->delay);
        for(int out=0;out<8;++out){outputs[out].setVoltage(signals[out]);lights[out].setSmoothBrightness(signals[out]*.2f*(connected?message->level:1.f),args.sampleTime);}
        lights[CONNECTED_LIGHT].setBrightness(connected?1.f:0.f);
    }
    void onSampleRateChange(const SampleRateChangeEvent& e) override {outputDelay.prepare(e.sampleRate);}
};
