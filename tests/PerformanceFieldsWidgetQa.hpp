// Complete native screens plus live meters, action context and state isolation.
using M=Metropolix;
#include "PerformanceFieldsReference.hpp"
APP->engine->bypassModule(m,true);std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());
float oldBpm=m->displayBpm.load();bool oldExternal=m->externalClock.load();std::array<float,3> oldAux;std::array<float,8> oldMod;
for(int i=0;i<3;++i)oldAux[i]=m->auxVoltage[i].load();for(int i=0;i<8;++i)oldMod[i]=m->modVoltage[i].load();
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
auto clean=[&]{closeMenus();for(auto* a:APP->history->actions)delete a;APP->history->actions.clear();APP->history->savedIndex=APP->history->actionIndex=0;for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->clearWorkflows();m->cancelDraft();for(int k=0;k<2;++k){m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);m->outputDraft[k].store(-1);}m->params[M::RUN].setValue(0);m->params[M::ENC_CONFIRM].setValue(0);m->params[M::CLOCK_SOURCE].setValue(1);m->params[M::TEMPO].setValue(100);m->displayBpm.store(100);m->externalClock.store(false);m->uiMod.store(false);m->uiAlt.store(false);m->uiEdit.store(false);m->uiChooseFunction.store(false);m->scaleEditing.store(false);m->tunerActive.store(false);m->loopyLatched.store(false);m->loopyMuteHeld.store(false);m->selectedTrack.store(0);m->selectedLane.store(0);m->selectedStage.store(0);m->selectedCtrl.store(0);m->selectedAux.store(0);m->selectedRow.store(M::SLIDE);m->uiField.store(0);for(auto& v:m->auxVoltage)v.store(0);for(auto& v:m->modVoltage)v.store(0);};
auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));
auto capture=[&](const std::string& name,const OledCanvas& c){std::ofstream out(dir+"/field-"+name+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<const char*>(c.pixels),8192);};
bool native=true;int count=0;
for(const auto& ref:performanceFieldReferences){clean();std::string name=ref.name;
    if(name=="internal" || name=="external" || name=="modulated"){screenPage(m,M::BPM_PAGE);if(name=="external"){m->externalClock.store(true);m->displayBpm.store(85);m->params[M::CLOCK_SOURCE].setValue(2);m->params[M::EXT_CLOCK_DIV].setValue(1);}else if(name=="modulated")m->displayBpm.store(115);}
    else if(name=="order"){screenPage(m,M::ORDER_PAGE);m->params[M::trackParam(0,M::ORDER)].setValue(0);m->params[M::extraParam(0,M::REVERSED)].setValue(0);}
    else if(name=="swing"){screenPage(m,M::SWING_PAGE);m->params[M::swingParam(0)].setValue(50);}
    else if(name=="slide"){screenPage(m,M::SLIDE_PAGE);m->params[M::trackParam(0,M::SLIDE_TIME)].setValue(20);m->params[M::slideTypeParam(0)].setValue(1);}
    else if(name=="gate"){screenPage(m,M::GATE_PAGE);m->params[M::trackParam(0,M::GATE_LENGTH)].setValue(50);m->params[M::gateScaleParam(0)].setValue(100);}
    else if(name=="loopy"){screenPage(m,M::LOOPY_PAGE);m->params[M::LOOPY_PULSES].setValue(0);m->params[M::LOOPY_TARGET].setValue(0);m->params[M::RUN].setValue(1);}
    else if(name=="output-a" || name=="output-b"){bool b=name=="output-b";screenPage(m,b?M::B_PAGE:M::A_PAGE);m->selectedRow.store(M::SKIP);m->params[b?M::OUTPUT_SOURCE_B:M::OUTPUT_SOURCE_A].setValue(metro::outputSource(b?metro::OutFirstPulse:metro::OutTrackCv,0));}
    else if(name=="aux"){screenPage(m,M::AUX_PAGE);m->params[M::auxParam(0,M::AUX_DESTINATION)].setValue(metro::ModPitchPre);m->params[M::auxParam(0,M::AUX_ASSIGNMENT)].setValue(0);}
    else if(name=="ctrl"){screenPage(m,M::CTRL_PAGE);m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlOrder);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);m->params[M::ctrlParam(0,M::CTRL_VALUE)].setValue(.4f);}
    auto before=raw();auto c=display->render();int differences=0;
    for(int y=0;y<64;++y)for(int x=0;x<128;++x)differences+=bool(c.pixels[y][x])!=bool(ref.packed[y*16+x/8]&(1<<(7-x%8)));
    if(differences)check(false,string::f("native complete field %s p%d: %d cells differ",ref.name,ref.page,differences));
    native &= !differences && raw()==before && !APP->history->actionIndex;capture("native-"+std::to_string(count)+"-"+name,c);++count;
}
check(native,std::to_string(count)+" complete native performance screens match all 98304 foreground cells, including frames, captions, meters and action footers, without parameter/history writes");
bool meters=true;int meterCount=0;
for(bool internal:{false,true})for(float volts:{-10.f,-5.f,-2.5f,0.f,2.5f,5.f,10.f}){clean();screenPage(m,internal?M::MOD_ASSIGN_PAGE:M::AUX_PAGE);m->uiMod.store(internal);m->params[internal?M::modParam(0,M::MOD_DESTINATION):M::auxParam(0,M::AUX_DESTINATION)].setValue(metro::ModPitchPre);m->params[M::auxParam(0,M::AUX_AMOUNT)].setValue(-.25f);if(internal)m->modVoltage[0].store(volts);else m->auxVoltage[0].store(volts);auto before=raw();auto c=display->render();int end=volts<=-5?83:volts>=5?123:volts<0?93:volts>0?113:103;
    for(int y=2;y<=8;++y)for(int x=82;x<=124;++x){bool border=y==2 || y==8 || x==82 || x==124;bool mark=volts==0?x==103 && y>=3 && y<=7:y>=4 && y<=6 && x>=std::min(103,end) && x<=std::max(103,end);meters &= bool(c.pixels[y][x])==(border || mark);}
    meters &= raw()==before && !APP->history->actionIndex;capture(std::string(internal?"mod-":"aux-")+std::to_string(meterCount),c);++meterCount;
}
check(meters,std::to_string(meterCount)+" live AUX/MOD meters preserve bipolar center, signed bounds and post-attenuverter values without scaling the signal twice");
bool gates=true;
for(float volts:{0.f,1.49f,1.5f,5.f}){clean();screenPage(m,M::AUX_PAGE);m->params[M::auxParam(0,M::AUX_DESTINATION)].setValue(metro::ModMute);m->auxVoltage[0].store(volts);auto c=display->render();for(int y=4;y<=6;++y)for(int x=118;x<=122;++x)gates &= bool(c.pixels[y][x])==(volts>=1.5f);capture("gate-"+std::to_string(int(volts*100)),c);}
check(gates,"AUX gate destinations retain the existing 1.5V indicator threshold");
bool waves=true;int waveCount=0;const uint16_t waveform[]={120,72,72,72,79};
for(int page:{M::STAGE_PAGE,M::HOME,M::CV_TYPE_PAGE,M::CTRL_PAGE,M::AUX_PAGE,M::MOD_ASSIGN_PAGE,M::SCALE_PAGE}){clean();screenPage(m,page);m->uiMod.store(true);m->selectedLane.store(1);m->selectedStage.store(0);m->params[M::modParam(1,M::MOD_DESTINATION)].setValue(metro::ModDirection);m->params[M::OUTPUT_SOURCE_A].setValue(3);m->params[M::OUTPUT_SOURCE_B].setValue(1);auto before=raw();auto c=display->render();int x=page==M::CTRL_PAGE || page==M::AUX_PAGE || page==M::MOD_ASSIGN_PAGE?109:110;
    for(int y=0;y<5;++y)for(int col=0;col<7;++col)waves &= bool(c.pixels[56+y][x+col])==bool(waveform[y]&(1<<(6-col)));
    waves &= raw()==before && !APP->history->actionIndex;capture("mod-wave-"+std::to_string(page),c);++waveCount;
}
check(waves,std::to_string(waveCount)+" MOD gate action footers retain the native waveform before their A routing suffix, including TYPE");
clean();m->uiMod.store(true);m->selectedLane.store(1);m->params[M::modParam(1,M::MOD_DESTINATION)].setValue(metro::ModNone);m->params[M::OUTPUT_SOURCE_A].setValue(3);m->params[M::OUTPUT_SOURCE_B].setValue(1);bool routing=display->functionName()=="OUTPUT";m->params[M::OUTPUT_SOURCE_A].setValue(0);routing &= display->functionName()=="UNASSIGNED";m->params[M::OUTPUT_SOURCE_B].setValue(3);routing &= display->functionName()=="OUTPUT";m->params[M::modParam(1,M::MOD_DESTINATION)].setValue(metro::ModProbability);routing &= display->functionName()=="Probability";
check(routing,"MOD action names distinguish routed OUTPUT, unassigned lanes and internal destinations");
clean();for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());m->displayBpm.store(oldBpm);m->externalClock.store(oldExternal);for(int i=0;i<3;++i)m->auxVoltage[i].store(oldAux[i]);for(int i=0;i<8;++i)m->modVoltage[i].store(oldMod[i]);for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
