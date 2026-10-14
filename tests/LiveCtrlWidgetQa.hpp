using M=Metropolix;APP->engine->bypassModule(m,true);
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};auto original=raw();
auto lastAction=[&]()->history::Action*{return APP->history->actions.empty()?nullptr:APP->history->actions.back();};
widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;move.mouseDelta=Vec(0,-22);
event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
auto gate=[&]{for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::ENC_CONFIRM].setValue(0);m->selectedTrack.store(0);m->uiMod.store(false);m->uiAlt.store(false);screenPage(m,M::GATE_PAGE);m->uiField.store(0);m->params[M::trackParam(0,M::GATE_LENGTH)].setValue(50);};
auto own=[&]{m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlGateLength);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);m->params[M::trackParam(0,M::GATE_LENGTH)].setValue(75);};
for(const std::string label:{std::string("Encoder"),std::string("Metropolix display")}) {
    gate();auto* c=find(label);c->onDragStart(start);own();auto before=raw();auto* previous=lastAction();c->onDragMove(move);
    check(raw()==before,"new CTRL ownership blocks further live "+label+" movement");c->onDragEnd(end);
    check(raw()==before && lastAction()==previous && !APP->window->isCursorLocked() && !c->adjustmentTip,"CTRL takeover cancels live "+label+" release without attributing controller writes to its history");
}
gate();ValueCell* grid=nullptr;for(auto* w:editor->children)if(auto* cell=dynamic_cast<ValueCell*>(w))if(cell->trackProperty==M::GATE_LENGTH)grid=cell;
check(grid!=nullptr,"optional grid exposes track gate length");if(grid){grid->step();grid->onDragStart(start);own();auto before=raw();auto* previous=lastAction();grid->onDragMove(move);grid->onDragEnd(end);check(raw()==before && lastAction()==previous && !APP->window->isCursorLocked(),"CTRL takeover releases grid gesture without attributing controller writes to native history");}
// Exercise every direct destination with real Rack control/cell callbacks.
// The probes bind the same quantities as the panel; the visible encoder/OLED
// and actual gate grid have been tested above through their normal bindings.
auto* control=new HardwareControl;control->metroModule=m;control->name="CTRL ownership probe";APP->scene->addChild(control);
auto* cell=new ValueCell;cell->metroModule=m;cell->module=m;APP->scene->addChild(cell);
bool panelCancel=true,gridCancel=true,latched=true,fresh=true,blocked=true;
auto exactUndo=[&](history::Action* previous,const std::vector<float>& before,const std::vector<float>& after){auto& a=APP->history->actions;bool one=previous?(a.size()>1 && a[a.size()-2]==previous):a.size()==1;bool ok=one && before!=after;APP->history->undo();ok &= raw()==before;APP->history->redo();return ok && raw()==after;};
auto clear=[&]{for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);};
for(int d=1;d<metro::CtrlDestinationCount;++d)if(metro::ctrlDirect(static_cast<metro::CtrlDestination>(d)))for(int knob=0;knob<2;++knob)for(int target=0;target<3;++target)for(int track=0;track<2;++track) {
    auto destination=static_cast<metro::CtrlDestination>(d);int id=M::ctrlBaseParam(destination,track),route=M::ctrlParam(knob,M::CTRL_DESTINATION),assignment=M::ctrlParam(knob,M::CTRL_ASSIGNMENT);
    if(target!=2 && target!=track && destination!=metro::CtrlTempo)continue;
    auto* q=m->getParamQuantity(id);control->binding=[id]{return id;};cell->paramId=id;
    auto assign=[&]{m->params[route].setValue(d);m->params[assignment].setValue(target);m->params[id].setValue(metro::ctrlDirectValue(destination,.5f));};
    for(int phase=0;phase<4;++phase)for(int grid=0;grid<2;++grid) {
        clear();q->setValue(q->defaultValue);ValueKnob* c=grid?static_cast<ValueKnob*>(cell):static_cast<ValueKnob*>(control);if(grid)cell->step();c->onDragStart(start);
        if(phase==3){move.mouseDelta=Vec(0,-1000);c->onDragMove(move);} // Earlier live movement becomes obsolete on takeover.
        assign();auto before=raw();auto* previous=lastAction();move.mouseDelta=Vec(0,q->getValue()>q->getMinValue()?1000:-1000);
        if(phase==0)c->onDragMove(move);
        if(phase==1 || phase==3){c->step();latched &= !c->adjustmentTip;clear();before=raw();c->onDragMove(move);}
        c->onDragEnd(end);bool ok=raw()==before && lastAction()==previous && !APP->window->isCursorLocked() && !c->adjustmentTip;
        if(grid)gridCancel &= ok && !cell->dragStarted;else panelCancel &= ok && !control->dragging;
        clear();before=raw();previous=lastAction();if(grid)cell->step();c->onDragStart(start);c->onDragMove(move);c->onDragEnd(end);fresh &= exactUndo(previous,before,raw()) && !APP->window->isCursorLocked();
        // Starting already owned must remain blocked if the route clears mid-hold.
        assign();if(grid)cell->step();c->onDragStart(start);clear();before=raw();previous=lastAction();c->onDragMove(move);c->onDragEnd(end);
        blocked &= raw()==before && lastAction()==previous && !APP->window->isCursorLocked();
    }
}
bool scope=true;
for(int d=1;d<metro::CtrlDestinationCount;++d)if(metro::ctrlDirect(static_cast<metro::CtrlDestination>(d)))for(int track=0;track<2;++track)for(int offset=0;offset<2;++offset) {
    auto destination=static_cast<metro::CtrlDestination>(d);if(!offset && destination==metro::CtrlTempo)continue;clear();int id=M::ctrlBaseParam(destination,track);auto* q=m->getParamQuantity(id);control->binding=[id]{return id;};q->setValue(q->getMinValue());control->onDragStart(start);
    m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(offset?metro::CtrlProbability:d);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(offset?track:1-track);
    if(!offset)m->params[M::ctrlBaseParam(destination,1-track)].setValue(metro::ctrlDirectValue(destination,.5f));
    auto before=raw();auto* previous=lastAction();move.mouseDelta=Vec(0,-1000);control->onDragMove(move);control->onDragEnd(end);scope &= m->ctrlOwner(id)<0 && exactUndo(previous,before,raw()) && !APP->window->isCursorLocked();
}
APP->scene->removeChild(control);delete control;APP->scene->removeChild(cell);delete cell;
check(panelCancel && gridCancel,"all fourteen direct CTRL destinations, both knobs and all track scopes cancel encoder-class/grid edits on movement, refresh or release without controller-derived undo");
check(latched && blocked,"CTRL takeover stays cancelled after clearing the route until release, including gestures begun already owned; adjustment tips and cursor capture clear");
check(fresh,"fresh gestures after unassignment regain editing and exact single-action raw-state undo/redo for every direct destination");
check(scope,"offset routes and direct assignments to another track retain live editing and exact undo; global Tempo ownership stays global");
// A pre-existing owned field still permits short navigation and whole-page
// resets of its other, unowned field; a newly acquired owner cancels the hold.
gate();own();m->params[M::gateScaleParam(0)].setValue(63);click(find("Encoder"));check(m->uiField.load()==1,"short encoder navigation from an already owned field remains available");m->uiField.store(0);
auto before=raw();auto* previous=lastAction();auto* encoder=find("Encoder");encoder->onDragStart(start);encoder->heldSince=system::getTime()-1.1;encoder->step();encoder->onDragEnd(end);
check(m->integer(M::trackParam(0,M::GATE_LENGTH))==75 && m->integer(M::gateScaleParam(0))==100 && exactUndo(previous,before,raw()),"long reset from an already owned Gate Length preserves it and undoes the unowned Scale reset once");
gate();m->params[M::gateScaleParam(0)].setValue(63);encoder->onDragStart(start);own();before=raw();previous=lastAction();encoder->heldSince=system::getTime()-1.1;encoder->step();encoder->onDragEnd(end);
check(raw()==before && lastAction()==previous && !encoder->longFired && !APP->window->isCursorLocked(),"new CTRL takeover during an encoder hold prevents long reset and its short action/history");
gate();m->params[M::ENC_CONFIRM].setValue(1);encoder->onDragStart(start);move.mouseDelta=Vec(0,-22);encoder->onDragMove(move);check(m->uiDraftParam==M::trackParam(0,M::GATE_LENGTH),"prepare confirming gate draft before ownership takeover");
m->params[M::ctrlParam(0,M::CTRL_DESTINATION)].setValue(metro::CtrlGateLength);m->params[M::ctrlParam(0,M::CTRL_ASSIGNMENT)].setValue(0);encoder->onDragMove(move);clear();before=raw();previous=lastAction();encoder->onDragEnd(end);
check(m->uiDraftParam<0 && raw()==before && lastAction()==previous,"observed CTRL takeover cancels matching confirmation even if the controller leaves its raw value unchanged and is cleared before release");
for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);screenPage(m,M::HOME);m->uiAlt.store(false);APP->engine->bypassModule(m,false);
