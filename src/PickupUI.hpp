struct PanelControlChange : history::ModuleAction {
    int paramId=0;float before=0,after=0;metro::Pickup pickupBefore,pickupAfter;
    void apply(bool undoing){if(auto* m=dynamic_cast<Metropolix*>(APP->engine->getModule(moduleId))){float physical=undoing?before:after;APP->engine->setParamValue(m,paramId,physical);Metropolix::PresetCommand c;c.kind=Metropolix::PresetCommand::PanelEdit;c.preservePhysicalEdits=true;int i=Metropolix::panelIndex(paramId);c.pickupMask=1u<<i;c.physical[i]=physical;c.pickup[i]=undoing?pickupBefore:pickupAfter;m->presetUiCommands.push_back(c);m->sendPresetCommand();}}
    void undo()override{apply(true);}void redo()override{apply(false);}
};
void panelControlHistory(Metropolix* m,int id,float before,float after,const metro::Pickup& oldState,const metro::Pickup& newState,const std::string& name){
    if(before==after && oldState==newState)return;
    auto* h=new PanelControlChange;h->moduleId=m->id;h->paramId=id;h->before=before;h->after=after;h->pickupBefore=oldState;h->pickupAfter=newState;h->name=name;APP->history->push(h);
    Metropolix::PresetCommand c;c.kind=Metropolix::PresetCommand::PanelEdit;c.preservePhysicalEdits=true;int i=Metropolix::panelIndex(id);c.pickupMask=1u<<i;c.physical[i]=after;c.pickup[i]=newState;m->presetUiCommands.push_back(c);m->sendPresetCommand();
}
metro::Pickup uiPickup(Metropolix* m,int id){metro::Pickup p;int i=Metropolix::panelIndex(id);p.active=m->waitingPickup(id);p.target=i>=0?m->pickupTargets[i].load():0;p.last=m->params[id].getValue();return p;}
metro::Pickup pickupAfterValue(Metropolix* m,int id,metro::Pickup state,float value){bool pitch=Metropolix::panelIndex(id)<8;int mode=m->integer(pitch?Metropolix::LOAD_PITCH:Metropolix::LOAD_CTRL);state.process(value,mode==(pitch?Metropolix::PitchThru:Metropolix::CtrlThru),mode>=(pitch?Metropolix::PitchThru:Metropolix::CtrlThru));return state;}
void takeOverPanel(Metropolix* m,int id){if(!m->waitingPickup(id))return;auto before=uiPickup(m,id),after=before;after.active=false;float value=m->params[id].getValue();panelControlHistory(m,id,value,value,before,after,"take over recalled control");}
struct PanelSyncChange : history::ModuleAction {
    metro::PanelPickup before,after;std::array<float,10> physical{};
    void apply(bool undoing){if(auto* m=dynamic_cast<Metropolix*>(APP->engine->getModule(moduleId))){Metropolix::PresetCommand command;command.kind=Metropolix::PresetCommand::PanelEdit;command.preservePhysicalEdits=true;command.pickupMask=1023;command.pickup=undoing?before:after;command.physical=physical;m->presetUiCommands.push_back(command);m->sendPresetCommand();}}
    void undo()override{apply(true);}void redo()override{apply(false);}
};
void syncPanel(Metropolix* m){
    if(!m || !m->pickupActiveMask.load())return;
    auto* change=new PanelSyncChange;change->moduleId=m->id;change->name="sync recalled controls to panel";
    for(int i=0;i<10;++i){int id=Metropolix::panelParam(i);change->physical[i]=m->params[id].getValue();change->before[i]=change->after[i]=uiPickup(m,id);change->after[i].active=false;}
    APP->history->push(change);change->apply(false);m->workflowMessage="SYNC TO PANEL";m->workflowUntil=system::getTime()+2;
}
