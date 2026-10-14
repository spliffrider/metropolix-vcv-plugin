bool modStagePage(Metropolix* m){return m && m->uiMod.load() && m->uiPage.load()==Metropolix::STAGE_PAGE;}
bool modStageGates(Metropolix* m){return m && m->effectiveModType(m->selectedLane.load())==metro::ModGate;}
std::vector<int> modStageIds(Metropolix* m,bool all=false){std::vector<int> ids;for(int s=0;s<8;++s)if(all || m->uiAlt.load() || s==m->selectedStage.load())ids.push_back(Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_VALUE+s));return ids;}
void resetModStages(Metropolix* m,bool record=true){auto ids=modStageIds(m);std::vector<float> before;for(int id:ids){before.push_back(m->params[id].getValue());m->params[id].setValue(0);}if(record)parameterHistory(m,ids,before,"reset MOD stages");}
void rotateModStages(Metropolix* m,int turns,bool record=false){auto ids=modStageIds(m,true);std::vector<float> before;for(int id:ids)before.push_back(m->params[id].getValue());for(int s=0;s<8;++s)m->params[ids[s]].setValue(before[metro::modulo(s-turns,8)]);if(record)parameterHistory(m,ids,before,"rotate MOD gates");}
void modStagePress(Metropolix* m){
    if(modStageGates(m)){auto ids=modStageIds(m);std::vector<float> before;for(int id:ids){before.push_back(m->params[id].getValue());m->params[id].setValue(m->getParamQuantity(id)->getValue()>=1.5f?0:5);}parameterHistory(m,ids,before,"invert MOD gates");return;}
    int lane=m->selectedLane.load(),next=1-m->integer(Metropolix::modParam(lane,Metropolix::MOD_RAMP+m->selectedStage.load()));std::vector<int> ids;std::vector<float> before;
    for(int s=0;s<8;++s)if(m->uiAlt.load() || s==m->selectedStage.load()){int id=Metropolix::modParam(lane,Metropolix::MOD_RAMP+s);ids.push_back(id);before.push_back(m->params[id].getValue());m->params[id].setValue(next);}
    parameterHistory(m,ids,before,"set MOD transitions");
}
void resetModAssignment(Metropolix* m,bool record=true){int lane=m->selectedLane.load();std::vector<int> ids{Metropolix::modParam(lane,Metropolix::MOD_DESTINATION),Metropolix::modParam(lane,Metropolix::MOD_ASSIGNMENT)};std::vector<float> before;for(int id:ids){before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(0);}m->cancelDraft();if(record)parameterHistory(m,ids,before,"clear MOD assignment");}
void appendModWorkflowMenu(ui::Menu* menu,Metropolix* m){
    if(!modStagePage(m))return;
    menu->addChild(createMenuItem("Reset this MOD stage","",[m]{bool alt=m->uiAlt.load();m->uiAlt.store(false);resetModStages(m);m->uiAlt.store(alt);}));
    menu->addChild(createMenuItem("Reset all MOD voltages","",[m]{bool alt=m->uiAlt.load();m->uiAlt.store(true);resetModStages(m);m->uiAlt.store(alt);}));
    if(modStageGates(m)){menu->addChild(createMenuItem("Invert MOD gate pattern","",[m]{bool alt=m->uiAlt.load();m->uiAlt.store(true);modStagePress(m);m->uiAlt.store(alt);}));for(int d:{-1,1})menu->addChild(createMenuItem(d<0?"Rotate MOD gates backward":"Rotate MOD gates forward","",[m,d]{rotateModStages(m,d,true);}));}
    else menu->addChild(createMenuItem("Apply this MOD transition to all stages","",[m]{int lane=m->selectedLane.load(),value=m->integer(Metropolix::modParam(lane,Metropolix::MOD_RAMP+m->selectedStage.load()));std::vector<int> ids;std::vector<float> values;for(int s=0;s<8;++s){ids.push_back(Metropolix::modParam(lane,Metropolix::MOD_RAMP+s));values.push_back(value);}workflowWrite(m,ids,values,"copy MOD transition to all stages");}));
}
void initModLane(Metropolix* m,bool all,bool record){
    std::vector<int> ids;std::vector<float> before;for(int lane=0;lane<8;++lane)if(all || lane==m->selectedLane.load())for(int s=0;s<8;++s)for(int p:{Metropolix::MOD_VALUE+s,Metropolix::MOD_RAMP+s}){int id=Metropolix::modParam(lane,p);ids.push_back(id);before.push_back(m->params[id].getValue());m->params[id].setValue(m->getParamQuantity(id)->defaultValue);}
    if(record)parameterHistory(m,ids,before,all?"initialize all MOD stages":"initialize MOD lane stages");
}
