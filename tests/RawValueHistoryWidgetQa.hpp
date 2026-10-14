using M=Metropolix;APP->engine->bypassModule(m,true);
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->actionIndex=0;APP->history->savedIndex=0;
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};auto original=raw();
auto last=[&]()->history::Action*{int n=APP->history->actionIndex;return n?APP->history->actions[n-1]:nullptr;};
auto exact=[&](history::Action* prior,const std::vector<float>& before,const std::vector<float>& after){if(before==after)return last()==prior;int n=APP->history->actionIndex;bool one=prior?(n>1 && APP->history->actions[n-2]==prior):n==1;if(!one)return false;APP->history->undo();bool ok=raw()==before;APP->history->redo();return ok && raw()==after;};
auto clean=[&]{for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::ENC_CONFIRM].setValue(0);m->cancelDraft();m->uiMod.store(true);m->uiAlt.store(false);m->selectedLane.store(0);m->selectedStage.store(0);screenPage(m,M::STAGE_PAGE);m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModNone);m->params[M::modParam(0,M::MOD_TYPE)].setValue(metro::ModUnipolar);};
widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
ValueCell* grid=nullptr;for(auto* w:modPanel->children)if(auto* c=dynamic_cast<ValueCell*>(w))if(c->modProperty==M::MOD_VALUE)grid=c;
check(grid!=nullptr,"raw MOD probe finds actual grid cell");
if(grid){clean();int id=M::modParam(0,M::MOD_VALUE);m->params[id].setValue(-3.75f);grid->step();auto before=raw();auto* prior=last();grid->onDragStart(start);move.mouseDelta=Vec(0,-22);grid->onDragMove(move);grid->onDragEnd(end);auto after=raw();check(after[id]>0 && exact(prior,before,after),"Unipolar MOD grid drag undoes to exact dormant negative raw voltage");}
auto* encoder=find("Encoder");clean();int id=M::modParam(0,M::MOD_VALUE);m->params[id].setValue(-3.75f);auto before=raw();auto* prior=last();encoder->onDragStart(start);move.mouseDelta=Vec(0,-22);encoder->onDragMove(move);encoder->onDragEnd(end);auto after=raw();check(after[id]>0 && exact(prior,before,after),"Unipolar MOD encoder drag undoes to exact dormant negative raw voltage");
clean();m->uiAlt.store(true);for(int s=0;s<8;++s)m->params[M::modParam(0,M::MOD_VALUE+s)].setValue(s%2?1.25f:-3.75f);before=raw();encoder->onDragStart(start);encoder->onDragMove(move);after=raw();float delta=after[id];bool relative=true;for(int s=0;s<8;++s){int next=M::modParam(0,M::MOD_VALUE+s);relative &= std::abs(after[next]-metro::clamp(std::max(0.f,before[next])+delta,0.f,5.f))<.00001f;}encoder->onDragEnd(end);check(relative,"ALT Unipolar MOD offsets use every stage's effective voltage without a raw-negative jump");
clean();m->uiMod.store(false);screenPage(m,M::CLOCK_OUT_PAGE);m->uiField.store(1);m->params[M::CLOCK_OUT_DIV].setValue(4);m->params[M::CLOCK_OUT_OFFSET].setValue(19);before=raw();prior=last();encoder->onDragStart(start);move.mouseDelta=Vec(0,-2);encoder->onDragMove(move);encoder->onDragEnd(end);after=raw();check(after[M::CLOCK_OUT_OFFSET]==3 && m->uiField.load()==0 && exact(prior,before,after),"tiny CLK offset edit retains its dormant raw offset in Undo before normal field navigation");
auto setup=[&](int lane,int stage,int type,int destination,bool alt){clean();m->selectedLane.store(lane);m->selectedStage.store(stage);m->uiAlt.store(alt);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(type);m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(destination);for(int s=0;s<8;++s){m->params[M::modParam(lane,M::MOD_VALUE+s)].setValue(s%2?1.25f:-3.75f);m->params[M::modParam(lane,M::MOD_RAMP+s)].setValue(0);}};
auto sequence=[&](history::Action* previous,const std::vector<float>& origin,const std::vector<float>& edited,const std::vector<float>& result){
    bool edit=edited!=origin,click=result!=edited;int count=int(edit)+int(click),index=APP->history->actionIndex;if(!count)return last()==previous;
    bool ok=previous?(index>count && APP->history->actions[index-count-1]==previous):index==count;if(!ok)return false;
    if(click){APP->history->undo();ok &= raw()==edited;}if(edit){APP->history->undo();ok &= raw()==origin;APP->history->redo();ok &= raw()==edited;}if(click){APP->history->redo();ok &= raw()==result;}return ok;
};
auto released=[&](ValueKnob* c){auto* hardware=dynamic_cast<HardwareControl*>(c);auto* cell=dynamic_cast<ValueCell*>(c);return !APP->window->isCursorLocked() && !c->adjusting && !c->adjustmentTip && (!hardware || !hardware->dragging) && (!cell || !cell->dragStarted);};
std::array<ValueCell*,8> cells{};for(auto* w:modPanel->children)if(auto* c=dynamic_cast<ValueCell*>(w))if(c->modProperty>=M::MOD_VALUE && c->modProperty<M::MOD_VALUE+8)cells[c->modProperty-M::MOD_VALUE]=c;
auto* display=find("Metropolix display");bool core=true,offsets=true,zero=true,confirmation=true,clock=true;int edits=0,clockEdits=0;
auto probe=[&](ValueKnob* c,int lane,int stage,int type,int destination,bool alt,float dy){
    setup(lane,stage,type,destination,alt);bool panel=dynamic_cast<HardwareControl*>(c)!=nullptr;if(auto* cell=dynamic_cast<ValueCell*>(c))cell->step();int selected=M::modParam(lane,M::MOD_VALUE+stage);auto origin=raw();auto* previous=last();c->onDragStart(start);move.mouseDelta=Vec(0,dy);c->onDragMove(move);auto edited=raw(),expected=edited;
    bool gate=(destination==metro::ModNone && type==metro::ModGate) || metro::modGateDestination(static_cast<metro::ModDestination>(destination));
    for(int i=0;i<M::NUM_PARAMS;++i){bool allowed=false;for(int s=0;s<8;++s)allowed |= i==M::modParam(lane,M::MOD_VALUE+s) && (s==stage || (panel && alt));if(!allowed)core &= edited[i]==origin[i];}
    if(dy==0)zero &= edited==origin;
    if(dy!=0 && panel && alt && !gate){float minimum=destination==metro::ModNone && type==metro::ModUnipolar?0.f:-5.f;float delta=edited[selected]-metro::clamp(origin[selected],minimum,5.f);for(int s=0;s<8;++s){int next=M::modParam(lane,M::MOD_VALUE+s);offsets &= std::abs(edited[next]-metro::clamp(metro::clamp(origin[next],minimum,5.f)+delta,minimum,5.f))<.00001f;}}
    if(std::abs(dy)<3){for(int s=0;s<8;++s)if(s==stage || (panel && alt)){int next=M::modParam(lane,M::MOD_VALUE+s);if(gate)expected[next]=edited[next]>=1.5f?0:5;else if(panel)expected[M::modParam(lane,M::MOD_RAMP+s)]=1;}}
    c->onDragEnd(end);core &= raw()==expected && sequence(previous,origin,edited,raw()) && released(c);++edits;
};
for(int lane=0;lane<8;++lane)for(int stage=0;stage<8;++stage)for(int type=0;type<3;++type){
    if(!cells[stage]){core=false;continue;}
    for(float dy:{-.2f,-2.f,2.f,-22.f,0.f})probe(cells[stage],lane,stage,type,metro::ModNone,false,dy);
    for(ValueKnob* c:{static_cast<ValueKnob*>(encoder),static_cast<ValueKnob*>(display)})for(bool alt:{false,true})for(float dy:{-.2f,-2.f,2.f,-22.f,0.f})probe(c,lane,stage,type,metro::ModNone,alt,dy);
}
for(int destination=1;destination<metro::ModDestinationCount;++destination)for(int type=0;type<3;++type)for(ValueKnob* c:{static_cast<ValueKnob*>(encoder),static_cast<ValueKnob*>(display)})for(bool alt:{false,true})for(float dy:{-2.f,-22.f})probe(c,destination%8,(destination/8)%8,type,destination,alt,dy);
check(core && zero,string::f("%d actual MOD drags/clicks cover every lane, stage, type and destination with exact raw edit-before-click Undo/Redo, ALT scope and zero-delta preservation",edits));
check(offsets,"ALT MOD relative voltage uses effective origins and per-stage limits without hidden-negative jumps");
// Confirmation covers every lane, both controllers and both ALT scopes; the
// pure preview must not acquire a false native receipt before its real apply.
for(int lane=0;lane<8;++lane)for(int type=0;type<2;++type)for(ValueKnob* c:{static_cast<ValueKnob*>(encoder),static_cast<ValueKnob*>(display)})for(bool alt:{false,true})for(float dy:{-.2f,-2.f,-22.f}){
    setup(lane,lane,type,metro::ModNone,alt);m->params[M::ENC_CONFIRM].setValue(1);auto origin=raw();auto* previous=last();c->onDragStart(start);move.mouseDelta=Vec(0,dy);c->onDragMove(move);confirmation &= raw()==origin && m->uiDraftParam==M::modParam(lane,M::MOD_VALUE+lane);c->onDragEnd(end);
    if(std::abs(dy)>=3){confirmation &= raw()==origin && last()==previous;c->onDragStart(start);c->onDragEnd(end);}
    auto result=raw();confirmation &= result!=origin && m->uiDraftParam<0 && sequence(previous,origin,origin,result) && released(c);for(int s=0;s<8;++s)confirmation &= result[M::modParam(lane,M::MOD_RAMP+s)]==0;
}
check(confirmation,"MOD tiny confirmations and larger pure previews apply once with exact raw undo and no extra transition click");
for(int division=1;division<=256;++division)for(auto* c:{encoder,display})for(float dy:{-2.f,2.f,-22.f}){
    clean();m->uiMod.store(false);screenPage(m,M::CLOCK_OUT_PAGE);m->uiField.store(1);m->params[M::CLOCK_OUT_DIV].setValue(division);m->params[M::CLOCK_OUT_OFFSET].setValue(255);auto origin=raw();auto* previous=last();c->onDragStart(start);move.mouseDelta=Vec(0,dy);c->onDragMove(move);auto edited=raw();for(int i=0;i<M::NUM_PARAMS;++i)if(i!=M::CLOCK_OUT_OFFSET)clock &= edited[i]==origin[i];c->onDragEnd(end);clock &= raw()==edited && sequence(previous,origin,edited,raw()) && released(c) && m->uiField.load()==(std::abs(dy)<3?0:1);++clockEdits;
    if(division==1)clock &= raw()==origin;else clock &= edited[M::CLOCK_OUT_OFFSET]>=0 && edited[M::CLOCK_OUT_OFFSET]<division;
}
check(clock,string::f("%d actual CLK offset drags across divisions 1–256 retain dormant raw offsets, normal field navigation, disabled /1 and exact Undo/Redo",clockEdits));
bool clockConfirm=true;
for(int division:{1,2,4,16,256})for(auto* c:{encoder,display}){
    clean();m->uiMod.store(false);screenPage(m,M::CLOCK_OUT_PAGE);m->uiField.store(1);m->params[M::CLOCK_OUT_DIV].setValue(division);m->params[M::CLOCK_OUT_OFFSET].setValue(255);m->params[M::ENC_CONFIRM].setValue(1);auto origin=raw();auto* previous=last();c->onDragStart(start);move.mouseDelta=Vec(0,22);c->onDragMove(move);c->onDragEnd(end);clockConfirm &= raw()==origin && last()==previous;
    if(division>1){clockConfirm &= m->uiDraftParam==M::CLOCK_OUT_OFFSET;c->onDragStart(start);c->onDragEnd(end);clockConfirm &= exact(previous,origin,raw());}else clockConfirm &= m->uiDraftParam<0;
}
check(clockConfirm,"CLK confirming previews preserve dormant offsets until one genuine apply, including /1 disabled and /256 limits");
for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(false);screenPage(m,M::HOME);
for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
