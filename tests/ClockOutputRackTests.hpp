void clockOutputRackTests(){
    using M=Metropolix;auto m=std::unique_ptr<M>(new M),other=std::unique_ptr<M>(new M);
    static_assert(M::CLOCK_OUT_DIV==705 && M::CLOCK_OUT_OFFSET==706 && M::OUTPUT_TRACK_OPTIONS==707 && M::FORMAT19_NUM_PARAMS==719,"clock settings append without moving old IDs");
    static_assert(M::FORMAT5_PARAMS==478,"global clock parameters never change preset format5");
    require(M::presetIndex(M::CLOCK_OUT_DIV)==-1 && M::presetIndex(M::CLOCK_OUT_OFFSET)==-1,"clock settings excluded from internal presets");
    auto* div=m->getParamQuantity(M::CLOCK_OUT_DIV);auto* offset=m->getParamQuantity(M::CLOCK_OUT_OFFSET);
    div->setDisplayValueString("/16");offset->setValue(15);require(div->getValue()==16 && offset->getMaxValue()==15,"native numeric entry accepts divider notation");
    div->setValue(4);require(offset->getValue()==3 && m->params[M::CLOCK_OUT_OFFSET].getValue()==15,"smaller division safely masks saved offset");
    div->setValue(16);require(offset->getValue()==15,"restoring division restores saved offset");
    m->params[M::CLOCK_OUT_DIV].setValue(16.9f);require(offset->getMaxValue()==16,"fractional automation uses the same nearest divider as DSP");div->setValue(16);
    m->readSettings();require(m->sequencer.settings.clockOutDivision==16 && m->sequencer.settings.clockOutOffset==15,"global parameters reach engine");
    m->putPreset(0,m->capturePreset());div->setValue(4);offset->setValue(2);m->requestPreset(0);m->sendPresetCommand();Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;
    m->process(args);require(div->getValue()==4 && offset->getValue()==2,"preset recall leaves global clock settings unchanged");
    m->uiPage.store(M::CLOCK_OUT_PAGE);m->uiField.store(1);
    // This minimal host has no registered Model; native-file testing uses Rack.
    json_t* params=m->paramsToJson(),*data=m->dataToJson();other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);json_decref(data);
    require(other->integer(M::CLOCK_OUT_DIV)==4 && other->integer(M::CLOCK_OUT_OFFSET)==2 && other->uiPage.load()==M::CLOCK_OUT_PAGE && other->uiField.load()==1,"native JSON preserves clock parameters/page");
    // Rack's engine normally connects ports. setChannels cannot connect a
    // disconnected input in this minimal host (same setup as other Rack tests).
    other->params[M::RUN].setValue(1);other->inputs[M::CLOCK_IN].channels=1;
    other->inputs[M::CLOCK_IN].setVoltage(0);other->process(args); // Arm Rack's Schmitt trigger from a low level.
    for(int p=0;p<12;++p){
        other->inputs[M::CLOCK_IN].setVoltage(10);other->process(args);
        require((other->outputs[M::CLOCK_OUT].getVoltage()>0)==(p%4==2),"real Rack clock jack uses division/offset");
        other->inputs[M::CLOCK_IN].setVoltage(0);for(int f=0;f<100;++f)other->process(args);
        require(other->outputs[M::CLOCK_OUT].getVoltage()==0,"real Rack output trigger decays");
    }
    other->params[M::RUN].setValue(0);other->process(args);require(other->outputs[M::CLOCK_OUT].getVoltage()==0,"real Rack stop clears clock");
    json_t* old=json_pack("{s:i}","schemaVersion",17);other->dataFromJson(old);json_decref(old);
    require(other->integer(M::CLOCK_OUT_DIV)==1 && other->integer(M::CLOCK_OUT_OFFSET)==0,"older patches retain undivided zero-offset clocks");
}
