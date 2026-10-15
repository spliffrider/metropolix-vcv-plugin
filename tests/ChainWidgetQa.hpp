auto lastChainAction=[]()->history::Action*{return APP->history->actionIndex?APP->history->actions[APP->history->actionIndex-1]:nullptr;};
auto oneNewChainAction=[&](history::Action* prior){int index=APP->history->actionIndex;return lastChainAction()!=prior && (prior?(index>=2 && APP->history->actions[index-2]==prior):index==1);};
auto hold=[&](HardwareControl* c,double seconds) {
    press(c,GLFW_MOUSE_BUTTON_LEFT);
    widget::EventContext ctx;event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragStart(start);
    c->heldSince=system::getTime()-seconds;c->step();
    event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragEnd(end);
};
auto imageChain=[&](const char* name) {
    auto* d=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));
    auto c=d->render();std::ofstream pgm(dir+"/"+name+".pgm",std::ios::binary);pgm<<"P5\n128 64\n255\n";pgm.write(reinterpret_cast<const char*>(c.pixels),sizeof(c.pixels));
    metroqa::screenshot(dir+"/"+name+".png");
};
if(std::getenv("METROPOLIX_CHAIN_REOPEN_QA")) {
    if(qaFrame==10) {
        auto s=m->getChainState();
        check(s.working.size==2 && s.saved[0].size==2 && s.saved[7].size==2 && s.loaded==0,"working and two saved chains survive actual Rack file reopening");
        check(s.working.links[0].repeats==2 && s.working.links[1].mutes==2,"repeat counts and mute flags persist");
        check(s.enabled && m->chainPlayingLink.load()>=0,"reopened enabled chain runs automatically");
        check(m->getPreset(0).occupied,"referenced presets persist alongside chains");
        check(!qaFailed,"CHAIN REOPEN QA COMPLETE");APP->window->close();
    }
    return;
}
#include "ChainHistoryWidgetQa.hpp"
#include "ChainDisplayWidgetQa.hpp"
if(qaFrame==5) {
    m->params[Metropolix::RUN].setValue(0);
    for(int k=0;k<2;++k)m->params[Metropolix::ctrlParam(k,Metropolix::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    json_t* json=APP->engine->moduleToJson(m);auto preset=m->presetFromParamsJson(json_object_get(json,"params"));json_decref(json);
    preset.values[Metropolix::TOTAL_PULSES]=2;m->putPreset(0,preset);preset.values[Metropolix::PITCH]=.6f;m->putPreset(9,preset);
    click(find("SETUP"));m->setupCursor.store(0);drag(find("Encoder"),-22*Metropolix::SetupChains);click(find("Encoder"));
    check(chainPage(m),"Setup encoder navigation opens Preset Chains");imageChain("empty");
    auto* editHistory=lastChainAction();click(find("Encoder"));
    check(oneNewChainAction(editHistory),"new chain insertion records exactly one history action");
    click(find("Stage 1 / SLIDE"));click(find("Encoder"));
    check(m->chainEdit.load()==2 && m->getChainState().working.size==1,"New Chain creates first link and selects repeat field");
    drag(find("Encoder"),-44);check(m->getChainState().working.links[0].repeats==3,"held upward drag increases repeats");
    APP->history->undo();check(m->getChainState().working.links[0].repeats==1,"one undo restores complete repeat drag");
    APP->history->redo();check(m->getChainState().working.links[0].repeats==3,"repeat drag redo works");
    click(find("TRK 1"));click(find("MOD"));check(m->getChainState().working.links[0].mutes==5,"track and MOD buttons edit independent link mutes");imageChain("edit");
    click(find("EXIT"));check(chainPage(m) && !m->chainEdit.load(),"EXIT finishes link editing without leaving chain screen");
    hold(find("Stage 8 / CV"),2.1);check(m->getChainState().saved[7].size==1,"long stage press saves chain to slot 8");
    editHistory=lastChainAction();hold(find("Encoder"),1.1);check(!m->getChainState().working.size,"long encoder press deletes selected link");
    check(oneNewChainAction(editHistory),"long chain deletion records exactly one history action");
    APP->history->undo();check(m->getChainState().working.size==1,"link deletion supports undo");
    m->chainCursor.store(4);editHistory=lastChainAction();click(find("Encoder"));check(!m->getChainState().working.size && m->getChainState().saved[7].size==1,"Clear Chain preserves saved copy");
    check(oneNewChainAction(editHistory),"clear chain records exactly one history action");
    APP->history->undo();check(m->getChainState().working.size==1,"clear chain supports undo");
    click(find("Stage 8 / CV"));check(m->getChainState().loaded==7,"stage click recalls saved chain");
    // Long hold enters Quick Chain; use the actual press/release callbacks,
    // then stage clicks while the Setup gesture remains held.
    auto* setup=find("SETUP");press(setup,GLFW_MOUSE_BUTTON_LEFT);
    widget::EventContext ctx;event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;setup->onDragStart(start);
    setup->heldSince=system::getTime()-1.1;setup->step();
    check(m->chainQuick && !m->getChainState().working.size && m->getChainState().loaded==-1,"held Setup starts new Quick Chain and clears loaded indicator");
    click(find("Stage 1 / SLIDE"));click(find("Stage 1 / SLIDE"));click(find("TRK 2"));click(find("Stage 1 / SLIDE"));
    check(m->getChainState().working.size==2 && m->getChainState().working.links[0].repeats==2 && m->getChainState().working.links[1].mutes==2,"Quick Chain handles Red 1, repeated slot and changed mute state");
    event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;setup->onDragEnd(end);
    check(m->chainEnabled.load(),"releasing Setup enables Quick Chain");
    auto bankBefore=m->getPreset(0);openPresetPage(m,true);savePresetSlot(m,0,false,false);
    check(m->getPreset(0).values==bankBefore.values && m->presetNotice.load()==6,"chain mode blocks saving pattern bank");
    m->requestPreset(9);check(m->presetNotice.load()==6,"chain mode blocks manual preset loading");
    openChainPage(m);toggleChain(m);chainSave(m,0);chainSave(m,7,true);
    check(m->getChainState().saved[0].size==2 && m->getChainState().saved[7].size==2,"save and explicit overwrite preserve latest chain");
    chainStage(m,0);toggleChain(m);m->params[Metropolix::RUN].setValue(1);imageChain("playing");
}
if(qaFrame==15) {
    auto* encoder=find("Encoder");press(encoder,GLFW_MOUSE_BUTTON_RIGHT);
    auto* overlay=seek(APP->scene,[](widget::Widget* w){return w->visible && dynamic_cast<ui::MenuOverlay*>(w);});
    check(overlay!=nullptr,"encoder right-click opens chain menu");closeMenus();
    imageChain("following");
}
if(qaFrame==25)check(m->chainPlayingLink.load()>=0,"automatic chain reaches audio engine on next base-clock edge");
if(qaFrame==30) {
    bool following=m->chainFollow.load();click(find("Encoder"));check(m->chainFollow.load()!=following,"encoder click toggles Chain Follow during playback");click(find("Encoder"));
    bool followingAgain=m->chainFollow.load();int playing=m->chainPlayingLink.load();
    drag(find("Encoder"),-22);
    check(!m->chainFollow.load() && (!followingAgain || m->chainCursor.load()==2*playing+2),"turning while following navigates from the playing link");
    m->chainFollow.store(true);
    APP->patch->save(dir+"/chain-roundtrip.vcv");check(!qaFailed,"CHAIN UI QA COMPLETE");APP->window->close();
}
