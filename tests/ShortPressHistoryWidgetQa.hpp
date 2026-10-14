// Actual Rack callbacks; processing is paused for exact raw/history receipts.
using M=Metropolix;APP->engine->bypassModule(m,true);
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};auto original=raw();
auto clean=[&]{for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::ENC_CONFIRM].setValue(0);m->uiMod.store(false);m->uiAlt.store(false);m->selectedTrack.store(0);m->selectedLane.store(0);};clean();
auto last=[&]()->history::Action*{int n=APP->history->actionIndex;return n?APP->history->actions[n-1]:nullptr;};
auto exact=[&](history::Action* prior,const std::vector<float>& before,const std::vector<float>& after){int n=APP->history->actionIndex;if(before==after)return last()==prior;bool one=prior?(n>1 && APP->history->actions[n-2]==prior):n==1;if(!one || last()==prior)return false;APP->history->undo();bool ok=raw()==before;APP->history->redo();return ok && raw()==after;};
widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;
for(const std::string label:{std::string("Encoder"),std::string("Metropolix display")}){
    screenPage(m,M::GATE_PAGE);m->uiField.store(0);int id=M::trackParam(0,M::GATE_LENGTH);m->params[id].setValue(50);auto* c=find(label);c->onDragStart(start);m->getParamQuantity(id)->setValue(75);auto before=raw();auto* prior=last();c->onDragEnd(end);
    check(raw()==before && m->uiField.load()==1 && last()==prior && !APP->window->isCursorLocked() && !c->adjustmentTip && !c->adjusting && !c->dragging,label+" short navigation retains an external value without native drag history or captured cursor");
}
ValueCell* gate=nullptr;ValueCell* slide=nullptr;for(auto* w:editor->children)if(auto* c=dynamic_cast<ValueCell*>(w)){if(c->trackProperty==M::GATE_LENGTH)gate=c;if(!c->compact && c->stageRow==M::SLIDE && c->stageColumn==0)slide=c;}
check(gate && slide,"stationary-click probes find actual gate and slide grid cells");
if(gate){screenPage(m,M::GATE_PAGE);int id=M::trackParam(0,M::GATE_LENGTH);m->params[id].setValue(50);gate->step();gate->onDragStart(start);m->getParamQuantity(id)->setValue(75);auto before=raw();auto* prior=last();gate->onDragEnd(end);check(raw()==before && last()==prior && !APP->window->isCursorLocked(),"actual gate grid click retains external write without native drag history");}
if(slide){int id=M::stageParam(0,0,M::SLIDE);m->params[id].setValue(0);slide->step();slide->onDragStart(start);m->params[id].setValue(1);auto before=raw(),expected=before;expected[id]=0;auto* prior=last();slide->onDragEnd(end);auto after=raw();check(after==expected && exact(prior,before,after),"actual slide grid click toggles latest external value with exactly one genuine undo/redo");}

bool shortcuts=true;const char* names[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};const int gateValues[]={1,15,30,45,60,75,90,100};
for(int track=0;track<2;++track)for(int kind=0;kind<3;++kind)for(int stage=0;stage<8;++stage){
    clean();m->selectedTrack.store(track);screenPage(m,kind==2?M::SLIDE_PAGE:M::GATE_PAGE);m->uiField.store(kind==1?1:0);int id=kind==2?M::trackParam(track,M::SLIDE_TIME):kind?M::gateScaleParam(track):M::trackParam(track,M::GATE_LENGTH);m->params[id].setValue(63);
    auto* held=find(kind==2?"SLIDE":"GATE");held->onDragStart(start);auto before=raw(),expected=before;expected[id]=kind==2?metro::slideShortcut(stage):gateValues[stage];auto* prior=last();click(find("Stage "+std::to_string(stage+1)+" / "+names[stage]));held->onDragEnd(end);auto after=raw();shortcuts &= after==expected && exact(prior,before,after) && !APP->window->isCursorLocked();
}
check(shortcuts,"all eight held Gate Length/Gate Scale/Slide shortcuts on both tracks retain one exact undo despite a stationary parent press");
auto* control=new HardwareControl;control->metroModule=m;control->name="Stationary click probe";APP->scene->addChild(control);
auto* cell=new ValueCell;cell->metroModule=m;cell->module=m;APP->scene->addChild(cell);
bool numeric=true,action=true,toggles=true,fine=true,menus=true;int clicks=0;bool bound=true;
control->action=[&]{++clicks;bound &= control->getParamQuantity()!=nullptr;};
for(int destination=1;destination<metro::CtrlDestinationCount;++destination)if(metro::ctrlDirect(static_cast<metro::CtrlDestination>(destination)))for(int track=0;track<2;++track){
    clean();int id=M::ctrlBaseParam(static_cast<metro::CtrlDestination>(destination),track);auto* q=m->getParamQuantity(id);control->binding=[id]{return id;};cell->paramId=id;
    for(int grid=0;grid<2;++grid){q->setValue(q->getMinValue());ValueKnob* c=grid?static_cast<ValueKnob*>(cell):static_cast<ValueKnob*>(control);if(grid)cell->step();c->onDragStart(start);q->setValue(q->getMaxValue());auto before=raw();auto* prior=last();int count=clicks;c->onDragEnd(end);
        numeric &= raw()==before && last()==prior && !APP->window->isCursorLocked() && !c->adjustmentTip && !c->adjusting;
        if(!grid)action &= clicks==count+1 && bound && !control->dragging;else numeric &= !cell->dragStarted;
    }
}
check(numeric,"stationary controls and grid cells preserve external writes across all fourteen direct destination quantities on both tracks without new history");
check(action,"stationary HardwareControl dispatches its genuine action exactly once with its quantity rebound");

// Toggle receipts must originate in the latest value, not mouse-down state.
for(int track=0;track<2;++track)for(int stage=0;stage<8;++stage)for(int row:{M::SLIDE,M::SKIP,M::CV}){
    clean();m->selectedTrack.store(track);m->params[M::cvTypeParam(track)].setValue(metro::GateCv);cell->stageRow=row;cell->stageColumn=stage;cell->step();int id=cell->paramId;m->params[id].setValue(0);cell->onDragStart(start);m->params[id].setValue(row==M::CV?5:1);auto before=raw(),expected=before;expected[id]=0;auto* prior=last();cell->onDragEnd(end);auto after=raw();toggles &= after==expected && exact(prior,before,after) && !APP->window->isCursorLocked();
}
cell->stageRow=-1;cell->stageColumn=-1;
for(int lane=0;lane<8;++lane)for(int stage=0;stage<8;++stage)for(int mode=0;mode<4;++mode){clean();m->selectedLane.store(lane);m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(mode?metro::ModDirection:metro::ModNone);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(mode?mode-1:metro::ModGate);cell->modProperty=M::MOD_VALUE+stage;cell->step();int id=cell->paramId;m->params[id].setValue(0);cell->onDragStart(start);m->params[id].setValue(5);auto before=raw(),expected=before;expected[id]=0;auto* prior=last();cell->onDragEnd(end);auto after=raw();toggles &= after==expected && exact(prior,before,after);}
cell->modProperty=-1;
check(toggles,"Slide/Skip, Gate CV on both tracks and every MOD gate toggle the latest external value with one exact raw undo/redo");

clean();cell->choices={"First choice","Second choice"};cell->paramId=M::trackParam(0,M::ORDER);m->params[cell->paramId].setValue(0);cell->onDragStart(start);m->params[cell->paramId].setValue(1);auto before=raw();auto* prior=last();cell->onDragEnd(end);
auto* item=dynamic_cast<ui::MenuItem*>(seek(APP->scene,[&](widget::Widget* w){auto* c=dynamic_cast<ui::MenuItem*>(w);return c && c->text=="First choice";}));menus &= item && raw()==before && last()==prior;
if(item){event::Action click;item->onAction(click);auto after=raw();menus &= m->integer(cell->paramId)==0 && exact(prior,before,after);}closeMenus();cell->choices.clear();
check(menus,"stationary choice cell still opens its menu and chosen value has one exact action from the external value");

// A real sub-three-pixel continuous edit retains its native drag receipt.
clean();control->action=nullptr;int id=M::ctrlParam(0,M::CTRL_VALUE);control->binding=[id]{return id;};cell->paramId=id;
for(int grid=0;grid<2;++grid){m->params[id].setValue(0);ValueKnob* c=grid?static_cast<ValueKnob*>(cell):static_cast<ValueKnob*>(control);if(grid)cell->step();before=raw();prior=last();c->onDragStart(start);move.mouseDelta=Vec(0,-2);c->onDragMove(move);c->onDragEnd(end);auto after=raw();fine &= after[id]!=0 && exact(prior,before,after) && !APP->window->isCursorLocked();}
check(fine,"actual two-pixel continuous control and grid drags remain editable with exact native undo/redo");
APP->scene->removeChild(control);delete control;APP->scene->removeChild(cell);delete cell;clean();screenPage(m,M::HOME);APP->engine->bypassModule(m,false);
