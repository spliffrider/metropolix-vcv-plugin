using M=Metropolix;
#include "BinaryStageReference.hpp"
APP->engine->bypassModule(m,true);std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));
auto clean=[&]{closeMenus();for(auto* h:APP->history->actions)delete h;APP->history->actions.clear();APP->history->savedIndex=APP->history->actionIndex=0;m->clearWorkflows();m->clearPitchPreview();m->clearPickup();m->cancelDraft();m->params[M::RUN].setValue(0);m->params[M::ENC_CONFIRM].setValue(0);m->params[M::SHOW_POSITION].setValue(0);m->params[M::MOD_GROUP_MUTE].setValue(0);m->params[M::OUTPUT_SOURCE_A].setValue(0);m->params[M::OUTPUT_SOURCE_B].setValue(1);m->uiMod.store(false);m->uiAlt.store(false);m->uiEdit.store(false);m->uiChooseFunction.store(false);m->uiStageDetail.store(false);m->selectedTrack.store(0);m->selectedLane.store(0);m->selectedStage.store(0);m->selectedRow.store(M::SLIDE);m->uiField.store(0);for(int k=0;k<2;++k){m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->outputDraft[k].store(-1);}for(int t=0;t<2;++t){m->params[M::cvTypeParam(t)].setValue(metro::GateCv);m->params[M::midiTrackParam(t,M::MIDI_CV_CC)].setValue(0);for(int s=0;s<8;++s)for(int row:{M::SLIDE,M::SKIP,M::CV})m->params[M::stageParam(t,s,row)].setValue(0);}for(int l=0;l<8;++l){m->params[M::modParam(l,M::MOD_DESTINATION)].setValue(metro::ModNone);m->params[M::modParam(l,M::MOD_TYPE)].setValue(metro::ModBipolar);m->params[M::modParam(l,M::MOD_MUTE)].setValue(0);m->params[M::midiModParam(l,M::MIDI_MOD_CC)].setValue(0);for(int s=0;s<8;++s)m->params[M::modParam(l,M::MOD_VALUE+s)].setValue(0);}screenPage(m,M::STAGE_PAGE);};
auto capture=[&](const std::string& name){auto c=display->render();std::ofstream out(dir+"/binary-stage-"+name+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<char*>(c.pixels),8192);};
auto exact=[&](const std::vector<float>& before,const std::vector<float>& after){bool ok=raw()==after && APP->history->actionIndex==1;APP->history->undo();ok &= raw()==before;APP->history->redo();return ok && raw()==after;};
if(display) {
    bool native=true;int cells=0;
    for(const auto& reference:binaryStageReferences) {
        clean();int page=reference.page;
        if(page==87 || page==88) {int row=page==87?M::SLIDE:M::SKIP;m->selectedRow.store(row);for(int stage=0;stage<8;++stage)m->params[M::stageParam(0,stage,row)].setValue(page==87?(stage==4 || stage==5):(stage==2 || stage==4 || stage==5));click(find("EDIT"));click(find(page==87?"Stage 1 / SLIDE":"Stage 2 / SKIP"));}
        else if(page==106) {m->selectedRow.store(M::CV);for(int stage:{2,4,5})m->params[M::stageParam(0,stage,M::CV)].setValue(5);click(find("EDIT"));click(find("Stage 8 / CV"));click(find("Stage 3 / PITCH"));}
        else if(page==140) {m->uiMod.store(true);m->selectedLane.store(1);m->params[M::modParam(1,M::MOD_DESTINATION)].setValue(metro::ModDirection);m->params[M::OUTPUT_SOURCE_A].setValue(3);for(int stage:{0,4,5})m->params[M::modParam(1,M::MOD_VALUE+stage)].setValue(5);click(find("Stage 1 / SLIDE"));}
        else {m->uiMod.store(true);m->selectedLane.store(0);m->params[M::OUTPUT_SOURCE_A].setValue(2);click(find("ALT"));click(find("Stage 3 / PITCH"));}
        auto before=raw();int history=APP->history->actionIndex;auto image=display->render();bool same=raw()==before && APP->history->actionIndex==history;
        for(int y=0;y<64;++y)for(int x=0;x<128;++x){same &= bool(image.pixels[y][x])==bool(reference.packed[y*16+x/8]&(1<<(7-x%8)));++cells;}
        if(!same)check(false,"complete actual native binary/output screen p"+std::to_string(page));native &= same;capture("native-p"+std::to_string(page));
    }
    check(native,std::to_string(cells)+" actual Rack foreground cells match five complete independent native binary/output screens after real panel callbacks");
    bool actions=true;int count=0;
    for(int mode=0;mode<4;++mode)for(int target=0;target<(mode==3?8:2);++target)for(int stage=0;stage<8;++stage) {
        clean();int row=mode==0?M::SLIDE:mode==1?M::SKIP:M::CV,id;
        if(mode==3){m->uiMod.store(true);m->selectedLane.store(target);m->params[M::modParam(target,M::MOD_DESTINATION)].setValue(metro::ModDirection);id=M::modParam(target,M::MOD_VALUE+stage);}
        else {m->selectedTrack.store(target);m->selectedRow.store(row);id=M::stageParam(target,stage,row);}
        const char* names[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
        auto before=raw();click(find("Stage "+std::to_string(stage+1)+" / "+names[stage]));auto after=before;after[id]=mode<2?1:5;
        actions &= m->selectedStage.load()==stage && (mode==3?m->selectedLane.load():m->selectedTrack.load())==target && exact(before,after);++count;
    }
    check(actions,std::to_string(count)+" actual binary stage buttons retain target selection, one-parameter edits and exact Undo/Redo across both tracks and eight MOD lanes");
    clean();m->selectedRow.store(M::SLIDE);auto before=raw();auto plain=display->render();m->uiStageDetail.store(true);auto detailed=display->render();bool noArrow=true;for(int y=0;y<54;++y)for(int x=0;x<128;++x)noArrow &= plain.pixels[y][x]==detailed.pixels[y][x];check(noArrow && raw()==before,"binary overview/detail share native circles without redundant stage numbers or cursor arrows");
    m->params[M::SHOW_POSITION].setValue(1);int position=m->playStage[0].load();m->playStage[0].store(3);auto playing=display->render();bool underline=true;for(int x=50;x<61;++x)underline &= playing.pixels[53][x]>0;m->params[M::SHOW_POSITION].setValue(0);auto hidden=display->render();for(int x=50;x<61;++x)underline &= hidden.pixels[53][x]==0;m->playStage[0].store(position);check(underline,"binary stage diagrams retain optional playing-position underlines");capture("position-hidden");
}
clean();for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());for(auto* h:APP->history->actions)delete h;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
