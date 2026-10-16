using M=Metropolix;
static_assert(M::FORMAT34_NUM_PARAMS==787 && M::NUM_PARAMS==789 && M::SNAPSHOT_PARAMS==522 && M::PRESET_FORMAT==12,"CTRL editor adds no saved fields");
if(std::getenv("METROPOLIX_CTRLEDITOR_REOPEN_QA")) {
    if(qaFrame==6) {
        check(m->selectedCtrl.load()==1 && m->uiPage.load()==M::CTRL_PAGE && m->uiField.load()==1,"Rack reopen retains selected CTRL and assignment field");
        check(m->integer(M::ctrlParam(0,M::CTRL_DESTINATION))==metro::CtrlGateLength && m->integer(M::ctrlParam(0,M::CTRL_ASSIGNMENT))==0 && m->integer(M::ctrlParam(1,M::CTRL_DESTINATION))==metro::CtrlSlideAmount && m->integer(M::ctrlParam(1,M::CTRL_ASSIGNMENT))==1,"both confirmed routes survive reopen");
        check(m->uiDraftParam<0 && m->integer(M::MOD_GROUP_MUTE)==1,"pending CTRL route is transient and unrelated mute survives");
        auto p=m->getPreset(61);check(p.occupied && p.values[M::presetIndex(M::ctrlParam(1,M::CTRL_DESTINATION))]==metro::CtrlSlideAmount,"confirmed CTRL assignment stored in unchanged musical preset");
        check(!qaFailed,"CTRLEDITOR REOPEN QA COMPLETE");APP->window->close();
    }return;
}
if(qaFrame==3){
#include "LiveCtrlWidgetQa.hpp"
}
auto raw=[&]{std::vector<float> a;for(auto& p:m->params)a.push_back(p.getValue());return a;};
auto lastAction=[&]()->history::Action*{return APP->history->actions.empty()?nullptr:APP->history->actions.back();};
auto exactUndo=[&](history::Action* previous,const std::vector<float>& before,const std::vector<float>& after){if(before==after)return lastAction()==previous;auto* action=lastAction();bool ok=action && action!=previous && (dynamic_cast<history::ComplexAction*>(action) || dynamic_cast<history::ParamChange*>(action));APP->history->undo();ok &= raw()==before;APP->history->redo();return ok && raw()==after;};
auto only=[&](const std::vector<float>& before,int allowed){for(int id=0;id<M::NUM_PARAMS;++id)if(id!=allowed && before[id]!=m->params[id].getValue())return false;return true;};
auto open=[&](int knob,int field){m->selectedCtrl.store(knob);m->uiMod.store(false);m->uiAlt.store(false);screenPage(m,M::CTRL_PAGE);m->uiField.store(field);};
widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
auto hold=[&](HardwareControl* c,double seconds){press(c,GLFW_MOUSE_BUTTON_LEFT);c->onDragStart(start);c->heldSince=system::getTime()-seconds;c->step();bool fired=c->longFired;c->onDragEnd(end);return fired;};
auto menuItem=[&](const std::string& label)->ui::MenuItem*{for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if((*it)->visible && dynamic_cast<ui::MenuOverlay*>(*it))return dynamic_cast<ui::MenuItem*>(seek(*it,[&](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text==label;}));return nullptr;};
if(qaFrame==6) {
    // Isolate raw undo receipts from asynchronous direct CTRL writes. Routing
    // signal behavior remains covered by the separate native integration suite.
    APP->engine->bypassModule(m,true);
    m->params[M::RUN].setValue(0);m->params[M::RUN_ON_BOOT].setValue(2);m->params[M::MOD_GROUP_MUTE].setValue(1);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    auto* encoder=find("Encoder");auto* display=find("Metropolix display");bool preview=true,confirmation=true,history=true,cancel=true,unrelated=true;
    for(int setting:{0,1})for(int knob=0;knob<2;++knob)for(int field=0;field<2;++field) {
        int id=M::ctrlParam(knob,field?M::CTRL_ASSIGNMENT:M::CTRL_DESTINATION),count=field?3:metro::CtrlDestinationCount;
        m->params[M::ENC_CONFIRM].setValue(setting);
        for(int value=0;value<count;++value) {
            int origin=(value+1)%count;m->params[id].setValue(origin);open(knob,field);auto before=raw();auto* previous=lastAction();
            auto* c=value%2?display:encoder;drag(c,-22.f*(value-origin));
            preview &= pendingConfirmation(m) && m->uiDraftValue==value && raw()==before && lastAction()==previous;
            confirmation &= c->adjustmentText().find("Press encoder to apply")!=std::string::npos;
            click(encoder);confirmation &= m->integer(id)==value && m->uiDraftParam<0 && m->uiField.load()==field;
            history &= exactUndo(previous,before,raw());unrelated &= only(before,id);
            click(encoder);confirmation &= m->uiField.load()==1-field;
            open(knob,field);before=raw();previous=lastAction();int alternate=(value+1)%count;drag(display,-22.f*(alternate-value));click(find("EXIT"));cancel &= raw()==before && m->uiDraftParam<0 && lastAction()==previous;
        }
    }
    check(preview && confirmation,"both CTRLs preview/confirm every destination and target with ENC_CONFIRM off/on, using encoder/display callbacks");
    check(history && unrelated,"confirmed routing edits have exact one-action undo/redo without unrelated parameter changes");
    check(cancel,"EXIT discards every unconfirmed CTRL destination/target without history");
    bool button=true,clear=true,threshold=true,noops=true;
    for(int knob=0;knob<2;++knob) {
        int dest=M::ctrlParam(knob,M::CTRL_DESTINATION),target=M::ctrlParam(knob,M::CTRL_ASSIGNMENT),position=M::ctrlParam(knob,M::CTRL_VALUE);
        m->params[M::ENC_CONFIRM].setValue(0);m->params[dest].setValue(metro::CtrlNone);m->params[position].setValue(.375f);
        click(find(string::f("CTRL %d assignment",knob+1)));auto before=raw();auto* previous=lastAction();drag(find(string::f("CTRL %d assignment",knob+1)),-22.f*metro::CtrlGateLength);
        button &= m->selectedCtrl.load()==knob && m->uiField.load()==0 && m->uiDraftValue==metro::CtrlGateLength && raw()==before && lastAction()==previous;
        click(encoder);button &= m->integer(dest)==metro::CtrlGateLength && exactUndo(previous,before,raw());
        for(int field=0;field<2;++field)for(int assignment=0;assignment<3;++assignment) {
            m->params[dest].setValue(metro::CtrlSlideAmount);m->params[target].setValue(assignment);open(knob,field);before=raw();previous=lastAction();
            threshold &= !hold(encoder,.9) && raw()==before && lastAction()==previous;
            open(knob,field);clear &= hold(encoder,1.1) && m->integer(dest)==metro::CtrlNone && m->integer(target)==assignment && m->params[position].getValue()==.375f && m->uiDraftParam<0 && m->uiField.load()==field;
            clear &= exactUndo(previous,before,raw()) && only(before,dest);before=raw();previous=lastAction();noops &= hold(encoder,1.1) && raw()==before && lastAction()==previous;
            m->params[dest].setValue(metro::CtrlGateLength);open(knob,field);before=raw();previous=lastAction();int id=field?target:dest;int alternate=field?(assignment+1)%3:metro::CtrlSlideAmount;drag(encoder,-22.f*(alternate-m->integer(id)));
            clear &= pendingConfirmation(m) && hold(encoder,1.1) && m->uiDraftParam<0 && m->integer(dest)==metro::CtrlNone && exactUndo(previous,before,raw()) && only(before,dest);
        }
    }
    check(button,"black CTRL assignment buttons preview destination and require encoder confirmation");
    check(clear && threshold && noops,"CTRL long clear works from either field at threshold, cancels draft, preserves target/knob and has exact/no-op undo");
    // The currently confirmed route owns the base field until the pending route is committed.
    int dest=M::ctrlParam(0,M::CTRL_DESTINATION),position=M::ctrlParam(0,M::CTRL_VALUE);
    m->params[M::ctrlParam(1,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[dest].setValue(metro::CtrlGateLength);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);open(0,0);
    int gate=M::trackParam(0,M::GATE_LENGTH),slide=M::trackParam(0,M::SLIDE_TIME);drag(encoder,-22.f*(metro::CtrlSlideAmount-metro::CtrlGateLength));
    check(m->ctrlOwner(gate)==0 && m->ctrlOwner(slide)<0 && m->integer(dest)==metro::CtrlGateLength,"pending route leaves confirmed parameter ownership active");click(encoder);
    check(m->ctrlOwner(gate)<0 && m->ctrlOwner(slide)==0,"confirming CTRL destination transfers ownership");
    open(0,0);hold(encoder,1.1);check(m->ctrlOwner(slide)<0,"long clear releases direct parameter ownership");
    m->params[position].setValue(0);open(0,0);auto before=raw();auto* previous=lastAction();drag(find("CTRL 1"),-100);check(m->params[position].getValue()>0 && m->uiDraftParam<0 && exactUndo(previous,before,raw()),"physical CTRL performance knob stays live with native undo");
    // Explicit desktop choices apply directly and cancel obsolete pending route drafts.
    m->params[dest].setValue(metro::CtrlGateLength);open(0,0);drag(encoder,-22.f);before=raw();previous=lastAction();press(display,GLFW_MOUSE_BUTTON_RIGHT);auto* item=menuItem("Clear assignment");check(item && !item->disabled,"display offers CTRL clear assignment");if(item){event::Action action;item->onAction(action);}closeMenus();check(m->integer(dest)==metro::CtrlNone && m->uiDraftParam<0 && exactUndo(previous,before,raw()),"desktop clear discards stale preview and records exact undo");
    bool menus=true;
    for(int knob=0;knob<2;++knob)for(int field=0;field<2;++field) {
        int id=M::ctrlParam(knob,field?M::CTRL_ASSIGNMENT:M::CTRL_DESTINATION),choice=field?2:metro::CtrlSwing;
        m->params[id].setValue(0);open(knob,field);drag(encoder,-22);before=raw();previous=lastAction();press(encoder,GLFW_MOUSE_BUTTON_RIGHT);
        auto* parent=menuItem(field?"Target tracks":"Destination");auto* child=parent?parent->createChildMenu():nullptr;ui::MenuItem* selected=nullptr;
        std::string label=field?"Both tracks":M::ctrlDestinations()[choice];if(child)for(auto* w:child->children)if(auto* candidate=dynamic_cast<ui::MenuItem*>(w))if(candidate->text==label)selected=candidate;
        menus &= selected && !selected->disabled;if(selected){event::Action action;selected->onAction(action);}delete child;closeMenus();
        menus &= m->integer(id)==choice && m->uiDraftParam<0 && exactUndo(previous,before,raw()) && only(before,id);
    }
    check(menus,"named desktop destination/target choices apply directly and discard stale pending choices on both CTRLs");
    for(int knob=0;knob<2;++knob){m->params[M::ctrlParam(knob,M::CTRL_DESTINATION)].setValue(knob?metro::CtrlSlideAmount:metro::CtrlGateLength);m->params[M::ctrlParam(knob,M::CTRL_ASSIGNMENT)].setValue(knob);m->params[M::ctrlParam(knob,M::CTRL_VALUE)].setValue(0);}
    open(1,0);drag(encoder,-22);check(pendingConfirmation(m),"prepare transient destination preview for rendering");
    APP->engine->bypassModule(m,false);
}
if(qaFrame==8)metroqa::screenshot(dir+"/ctrl-destination-pending.png");
if(qaFrame==10){open(1,1);m->params[M::ctrlParam(1,M::CTRL_ASSIGNMENT)].setValue(1);drag(find("Metropolix display"),-22);}
if(qaFrame==12)metroqa::screenshot(dir+"/ctrl-target-pending.png");
if(qaFrame==14){open(1,1);hold(find("Encoder"),1.1);}
if(qaFrame==16)metroqa::screenshot(dir+"/ctrl-cleared.png");
if(qaFrame==18){m->params[M::ctrlParam(1,M::CTRL_DESTINATION)].setValue(metro::CtrlSlideAmount);m->params[M::ctrlParam(1,M::CTRL_ASSIGNMENT)].setValue(1);open(1,1);m->putPreset(61,m->capturePreset());drag(find("Encoder"),-22);APP->engine->bypassModule(m,false);APP->patch->save(dir+"/ctrleditor-roundtrip.vcv");check(!qaFailed,"CTRLEDITOR UI QA COMPLETE");APP->window->close();}
