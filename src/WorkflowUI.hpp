// Clipboard and ALT baseline belong to the UI thread and are deliberately transient.
void workflowNotice(Metropolix* m,const std::string& message){m->workflowMessage=message;m->workflowUntil=system::getTime()+2.0;}
std::vector<int> workflowStageIds(Metropolix* m,int stage){
    using M=Metropolix;int t=m->selectedTrack.load(),lane=m->selectedLane.load(),row=m->selectedRow.load();
    if(m->uiMod.load())return {M::modParam(lane,M::MOD_VALUE+stage),M::modParam(lane,M::MOD_RAMP+stage)};
    std::vector<int> ids{M::stageParam(t,stage,row)};
    if(row==M::PROBABILITY)ids.push_back(M::stageOption(t,stage,M::PROB_TARGET));
    if(row==M::ACCUMULATION)ids.push_back(M::stageOption(t,stage,M::ACCUM_TRIGGER));
    if(row==M::CV)ids.push_back(M::cvRampParam(t,stage));
    return ids;
}
std::vector<int> workflowTrackIds(Metropolix* m,int track,bool randomize=false){
    using M=Metropolix;bool mod=m->uiMod.load();int lane=m->selectedLane.load();
    switch(m->uiPage.load()){
        case M::ORDER_PAGE:return mod?std::vector<int>{M::modParam(lane,M::MOD_ORDER),M::modParam(lane,M::MOD_REVERSED)}:std::vector<int>{M::trackParam(track,M::ORDER),M::extraParam(track,M::REVERSED)};
        case M::LENGTH_PAGE:{std::vector<int> ids=mod?std::vector<int>{M::modParam(lane,M::MOD_LENGTH),M::modParam(lane,M::MOD_STAGE_OFFSET)}:std::vector<int>{M::trackParam(track,M::LENGTH),M::trackParam(track,M::OFFSET)};if(!randomize)ids.push_back(mod?M::modParam(lane,M::MOD_PULSE_LIMIT):M::extraParam(track,M::PULSE_LIMIT));return ids;}
        case M::DIV_PAGE:return mod?std::vector<int>{M::modParam(lane,M::MOD_DIVISION)}:std::vector<int>{M::trackParam(track,M::DIVISION),M::extraParam(track,M::PULSE_DIV)};
        case M::SWING_PAGE:return mod?std::vector<int>{}:std::vector<int>{M::swingParam(track)};
        case M::SLIDE_PAGE:return mod?std::vector<int>{}:std::vector<int>{M::trackParam(track,M::SLIDE_TIME),M::slideTypeParam(track)};
        case M::GATE_PAGE:return mod?std::vector<int>{}:std::vector<int>{M::trackParam(track,M::GATE_LENGTH),M::gateScaleParam(track)};
        case M::ACCUM_PAGE:{std::vector<int> ids;for(int f=0;f<6;++f)ids.push_back(M::accumScreenParam(track,f));return ids;}
        default:return {};
    }
}
std::vector<int> workflowScreenIds(Metropolix* m){
    std::vector<int> ids;if(m->uiPage.load()!=Metropolix::STAGE_PAGE)return workflowTrackIds(m,m->selectedTrack.load(),true);
    for(int stage=0;stage<8;++stage){auto next=workflowStageIds(m,stage);ids.insert(ids.end(),next.begin(),next.end());}return ids;
}
void captureWorkflowBaseline(Metropolix* m){
    m->workflowBaseline.assign(Metropolix::NUM_PARAMS,0);json_t* json=APP->engine->moduleToJson(m);size_t index;json_t* entry;
    json_array_foreach(json_object_get(json,"params"),index,entry){int id=int(json_integer_value(json_object_get(entry,"id")));if(id>=0 && id<Metropolix::NUM_PARAMS)m->workflowBaseline[id]=float(json_number_value(json_object_get(entry,"value")));}json_decref(json);
}
void workflowWrite(Metropolix* m,const std::vector<int>& ids,const std::vector<float>& values,const std::string& name){
    m->cancelDraft();
    if(ids.size()!=values.size())return;
    std::vector<float> before;for(size_t i=0;i<ids.size();++i){int id=ids[i];before.push_back(m->params[id].getValue());if(m->ctrlOwner(id)<0){auto* q=m->getParamQuantity(id);if(dynamic_cast<RoutingAssignmentQuantity*>(q))m->params[id].setValue(values[i]);else q->setValue(values[i]);}}
    parameterHistory(m,ids,before,name);
}
void randomizeWorkflow(Metropolix* m,int kind){
    if(!m)return;
    auto ids=workflowScreenIds(m);if(ids.empty()){workflowNotice(m,"SELECT TRACK SCREEN");return;}
    if(m->workflowBaseline.empty())captureWorkflowBaseline(m);
    std::vector<float> values;
    for(int id:ids){auto* q=m->getParamQuantity(id);if(kind==2){values.push_back(m->workflowBaseline[id]);continue;}
        metro::WorkflowRange range;range.low=q->getMinValue();range.high=q->getMaxValue();range.discrete=q->snapEnabled || dynamic_cast<SwitchQuantity*>(q);range.toggle=range.discrete && range.high-range.low==1;
        if(auto* cv=dynamic_cast<TrackCvQuantity*>(q))if(cv->type()==metro::GateCv){range.low=0;range.high=5;range.toggle=true;}
        if(auto* cv=dynamic_cast<ModVoltageQuantity*>(q))if(cv->gate()){range.low=0;range.high=5;range.toggle=true;}
        if(dynamic_cast<TrackCvQuantity*>(q) || dynamic_cast<ModVoltageQuantity*>(q))range.unit=.001f;
        values.push_back(metro::workflowRandom(q->getValue(),range,m->workflowRandomSource,kind==1));
    }
    workflowWrite(m,ids,values,kind==0?"randomize screen":kind==1?"morph screen":"restore ALT screen");workflowNotice(m,kind==0?"RANDOMIZED":kind==1?"MORPHED":"ALT STATE RESTORED");
}
std::vector<int> workflowCopyIds(Metropolix* m,int kind,int target){
    using M=Metropolix;if(kind==1)return workflowStageIds(m,target);
    if(kind==2)return workflowTrackIds(m,target);
    if(kind==3){int prior=m->selectedTrack.load();m->selectedTrack.store(target);std::vector<int> ids;for(int item=0;item<TrackMenuCount;++item){int id=trackMenuParam(m,item);if(id>=0)ids.push_back(id);}m->selectedTrack.store(prior);return ids;}
    std::vector<int> ids;if(kind==4){for(int p=0;p<M::MOD_STRIDE;++p)ids.push_back(M::modParam(target,p));for(int p=0;p<M::MIDI_MOD_STRIDE;++p)ids.push_back(M::midiModParam(target,p));}return ids;
}
int workflowTag(Metropolix* m,int kind){return kind==1?(m->uiMod.load()?8:m->selectedRow.load()):kind==2?m->uiPage.load():0;}
void copyWorkflow(Metropolix* m,int kind,int source){
    auto ids=workflowCopyIds(m,kind,source);if(ids.empty())return;m->workflowClipKind=kind;m->workflowClipTag=workflowTag(m,kind);m->workflowClipboard.clear();for(int id:ids)m->workflowClipboard.push_back(m->getParamQuantity(id)->getValue());workflowNotice(m,"COPIED");
}
bool canPasteWorkflow(Metropolix* m,int kind){return m->workflowClipKind==kind && m->workflowClipTag==workflowTag(m,kind);}
void pasteWorkflow(Metropolix* m,int kind,int target){
    if(!canPasteWorkflow(m,kind))return;
    workflowWrite(m,workflowCopyIds(m,kind,target),m->workflowClipboard,kind==1?"paste stage parameter":kind==2?"paste track settings":kind==3?"paste track menu":"paste MOD lane");workflowNotice(m,"PASTED");
}
void clearWorkflowAux(Metropolix* m,bool record=true){
    int input=m->selectedAux.load();std::vector<int> ids{Metropolix::auxParam(input,Metropolix::AUX_DESTINATION),Metropolix::auxParam(input,Metropolix::AUX_ASSIGNMENT)};std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(0);}
    if(record)parameterHistory(m,ids,before,"clear AUX assignment");
    m->uiField.store(0);workflowNotice(m,"AUX CLEARED");
}
void workflowTrackPress(Metropolix* m,int track){
    unsigned prior=m->heldTrackReset.load();int kind=trackMenuPage(m)?3:!m->uiMod.load() && !workflowTrackIds(m,track).empty()?2:0;
    if(kind && (prior & (1u<<(1-track)))){copyWorkflow(m,kind,1-track);pasteWorkflow(m,kind,track);m->workflowCopyUsed=true;}
}
bool workflowStagePress(Metropolix* m,int stage,bool editHeld){
    if(m->uiPage.load()==Metropolix::LOOPY_PAGE || m->loopyMuteHeld.load() || m->scaleShortcutHeld.load() || m->orderShortcutHeld.load())return false;
    if(!m->workflowStageHeld){m->workflowStageSource=stage;m->workflowLaneCopy=m->uiMod.load() && editHeld;m->workflowUntil=0;
        if(m->workflowLaneCopy){m->selectedLane.store(stage);m->workflowCopyUsed=m->workflowEditUsed=true;m->workflowStageHeld=1u<<stage;return true;}}
    if(m->workflowStageHeld && !(m->workflowStageHeld & (1u<<stage))){int kind=m->workflowLaneCopy?4:1;
        if(kind==4 || m->uiPage.load()==Metropolix::STAGE_PAGE){copyWorkflow(m,kind,m->workflowStageSource);pasteWorkflow(m,kind,stage);m->workflowCopyUsed=true;m->workflowStageHeld|=1u<<stage;return true;}}
    m->workflowStageHeld|=1u<<stage;return false;
}
void workflowStageRelease(Metropolix* m,int stage){m->workflowStageHeld&=~(1u<<stage);if(!m->workflowStageHeld){m->workflowCopyUsed=false;m->workflowLaneCopy=false;}}
void appendWorkflowMenu(ui::Menu* menu,Metropolix* m,int forcedKind,int forcedTarget){
    if(!m)return;
    menu->addChild(new ui::MenuSeparator);menu->addChild(createMenuLabel("Editing workflows"));
    bool available=!workflowScreenIds(m).empty();
    for(int kind=0;kind<3;++kind){const char* names[]={"Randomize current screen","Morph current screen","Restore pre-ALT screen"};menu->addChild(createMenuItem(names[kind],kind==0?"ALT + AUX X":kind==1?"ALT + AUX Y":"ALT + AUX Z",[m,kind]{randomizeWorkflow(m,kind);},!available || (kind==2 && m->workflowBaseline.empty())));}
    int kind=forcedKind?forcedKind:trackMenuPage(m)?3:m->uiPage.load()==Metropolix::STAGE_PAGE?1:!m->uiMod.load() && !workflowTrackIds(m,m->selectedTrack.load()).empty()?2:0;
    int source=forcedTarget>=0?forcedTarget:kind==1?m->selectedStage.load():m->selectedTrack.load();
    if(kind){menu->addChild(createMenuItem(kind==1?"Copy this stage parameter":kind==2?"Copy these track settings":kind==3?"Copy track menu settings":"Copy entire MOD lane","",[m,kind,source]{copyWorkflow(m,kind,source);}));menu->addChild(createMenuItem("Paste clipboard here","",[m,kind,source]{pasteWorkflow(m,kind,source);},!canPasteWorkflow(m,kind)));}
    if(m->uiMod.load() && forcedKind!=4){int lane=m->selectedLane.load();menu->addChild(createMenuItem("Copy entire MOD lane","",[m,lane]{copyWorkflow(m,4,lane);}));menu->addChild(createMenuItem("Paste MOD lane here","",[m,lane]{pasteWorkflow(m,4,lane);},!canPasteWorkflow(m,4)));}
}
