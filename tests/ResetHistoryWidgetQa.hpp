using M=Metropolix;APP->engine->bypassModule(m,true);
// Isolate this exhaustive fixture from Rack's bounded history and restore the
// original QA history afterward. No user profile is opened by this suite.
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->actionIndex=0;APP->history->savedIndex=0;
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};auto original=raw();
for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::ENC_CONFIRM].setValue(0);m->uiMod.store(false);m->uiAlt.store(false);m->selectedTrack.store(0);
auto* encoder=find("Encoder");widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
auto lastAction=[&]()->history::Action*{return APP->history->actionIndex?APP->history->actions[APP->history->actionIndex-1]:nullptr;};
auto receipt=[&](history::Action* previous,const std::vector<float>& before,const std::vector<float>& after){if(before==after)return lastAction()==previous;auto& a=APP->history->actions;int index=APP->history->actionIndex;bool one=previous?(index>1 && a[index-2]==previous):index==1;if(!one || lastAction()==previous)return false;APP->history->undo();bool ok=raw()==before;APP->history->redo();return ok && raw()==after;};
screenPage(m,M::GATE_PAGE);m->uiField.store(1);m->params[M::trackParam(0,M::GATE_LENGTH)].setValue(75);m->params[M::gateScaleParam(0)].setValue(63);encoder->onDragStart(start);m->getParamQuantity(M::gateScaleParam(0))->setValue(27);auto before=raw();auto* previous=lastAction();encoder->heldSince=system::getTime()-1.1;encoder->step();encoder->onDragEnd(end);
check(m->integer(M::trackParam(0,M::GATE_LENGTH))==50 && m->integer(M::gateScaleParam(0))==100 && receipt(previous,before,raw()),"long gate reset undoes to latest applied values immediately before reset, with one action");
screenPage(m,M::A_PAGE);m->params[M::OUTPUT_SOURCE_A].setValue(0);encoder->onDragStart(start);m->getParamQuantity(M::OUTPUT_SOURCE_A)->setValue(37);before=raw();previous=lastAction();encoder->heldSince=system::getTime()-1.1;encoder->step();encoder->onDragEnd(end);
check(m->integer(M::OUTPUT_SOURCE_A)==0 && receipt(previous,before,raw()),"long A/B reset records and undoes a changed external source even when its default equals the original mouse-down value");
auto clean=[&]{for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->selectedTrack.store(0);m->scaleEditing.store(false);};
event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;move.mouseDelta=Vec(0,-44);
bool families=true,threshold=true,immediate=true,consumed=true,noops=true,unchanged=true;int cases=0;
// Defaults define reset behavior. Origin/history, unrelated raw state and
// release are checked independently of HardwareControl's groupedParams().
auto reset=[&](const std::vector<int>& ids,const std::vector<float>& defaults,bool external=true){
    for(size_t i=0;i<ids.size();++i)m->params[ids[i]].setValue(defaults[i]);encoder->onDragStart(start);
    if(external)for(size_t i=0;i<ids.size();++i){auto* q=m->getParamQuantity(ids[i]);m->params[ids[i]].setValue(q->maxValue!=defaults[i]?q->maxValue:q->minValue);}
    auto origin=raw(),expected=origin;for(size_t i=0;i<ids.size();++i)expected[ids[i]]=defaults[i];auto* prior=lastAction();
    encoder->heldSince=system::getTime()-.9;encoder->step();threshold &= !encoder->longFired && raw()==origin && lastAction()==prior;
    encoder->heldSince=system::getTime()-1.1;encoder->step();auto after=raw();
    if(after!=expected){std::string diff;for(int id=0;id<M::NUM_PARAMS;++id)if(after[id]!=expected[id])diff+=string::f(" param%d=%g expected%g",id,after[id],expected[id]);check(false,string::f("reset case%d page%d row%d field%d:",cases,m->uiPage.load(),m->selectedRow.load(),m->uiField.load())+diff);}
    families &= encoder->longFired && after==expected;immediate &= receipt(prior,origin,after);if(!external)noops &= lastAction()==prior;
    auto* recorded=lastAction();encoder->step();consumed &= raw()==after && lastAction()==recorded;
    m->params[M::MOD_GROUP_MUTE].setValue(1-m->params[M::MOD_GROUP_MUTE].getValue());if(!ids.empty())m->params[ids[0]].setValue(origin[ids[0]]);
    auto late=raw();encoder->onDragMove(move);encoder->onDragEnd(end);
    consumed &= raw()==late && lastAction()==recorded && !APP->window->isCursorLocked() && !encoder->dragging;++cases;
};
auto defaults=[&](const std::vector<int>& ids){std::vector<float> values;for(int id:ids)values.push_back(m->getParamQuantity(id)->defaultValue);reset(ids,values);};
auto page=[&](int which,int target=0,bool mod=false,int field=0){clean();m->params[M::ENC_CONFIRM].setValue(cases%2);m->uiMod.store(mod);m->selectedTrack.store(target%2);m->selectedLane.store(target%8);screenPage(m,which);m->uiField.store(field);};
for(int t=0;t<2;++t){
    page(M::GATE_PAGE,t,false,1);defaults({M::trackParam(t,M::GATE_LENGTH),M::gateScaleParam(t)});
    page(M::SLIDE_PAGE,t,false,1);defaults({M::trackParam(t,M::SLIDE_TIME),M::slideTypeParam(t)});
    page(M::SWING_PAGE,t);reset({M::swingParam(t)},{50});
    page(M::CV_TYPE_PAGE,t);reset({M::cvTypeParam(t)},{0});
    for(int setting:{0,1})for(int stage=0;stage<8;++stage)for(bool alt:{false,true})for(int row:{M::PROBABILITY,M::ACCUMULATION,M::RATCHETS,M::CV,M::PITCH_OVERRIDE,M::GATE_OVERRIDE,M::SLIDE,M::SKIP}){
        if(!alt && (row==M::SLIDE || row==M::SKIP))continue;
        page(M::STAGE_PAGE,t);m->params[M::ENC_CONFIRM].setValue(setting);m->selectedStage.store(stage);m->selectedRow.store(row);m->uiAlt.store(alt);m->params[M::cvTypeParam(t)].setValue(metro::UnipolarCv);
        std::vector<int> ids;std::vector<float> values;
        for(int s=0;s<8;++s)if(alt || s==stage){int id=M::stageParam(t,s,row);ids.push_back(id);values.push_back(row==M::CV?0:m->getParamQuantity(id)->defaultValue);if(row==M::PROBABILITY || row==M::ACCUMULATION){int secondary=M::stageOption(t,s,row==M::PROBABILITY?M::PROB_TARGET:M::ACCUM_TRIGGER);ids.push_back(secondary);values.push_back(m->getParamQuantity(secondary)->defaultValue);}}
        reset(ids,values);
    }
    page(M::STAGE_PAGE,t);m->selectedStage.store(3);m->selectedRow.store(M::CV);m->uiAlt.store(true);m->params[M::cvTypeParam(t)].setValue(metro::UnipolarCv);encoder->onDragStart(start);
    for(int s=0;s<8;++s)m->params[M::stageParam(t,s,M::CV)].setValue(-4.75f+.125f*s);before=raw();previous=lastAction();encoder->heldSince=system::getTime()-1.1;encoder->step();auto after=raw();unchanged &= receipt(previous,before,after);for(int s=0;s<8;++s)unchanged &= after[M::stageParam(t,s,M::CV)]==0;encoder->onDragEnd(end);
}
for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target){
    page(M::LENGTH_PAGE,target,mod,1);defaults(mod?std::vector<int>{M::modParam(target,M::MOD_LENGTH),M::modParam(target,M::MOD_STAGE_OFFSET),M::modParam(target,M::MOD_PULSE_LIMIT)}:std::vector<int>{M::trackParam(target,M::LENGTH),M::trackParam(target,M::OFFSET),M::extraParam(target,M::PULSE_LIMIT)});
    page(M::DIV_PAGE,target,mod,mod?0:1);defaults(mod?std::vector<int>{M::modParam(target,M::MOD_DIVISION)}:std::vector<int>{M::trackParam(target,M::DIVISION),M::extraParam(target,M::PULSE_DIV)});
    page(M::ORDER_PAGE,target,mod,1);defaults(mod?std::vector<int>{M::modParam(target,M::MOD_ORDER),M::modParam(target,M::MOD_REVERSED)}:std::vector<int>{M::trackParam(target,M::ORDER),M::extraParam(target,M::REVERSED)});
}
for(int lane=0;lane<8;++lane){
    for(int stage=0;stage<8;++stage)for(bool alt:{false,true}){page(M::STAGE_PAGE,lane,true);m->selectedStage.store(stage);m->uiAlt.store(alt);std::vector<int> ids;for(int s=0;s<8;++s)if(alt || s==stage)ids.push_back(M::modParam(lane,M::MOD_VALUE+s));reset(ids,std::vector<float>(ids.size(),0));}
    page(M::MOD_ASSIGN_PAGE,lane,true);defaults({M::modParam(lane,M::MOD_DESTINATION),M::modParam(lane,M::MOD_ASSIGNMENT)});
    for(int n=0;n<2;++n){page(M::MOD_MENU_PAGE,lane,true,1);m->modMenuCursor.store(n);defaults({M::midiModParam(lane,n)});}
}
for(int n=0;n<3;++n){page(M::AUX_PAGE);m->selectedAux.store(n);reset({M::auxParam(n,M::AUX_DESTINATION),M::auxParam(n,M::AUX_ASSIGNMENT)},{0,0});}
for(int n=0;n<2;++n){page(M::CTRL_PAGE);m->selectedCtrl.store(n);m->params[M::ctrlParam(n,M::CTRL_ASSIGNMENT)].setValue(2);reset({M::ctrlParam(n,M::CTRL_DESTINATION)},{float(metro::CtrlNone)});}
for(int n=0;n<2;++n){page(n?M::B_PAGE:M::A_PAGE,0,false,1);reset({M::OUTPUT_SOURCE_A+n},{float(n?metro::outputSource(metro::OutFirstPulse):0)});}
page(M::CLOCK_OUT_PAGE,0,false,1);defaults({M::CLOCK_OUT_DIV,M::CLOCK_OUT_OFFSET});
page(M::MX_SETUP_PAGE,0,false,2);defaults({M::MX_MODE,M::MX_TRACK,M::MX_LANE});
for(int n=0;n<UiSetupCount;++n){page(M::UI_SETUP_PAGE,0,false,1);m->uiSetupCursor.store(n);defaults({uiSetupParam(m,n)});}
for(int n=0;n<GlobalMenuCount-1;++n){page(M::GLOBAL_SETUP_PAGE,0,false,1);m->globalCursor.store(n);defaults({globalMenuParam(m,n)});}
for(int n=0;n<MidiSetupCount-1;++n){page(M::MIDI_SETUP_PAGE,0,false,1);m->midiSetupCursor.store(n);defaults({midiSetupParam(m,n)});}
for(int t=0;t<2;++t)for(int n=0;n<TrackMenuCount;++n){page(M::TRACK_MENU_PAGE,t,false,1);m->trackMenuCursor.store(n);if(n<TrackMenuCount-1)defaults({trackMenuParam(m,n)});else defaults(trackResetParams(m));}
page(M::A_PAGE);reset({M::OUTPUT_SOURCE_A},{0},false);
page(M::GATE_PAGE);reset({M::trackParam(0,M::GATE_LENGTH),M::gateScaleParam(0)},{50,100},false);
check(families,"every reset family applies its defaults and preserves all unrelated raw parameters ("+std::to_string(cases)+" held cases)");
check(threshold,"all reset families wait through 0.9 seconds and fire at 1.1 seconds");
check(immediate,"all reset families record exactly one raw-state undo/redo at firing time, using the latest external values");
check(consumed,"repeat refresh, movement and release after firing preserve subsequent external writes without new undo or cursor capture");
check(noops && unchanged,"no-op reset adds no history and dormant negative Unipolar CV restores exactly");
page(M::STAGE_PAGE);m->selectedRow.store(M::PROBABILITY);m->selectedStage.store(0);encoder->onDragStart(start);m->selectedTrack.store(1);m->selectedStage.store(7);m->uiAlt.store(true);
for(int s=0;s<8;++s){m->params[M::stageParam(1,s,M::PROBABILITY)].setValue(23+s);m->params[M::stageOption(1,s,M::PROB_TARGET)].setValue(1);}before=raw();previous=lastAction();encoder->heldSince=system::getTime()-1.1;encoder->step();auto after=raw();bool freshScope=receipt(previous,before,after);for(int s=0;s<8;++s)freshScope &= after[M::stageParam(1,s,M::PROBABILITY)]==100 && after[M::stageOption(1,s,M::PROB_TARGET)]==0 && after[M::stageParam(0,s,M::PROBABILITY)]==before[M::stageParam(0,s,M::PROBABILITY)];encoder->onDragEnd(end);
check(freshScope,"long reset captures the current track/stage/ALT scope when it fires and undoes all affected values exactly");
page(M::GATE_PAGE);m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlGateLength);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);m->params[M::trackParam(0,M::GATE_LENGTH)].setValue(75);encoder->onDragStart(start);m->params[M::gateScaleParam(0)].setValue(27);before=raw();previous=lastAction();encoder->heldSince=system::getTime()-1.1;encoder->step();after=raw();bool partial=after[M::trackParam(0,M::GATE_LENGTH)]==75 && after[M::gateScaleParam(0)]==100 && receipt(previous,before,after);encoder->onDragEnd(end);
check(partial,"owned gate remains unchanged while long reset records its unowned scale from the latest raw value");
page(M::SETUP_PAGE);auto* setup=find("SETUP");before=raw();previous=lastAction();setup->onDragStart(start);setup->heldSince=system::getTime()-1.1;setup->step();bool opened=chainPage(m);setup->onDragMove(move);setup->onDragEnd(end);
check(opened && raw()==before && lastAction()==previous && !APP->window->isCursorLocked(),"SETUP hold opens chain view and consumes late mouse movement without editing the previous page binding");
clean();screenPage(m,M::HOME);m->uiAlt.store(false);for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->actionIndex=savedActionIndex;APP->history->savedIndex=savedIndex;APP->engine->bypassModule(m,false);
