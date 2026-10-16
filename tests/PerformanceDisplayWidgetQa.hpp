// The golden text/helper pixels come from native image regions, independently
// of the glyph lookup and module renderer. Existing input handlers stay live.
using M=Metropolix;
#include "OledValueReference.hpp"
#include "OledHelperReference.hpp"
bool nativeFont=true;int fontCount=0;
for(const auto& ref:valueReferences){OledCanvas actual;actual.valueText(ref.origin,0,ref.text,ref.large);bool ok=true;for(int y=0;y<ref.height;++y)for(int x=0;x<128;++x)ok &= bool(actual.pixels[y][x])==bool(ref.packed[y*16+x/8]&(1<<(7-x%8)));if(ref.origin!=2 && ref.page!=61)ok &= ref.origin+actual.valueWidth(ref.text,ref.large)==(ref.page==122 && std::string(ref.text)=="Global"?60:126);if(!ok)check(false,string::f("native performance font p%d %s",ref.page,ref.text));nativeFont &= ok;++fontCount;}
check(nativeFont,std::to_string(fontCount)+" independent native performance captions reproduce bold/large glyphs, units, spacing and right-edge metrics");
bool metrics=true;int metricCount=0;
for(bool big:{false,true})for(int code=1;code<=127;++code)if(code==1 || code>=32){auto g=metrodisplay::valueGlyph(static_cast<unsigned char>(code),big);std::string text(1,char(code));OledCanvas c;c.valueText(0,0,text,big);metrics &= g.width<=10 && g.height<=18 && g.right<=g.advance && g.bearing+g.width<=g.advance && c.valueWidth(text,big)==g.right;for(int y=0;y<64;++y)for(int x=0;x<128;++x)if(c.pixels[y][x])metrics &= x>=g.bearing && x<g.bearing+g.width && y<g.height;++metricCount;}
for(bool big:{false,true}){OledCanvas a,b;a.valueText(0,0,"±°",big);b.valueText(0,0,std::string(1,char(127))+char(1),big);metrics &= std::memcmp(a.pixels,b.pixels,sizeof(a.pixels))==0 && a.valueWidth("±°",big)==b.valueWidth(std::string(1,char(127))+char(1),big);}
check(metrics,std::to_string(metricCount)+" performance glyph metrics retain all descenders and normalize degree/plus-minus units");
bool helperPixels=true;int helperCount=0;
for(const auto& ref:helperReferences){OledCanvas c;if(ref.kind<2)c.turnHint(0,0,ref.kind==1);else if(ref.kind==5)c.turnHint(0,0,true,false,255,true);else c.pressHint(0,0,ref.kind-1);for(int y=0;y<7;++y)for(int x=0;x<128;++x)helperPixels &= bool(c.pixels[y][x])==(y<ref.height && x<ref.width && bool(ref.rows[y]&(1u<<(ref.width-1-x))));++helperCount;}
check(helperPixels,std::to_string(helperCount)+" native encoder helper regions match immediate, queued, TYPE and one/two/three-choice press cues");

APP->engine->bypassModule(m,true);std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());float originalBpm=m->displayBpm.load();bool originalExternal=m->externalClock.load();
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
auto clean=[&]{closeMenus();for(auto* a:APP->history->actions)delete a;APP->history->actions.clear();APP->history->savedIndex=APP->history->actionIndex=0;for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->clearWorkflows();m->cancelDraft();for(int k=0;k<2;++k){m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);m->outputDraft[k].store(-1);}m->params[M::RUN].setValue(0);m->params[M::ENC_CONFIRM].setValue(0);m->params[M::CLOCK_SOURCE].setValue(1);m->params[M::TEMPO].setValue(100);m->displayBpm.store(100);m->externalClock.store(false);m->params[M::SCALE].setValue(1);m->params[M::ROOT].setValue(0);m->params[M::SCALE_LOCK].setValue(-1);m->uiMod.store(false);m->uiAlt.store(false);m->uiEdit.store(false);m->scaleEditing.store(false);m->tunerActive.store(false);m->uiStageDetail.store(true);m->selectedTrack.store(0);m->selectedLane.store(0);m->selectedStage.store(0);m->selectedCtrl.store(0);m->selectedAux.store(0);m->selectedRow.store(M::SLIDE);m->uiField.store(0);m->presetBankIndex.store(0);m->presetSaveAction.store(0);};
auto stage=[&](int row,int value){screenPage(m,M::STAGE_PAGE);m->selectedRow.store(row);m->params[M::stageParam(0,0,row)].setValue(float(value));};
auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));auto* encoder=find("Encoder");
auto capture=[&](const std::string& name){auto c=display->render();std::ofstream out(dir+"/performance-"+name+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<char*>(c.pixels),128*64);};
bool modulePixels=true;int moduleCount=0;
for(const auto& ref:valueReferences){clean();std::string text=ref.text;
    if(ref.page==40){screenPage(m,M::LOOPY_PAGE);m->params[M::LOOPY_PULSES].setValue(0);m->params[M::LOOPY_TARGET].setValue(0);}
    else if(ref.page==60 || ref.page==61){screenPage(m,M::BPM_PAGE);if(text=="85.0" || text=="/1"){m->params[M::CLOCK_SOURCE].setValue(2);m->externalClock.store(true);m->displayBpm.store(85);m->params[M::EXT_CLOCK_DIV].setValue(1);}else if(ref.page==61)m->displayBpm.store(115);}
    else if(ref.page==62 || ref.page==66){screenPage(m,M::SCALE_PAGE);m->uiField.store(1);m->params[M::SCALE].setValue(ref.page==62?7:2);}
    else if(ref.page==70){screenPage(m,M::ORDER_PAGE);m->params[M::trackParam(0,M::ORDER)].setValue(0);m->params[M::extraParam(0,M::REVERSED)].setValue(0);}
    else if(ref.page==77){screenPage(m,M::DIV_PAGE);m->params[M::trackParam(0,M::DIVISION)].setValue(1);}
    else if(ref.page==79){screenPage(m,M::SWING_PAGE);m->params[M::swingParam(0)].setValue(50);}
    else if(ref.page==80){screenPage(m,M::SLIDE_PAGE);m->params[M::trackParam(0,M::SLIDE_TIME)].setValue(20);m->params[M::slideTypeParam(0)].setValue(1);}
    else if(ref.page==108 || ref.page==110 || ref.page==111){screenPage(m,M::LOAD_PAGE);m->params[M::LOAD_DELAY].setValue(ref.page==110?1:ref.page==111?2:0);}
    else if(ref.page==112 || ref.page==114){screenPage(m,M::SAVE_PAGE);m->presetSaveAction.store(ref.page==114?2:0);}
    else if(ref.page==115 || ref.page==141){screenPage(m,M::A_PAGE);m->params[M::OUTPUT_SOURCE_A].setValue(metro::outputSource(ref.page==141?metro::OutModCv:text=="First Pulse"?metro::OutFirstPulse:metro::OutTrackCv,0));}
    else if(ref.page==122){screenPage(m,M::MX_SETUP_PAGE);m->params[M::MX_MODE].setValue(0);}
    else if(ref.page==144){screenPage(m,M::MOD_ASSIGN_PAGE);m->uiMod.store(true);m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModProbability);m->params[M::modParam(0,M::MOD_ASSIGNMENT)].setValue(0);}
    else if(ref.page==146){screenPage(m,M::CTRL_PAGE);m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlOrder);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);}
    else if(ref.page==154){screenPage(m,M::AUX_PAGE);m->params[M::auxParam(0,M::AUX_DESTINATION)].setValue(metro::ModPitchPre);m->params[M::auxParam(0,M::AUX_ASSIGNMENT)].setValue(0);}
    else if(ref.page==50){screenPage(m,M::TUNER_PAGE);m->tunerActive.store(true);m->params[M::TUNER_OCTAVE].setValue(0);m->params[M::TUNER_GATE].setValue(0);}
    else if(ref.page==136){screenPage(m,M::CV_TYPE_PAGE);m->params[M::cvTypeParam(0)].setValue(0);m->params[M::outputTrackParam(0,M::VELOCITY_SOURCE)].setValue(2);}
    else if(ref.page==90)stage(M::PITCH_OVERRIDE,28);
    else if(ref.page==92)stage(M::GATE_OVERRIDE,82);
    else if(ref.page==93)stage(M::RATCHETS,7);
    else if(ref.page==99)stage(M::PROBABILITY,58);
    else if(ref.page==101)stage(M::ACCUMULATION,22);
    else check(false,"unmapped native performance page");
    auto before=raw();auto actual=display->render();bool same=raw()==before && !APP->history->actionIndex;
    for(int y=0;y<ref.height;++y)for(int x=ref.left;x<ref.right;++x)same &= bool(actual.pixels[ref.y+y][x])==bool(ref.packed[y*16+x/8]&(1<<(7-x%8)));
    if(!same)check(false,string::f("actual native performance region p%d %s",ref.page,ref.text));modulePixels &= same;capture("native-"+std::to_string(moduleCount)+"-p"+std::to_string(ref.page));++moduleCount;
}
check(modulePixels,std::to_string(moduleCount)+" actual module text regions match native reference pixels without parameter/history changes");

bool tempo=true;int tempoCount=0;
for(bool modulated:{false,true})for(auto* control:{encoder,static_cast<HardwareControl*>(display)}){clean();screenPage(m,M::BPM_PAGE);m->params[M::ENC_CONFIRM].setValue(1);m->displayBpm.store(modulated?115:100);auto before=raw();drag(control,-40);tempo &= pendingConfirmation(m) && m->uiDraftParam==M::TEMPO && raw()==before && !APP->history->actionIndex;float preview=m->uiValue(M::TEMPO);OledCanvas expected;expected.fieldCorners(0,0,128,27);expected.valueFit(modulated?56:18,23,modulated?70:108,string::f("%.1f ?",preview),true);auto actual=display->render();for(int y=9;y<27;++y)for(int x=modulated?56:20;x<127;++x)tempo &= bool(actual.pixels[y][x])==bool(expected.pixels[y][x]);capture(std::string(modulated?"modulated":"normal")+"-pending-"+std::to_string(tempoCount));click(control);auto after=before;after[M::TEMPO]=preview;tempo &= raw()==after && !pendingConfirmation(m) && APP->history->actionIndex==1;APP->history->undo();tempo &= raw()==before;APP->history->redo();tempo &= raw()==after;++tempoCount;}
check(tempo,std::to_string(tempoCount)+" actual pending tempo renders distinguish applied modulation from a preview, retain one question mark and exact Undo/Redo");

bool cues=true;int cueCount=0;
for(int page:{M::CLOCK_OUT_PAGE,M::CV_TYPE_PAGE,M::AUX_PAGE,M::CTRL_PAGE,M::MOD_ASSIGN_PAGE,M::MX_SETUP_PAGE,M::A_PAGE})for(bool pending:{false,true}){clean();screenPage(m,page);m->params[M::ENC_CONFIRM].setValue(1);m->params[M::CLOCK_OUT_DIV].setValue(1);m->params[M::cvTypeParam(0)].setValue(0);m->params[M::outputTrackParam(0,M::VELOCITY_SOURCE)].setValue(2);m->params[M::auxParam(0,M::AUX_DESTINATION)].setValue(0);m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(0);m->params[M::MX_MODE].setValue(0);m->params[M::OUTPUT_SOURCE_A].setValue(metro::outputSource(metro::OutTrackCv,0));auto before=raw();if(pending)drag(encoder,-22);bool held=pendingConfirmation(m) || (page==M::A_PAGE && m->outputDraft[0].load()>=0);cues &= held==pending && raw()==before && !APP->history->actionIndex;OledCanvas expected;expected.turnHint(3,18,true,pending,255,page==M::CV_TYPE_PAGE);auto actual=display->render();for(int y=18;y<25;++y)for(int x=3;x<(page==M::CV_TYPE_PAGE?19:20);++x)cues &= actual.pixels[y][x]==expected.pixels[y][x];capture("cue-"+std::to_string(page)+"-"+std::to_string(pending));++cueCount;}
check(cues,std::to_string(cueCount)+" actual queued/pending field helpers follow existing confirmation state on clock, TYPE, AUX, CTRL, MOD, MX and A/B pages");
clean();for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());m->displayBpm.store(originalBpm);m->externalClock.store(originalExternal);for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
