// Real Rack event callbacks. OS mouse/focus behavior remains a human check.
using M=Metropolix;
static_assert(M::FORMAT34_NUM_PARAMS==787 && M::NUM_PARAMS==789 && M::SNAPSHOT_PARAMS==522 && M::PRESET_FORMAT==12,"Timing adds no saved fields");
if(std::getenv("METROPOLIX_TIMING_REOPEN_QA")) {
    if(qaFrame==6) {
        check(!m->timingShortcutHeld.load() && !m->timingShortcutUsed.load(),"Rack reopen clears transient LEN/DIV held gestures");
        check(m->uiPage.load()==M::LENGTH_PAGE && m->uiField.load()==1 && !m->uiMod.load(),"Rack reopen retains timing screen and selected pulse field");
        check(m->integer(M::extraParam(1,M::PULSE_LIMIT))==48 && m->integer(M::trackParam(1,M::OFFSET))==3,"Rack reopen retains pulse length and offset");
        check(m->integer(M::MOD_GROUP_MUTE)==1 && m->integer(M::modParam(7,M::MOD_DIVISION))==16,"Rack reopen retains unrelated group mute and MOD clock division");
        check(!qaFailed,"TIMING REOPEN QA COMPLETE");APP->window->close();
    }return;
}
widget::EventContext timingContext;
event::DragStart timingStart;timingStart.context=&timingContext;timingStart.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragEnd timingEnd;timingEnd.context=&timingContext;timingEnd.button=GLFW_MOUSE_BUTTON_LEFT;
auto begin=[&](HardwareControl* c){c->onDragStart(timingStart);};
auto end=[&](HardwareControl* c){c->onDragEnd(timingEnd);};
const char* timingStageNames[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
auto stage=[&](int slot){return find("Stage "+std::to_string(slot+1)+" / "+timingStageNames[slot]);};
auto menuItem=[&](const std::string& text){return dynamic_cast<ui::MenuItem*>(seek(APP->scene,[&](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text==text;}));};
auto activate=[&](const std::string& text){auto* item=menuItem(text);check(item && !item->disabled,"timing menu enables "+text);if(item && !item->disabled){event::Action action;item->onAction(action);}closeMenus();};
if(qaFrame==6) {
    m->params[M::RUN].setValue(0);m->params[M::RUN_ON_BOOT].setValue(2);m->expanded.store(false);m->params[M::ENC_CONFIRM].setValue(0);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->selectedStage.store(5);m->selectedRow.store(M::SKIP);m->uiChooseFunction.store(false);
    bool allShortcuts=true,allHistory=true,unrelated=true,bindings=true,lights=true;
    // Every stage shortcut, both fields and all ten targets. Values are literal
    // manual fixtures, independent of the implementation's lookup helper.
    const int clockValues[]={1,2,3,4,6,8,16,32};
    for(int page:{M::LENGTH_PAGE,M::DIV_PAGE})for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target)for(int field=0;field<2;++field)for(int slot=0;slot<8;++slot) {
        m->uiMod.store(mod);m->selectedTrack.store(mod?0:target);m->selectedLane.store(mod?target:0);screenPage(m,page);m->uiField.store(field);
        int id=timingParam(m,page,field),expected=page==M::LENGTH_PAGE?(field?8*(slot+1):slot+1):field && !mod?slot+1:clockValues[slot];
        m->params[id].setValue(expected==1?2:1);float prior=m->params[id].getValue();std::vector<float> before;for(auto& p:m->params)before.push_back(p.getValue());
        int h=APP->history->actionIndex;auto* parent=find(page==M::LENGTH_PAGE?"LEN":"DIV");begin(parent);
        bindings &= parent->getParamQuantity() && parent->paramId==id && m->uiField.load()==field;
        auto* child=stage(slot);child->bind();bindings &= !child->getParamQuantity();click(child);
        allShortcuts &= m->integer(id)==expected && m->uiPage.load()==page && m->uiField.load()==field && m->selectedStage.load()==5 && !m->workflowStageHeld;
        lights &= child->active() && child->detailHelp().find(timingShortcutLabel(m,page,slot))!=std::string::npos;
        allHistory &= APP->history->actionIndex==h;end(parent);allHistory &= APP->history->actionIndex==h+1 && !m->timingShortcutHeld.load();
        for(int other=0;other<M::NUM_PARAMS;++other)if(other!=id)unrelated &= before[other]==m->params[other].getValue();
        APP->history->undo();allHistory &= m->params[id].getValue()==prior;APP->history->redo();allHistory &= m->integer(id)==expected;
    }
    check(allShortcuts,"LEN/DIV manual shortcuts cover every stage, both fields and all two note/eight MOD targets without changing stage selection");
    check(bindings && lights,"held timing preserves selected field, disables stage value edits and shows selected shortcut/readout");
    check(allHistory && unrelated,"each held timing gesture is one undo/redo with no child action or unrelated parameter changes");
    bool resets=true;
    for(int page:{M::LENGTH_PAGE,M::DIV_PAGE})for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target) {
        m->uiMod.store(mod);m->selectedTrack.store(mod?0:target);m->selectedLane.store(mod?target:0);screenPage(m,page);auto ids=timingIds(m,page);
        std::vector<float> before;for(int id:ids){auto* q=m->getParamQuantity(id);q->setValue(q->defaultValue==q->getMaxValue()?q->getMinValue():q->getMaxValue());before.push_back(q->getValue());}
        auto* encoder=find("Encoder");int h=APP->history->actionIndex;begin(encoder);encoder->heldSince=system::getTime()-.9;encoder->step();resets &= !encoder->longFired;
        encoder->heldSince=system::getTime()-1.1;encoder->step();resets &= encoder->longFired;end(encoder);resets &= APP->history->actionIndex==h+1;
        for(int id:ids)resets &= m->params[id].getValue()==m->getParamQuantity(id)->defaultValue;
        APP->history->undo();for(size_t i=0;i<ids.size();++i)resets &= m->params[ids[i]].getValue()==before[i];APP->history->redo();
    }
    check(resets,"long encoder resets all three LEN/two note DIV/one MOD DIV fields after threshold with one undo/redo");
    m->uiMod.store(false);m->selectedTrack.store(0);screenPage(m,M::LENGTH_PAGE);m->uiField.store(1);
    m->params[M::extraParam(0,M::PULSE_LIMIT)].setValue(8);drag(find("LEN"),-50);check(m->integer(M::extraParam(0,M::PULSE_LIMIT))>8 && m->uiField.load()==1,"upward panel drag edits the active pulse field and retains it");
    drag(find("LEN"),50);check(m->integer(M::extraParam(0,M::PULSE_LIMIT))==8,"downward panel drag decreases pulse length");
    press(find("LEN"),GLFW_MOUSE_BUTTON_RIGHT);auto* overlay=seek(APP->scene,[](widget::Widget* w){return w->visible && dynamic_cast<ui::MenuOverlay*>(w);});
    auto* entry=overlay?dynamic_cast<ui::TextField*>(seek(overlay,[m](widget::Widget* w){auto* f=dynamic_cast<ui::TextField*>(w);return f && f->text==m->getParamQuantity(M::extraParam(0,M::PULSE_LIMIT))->getDisplayValueString();})):nullptr;
    check(entry!=nullptr,"timing button retains Rack numeric entry");
    if(entry){entry->text="37";event::SelectKey key;key.context=&timingContext;key.key=GLFW_KEY_ENTER;key.action=GLFW_PRESS;key.mods=0;entry->onSelectKey(key);check(m->integer(M::extraParam(0,M::PULSE_LIMIT))==37,"Rack numeric entry edits the selected pulse parameter (actual="+std::to_string(m->integer(M::extraParam(0,M::PULSE_LIMIT)))+", field="+entry->text+", bound="+std::to_string(find("LEN")->paramId)+", consumed="+std::to_string(timingContext.consumed)+")");}closeMenus();
    press(find("Encoder"),GLFW_MOUSE_BUTTON_RIGHT);activate("Edit stages");check(m->uiField.load()==0 && !m->uiAlt.load(),"encoder context menu selects stages");
    press(find("LEN"),GLFW_MOUSE_BUTTON_RIGHT);activate("Edit stage offset");check(m->uiAlt.load() && screenParam(m)==M::trackParam(0,M::OFFSET),"LEN menu exposes existing ALT offset field");
    m->params[M::trackParam(0,M::LENGTH)].setValue(3);press(stage(7),GLFW_MOUSE_BUTTON_RIGHT);int h=APP->history->actionIndex;activate("Set 8 stages");check(m->integer(M::trackParam(0,M::LENGTH))==8 && APP->history->actionIndex==h+1 && !m->uiAlt.load(),"stage menu applies a direct timing shortcut with one undo");APP->history->undo();APP->history->redo();
    // Two targets and a MOD lane under one parent receipt. Unrelated mute stays
    // outside it even if changed while the timing button is held.
    screenPage(m,M::DIV_PAGE);m->uiField.store(0);m->params[M::trackParam(0,M::DIVISION)].setValue(3);m->params[M::trackParam(1,M::DIVISION)].setValue(4);m->params[M::modParam(7,M::MOD_DIVISION)].setValue(2);m->selectedLane.store(7);
    h=APP->history->actionIndex;auto* division=find("DIV");begin(division);click(stage(5));click(find("TRK 2"));click(stage(6));click(find("MOD"));click(stage(7));m->params[M::MOD_GROUP_MUTE].setValue(1);end(division);
    check(APP->history->actionIndex==h+1 && m->uiMod.load() && m->uiPage.load()==M::DIV_PAGE,"one held DIV gesture spans TRK 1, TRK 2 and MOD without leaving timing page");
    APP->history->undo();check(m->integer(M::trackParam(0,M::DIVISION))==3 && m->integer(M::trackParam(1,M::DIVISION))==4 && m->integer(M::modParam(7,M::MOD_DIVISION))==2 && m->integer(M::MOD_GROUP_MUTE)==1,"multi-target undo restores timing fields while preserving unrelated group mute");APP->history->redo();
    m->uiMod.store(false);m->selectedTrack.store(0);screenPage(m,M::LENGTH_PAGE);m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlStagesLength);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);m->params[M::ctrlParam(0,M::CTRL_VALUE)].setValue(0);
}
if(qaFrame==8) {
    m->uiField.store(0);auto* len=find("LEN");int controlled=M::trackParam(0,M::LENGTH),h=APP->history->actionIndex;begin(len);
    check(std::find(len->gestureIds.begin(),len->gestureIds.end(),controlled)==len->gestureIds.end(),"held LEN receipt excludes CTRL-owned timing");
    click(stage(7));end(len);check(APP->history->actionIndex==h && m->workflowMessage=="TIMING OWNED BY CTRL","CTRL-owned shortcut is blocked without history");
    press(stage(7),GLFW_MOUSE_BUTTON_RIGHT);auto* blocked=menuItem("Set 8 stages");check(blocked && blocked->disabled,"stage shortcut menu disables CTRL-owned field");closeMenus();
    m->params[M::trackParam(0,M::OFFSET)].setValue(4);m->params[M::extraParam(0,M::PULSE_LIMIT)].setValue(32);
    auto* encoder=find("Encoder");h=APP->history->actionIndex;begin(encoder);check(std::find(encoder->gestureIds.begin(),encoder->gestureIds.end(),controlled)==encoder->gestureIds.end(),"long reset receipt excludes CTRL-owned field");encoder->heldSince=system::getTime()-1.1;encoder->step();end(encoder);
    check(APP->history->actionIndex==h+1 && m->integer(M::trackParam(0,M::OFFSET))==0 && m->integer(M::extraParam(0,M::PULSE_LIMIT))==0,"long reset resets unowned fields even when active Stages is CTRL-owned");APP->history->undo();check(m->integer(M::trackParam(0,M::OFFSET))==4 && m->integer(M::extraParam(0,M::PULSE_LIMIT))==32,"partial reset undo restores only unowned fields");APP->history->redo();
    m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->params[M::ENC_CONFIRM].setValue(1);m->uiField.store(1);float delta=0;turnDraft(m,M::extraParam(0,M::PULSE_LIMIT),-44,delta,false);check(m->uiDraftParam>=0,"confirmation draft fixture is active");
    begin(len);click(stage(5));end(len);check(m->uiDraftParam<0 && m->integer(M::extraParam(0,M::PULSE_LIMIT))==48,"explicit held shortcut cancels pending draft and applies without an extra encoder press");
    m->params[M::ENC_CONFIRM].setValue(0);begin(len);click(find("EXIT"));end(len);check(!m->timingShortcutHeld.load() && m->uiPage.load()==M::HOME,"EXIT cancels held timing without parent release reopening its page");
    m->selectedTrack.store(1);screenPage(m,M::LENGTH_PAGE);m->uiField.store(1);m->params[M::extraParam(1,M::PULSE_LIMIT)].setValue(48);m->params[M::trackParam(1,M::OFFSET)].setValue(3);m->params[M::modParam(7,M::MOD_DIVISION)].setValue(16);
    begin(len);APP->patch->save(dir+"/timing-roundtrip.vcv");
}
if(qaFrame==10){metroqa::screenshot(dir+"/len-held-pulses.png");end(find("LEN"));}
if(qaFrame==12){screenPage(m,M::DIV_PAGE);m->uiMod.store(true);m->selectedLane.store(7);begin(find("DIV"));}
if(qaFrame==14){metroqa::screenshot(dir+"/mod-div-held.png");end(find("DIV"));}
if(qaFrame==15){check(!qaFailed,"TIMING UI QA COMPLETE");APP->window->close();}
