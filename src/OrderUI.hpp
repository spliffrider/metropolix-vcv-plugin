std::vector<int> orderIds(Metropolix* m) {
    if(!m)return {};
    auto ids=Metropolix::orderParams(m->uiMod.load(),m->uiMod.load()?m->selectedLane.load():m->selectedTrack.load());
    return {ids[0],ids[1]};
}
bool orderControlled(Metropolix* m) {
    if(!m)return false;
    for(int id:orderIds(m))if(m->ctrlOwner(id)>=0)return true;
    return false;
}
std::string orderShortcutLabel(Metropolix* m,int slot) {
    return std::to_string(slot+1)+": "+Metropolix::orderLabels()[m->integer(Metropolix::orderShortcutParam(slot))]+" / "+(m->integer(Metropolix::orderShortcutParam(slot,true))?"Reverse":"Forward");
}
void orderShortcut(Metropolix* m,int slot,bool save) {
    if(!m)return;
    m->orderShortcutUsed.store(true);m->cancelDraft();
    auto ids=save?std::vector<int>{Metropolix::orderShortcutParam(slot),Metropolix::orderShortcutParam(slot,true)}:orderIds(m);
    std::vector<float> before;for(int id:ids)before.push_back(m->params[id].getValue());
    if(!m->orderShortcut(slot,save,m->uiMod.load(),m->uiMod.load()?m->selectedLane.load():m->selectedTrack.load())) {
        workflowNotice(m,"ORDER OWNED BY CTRL");return;
    }
    m->uiPage.store(Metropolix::ORDER_PAGE);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->uiField.store(0);
    if(!m->orderShortcutHeld.load())parameterHistory(m,ids,before,save?"save order shortcut":"recall order shortcut");
    if(save)workflowNotice(m,"SHORTCUT "+std::to_string(slot+1)+" SAVED");
}
void resetOrder(Metropolix* m,bool record=true) {
    if(!m)return;
    m->cancelDraft();
    if(orderControlled(m)){workflowNotice(m,"ORDER OWNED BY CTRL");return;}
    auto ids=orderIds(m);std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->params[id].setValue(m->paramQuantities[id]->defaultValue);}
    if(record)parameterHistory(m,ids,before,"reset play order and direction");
}
void appendOrderMenu(ui::Menu* menu,Metropolix* m) {
    if(!m)return;
    auto ids=orderIds(m);
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuLabel(m->uiMod.load()?"MOD "+std::to_string(m->selectedLane.load()+1)+" order":"TRK "+std::to_string(m->selectedTrack.load()+1)+" order"));
    menu->addChild(createIndexSubmenuItem("Direction",{"Forward","Reverse"},[m,ids]{return size_t(m->integer(ids[1]));},[m,ids](size_t value){setWithHistory(m->getParamQuantity(ids[1]),float(value));}));
    menu->addChild(createMenuItem("Reset play order and direction","",[m]{resetOrder(m);},orderControlled(m)));
    menu->addChild(createSubmenuItem("Order shortcuts (global)","",[m](ui::Menu* child){
        for(int slot=0;slot<8;++slot) {
            child->addChild(createMenuItem("Recall "+orderShortcutLabel(m,slot),"",[m,slot]{orderShortcut(m,slot,false);},orderControlled(m)));
            child->addChild(createMenuItem("Save current order/direction to "+std::to_string(slot+1),"",[m,slot]{orderShortcut(m,slot,true);}));
        }
    }));
}
