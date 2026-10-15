if(std::getenv("METROPOLIX_SCALE_REOPEN_QA")) {
    if(qaFrame==6) {
        check(m->integer(Metropolix::SCALE)==49 && m->integer(Metropolix::SCALE_LOCK)==49 && m->integer(Metropolix::userScaleParam(0))==0x91 && m->integer(Metropolix::userScaleParam(0,true))==9,"native Rack file preserves custom scale/lock/notes/root");
        check(m->integer(Metropolix::scaleShortcutParam(3))==49 && m->integer(Metropolix::scaleShortcutParam(3,true))==9 && !m->scaleEditing.load() && !m->scaleShortcutHeld.load(),"native Rack file preserves shortcuts and clears transient scale gestures");
        check(true,"SCALE REOPEN QA COMPLETE");APP->window->close();
    }
    return;
}
#include "ScaleDisplayWidgetQa.hpp"
auto begin=[&](HardwareControl* c){if(!c){check(false,"scale hold control exists");return;}widget::EventContext context;event::DragStart e;e.context=&context;e.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragStart(e);};
auto end=[&](HardwareControl* c){widget::EventContext context;event::DragEnd e;e.context=&context;e.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragEnd(e);};
if(qaFrame==6) {
    using M=Metropolix;for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->params[M::RUN].setValue(0);m->params[M::SCALE].setValue(1);m->params[M::SCALE_LOCK].setValue(-1);
    // Printed p67: note-editor entry is available only with SCALE selected.
    auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
    auto hold=[&](double seconds){auto* c=find("Encoder");begin(c);c->heldSince=system::getTime()-seconds;c->step();bool fired=c->longFired;end(c);return fired;};
    bool roots=true,entry=true,exit=true,threshold=true,factory=true;
    for(int id=0;id<149;++id) {
        m->params[M::SCALE].setValue(id);screenPage(m,M::SCALE_PAGE);m->uiField.store(0);m->scaleEditing.store(false);auto before=raw();int history=APP->history->actionIndex;
        roots &= !hold(1.1) && !m->scaleEditing.load() && raw()==before && APP->history->actionIndex==history;
        screenPage(m,M::SCALE_PAGE);m->uiField.store(1);before=raw();bool user=id>=49;
        if(user) {
            threshold &= !hold(.9) && !m->scaleEditing.load();if(raw()!=before)APP->history->undo();
            history=APP->history->actionIndex;entry &= hold(1.1) && m->scaleEditing.load() && screenParam(m)==M::SCALE_NOTE && raw()==before && APP->history->actionIndex==history;
            for(int field=0;field<2;++field) {
                m->scaleEditing.store(true);m->uiField.store(field);before=raw();history=APP->history->actionIndex;
                exit &= hold(1.1) && !m->scaleEditing.load() && raw()==before && APP->history->actionIndex==history;
            }
        } else {
            factory &= !hold(1.1) && !m->scaleEditing.load();if(raw()!=before)APP->history->undo();
        }
    }
    check(roots,"Root-selected long encoder cannot enter notes for any factory/user scale and preserves values/history");
    check(entry && threshold && exit,"all 100 user scales enter only from SCALE after threshold and exit from notes/root without value/history changes");
    check(factory,"all 49 factory choices keep the note editor unavailable");
    m->params[M::SCALE].setValue(49);screenPage(m,M::SCALE_PAGE);m->uiField.store(1);hold(1.1);m->params[M::SCALE].setValue(1);auto beforeFactory=raw();int factoryHistory=APP->history->actionIndex;
    find("Encoder")->step();check(!m->scaleEditing.load() && raw()==beforeFactory && APP->history->actionIndex==factoryHistory,"factory selection automatically closes existing note editor without changing values/history");
    m->params[M::SCALE].setValue(49);screenPage(m,M::SCALE_PAGE);m->uiField.store(0);m->params[M::ENC_CONFIRM].setValue(1);m->params[M::userScaleParam(0,true)].setValue(0);drag(find("Encoder"),-22);auto beforeMenu=raw();int menuHistory=APP->history->actionIndex;
    check(pendingConfirmation(m),"prepare pending user root before explicit note-editor menu entry");
    press(find("Encoder"),GLFW_MOUSE_BUTTON_RIGHT);ui::MenuItem* edit=nullptr;
    for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if((*it)->visible && dynamic_cast<ui::MenuOverlay*>(*it)){edit=dynamic_cast<ui::MenuItem*>(seek(*it,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text=="Edit user scale notes";}));break;}
    check(edit && !edit->disabled,"desktop note-editor choice is available from ROOT");if(edit){event::Action action;edit->onAction(action);}closeMenus();
    check(m->scaleEditing.load() && m->uiDraftParam<0 && raw()==beforeMenu && APP->history->actionIndex==menuHistory,"explicit desktop entry from ROOT discards obsolete root preview without a parameter change");
    hold(1.1);m->params[M::ENC_CONFIRM].setValue(0);m->params[M::SCALE].setValue(1);m->params[M::SCALE_LOCK].setValue(-1);screenPage(m,M::SCALE_PAGE);m->uiField.store(1);
    click(find("SCALE"));check(m->uiPage.load()==M::SCALE_PAGE && screenParam(m)==M::SCALE,"SCALE selects factory scale field");
    drag(find("Encoder"),-23);check(m->integer(M::SCALE)==3,"encoder browses hardware bank order: Major to Dorian");APP->history->undo();check(m->integer(M::SCALE)==1,"factory scale drag uses one Rack undo");
    click(find("Encoder"));check(m->integer(M::SCALE_LOCK)==1,"encoder click locks current scale");drag(find("Encoder"),-23);check(m->integer(M::SCALE)==3 && m->integer(M::SCALE_LOCK)==1,"browsing locked scales preserves sounding scale");
    click(find("Encoder"));check(m->integer(M::SCALE_LOCK)==3,"encoder applies searched scale while retaining lock");click(find("Encoder"));check(m->integer(M::SCALE_LOCK)==-1,"second encoder click unlocks displayed scale");
    m->params[M::SCALE].setValue(49);auto* encoder=find("Encoder");begin(encoder);encoder->heldSince=system::getTime()-1.1;encoder->step();end(encoder);
    check(m->scaleEditing.load() && screenParam(m)==M::SCALE_NOTE,"long encoder press enters user note editor");
    unsigned before=m->integer(M::userScaleParam(0));m->params[M::SCALE_NOTE].setValue(1);click(encoder);check(m->integer(M::userScaleParam(0))==int(before^2u),"encoder toggles selected custom note");APP->history->undo();check(m->integer(M::userScaleParam(0))==int(before),"custom note toggle has Rack undo");
    m->params[M::userScaleParam(0)].setValue(1);m->params[M::SCALE_NOTE].setValue(0);click(encoder);check(m->integer(M::userScaleParam(0))==1,"editor retains final enabled note");
    m->params[M::userScaleParam(0)].setValue(0x91);click(find("ALT"));check(screenParam(m)==M::userScaleParam(0,true),"ALT edits this user scale's stored root");drag(encoder,-60);check(m->integer(M::userScaleParam(0,true))>0,"root drag transposes user scale");APP->history->undo();check(m->integer(M::userScaleParam(0,true))==0,"user root drag undoes cleanly");
    m->params[M::userScaleParam(0,true)].setValue(9);click(find("ALT"));begin(encoder);encoder->heldSince=system::getTime()-1.1;encoder->step();end(encoder);check(!m->scaleEditing.load(),"long encoder press saves/exits user editor");
}
if(qaFrame==8) {
    using M=Metropolix;auto* scale=find("SCALE");auto* stage=find("Stage 4 / GATE");
    begin(scale);begin(stage);stage->heldSince=system::getTime()-1.1;stage->step();end(stage);end(scale);
    check(m->integer(M::scaleShortcutParam(3))==49 && m->integer(M::scaleShortcutParam(3,true))==9,"held SCALE + long stage press saves scale/root shortcut");
    m->params[M::SCALE].setValue(1);m->params[M::ROOT].setValue(2);begin(scale);click(stage);end(scale);
    check(m->integer(M::SCALE)==49 && m->integer(M::userScaleParam(0,true))==9,"held SCALE + stage click recalls shortcut");APP->history->undo();check(m->integer(M::SCALE)==1 && m->integer(M::ROOT)==2,"shortcut recall groups all changes into one undo");
    APP->history->redo();check(m->integer(M::SCALE)==49,"shortcut recall redo restores custom selection");
    click(find("Encoder"));check(m->integer(M::SCALE_LOCK)==49,"custom scale locks for modulation");
    press(scale,GLFW_MOUSE_BUTTON_RIGHT);check(seek(APP->scene,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="User (Bank 10)";})!=nullptr,"scale menu exposes all ten custom banks");closeMenus();
    m->params[M::userScaleParam(0)].setValue(1u<<11);m->uiPage.store(M::STAGE_PAGE);m->uiMod.store(false);m->selectedRow.store(M::PITCH_OVERRIDE);m->selectedStage.store(0);m->uiAlt.store(true);
    for(int s=0;s<8;++s)m->params[M::stageParam(0,s,M::PITCH_OVERRIDE)].setValue(0);
    drag(find("Encoder"),-220);bool shifted=true;for(int s=0;s<8;++s)shifted &= m->integer(M::stageParam(0,s,M::PITCH_OVERRIDE))==11;
    check(shifted,"ALT pitch drag treats negative custom-scale degree separately from disabled override");APP->history->undo();bool restored=true;for(int s=0;s<8;++s)restored &= m->integer(M::stageParam(0,s,M::PITCH_OVERRIDE))==0;check(restored,"custom-scale ALT pitch gesture undoes all eight anchors");
    m->params[M::userScaleParam(0)].setValue(0x91);m->uiPage.store(M::SCALE_PAGE);m->uiField.store(1);m->uiAlt.store(false);
    metroqa::screenshot(dir+"/scale-user-locked.png");
    m->scaleEditing.store(true);m->params[M::SCALE_NOTE].setValue(4);
}
if(qaFrame==10) {metroqa::screenshot(dir+"/scale-note-editor.png");APP->patch->save(dir+"/scale-roundtrip.vcv");}
if(qaFrame==14) {check(!qaFailed,"SCALE UI QA COMPLETE");APP->window->close();}
