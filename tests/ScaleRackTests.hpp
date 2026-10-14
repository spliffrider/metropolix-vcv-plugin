void scaleRackTests(){
    using M=Metropolix;auto m=std::unique_ptr<M>(new M),other=std::unique_ptr<M>(new M);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    for(int i=0;i<100;++i){m->params[M::userScaleParam(i)].setValue(i+1);m->params[M::userScaleParam(i,true)].setValue(i%12);}
    m->params[M::SCALE].setValue(149-1);m->params[M::SCALE_LOCK].setValue(67);m->params[M::scaleShortcutParam(7)].setValue(148);m->params[M::scaleShortcutParam(7,true)].setValue(9);
    m->scaleEditing.store(true);m->scaleShortcutHeld.store(true);
    auto* params=m->paramsToJson();auto* data=m->dataToJson();other->paramsFromJson(params);other->dataFromJson(data);json_decref(params);json_decref(data);
    require(other->integer(M::SCALE)==148 && other->integer(M::SCALE_LOCK)==67 && !other->scaleEditing.load() && !other->scaleShortcutHeld.load(),"scale selection/lock persist while edit/held gestures clear");
    for(int i=0;i<100;++i)require(other->integer(M::userScaleParam(i))==i+1 && other->integer(M::userScaleParam(i,true))==i%12,"all one hundred custom masks/roots survive Rack roundtrip");
    require(other->integer(M::scaleShortcutParam(7))==148 && other->integer(M::scaleShortcutParam(7,true))==9,"scale/root shortcuts roundtrip");
    require(M::FORMAT3_PARAMS==458 && M::presetIndex(M::SCALE_LOCK)==M::FORMAT9_PARAMS && M::presetIndex(M::userScaleParam(99,true))<0,"scale lock appends after the old snapshot while custom definitions stay global");
    auto preset=m->capturePreset();m->params[M::userScaleParam(0)].setValue(1);m->params[M::SCALE_LOCK].setValue(-1);m->putPreset(0,preset);m->requestPreset(0);m->sendPresetCommand();
    Module::ProcessArgs args;args.sampleRate=48000;args.sampleTime=1.f/48000;m->process(args);
    require(m->integer(M::SCALE)==148 && m->integer(M::userScaleParam(0))==1 && m->integer(M::SCALE_LOCK)==67,"presets recall their stored scale lock while definitions stay global");
    m->params[M::SCALE_LOCK].setValue(-1);m->params[M::SCALE].setValue(49);m->params[M::userScaleParam(0)].setValue(0x91);m->params[M::userScaleParam(0,true)].setValue(5);
    auto* q=m->getParamQuantity(M::stageParam(0,0,M::PITCH_OVERRIDE));q->setDisplayValueString("D2");
    require(m->integer(M::stageParam(0,0,M::PITCH_OVERRIDE))==24 && q->getDisplayValueString()=="C2","native note entry quantizes using custom notes");
    m->params[M::RUN].setValue(1);m->readSettings();m->sequencer.reset();m->process(args);
    require(std::abs(m->outputs[M::PITCH_1].getVoltage()-29.f/12)<1e-5,"user root/override reach actual Rack pitch port");
    m->params[M::userScaleParam(0)].setValue(1u<<11);q->setValue(0);
    require(m->integer(M::stageParam(0,0,M::PITCH_OVERRIDE))==0 && q->getDisplayValueString()=="B-1","custom scales without root retain C0 anchor and display negative octave safely");
    m->params[M::userScaleParam(0)].setValue(1u<<1);q->setValue(60);
    require(m->integer(M::stageParam(0,0,M::PITCH_OVERRIDE))==60 && q->getDisplayValueString()=="C#5","custom upper octave anchor stays inside stored range");
    auto* old=json_pack("{s:i}","schemaVersion",14);other->dataFromJson(old);json_decref(old);
    require(other->integer(M::SCALE)==148 && other->integer(M::SCALE_LOCK)==-1 && other->integer(M::userScaleParam(99))==0xAB5,"old schema resets new definitions/lock safely without changing selector ID");
    for(int i=0;i<7;++i){m->params[M::SCALE].setValue(i);old=json_pack("{s:i}","schemaVersion",14);m->dataFromJson(old);json_decref(old);require(m->sequencer.settings.scale==i,"old seven scale IDs retain their exact scale selection");}
}
