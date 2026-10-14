void modTypeRackTests(){
    using M=Metropolix;using namespace metro;
    auto m=std::unique_ptr<M>(new M),restored=std::unique_ptr<M>(new M);
    int cases=0;
    for(int lane=0;lane<8;++lane)for(int destination=0;destination<ModDestinationCount;++destination)for(int saved=0;saved<3;++saved){
        int typeId=M::modParam(lane,M::MOD_TYPE),destId=M::modParam(lane,M::MOD_DESTINATION);
        m->params[typeId].setValue(saved);m->params[destId].setValue(destination);
        bool gate=modGateDestination(static_cast<ModDestination>(destination)) || (destination==ModNone && saved==ModGate);
        bool unipolar=destination==ModNone && saved==ModUnipolar;
        for(int stage=0;stage<8;++stage)for(float value:{-5.f,-3.25f,0.f,1.49f,1.5f,2.25f,5.f}){
            int id=M::modParam(lane,M::MOD_VALUE+stage);auto* q=m->getParamQuantity(id);m->params[id].setValue(value);
            float expected=gate?(value>=1.5f?5.f:0.f):(unipolar?std::max(0.f,value):value);
            require(q->getMinValue()==(gate || unipolar?0.f:-5.f),"internal MOD destination determines native voltage range independently of saved type");
            require(q->getValue()==(gate || unipolar?std::max(0.f,value):value),"MOD native edit origin follows effective type without rewriting stored voltage");
            require(m->effectiveModVoltage(lane,value)==expected && m->params[id].getValue()==value,"MOD sounding voltage follows destination type and preserves raw stage data");
            ++cases;
        }
        auto* type=m->getParamQuantity(typeId);
        if(destination!=ModNone){
            require(type->getDisplayValueString().find(gate?"Gate":"Bipolar")!=std::string::npos,"native MOD type readout explains effective internal mode");
            type->setValue((saved+1)%3);require(m->integer(typeId)==saved,"forced MOD type edit retains its saved unassigned choice");
        }
        m->params[destId].setValue(ModNone);require(m->integer(typeId)==saved,"removing internal destination restores saved MOD type");
        type->setValue((saved+1)%3);require(m->integer(typeId)==(saved+1)%3,"unassigned MOD type remains natively editable");
    }
    for(int lane=0;lane<8;++lane){
        m->params[M::modParam(lane,M::MOD_TYPE)].setValue(lane%3);
        m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(lane%2?ModDirection:ModPitchOffset);
        for(int s=0;s<8;++s){m->params[M::modParam(lane,M::MOD_VALUE+s)].setValue(s%2?2.25f:-3.25f);m->params[M::modParam(lane,M::MOD_RAMP+s)].setValue(s%2);}
    }
    auto preset=m->capturePreset();m->putPreset(0,preset);json_t* params=m->paramsToJson();json_t* data=m->dataToJson();restored->paramsFromJson(params);restored->dataFromJson(data);json_decref(params);json_decref(data);
    require(restored->getPreset(0).values==preset.values,"MOD forced types preserve exact musical preset data through Rack serialization");
    for(int lane=0;lane<8;++lane){
        require(restored->integer(M::modParam(lane,M::MOD_TYPE))==lane%3,"Rack reopen preserves saved MOD type under internal assignment");
        for(int s=0;s<8;++s){int id=M::modParam(lane,M::MOD_VALUE+s);require(restored->params[id].getValue()==(s%2?2.25f:-3.25f),"Rack reopen preserves dormant raw MOD stage voltage");}
    }
    auto* driver=new MidiTestDriver;midi::addDriver(24050,driver);m->midiOutput.setDriverId(24050);m->midiOutput.setDeviceId(0);
    Module::ProcessArgs args{};args.sampleRate=48000;args.sampleTime=1.f/48000;
    auto frames=[&](int n){for(int i=0;i<n;++i){m->process(args);++args.frame;}};
    auto hasCC=[&](int lane,int value){for(auto& message:driver->output.messages)if(message.bytes==std::vector<uint8_t>({0xb0,uint8_t(80+lane),uint8_t(value)}))return true;return false;};
    for(int lane=0;lane<8;++lane){m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(ModNone);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(ModBipolar);m->params[M::midiModParam(lane,M::MIDI_MOD_CHANNEL)].setValue(1);m->params[M::midiModParam(lane,M::MIDI_MOD_CC)].setValue(81+lane);for(int s=0;s<8;++s)m->params[M::modParam(lane,M::MOD_VALUE+s)].setValue(0);}
    m->params[M::CLOCK_SOURCE].setValue(2);m->inputs[M::CLOCK_IN].channels=1;
    int outputCases=0;
    for(int lane=0;lane<8;++lane)for(int destination:{ModNone,ModPitchOffset,ModDirection})for(int saved=0;saved<3;++saved)for(float value:{-2.5f,2.5f}){
        m->params[M::RUN].setValue(0);frames(64);
        m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(destination);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(saved);
        for(int s=0;s<8;++s){m->params[M::modParam(lane,M::MOD_VALUE+s)].setValue(value);m->params[M::modParam(lane,M::MOD_RAMP+s)].setValue(0);}
        m->params[M::OUTPUT_SOURCE_A].setValue(outputSource(OutModCv,lane));m->params[M::OUTPUT_SOURCE_B].setValue(outputSource(OutModCv,lane));m->inputs[M::CLOCK_IN].setVoltage(0);frames(64);driver->output.messages.clear();
        m->params[M::RUN].setValue(1);frames(64);m->inputs[M::CLOCK_IN].setVoltage(5);frames(1);m->inputs[M::CLOCK_IN].setVoltage(0);frames(64);
        bool gate=destination==ModDirection || (destination==ModNone && saved==ModGate),unipolar=destination==ModNone && saved==ModUnipolar,bipolar=!gate && !unipolar;
        float expected=gate?(value>=1.5f?5.f:0.f):(unipolar?std::max(0.f,value):value);
        require(m->outputs[M::A_OUT].getVoltage()==expected && m->outputs[M::B_OUT].getVoltage()==expected,"actual A/B MOD outputs use effective destination type for every lane and saved mode");
        int cc=int(std::lround((bipolar?(expected+5.f)/10.f:expected/5.f)*127));require(hasCC(lane,cc),"running native MIDI CC uses effective MOD voltage and bipolar/positive scaling");
        m->params[M::RUN].setValue(0);frames(64);driver->output.messages.clear();m->params[M::modParam(lane,M::MOD_VALUE)].setValue(-value);frames(64);
        expected=gate?(-value>=1.5f?5.f:0.f):(unipolar?std::max(0.f,-value):-value);cc=int(std::lround((bipolar?(expected+5.f)/10.f:expected/5.f)*127));require(hasCC(lane,cc),"stopped native MIDI learning uses effective MOD mode after raw stage editing");
        m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(ModNone);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(ModBipolar);for(int s=0;s<8;++s)m->params[M::modParam(lane,M::MOD_VALUE+s)].setValue(0);++outputCases;
    }
    m->midiOutput.setDeviceId(-1);
    std::cout<<"PASS: "<<cases<<" native MOD effective range/value cases, all destinations/types/lanes/stages, locked type selection, restored preference and exact preset/Rack roundtrip\n";
    std::cout<<"PASS: "<<outputCases<<" actual MOD A/B output and running/stopped MIDI cases across all lanes, saved types and destination modes\n";
}
