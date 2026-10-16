using M=Metropolix;
static_assert(M::FORMAT34_NUM_PARAMS==787 && M::NUM_PARAMS==789 && M::SNAPSHOT_PARAMS==522 && M::PRESET_FORMAT==12,"Stage gestures add no saved fields");
if(std::getenv("METROPOLIX_STAGEPAIR_REOPEN_QA")) {
    if(qaFrame==6) {
        check(stagePairPage(m) && m->selectedTrack.load()==1 && m->selectedRow.load()==M::ACCUMULATION && m->selectedStage.load()==3 && !m->uiAlt.load(),"Rack reopen retains selected stage-pair screen and clears transient ALT mode");
        check(m->integer(M::stageParam(1,3,M::ACCUMULATION))==-12 && m->integer(M::stageOption(1,3,M::ACCUM_TRIGGER))==2,"signed accumulation and Ratchet trigger survive Rack reopen");
        auto preset=m->getPreset(63);check(preset.occupied && preset.values[M::presetIndex(M::stageParam(1,3,M::ACCUMULATION))]==-12 && preset.values[M::presetIndex(M::stageOption(1,3,M::ACCUM_TRIGGER))]==2,"musical preset stores paired stage edits with unchanged format");
        check(m->integer(M::MOD_GROUP_MUTE)==1 && !m->timingShortcutHeld.load(),"unrelated mute survives and transient timing state remains clear");
        check(!qaFailed,"STAGEPAIR REOPEN QA COMPLETE");APP->window->close();
    }return;
}
widget::EventContext pairContext;
event::DragStart pairStart;pairStart.context=&pairContext;pairStart.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragMove pairMove;pairMove.context=&pairContext;pairMove.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragEnd pairEnd;pairEnd.context=&pairContext;pairEnd.button=GLFW_MOUSE_BUTTON_LEFT;
auto begin=[&](HardwareControl* c){c->onDragStart(pairStart);};
auto end=[&](HardwareControl* c){c->onDragEnd(pairEnd);};
auto open=[&](int track,int row,int selected,bool alt){m->uiMod.store(false);m->selectedTrack.store(track);m->selectedRow.store(row);m->selectedStage.store(selected);screenPage(m,M::STAGE_PAGE);m->uiStageDetail.store(true);m->uiAlt.store(alt);};
auto raw=[&]{std::vector<float> values;for(auto& param:m->params)values.push_back(param.getValue());return values;};
auto unchanged=[&](const std::vector<float>& before,const std::vector<int>& allowed){bool same=true;for(int id=0;id<M::NUM_PARAMS;++id)if(std::find(allowed.begin(),allowed.end(),id)==allowed.end())same &= before[id]==m->params[id].getValue();return same;};
auto reset=[&]{auto* c=find("Encoder");begin(c);c->heldSince=system::getTime()-1.1;c->step();bool fired=c->longFired;end(c);return fired;};
auto fixture=[&](int track,int row,int choices){for(int s=0;s<8;++s){m->params[M::stageParam(track,s,row)].setValue(row==M::PROBABILITY?3+13*s:-28+8*s);m->params[M::stageOption(track,s,row==M::PROBABILITY?M::PROB_TARGET:M::ACCUM_TRIGGER)].setValue((s+1)%choices);}};
auto activate=[&](const std::string& label){auto* item=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[&](widget::Widget* w){auto* p=dynamic_cast<ui::MenuItem*>(w);return p && p->text==label;}));check(item && !item->disabled,"paired stage menu offers "+label);if(item && !item->disabled){event::Action action;item->onAction(action);}closeMenus();};
if(qaFrame==6) {
    m->params[M::RUN].setValue(0);m->params[M::RUN_ON_BOOT].setValue(2);m->params[M::ENC_CONFIRM].setValue(0);m->expanded.store(false);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    bool selectedReset=true,allReset=true,history=true,other=true,choicesPass=true,amountsKept=true,deltaPass=true;
    for(int track=0;track<2;++track)for(int row:{M::PROBABILITY,M::ACCUMULATION})for(int selected=0;selected<8;++selected) {
        int choices=row==M::PROBABILITY?2:3;
        open(track,row,selected,false);fixture(track,row,choices);auto before=raw();auto ids=stagePairIds(m);int h=APP->history->actionIndex;
        auto* encoder=find("Encoder");begin(encoder);encoder->heldSince=system::getTime()-.9;encoder->step();selectedReset &= !encoder->longFired;encoder->heldSince=system::getTime()-1.1;encoder->step();selectedReset &= encoder->longFired;end(encoder);
        for(int id:ids)selectedReset &= m->params[id].getValue()==m->getParamQuantity(id)->defaultValue;
        other &= unchanged(before,ids);if(APP->history->actionIndex!=h+1)check(false,string::f("history count track%d row%d stage%d: before%d after%d",track,row,selected,h,APP->history->actionIndex));history &= APP->history->actionIndex==h+1;APP->history->undo();if(raw()!=before){auto now=raw();for(int id=0;id<M::NUM_PARAMS;++id)if(now[id]!=before[id])check(false,string::f("undo difference track%d row%d stage%d id%d before%.3f after%.3f",track,row,selected,id,before[id],now[id]));}history &= raw()==before;APP->history->redo();
        open(track,row,selected,true);fixture(track,row,choices);before=raw();ids=stagePairIds(m);h=APP->history->actionIndex;allReset &= reset();
        for(int id:ids)allReset &= m->params[id].getValue()==m->getParamQuantity(id)->defaultValue;
        other &= unchanged(before,ids);if(APP->history->actionIndex!=h+1)check(false,string::f("history count track%d row%d stage%d: before%d after%d",track,row,selected,h,APP->history->actionIndex));history &= APP->history->actionIndex==h+1;APP->history->undo();if(raw()!=before){auto now=raw();for(int id=0;id<M::NUM_PARAMS;++id)if(now[id]!=before[id])check(false,string::f("undo difference track%d row%d stage%d id%d before%.3f after%.3f",track,row,selected,id,before[id],now[id]));}history &= raw()==before;APP->history->redo();
        for(bool alt:{false,true})for(int choice=0;choice<choices;++choice) {
            open(track,row,selected,alt);fixture(track,row,choices);m->params[stagePairSecondary(m,selected)].setValue(choice);before=raw();h=APP->history->actionIndex;click(encoder);
            std::vector<int> secondary;int next=(choice+1)%choices;
            for(int s=0;s<8;++s){int id=stagePairSecondary(m,s);if(alt || s==selected)secondary.push_back(id);choicesPass &= m->integer(id)==(alt || s==selected?next:int(before[id]));amountsKept &= m->params[M::stageParam(track,s,row)].getValue()==before[M::stageParam(track,s,row)];}
            other &= unchanged(before,secondary);if(APP->history->actionIndex!=h+1)check(false,string::f("history count track%d row%d stage%d: before%d after%d",track,row,selected,h,APP->history->actionIndex));history &= APP->history->actionIndex==h+1;APP->history->undo();if(raw()!=before){auto now=raw();for(int id=0;id<M::NUM_PARAMS;++id)if(now[id]!=before[id])check(false,string::f("undo difference track%d row%d stage%d id%d before%.3f after%.3f",track,row,selected,id,before[id],now[id]));}history &= raw()==before;APP->history->redo();
        }
        // Relative edit, saturation, then reversal within a single native drag.
        open(track,row,selected,true);fixture(track,row,choices);int amount=M::stageParam(track,selected,row);m->params[amount].setValue(row==M::PROBABILITY?50:0);before=raw();h=APP->history->actionIndex;begin(encoder);
        for(float dy:{-200.f,400.f,-320.f}) {
            pairMove.mouseDelta=Vec(0,dy);encoder->onDragMove(pairMove);float delta=m->params[amount].getValue()-before[amount];
            for(int id:stagePairIds(m,false,true)){auto* q=m->getParamQuantity(id);float expected=metro::clamp(before[id]+delta,q->getMinValue(),q->getMaxValue());deltaPass &= m->params[id].getValue()==expected;}
            deltaPass &= unchanged(before,stagePairIds(m,false,true));
        }
        end(encoder);auto after=raw();if(APP->history->actionIndex!=h+1)check(false,string::f("history count track%d row%d stage%d: before%d after%d",track,row,selected,h,APP->history->actionIndex));history &= APP->history->actionIndex==h+1;APP->history->undo();if(raw()!=before){auto now=raw();for(int id=0;id<M::NUM_PARAMS;++id)if(now[id]!=before[id])check(false,string::f("undo difference track%d row%d stage%d id%d before%.3f after%.3f",track,row,selected,id,before[id],now[id]));}history &= raw()==before;APP->history->redo();history &= raw()==after;
    }
    check(selectedReset && allReset,"PROB/ACCUM long encoder resets amount and target/trigger on every stage of both tracks; ALT resets all eight pairs after threshold");
    check(choicesPass && amountsKept,"selected/ALT encoder clicks cycle every probability target and transpose trigger, applying a uniform choice without changing amounts");
    check(deltaPass,"ALT encoder edits all eight amounts relatively with saturation and drag reversal, preserving secondary values");
    check(history && other,"paired edits/reset/ALT gestures each have one exact undo/redo and leave unrelated track/global/stage fields unchanged");
    // Public EDIT workflow, rather than only setting screen state in fixtures.
    m->uiAlt.store(false);m->uiChooseFunction.store(false);m->uiMod.store(false);click(find("TRK 1"));click(find("EDIT"));click(find("Stage 6 / PROB"));click(find("Stage 2 / SKIP"));
    check(stagePairPage(m) && m->selectedRow.load()==M::PROBABILITY && m->selectedStage.load()==1,"actual EDIT + PROB + stage selection reaches paired editor");
    m->params[M::stageParam(0,1,M::PROBABILITY)].setValue(23);m->params[M::stageOption(0,1,M::PROB_TARGET)].setValue(1);int h=APP->history->actionIndex;press(find("Encoder"),GLFW_MOUSE_BUTTON_RIGHT);activate("Reset this stage value and trigger");
    check(m->integer(M::stageParam(0,1,M::PROBABILITY))==100 && m->integer(M::stageOption(0,1,M::PROB_TARGET))==0 && APP->history->actionIndex==h+1,"encoder menu resets both probability fields with one undo");APP->history->undo();APP->history->redo();
    m->uiAlt.store(true);press(find("Stage 2 / SKIP"),GLFW_MOUSE_BUTTON_RIGHT);activate("Reset all stage values and triggers");check(m->uiAlt.load(),"ALT stage context menu exposes paired actions without losing ALT mode");
    open(1,M::ACCUMULATION,3,false);fixture(1,M::ACCUMULATION,3);m->params[M::stageOption(1,3,M::ACCUM_TRIGGER)].setValue(2);auto before=raw();h=APP->history->actionIndex;press(find("Metropolix display"),GLFW_MOUSE_BUTTON_RIGHT);activate("Apply this transpose trigger to all stages");
    bool all=true;for(int s=0;s<8;++s)all &= m->integer(M::stageOption(1,s,M::ACCUM_TRIGGER))==2 && m->params[M::stageParam(1,s,M::ACCUMULATION)].getValue()==before[M::stageParam(1,s,M::ACCUMULATION)];check(all && APP->history->actionIndex==h+1,"display menu applies current trigger across eight stages with one secondary-only undo");APP->history->undo();APP->history->redo();
    // Drafts capture amounts only; confirmation and long reset keep one action.
    bool confirmation=true;
    for(int row:{M::PROBABILITY,M::ACCUMULATION}) {
        open(1,row,3,true);fixture(1,row,row==M::PROBABILITY?2:3);before=raw();h=APP->history->actionIndex;m->params[M::ENC_CONFIRM].setValue(1);before[M::ENC_CONFIRM]=1;
        auto* encoder=find("Encoder");begin(encoder);pairMove.mouseDelta=Vec(0,-44);encoder->onDragMove(pairMove);float delta=m->uiDraftValue-m->uiDraftOrigin;
        confirmation &= m->uiDraftIds.size()==8 && unchanged(before,{}) && encoder->adjustmentTip && encoder->adjustmentTip->text.find("?")!=std::string::npos;end(encoder);confirmation &= APP->history->actionIndex==h;click(encoder);
        for(int id:stagePairIds(m,false,true)){auto* q=m->getParamQuantity(id);confirmation &= m->params[id].getValue()==metro::clamp(before[id]+delta,q->getMinValue(),q->getMaxValue());}
        confirmation &= APP->history->actionIndex==h+1 && m->uiDraftParam<0 && unchanged(before,stagePairIds(m,false,true));APP->history->undo();confirmation &= raw()==before;APP->history->redo();
        begin(encoder);pairMove.mouseDelta=Vec(0,-44);encoder->onDragMove(pairMove);end(encoder);confirmation &= m->uiDraftParam>=0;h=APP->history->actionIndex;reset();confirmation &= m->uiDraftParam<0 && APP->history->actionIndex==h+1;
        m->params[M::ENC_CONFIRM].setValue(0);
    }
    check(confirmation,"ALT pending edits capture eight amounts, apply on confirmation with one undo, and cancel on paired reset");
    open(0,M::PROBABILITY,0,false);m->uiChooseFunction.store(true);check(!stagePairPage(m),"EDIT function chooser does not activate stale paired stage controls");m->uiChooseFunction.store(false);
    open(1,M::ACCUMULATION,3,true);fixture(1,M::ACCUMULATION,3);m->params[M::stageParam(1,3,M::ACCUMULATION)].setValue(-12);m->params[M::stageOption(1,3,M::ACCUM_TRIGGER)].setValue(2);m->params[M::MOD_GROUP_MUTE].setValue(1);m->putPreset(63,m->capturePreset());APP->patch->save(dir+"/stagepair-roundtrip.vcv");
}
if(qaFrame==8){metroqa::screenshot(dir+"/accum-alt-pairs.png");auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));if(display){auto c=display->render();std::ofstream pgm(dir+"/accum-alt.pgm",std::ios::binary);pgm<<"P5\n128 64\n255\n";pgm.write(reinterpret_cast<char*>(c.pixels),128*64);}}
if(qaFrame==10){open(0,M::PROBABILITY,1,false);fixture(0,M::PROBABILITY,2);m->params[M::stageParam(0,1,M::PROBABILITY)].setValue(58);}
if(qaFrame==12){metroqa::screenshot(dir+"/prob-stage-pair.png");}
if(qaFrame==13){check(!qaFailed,"STAGEPAIR UI QA COMPLETE");APP->window->close();}
