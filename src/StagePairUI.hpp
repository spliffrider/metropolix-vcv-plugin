bool stagePairPage(Metropolix* m) {
    return m && !m->uiChooseFunction.load() && !m->uiMod.load() && m->uiPage.load()==Metropolix::STAGE_PAGE &&
        (m->selectedRow.load()==Metropolix::PROBABILITY || m->selectedRow.load()==Metropolix::ACCUMULATION);
}
int stagePairSecondary(Metropolix* m,int stage) {
    return Metropolix::stageOption(m->selectedTrack.load(),stage,
        m->selectedRow.load()==Metropolix::PROBABILITY?Metropolix::PROB_TARGET:Metropolix::ACCUM_TRIGGER);
}
std::vector<int> stagePairIds(Metropolix* m,bool all=false,bool amountsOnly=false) {
    std::vector<int> ids;
    if(!stagePairPage(m))return ids;
    for(int stage=0;stage<8;++stage)if(all || m->uiAlt.load() || stage==m->selectedStage.load()) {
        int amount=Metropolix::stageParam(m->selectedTrack.load(),stage,m->selectedRow.load());
        if(m->ctrlOwner(amount)<0)ids.push_back(amount);
        int secondary=stagePairSecondary(m,stage);
        if(!amountsOnly && m->ctrlOwner(secondary)<0)ids.push_back(secondary);
    }
    return ids;
}
void resetStagePairs(Metropolix* m,bool all=false,bool record=true) {
    if(!stagePairPage(m))return;
    m->cancelDraft();auto ids=stagePairIds(m,all);std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(m->getParamQuantity(id)->defaultValue);}
    if(record)parameterHistory(m,ids,before,"reset stage value and trigger");
}
void stagePairChoice(Metropolix* m,int value,bool all=false) {
    if(!stagePairPage(m))return;
    m->cancelDraft();std::vector<int> ids;std::vector<float> before;
    for(int stage=0;stage<8;++stage)if(all || m->uiAlt.load() || stage==m->selectedStage.load()) {
        int id=stagePairSecondary(m,stage);if(m->ctrlOwner(id)>=0)continue;
        ids.push_back(id);before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(float(value));
    }
    parameterHistory(m,ids,before,"change stage trigger");
}
void stagePairPress(Metropolix* m) {
    int choices=m->selectedRow.load()==Metropolix::PROBABILITY?2:3;
    stagePairChoice(m,(m->integer(stagePairSecondary(m,m->selectedStage.load()))+1)%choices);
}
void stagePairDrag(Metropolix* m,int selected,const std::vector<int>& ids,const std::vector<float>& before) {
    if(!stagePairPage(m) || !m->uiAlt.load())return;
    auto origin=std::find(ids.begin(),ids.end(),selected);if(origin==ids.end())return;
    float delta=m->params[selected].getValue()-before[origin-ids.begin()];
    for(int id:stagePairIds(m,false,true))if(id!=selected) {
        auto it=std::find(ids.begin(),ids.end(),id);
        if(it!=ids.end())m->getParamQuantity(id)->setValue(before[it-ids.begin()]+delta);
    }
}
void appendStagePairMenu(ui::Menu* menu,Metropolix* m) {
    if(!stagePairPage(m))return;
    bool probability=m->selectedRow.load()==Metropolix::PROBABILITY;
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuLabel(probability?"Stage probability":"Stage accumulation"));
    int id=stagePairSecondary(m,m->selectedStage.load());
    std::vector<std::string> choices=probability?std::vector<std::string>{"Stage","Pulse"}:std::vector<std::string>{"Stage","Pulse","Ratchet"};
    menu->addChild(createIndexSubmenuItem(probability?"Probability target":"Transpose trigger",choices,[m,id]{return size_t(m->integer(id));},[m](size_t value){stagePairChoice(m,int(value));},m->ctrlOwner(id)>=0));
    menu->addChild(createMenuItem(probability?"Apply this probability target to all stages":"Apply this transpose trigger to all stages","",[m,id]{stagePairChoice(m,m->integer(id),true);}));
    menu->addChild(createMenuItem("Reset this stage value and trigger","",[m]{bool alt=m->uiAlt.load();m->uiAlt.store(false);resetStagePairs(m);m->uiAlt.store(alt);}));
    menu->addChild(createMenuItem("Reset all stage values and triggers","ALT + hold encoder",[m]{resetStagePairs(m,true);}));
}
