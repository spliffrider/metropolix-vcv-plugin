using M=Metropolix;APP->engine->bypassModule(m,true);
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->actionIndex=0;APP->history->savedIndex=0;
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};auto original=raw();
auto clean=[&]{for(auto* a:APP->history->actions)delete a;APP->history->actions.clear();APP->history->actionIndex=0;APP->history->savedIndex=0;for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::ENC_CONFIRM].setValue(0);m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(false);m->uiChooseFunction.store(false);};
widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;move.mouseDelta=Vec(0,-80);event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
int cases=0,failures=0;
auto probe=[&](HardwareControl* c,int own,int partner,float external,const std::string& label){
    auto origin=raw();c->onDragStart(start);c->onDragMove(move);auto edited=raw();if(cases%2)setWithHistory(m->getParamQuantity(partner),external);else m->params[partner].setValue(external);int prior=APP->history->actionIndex;origin[partner]=edited[partner]=external;c->onDragEnd(end);
    bool pass=raw()==edited && edited[own]!=origin[own] && APP->history->actionIndex==prior+1;
    if(APP->history->actionIndex>prior){APP->history->undo();pass &= raw()==origin;APP->history->redo();pass &= raw()==edited;}
    pass &= !APP->window->isCursorLocked() && !c->adjusting && !c->adjustmentTip && !c->dragging;
    ++cases;if(!pass){++failures;if(failures<12)check(false,label+" owns only its edited fields; external partner survives exact Undo/Redo");}
};
for(auto* c:{find("Encoder"),find("Metropolix display")})for(int field=0;field<2;++field){
    clean();screenPage(m,M::CLOCK_OUT_PAGE);m->uiField.store(field);m->params[M::CLOCK_OUT_DIV].setValue(16);m->params[M::CLOCK_OUT_OFFSET].setValue(5);
    probe(c,field?M::CLOCK_OUT_OFFSET:M::CLOCK_OUT_DIV,field?M::CLOCK_OUT_DIV:M::CLOCK_OUT_OFFSET,field?128:2,"CLK field "+std::to_string(field));
}
for(int page:{M::GATE_PAGE,M::SLIDE_PAGE})for(int track=0;track<2;++track)for(int field=0;field<(page==M::GATE_PAGE?2:1);++field)for(auto* c:{find("Encoder"),find("Metropolix display")}){
    clean();m->selectedTrack.store(track);screenPage(m,page);m->uiField.store(field);bool gate=page==M::GATE_PAGE;int amount=M::trackParam(track,gate?M::GATE_LENGTH:M::SLIDE_TIME),secondary=gate?M::gateScaleParam(track):M::slideTypeParam(track);m->params[amount].setValue(20);m->params[secondary].setValue(gate?50:0);
    probe(c,field?secondary:amount,field?amount:secondary,field?65.f:gate?75.f:2.f,std::string(gate?"GATE":"SLIDE")+" track "+std::to_string(track)+" field "+std::to_string(field));
}
for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target)for(auto* c:{find("Encoder"),find("Metropolix display")}){
    clean();m->uiMod.store(mod);m->selectedTrack.store(target%2);m->selectedLane.store(target);screenPage(m,M::ORDER_PAGE);auto ids=M::orderParams(mod,target);m->params[ids[0]].setValue(0);m->params[ids[1]].setValue(0);probe(c,ids[0],ids[1],1,"ORDER target "+std::to_string(target));
}
for(int page:{M::LENGTH_PAGE,M::DIV_PAGE})for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target)for(int field=0;field<(page==M::LENGTH_PAGE?3:mod?0:2);++field)for(auto* c:{find("Encoder"),find("Metropolix display")}){
    clean();m->uiMod.store(mod);m->selectedTrack.store(target%2);m->selectedLane.store(target);screenPage(m,page);m->uiField.store(field==1?1:0);m->uiAlt.store(field==2);auto ids=timingIds(m,page);for(size_t i=0;i<ids.size();++i)m->params[ids[i]].setValue(page==M::LENGTH_PAGE?(i==0?4:i==1?1:32):i==0?3:2);int own=timingParam(m,page,field),partner=ids[(std::find(ids.begin(),ids.end(),own)-ids.begin()+1)%ids.size()];probe(c,own,partner,page==M::LENGTH_PAGE?(partner==ids[2]?24:2):4,std::string(page==M::LENGTH_PAGE?"LEN":"DIV")+" target "+std::to_string(target)+" field "+std::to_string(field));
}
for(int row:{M::PROBABILITY,M::ACCUMULATION})for(int track=0;track<2;++track)for(int stage=0;stage<8;++stage)for(bool alt:{false,true})for(int surface=0;surface<(alt?2:3);++surface)for(int changed=0;changed<(alt?8:1);++changed){
    clean();m->selectedTrack.store(track);m->selectedStage.store(stage);m->selectedRow.store(row);screenPage(m,M::STAGE_PAGE);m->uiAlt.store(alt);
    for(int s=0;s<8;++s){m->params[M::stageParam(track,s,row)].setValue(row==M::PROBABILITY?40:0);m->params[M::stageOption(track,s,row==M::PROBABILITY?M::PROB_TARGET:M::ACCUM_TRIGGER)].setValue(0);}
    static const char* names[]={"Stage 1 / SLIDE","Stage 2 / SKIP","Stage 3 / PITCH","Stage 4 / GATE","Stage 5 / RATCH","Stage 6 / PROB","Stage 7 / ACCUM","Stage 8 / CV"};
    auto* c=find(surface==0?"Encoder":surface==1?"Metropolix display":names[stage]);int partner=M::stageOption(track,alt?changed:stage,row==M::PROBABILITY?M::PROB_TARGET:M::ACCUM_TRIGGER);
    probe(c,M::stageParam(track,stage,row),partner,row==M::PROBABILITY?1.f:2.f,std::string(row==M::PROBABILITY?"PROB":"ACCUM")+" track "+std::to_string(track)+" stage "+std::to_string(stage));
}
check(failures==0,string::f("%d paired-page drag cases retain external CLK/GATE/SLIDE/LEN/DIV/ORDER partner or PROB/ACCUM target/trigger writes through exact Undo/Redo (%d failures)",cases,failures));
for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(false);m->selectedTrack.store(0);screenPage(m,M::HOME);
for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
