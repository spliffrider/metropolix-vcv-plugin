using M=Metropolix;
#include "CurveStageReference.hpp"
APP->engine->bypassModule(m,true);std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));int oldPosition=m->playStage[0].load(),oldMod=m->modStage[0].load();
auto clean=[&]{closeMenus();for(auto* h:APP->history->actions)delete h;APP->history->actions.clear();APP->history->savedIndex=APP->history->actionIndex=0;m->clearWorkflows();m->clearPitchPreview();m->clearPickup();m->cancelDraft();m->params[M::RUN].setValue(0);m->params[M::ENC_CONFIRM].setValue(0);m->params[M::SHOW_POSITION].setValue(1);m->params[M::MOD_GROUP_MUTE].setValue(0);m->params[M::OUTPUT_SOURCE_A].setValue(0);m->params[M::OUTPUT_SOURCE_B].setValue(1);m->params[M::cvTypeParam(0)].setValue(metro::BipolarCv);m->params[M::outputTrackParam(0,M::VELOCITY_SOURCE)].setValue(2);m->params[M::midiTrackParam(0,M::MIDI_CV_CC)].setValue(1);m->params[M::midiModParam(0,M::MIDI_MOD_CC)].setValue(1);m->uiMod.store(false);m->uiAlt.store(false);m->uiEdit.store(false);m->uiChooseFunction.store(false);m->uiStageDetail.store(false);m->selectedTrack.store(0);m->selectedLane.store(0);m->selectedStage.store(0);m->uiField.store(0);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);screenPage(m,M::STAGE_PAGE);};
auto capture=[&](const std::string& name){auto c=display->render();std::ofstream out(dir+"/curve-stage-"+name+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<char*>(c.pixels),8192);};
if(display) {
    bool native=true;int cells=0;
    for(int mode=0;mode<2;++mode) {
        clean();const float volts[]={5,-1.8f,1.62f,-3.8f,-2.4f,4,-5,-2.4f};const int ramps[]={0,0,2,0,1,1,1,1};
        for(int s=0;s<8;++s){m->params[M::stageParam(0,s,M::CV)].setValue(volts[s]);m->params[M::cvRampParam(0,s)].setValue(ramps[s]);m->params[M::PULSES+s].setValue(s==2?2:1);m->params[M::modParam(0,M::MOD_VALUE+s)].setValue(s==2?2.f:volts[s]);m->params[M::modParam(0,M::MOD_RAMP+s)].setValue(bool(ramps[s]));}
        if(mode){m->uiMod.store(true);m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModProbability);m->params[M::OUTPUT_SOURCE_A].setValue(2);}
        else m->selectedRow.store(M::CV);
        click(find("Stage 3 / PITCH"));m->playStage[0].store(0);m->modStage[0].store(0);auto before=raw();int history=APP->history->actionIndex;auto image=display->render();bool same=raw()==before && APP->history->actionIndex==history;
        for(const auto& reference:curveStageReferences)if(reference.page==(mode?137:103))for(int y=0;y<reference.h;++y)for(int x=0;x<reference.w;++x){int index=y*reference.w+x;if(reference.mask[index]){same &= bool(image.pixels[reference.y+y][reference.x+x])==bool(reference.expected[index]);++cells;}}
        if(!same)check(false,"coherent native curve sidebar/helper/footer p"+std::to_string(mode?137:103));native &= same;capture(mode?"native-p137-regions":"native-p103-regions");
        bool underline=true;for(int x=40;x<48;++x)underline &= image.pixels[53][x]>0;for(int x=0;x<38;++x)underline &= image.pixels[53][x]==0;check(underline,"detailed CV/MOD playing underline stays in the graph beside the sidebar");
        if(!mode){bool pulse=image.pixels[14][65]>0 && image.pixels[18][65]>0;m->params[M::cvRampParam(0,2)].setValue(metro::CvStageRamp);auto stage=display->render();pulse &= stage.pixels[14][71]>0 && stage.pixels[18][71]>0;check(pulse,"CV pulse-ramp vertex reaches the first pulse boundary while a stage ramp reaches the stage end");capture("stage-ramp");}
    }
    check(native,std::to_string(cells)+" coherent native curve sidebar/helper/footer cells match actual Rack CV and destination-unit MOD screens; filtered and illustrative graph/MIDI cells are not claimed");
    clean();m->params[M::ENC_CONFIRM].setValue(1);screenPage(m,M::CV_TYPE_PAGE);auto before=raw();auto type=display->render();bool same=raw()==before;
    for(int y=0;y<64;++y)for(int x=0;x<128;++x)same &= bool(type.pixels[y][x])==bool(cvTypeNative[y*16+x/8]&(1<<(7-x%8)));check(same,"all8192 foreground cells match the complete native CV Type screen in its illuminated frame phase");capture("native-type-p136");
}
clean();m->playStage[0].store(oldPosition);m->modStage[0].store(oldMod);for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());for(auto* h:APP->history->actions)delete h;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
