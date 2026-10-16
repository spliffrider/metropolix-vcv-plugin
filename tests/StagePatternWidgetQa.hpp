using M=Metropolix;
static_assert(M::FORMAT34_NUM_PARAMS==787 && M::NUM_PARAMS==789 && M::SNAPSHOT_PARAMS==522 && M::PRESET_FORMAT==12,"Stage pattern gestures add no saved fields");
if(std::getenv("METROPOLIX_STAGEPATTERN_REOPEN_QA")) {
    if(qaFrame==6) {
        bool values=true;for(int s=0;s<8;++s)values &= m->integer(M::stageParam(1,s,M::RATCHETS))==s+1;
        check(simpleStagePage(m) && m->selectedTrack.load()==1 && m->selectedRow.load()==M::RATCHETS && !m->uiAlt.load(),"Rack reopen retains ratchet editor and clears transient ALT");
        check(values && m->integer(M::stageParam(0,2,M::SKIP))==1 && m->integer(M::stageParam(0,5,M::SLIDE))==1,"ratchet counts and unrelated skip/slide patterns survive reopen");
        auto preset=m->getPreset(62);check(preset.occupied && preset.values[M::presetIndex(M::stageParam(1,7,M::RATCHETS))]==8 && m->integer(M::MOD_GROUP_MUTE)==1,"musical preset stores stage pattern/counts with unchanged format and unrelated mute");
        check(!qaFailed,"STAGEPATTERN REOPEN QA COMPLETE");APP->window->close();
    }return;
}
widget::EventContext patternContext;
event::DragStart patternStart;patternStart.context=&patternContext;patternStart.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragMove patternMove;patternMove.context=&patternContext;patternMove.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragEnd patternEnd;patternEnd.context=&patternContext;patternEnd.button=GLFW_MOUSE_BUTTON_LEFT;
auto begin=[&](HardwareControl* c){c->onDragStart(patternStart);};
auto end=[&](HardwareControl* c){c->onDragEnd(patternEnd);};
auto open=[&](int track,int row,int selected,bool alt){m->uiMod.store(false);m->selectedTrack.store(track);m->selectedRow.store(row);m->selectedStage.store(selected);screenPage(m,M::STAGE_PAGE);m->uiStageDetail.store(true);m->uiAlt.store(alt);};
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};
auto sameOther=[&](const std::vector<float>& before,const std::vector<int>& allowed){for(int id=0;id<M::NUM_PARAMS;++id)if(std::find(allowed.begin(),allowed.end(),id)==allowed.end() && before[id]!=m->params[id].getValue())return false;return true;};
auto lastAction=[&]()->history::Action*{return APP->history->actions.empty()?nullptr:APP->history->actions.back();};
auto historyExact=[&](history::Action* previous,const std::vector<float>& before,const std::vector<float>& after){if(before==after)return lastAction()==previous;auto* action=lastAction();bool okay=action && action!=previous && (dynamic_cast<history::ComplexAction*>(action) || dynamic_cast<history::ParamChange*>(action));APP->history->undo();okay &= raw()==before;APP->history->redo();return okay && raw()==after;};
auto setMask=[&](int track,int row,int mask){for(int s=0;s<8;++s)m->params[M::stageParam(track,s,row)].setValue((mask>>s)&1);};
auto maskOf=[&](int track,int row){int mask=0;for(int s=0;s<8;++s)mask|=m->integer(M::stageParam(track,s,row))<<s;return mask;};
auto rotated=[](int mask,int shift){int out=0;for(int s=0;s<8;++s)if(mask&(1<<s))out|=1<<metro::modulo(s+shift,8);return out;};
auto reset=[&]{auto* c=find("Encoder");begin(c);c->heldSince=system::getTime()-1.1;c->step();bool fired=c->longFired;end(c);return fired;};
auto menuItem=[&](const std::string& label)->ui::MenuItem*{for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if((*it)->visible && dynamic_cast<ui::MenuOverlay*>(*it))return dynamic_cast<ui::MenuItem*>(seek(*it,[&](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text==label;}));return nullptr;};
auto activate=[&](const std::string& label){auto* item=menuItem(label);check(item && !item->disabled,"stage pattern menu offers "+label);if(item && !item->disabled){event::Action e;item->onAction(e);}closeMenus();};
if(qaFrame==6) {
    m->params[M::RUN].setValue(0);m->params[M::RUN_ON_BOOT].setValue(2);m->params[M::ENC_CONFIRM].setValue(1);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    bool rotations=true,inversions=true,resets=true,history=true,unrelated=true,instant=true;auto* encoder=find("Encoder");
    for(int track=0;track<2;++track)for(int row:{M::SLIDE,M::SKIP})for(int mask=0;mask<256;++mask) {
        for(int shift:{-9,-1,1,9,8}) {
            open(track,row,mask%8,true);setMask(track,row,mask);auto before=raw();auto* previous=lastAction();begin(encoder);patternMove.mouseDelta=Vec(0,-22.f*shift);encoder->onDragMove(patternMove);end(encoder);auto after=raw();
            rotations &= maskOf(track,row)==rotated(mask,shift);history &= historyExact(previous,before,after);unrelated &= sameOther(before,simpleStageIds(m,true));instant &= m->uiDraftParam<0;
        }
        open(track,row,mask%8,true);setMask(track,row,mask);auto before=raw();auto* previous=lastAction();click(encoder);auto after=raw();inversions &= maskOf(track,row)==(mask^255);history &= historyExact(previous,before,after);unrelated &= sameOther(before,simpleStageIds(m,true));click(encoder);inversions &= maskOf(track,row)==mask;
        before=raw();previous=lastAction();resets &= reset() && maskOf(track,row)==0;after=raw();history &= historyExact(previous,before,after);unrelated &= sameOther(before,simpleStageIds(m,true));
    }
    check(rotations,"ALT encoder rotates all 256 Slide/Skip patterns forward/backward across both tracks, including multi-wrap and full-wrap no-op");
    check(inversions,"ALT encoder inverts every Slide/Skip pattern and a second inversion restores it");
    check(resets,"ALT long encoder resets every Slide/Skip pattern to eight Off states");
    check(history && unrelated,"all binary pattern operations have exact one-action undo/redo or no-op history, leaving other rows/track/global values intact");
    check(instant,"binary pattern operations apply directly even with Enc Confirm enabled");
    // Reversal and threshold within one gesture; unmodified long is inactive.
    open(0,M::SKIP,2,true);setMask(0,M::SKIP,0x2d);auto before=raw();auto* previous=lastAction();begin(encoder);patternMove.mouseDelta=Vec(0,-66);encoder->onDragMove(patternMove);patternMove.mouseDelta=Vec(0,44);encoder->onDragMove(patternMove);end(encoder);check(maskOf(0,M::SKIP)==rotated(0x2d,1) && historyExact(previous,before,raw()),"rotation drag reversal records only its final pattern");
    before=raw();begin(encoder);encoder->heldSince=system::getTime()-.9;encoder->step();check(!encoder->longFired && raw()==before,"binary pattern reset waits for threshold");encoder->heldSince=system::getTime()-1.1;encoder->step();end(encoder);check(encoder->longFired && maskOf(0,M::SKIP)==0,"binary pattern reset fires after threshold");
    m->uiAlt.store(false);before=raw();check(!reset() && raw()==before,"ordinary Slide/Skip long encoder does not activate an undocumented reset");
    // Ratchet selected/all reset and relative count edits.
    bool ratchetReset=true,ratchetDelta=true,ratchetHistory=true; m->params[M::ENC_CONFIRM].setValue(0);
    for(int track=0;track<2;++track)for(int selected=0;selected<8;++selected) {
        for(bool alt:{false,true}) {
            open(track,M::RATCHETS,selected,alt);for(int s=0;s<8;++s)m->params[M::stageParam(track,s,M::RATCHETS)].setValue(s==selected?8:s+1);before=raw();previous=lastAction();ratchetReset &= reset();
            for(int s=0;s<8;++s)ratchetReset &= m->integer(M::stageParam(track,s,M::RATCHETS))==(alt || s==selected?1:int(before[M::stageParam(track,s,M::RATCHETS)]));
            ratchetHistory &= historyExact(previous,before,raw()) && sameOther(before,simpleStageIds(m));
        }
        open(track,M::RATCHETS,selected,true);for(int s=0;s<8;++s)m->params[M::stageParam(track,s,M::RATCHETS)].setValue(s+1);int amount=M::stageParam(track,selected,M::RATCHETS);m->params[amount].setValue(4);before=raw();previous=lastAction();begin(encoder);
        for(float dy:{-176.f,352.f,-264.f}){patternMove.mouseDelta=Vec(0,dy);encoder->onDragMove(patternMove);float delta=m->params[amount].getValue()-before[amount];for(int id:simpleStageIds(m))ratchetDelta &= m->params[id].getValue()==metro::clamp(before[id]+delta,1.f,8.f);}
        end(encoder);ratchetHistory &= historyExact(previous,before,raw()) && sameOther(before,simpleStageIds(m));
    }
    check(ratchetReset,"Ratchet long encoder resets selected/all counts to 1 on every stage of both tracks");
    check(ratchetDelta && ratchetHistory,"ALT Ratchet count edits preserve relative values through clipping/reversal with exact grouped undo");
    open(1,M::RATCHETS,3,true);for(int s=0;s<8;++s)m->params[M::stageParam(1,s,M::RATCHETS)].setValue(1+s%5);m->params[M::ENC_CONFIRM].setValue(1);before=raw();previous=lastAction();begin(encoder);patternMove.mouseDelta=Vec(0,-44);encoder->onDragMove(patternMove);end(encoder);bool pending=m->uiDraftIds.size()==8 && raw()==before;float delta=m->uiDraftValue-m->uiDraftOrigin;click(encoder);for(int id:simpleStageIds(m))pending &= m->params[id].getValue()==metro::clamp(before[id]+delta,1.f,8.f);check(pending && historyExact(previous,before,raw()),"Ratchet confirmation previews eight counts and applies them with exact one-action undo");m->params[M::ENC_CONFIRM].setValue(0);
    // Public EDIT/ALT workflow and actual native menu callbacks.
    m->uiAlt.store(false);click(find("TRK 1"));click(find("EDIT"));click(find("Stage 2 / SKIP"));click(find("ALT"));setMask(0,M::SKIP,0x25);before=raw();previous=lastAction();drag(find("Metropolix display"),-22);check(binaryStagePage(m) && maskOf(0,M::SKIP)==rotated(0x25,1) && historyExact(previous,before,raw()),"EDIT + SKIP + ALT reaches display rotation with grouped undo");
    press(find("Metropolix display"),GLFW_MOUSE_BUTTON_RIGHT);activate("Invert pattern");check(maskOf(0,M::SKIP)==(rotated(0x25,1)^255),"display menu inverts current skip pattern");
    press(find("Stage 4 / GATE"),GLFW_MOUSE_BUTTON_RIGHT);activate("Reset all stages");check(maskOf(0,M::SKIP)==0 && m->uiAlt.load(),"ALT stage menu resets pattern while retaining mode");
    open(1,M::RATCHETS,3,false);m->params[M::stageParam(1,3,M::RATCHETS)].setValue(8);press(encoder,GLFW_MOUSE_BUTTON_RIGHT);activate("Reset this stage");check(m->integer(M::stageParam(1,3,M::RATCHETS))==1,"encoder menu resets selected ratchet count");
    open(1,M::RATCHETS,3,true);for(int s=0;s<8;++s)m->params[M::stageParam(1,s,M::RATCHETS)].setValue(s+1);setMask(0,M::SKIP,4);setMask(0,M::SLIDE,32);m->params[M::MOD_GROUP_MUTE].setValue(1);m->putPreset(62,m->capturePreset());APP->patch->save(dir+"/stagepattern-roundtrip.vcv");
}
if(qaFrame==8){metroqa::screenshot(dir+"/ratchets-all.png");}
if(qaFrame==10){open(0,M::SKIP,2,true);setMask(0,M::SKIP,0x2d);}
if(qaFrame==12){metroqa::screenshot(dir+"/skip-pattern-all.png");}
if(qaFrame==13){check(!qaFailed,"STAGEPATTERN UI QA COMPLETE");APP->window->close();}
