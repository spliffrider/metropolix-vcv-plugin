using M=Metropolix;
if(std::getenv("METROPOLIX_ORDER_REOPEN_QA")) {
    if(qaFrame==6) {
        check(m->integer(M::orderShortcutParam(7))==13 && m->integer(M::orderShortcutParam(7,true))==1,"Rack file retains custom global order/direction shortcut");
        check(!m->orderShortcutHeld.load() && !m->orderShortcutUsed.load(),"Rack reopen clears held ORDER gestures");
        check(m->uiPage.load()==M::ORDER_PAGE && m->integer(M::trackParam(0,M::ORDER))==13 && m->integer(M::extraParam(0,M::REVERSED))==1,"Rack file retains selected order screen and pair");
        check(!qaFailed,"ORDER REOPEN QA COMPLETE");APP->window->close();
    }return;
}
auto beginOrderControl=[&](HardwareControl* c){widget::EventContext ctx;event::DragStart e;e.context=&ctx;e.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragStart(e);};
auto endOrderControl=[&](HardwareControl* c){widget::EventContext ctx;event::DragEnd e;e.context=&ctx;e.button=GLFW_MOUSE_BUTTON_LEFT;c->onDragEnd(e);};
auto longOrderControl=[&](HardwareControl* c){beginOrderControl(c);c->heldSince=system::getTime()-1.1;c->step();endOrderControl(c);};
if(qaFrame==6) {
    m->params[M::RUN].setValue(0);m->expanded.store(false);m->uiMod.store(false);m->selectedTrack.store(0);m->params[M::ENC_CONFIRM].setValue(0);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    auto* order=find("ORDER");auto* stage=find("Stage 4 / GATE");auto ids=M::orderParams(false,0);
    m->params[ids[0]].setValue(19);m->params[ids[1]].setValue(1);
    int previousOrder=m->integer(M::orderShortcutParam(3)),previousDirection=m->integer(M::orderShortcutParam(3,true));int h=APP->history->actionIndex;
    beginOrderControl(order);check(m->orderShortcutHeld.load() && m->uiPage.load()==M::ORDER_PAGE,"holding ORDER opens its page and enables shortcuts");
    bool white=true;for(int slot=0;slot<8;++slot){auto* button=find("Stage "+std::to_string(slot+1)+" / "+std::vector<std::string>{"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"}[slot]);button->bind();auto color=button->activeColor();white &= button->active() && color.r>.9f && color.g>.9f && color.b>.9f && !button->getParamQuantity();}
    check(white,"held ORDER lights all eight stage buttons white and disables ordinary numeric edits");
    longOrderControl(stage);check(m->integer(M::orderShortcutParam(3))==19 && m->integer(M::orderShortcutParam(3,true))==1,"long held-stage press saves both fields");
    check(APP->history->actionIndex==h && m->workflowMessage=="SHORTCUT 4 SAVED","save notice appears without a duplicate child history action");
    endOrderControl(order);check(APP->history->actionIndex==h+1 && !m->orderShortcutHeld.load(),"ORDER release finishes one grouped save undo");
    APP->history->undo();check(m->integer(M::orderShortcutParam(3))==previousOrder && m->integer(M::orderShortcutParam(3,true))==previousDirection,"shortcut save undo restores the previous pair");APP->history->redo();
    m->params[ids[0]].setValue(2);m->params[ids[1]].setValue(0);m->selectedTrack.store(1);auto second=M::orderParams(false,1);m->params[second[0]].setValue(5);m->params[second[1]].setValue(0);h=APP->history->actionIndex;
    beginOrderControl(order);check(std::find(order->gestureIds.begin(),order->gestureIds.end(),M::MOD_GROUP_MUTE)==order->gestureIds.end(),"ORDER history excludes independently changing MOD group mute");m->params[M::MOD_GROUP_MUTE].setValue(1);click(stage);endOrderControl(order);
    check(m->integer(second[0])==19 && m->integer(second[1])==1 && m->integer(ids[0])==2 && m->integer(ids[1])==0,"short held-stage press recalls only the selected note track");
    check(APP->history->actionIndex==h+1,"pair recall records one undo");APP->history->undo();check(m->integer(second[0])==5 && m->integer(second[1])==0 && m->integer(M::MOD_GROUP_MUTE)==1,"pair recall undo restores both fields and preserves independent group mute");APP->history->redo();m->params[M::MOD_GROUP_MUTE].setValue(0);
    m->uiMod.store(true);m->selectedLane.store(7);auto lane=M::orderParams(true,7);m->params[lane[0]].setValue(4);m->params[lane[1]].setValue(0);h=APP->history->actionIndex;
    beginOrderControl(order);click(stage);endOrderControl(order);check(m->integer(lane[0])==19 && m->integer(lane[1])==1 && APP->history->actionIndex==h+1,"same global shortcut recalls into selected MOD lane with one undo");APP->history->undo();
    h=APP->history->actionIndex;longOrderControl(find("Encoder"));check(m->integer(lane[0])==0 && m->integer(lane[1])==0 && APP->history->actionIndex==h+1,"MOD long encoder resets its own pair as one change");APP->history->undo();check(m->integer(lane[0])==4 && m->integer(lane[1])==0,"MOD paired reset has exact undo");
    m->uiMod.store(false);m->selectedTrack.store(0);m->params[second[0]].setValue(5);m->params[second[1]].setValue(0);h=APP->history->actionIndex;
    beginOrderControl(order);click(stage);m->selectedTrack.store(1);click(stage);endOrderControl(order);
    check(m->integer(ids[0])==19 && m->integer(ids[1])==1 && m->integer(second[0])==19 && APP->history->actionIndex==h+1,"changing track while ORDER is held keeps both recalls in one gesture");APP->history->undo();check(m->integer(ids[0])==2 && m->integer(ids[1])==0 && m->integer(second[0])==5 && m->integer(second[1])==0,"multi-target gesture undo retains each target's original pair");
    m->uiMod.store(false);m->selectedTrack.store(0);m->params[ids[0]].setValue(12);m->params[ids[1]].setValue(1);h=APP->history->actionIndex;longOrderControl(find("Encoder"));
    check(m->integer(ids[0])==0 && m->integer(ids[1])==0 && APP->history->actionIndex==h+1,"long encoder resets order and direction as one change");APP->history->undo();check(m->integer(ids[0])==12 && m->integer(ids[1])==1,"long reset undo restores both fields");APP->history->redo();
    m->params[ids[0]].setValue(3);drag(order,-40);check(m->integer(ids[0])>3 && !m->orderShortcutHeld.load(),"ordinary upward ORDER drag still edits and releases its held state");APP->history->undo();check(m->integer(ids[0])==3,"ordinary ORDER drag still has exact undo");
    m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlOrder);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);m->params[ids[1]].setValue(0);h=APP->history->actionIndex;
    beginOrderControl(order);check(std::find(order->gestureIds.begin(),order->gestureIds.end(),ids[0])==order->gestureIds.end() && std::find(order->gestureIds.begin(),order->gestureIds.end(),ids[1])==order->gestureIds.end(),"controller-owned target is excluded from the held history receipt");
    m->params[ids[0]].setValue(7);m->readSettings();int controllerOrder=m->integer(ids[0]);click(stage);endOrderControl(order);check(m->integer(ids[0])==controllerOrder && m->integer(ids[1])==0 && APP->history->actionIndex==h,"CTRL-owned order blocks the pair without capturing independent controller edits in undo");
    press(stage,GLFW_MOUSE_BUTTON_RIGHT);auto* blocked=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text.find("Recall order shortcut 4:")==0;}));check(blocked && blocked->disabled,"desktop shortcut menu identifies blocked CTRL recall");closeMenus();m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
}
if(qaFrame==8) {
    auto ids=M::orderParams(false,0);m->params[ids[0]].setValue(13);m->params[ids[1]].setValue(1);auto* stage=find("Stage 8 / CV");press(stage,GLFW_MOUSE_BUTTON_RIGHT);auto* save=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text=="Save order/direction to shortcut 8";}));int h=APP->history->actionIndex;
    check(save!=nullptr,"stage context menu offers direct shortcut save");if(save){event::Action action;save->onAction(action);}closeMenus();check(m->integer(M::orderShortcutParam(7))==13 && m->integer(M::orderShortcutParam(7,true))==1 && APP->history->actionIndex==h+1,"desktop save records one pair undo");
    m->params[ids[0]].setValue(6);m->params[ids[1]].setValue(0);press(stage,GLFW_MOUSE_BUTTON_RIGHT);auto* recall=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text.find("Recall order shortcut 8:")==0;}));h=APP->history->actionIndex;if(recall){event::Action action;recall->onAction(action);}closeMenus();check(recall && m->integer(ids[0])==13 && m->integer(ids[1])==1 && APP->history->actionIndex==h+1,"desktop recall applies its pair in one undo");APP->history->undo();check(m->integer(ids[0])==6 && m->integer(ids[1])==0,"desktop recall undo restores both fields");APP->history->redo();
    press(find("ORDER"),GLFW_MOUSE_BUTTON_RIGHT);check(seek(APP->scene,[](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text=="Order shortcuts (global)";})!=nullptr,"ORDER menu exposes all global shortcut pairs");closeMenus();
    beginOrderControl(find("ORDER"));APP->patch->save(dir+"/order-roundtrip.vcv");
}
if(qaFrame==10){metroqa::screenshot(dir+"/order-held-white.png");endOrderControl(find("ORDER"));}
if(qaFrame==12){metroqa::screenshot(dir+"/order-saved-pair.png");}
if(qaFrame==13){check(!qaFailed,"ORDER UI QA COMPLETE");APP->window->close();}
