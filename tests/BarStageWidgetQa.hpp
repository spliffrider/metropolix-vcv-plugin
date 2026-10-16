using M=Metropolix;
#include "BarStageReference.hpp"
APP->engine->bypassModule(m,true);std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));int oldPosition=m->playStage[0].load();
auto clean=[&]{closeMenus();for(auto* h:APP->history->actions)delete h;APP->history->actions.clear();APP->history->savedIndex=APP->history->actionIndex=0;m->clearWorkflows();m->clearPitchPreview();m->clearPickup();m->cancelDraft();m->params[M::RUN].setValue(0);m->params[M::ENC_CONFIRM].setValue(0);m->params[M::SHOW_POSITION].setValue(1);m->params[M::MOD_GROUP_MUTE].setValue(0);m->uiMod.store(false);m->uiAlt.store(false);m->uiEdit.store(false);m->uiChooseFunction.store(false);m->uiStageDetail.store(false);m->selectedTrack.store(0);m->selectedStage.store(0);m->uiField.store(0);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::SCALE].setValue(1);m->params[M::SCALE_LOCK].setValue(-1);m->params[M::ROOT].setValue(0);m->params[M::trackParam(0,M::TRANSPOSE)].setValue(0);screenPage(m,M::STAGE_PAGE);};
auto capture=[&](const std::string& name){auto c=display->render();std::ofstream out(dir+"/bar-stage-"+name+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<char*>(c.pixels),8192);};
if(display) {
    bool native=true;int cells=0;
    const int rows[]={M::PITCH_OVERRIDE,M::GATE_OVERRIDE,M::RATCHETS,M::PROBABILITY,M::ACCUMULATION},functionStages[]={2,3,4,5,6},selected[]={1,0,1,1,1},playing[]={3,2,3,2,2};
    const int values[][8]={{-1,28,-1,60,-1,-1,0,4},{82,-1,-1,0,-1,-1,61,10},{1,7,4,1,1,2,1,1},{100,58,100,100,82,0,100,100},{0,22,0,0,0,-21,2,-4}},options[][8]={{0},{0},{0},{0,0,0,1,1,1,0,0},{2,0,0,1,1,1,2,2}};
    const char* names[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
    for(int i=0;i<5;++i) {
        clean();m->selectedRow.store(M::SLIDE);for(int s=0;s<8;++s){m->params[M::stageParam(0,s,rows[i])].setValue(values[i][s]);if(i>=3)m->params[M::stageOption(0,s,i==3?M::PROB_TARGET:M::ACCUM_TRIGGER)].setValue(options[i][s]);}
        click(find("EDIT"));int f=functionStages[i];click(find("Stage "+std::to_string(f+1)+" / "+names[f]));click(find("Stage "+std::to_string(selected[i]+1)+" / "+names[selected[i]]));m->playStage[0].store(playing[i]);
        auto before=raw();int history=APP->history->actionIndex;auto image=display->render();bool same=raw()==before && APP->history->actionIndex==history && m->selectedRow.load()==rows[i] && m->selectedStage.load()==selected[i];
        for(int y=0;y<64;++y)for(int x=0;x<128;++x){same &= bool(image.pixels[y][x])==bool(barStageReferences[i].packed[y*16+x/8]&(1<<(7-x%8)));++cells;}
        if(!same)check(false,"complete actual native bar screen p"+std::to_string(barStageReferences[i].page));native &= same;capture("native-p"+std::to_string(barStageReferences[i].page));
    }
    check(native,std::to_string(cells)+" actual Rack foreground cells match five complete independent native pitch/gate/ratchet/probability/accumulator screens after real panel callbacks");
    clean();m->selectedRow.store(M::PITCH_OVERRIDE);m->selectedStage.store(7);m->uiStageDetail.store(true);m->params[M::stageParam(0,7,M::PITCH_OVERRIDE)].setValue(60);m->playStage[0].store(7);auto max=display->render();bool contrast=true;for(int dx=-2;dx<=2;++dx)contrast &= max.pixels[1][123+dx]==0;contrast &= max.pixels[4][123]>0;
    check(contrast,"selected maximum-pitch stage retains a contrasting edit arrow over its filled bar");capture("maximum-pitch");
    clean();m->selectedRow.store(M::ACCUMULATION);m->uiStageDetail.store(true);m->selectedStage.store(0);m->playStage[0].store(7);auto before=raw();m->params[M::stageParam(0,0,M::ACCUMULATION)].setValue(-28);m->params[M::stageParam(0,7,M::ACCUMULATION)].setValue(28);auto extremes=display->render();bool bounds=extremes.pixels[50][42]>0 && extremes.pixels[0][119]>0 && extremes.pixels[53][119]>0;
    m->params[M::SHOW_POSITION].setValue(0);auto hidden=display->render();for(int x=119;x<128;++x)bounds &= hidden.pixels[53][x]==0;check(bounds,"positive/negative accumulator limits and Show Pos stay inside their own stage columns");capture("accum-limits");
}
clean();m->playStage[0].store(oldPosition);for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());for(auto* h:APP->history->actions)delete h;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
