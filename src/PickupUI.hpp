struct PanelControlChange : history::ModuleAction {
    int paramId=0;float before=0,after=0;metro::Pickup pickupBefore,pickupAfter;
    void apply(bool undoing){if(auto* m=dynamic_cast<Metropolix*>(APP->engine->getModule(moduleId))){float physical=undoing?before:after;APP->engine->setParamValue(m,paramId,physical);Metropolix::PresetCommand c;c.kind=Metropolix::PresetCommand::PanelEdit;int i=Metropolix::panelIndex(paramId);c.pickupMask=1u<<i;c.physical[i]=physical;c.pickup[i]=undoing?pickupBefore:pickupAfter;m->presetUiCommands.push_back(c);m->sendPresetCommand();}}
    void undo()override{apply(true);}void redo()override{apply(false);}
};
void panelControlHistory(Metropolix* m,int id,float before,float after,const metro::Pickup& oldState,const metro::Pickup& newState,const std::string& name){
    if(before==after && oldState==newState)return;
    auto* h=new PanelControlChange;h->moduleId=m->id;h->paramId=id;h->before=before;h->after=after;h->pickupBefore=oldState;h->pickupAfter=newState;h->name=name;APP->history->push(h);
    Metropolix::PresetCommand c;c.kind=Metropolix::PresetCommand::PanelEdit;int i=Metropolix::panelIndex(id);c.pickupMask=1u<<i;c.physical[i]=after;c.pickup[i]=newState;m->presetUiCommands.push_back(c);m->sendPresetCommand();
}
metro::Pickup uiPickup(Metropolix* m,int id){metro::Pickup p;int i=Metropolix::panelIndex(id);p.active=m->waitingPickup(id);p.target=i>=0?m->pickupTargets[i].load():0;p.last=m->params[id].getValue();return p;}
void takeOverPanel(Metropolix* m,int id){if(!m->waitingPickup(id))return;auto before=uiPickup(m,id),after=before;after.active=false;float value=m->params[id].getValue();panelControlHistory(m,id,value,value,before,after,"take over recalled control");}
