using M=Metropolix;APP->engine->bypassModule(m,true);
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->actionIndex=0;APP->history->savedIndex=0;
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};auto original=raw();
for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->uiMod.store(false);m->selectedTrack.store(0);m->params[M::cvTypeParam(0)].setValue(metro::UnipolarCv);
ValueCell* cell=nullptr;for(auto* w:editor->children)if(auto* c=dynamic_cast<ValueCell*>(w))if(!c->compact && c->stageRow==M::CV && c->stageColumn==0)cell=c;
check(cell!=nullptr,"micro-CV probe finds actual stage grid");if(cell){cell->step();int id=cell->paramId;widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
    m->params[id].setValue(-3.75f);auto before=raw();auto* prior=APP->history->actions.empty()?nullptr:APP->history->actions.back();cell->onDragStart(start);move.mouseDelta=Vec(0,-2);cell->onDragMove(move);cell->onDragEnd(end);auto after=raw();int n=APP->history->actionIndex;bool one=prior?(n>1 && APP->history->actions[n-2]==prior):n==1;bool exact=after[id]>0 && one;APP->history->undo();exact &= raw()==before;APP->history->redo();exact &= raw()==after;
    check(exact,"two-pixel Unipolar CV edit undoes to its dormant negative raw value exactly");
    m->params[id].setValue(-3.75f);before=raw();prior=APP->history->actions.empty()?nullptr:APP->history->actions.back();cell->onDragStart(start);move.mouseDelta=Vec(0,0);cell->onDragMove(move);cell->onDragEnd(end);check(raw()==before && (APP->history->actions.empty()?nullptr:APP->history->actions.back())==prior,"zero-delta grid movement preserves dormant CV and adds no history");
    m->params[M::cvTypeParam(0)].setValue(metro::GateCv);m->params[id].setValue(-3.75f);before=raw();prior=APP->history->actions.empty()?nullptr:APP->history->actions.back();cell->onDragStart(start);cell->onDragEnd(end);after=raw();n=APP->history->actionIndex;one=prior?(n>1 && APP->history->actions[n-2]==prior):n==1;exact=after[id]==5 && one;APP->history->undo();exact &= raw()==before;APP->history->redo();exact &= raw()==after;
    check(exact,"Gate CV grid click undoes to dormant negative raw value exactly");
    m->uiMod.store(false);m->uiAlt.store(false);m->selectedStage.store(0);m->selectedRow.store(M::CV);m->params[M::ENC_CONFIRM].setValue(0);screenPage(m,M::STAGE_PAGE);auto* encoder=find("Encoder");m->params[id].setValue(-3.75f);before=raw();prior=APP->history->actions.empty()?nullptr:APP->history->actions.back();encoder->onDragStart(start);encoder->onDragEnd(end);after=raw();n=APP->history->actionIndex;one=prior?(n>1 && APP->history->actions[n-2]==prior):n==1;exact=after[id]==5 && one;APP->history->undo();exact &= raw()==before;APP->history->redo();exact &= raw()==after;
    check(exact,"Gate CV encoder click undoes to dormant negative raw value exactly");
    m->params[M::cvTypeParam(0)].setValue(metro::UnipolarCv);m->params[M::cvRampParam(0,0)].setValue(0);m->params[id].setValue(-3.75f);before=raw();prior=APP->history->actions.empty()?nullptr:APP->history->actions.back();encoder->onDragStart(start);move.mouseDelta=Vec(0,-2);encoder->onDragMove(move);encoder->onDragEnd(end);after=raw();n=APP->history->actionIndex;bool two=prior?(n>2 && APP->history->actions[n-3]==prior):n==2;exact=after[id]>0 && after[M::cvRampParam(0,0)]==1 && two;auto valueOnly=after;valueOnly[M::cvRampParam(0,0)]=0;APP->history->undo();exact &= raw()==valueOnly;APP->history->undo();exact &= raw()==before;APP->history->redo();APP->history->redo();exact &= raw()==after;
    check(exact,"two-pixel encoder CV edit retains edit-before-transition history order and exact dormant raw undo");
}
auto clean=[&](int track,int stage,int mode,bool alt=false){
    for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->params[M::ENC_CONFIRM].setValue(0);m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(alt);m->selectedTrack.store(track);m->selectedStage.store(stage);m->selectedRow.store(M::CV);screenPage(m,M::STAGE_PAGE);
    m->params[M::cvTypeParam(track)].setValue(mode==3?metro::BipolarCv:mode);m->params[M::outputTrackParam(track,M::VELOCITY_SOURCE)].setValue(mode==3?3:2);
    for(int s=0;s<8;++s){m->params[M::stageParam(track,s,M::CV)].setValue(s%2?1.25f:-3.75f);m->params[M::cvRampParam(track,s)].setValue(0);}
};
auto last=[&]()->history::Action*{int n=APP->history->actionIndex;return n?APP->history->actions[n-1]:nullptr;};
// Verify each genuine edit and click separately, including a click that brings
// a value back to its origin. Rack's bounded history may evict older receipts.
auto sequence=[&](history::Action* prior,const std::vector<float>& before,const std::vector<float>& edited,const std::vector<float>& after){
    bool edit=edited!=before,click=after!=edited;int count=int(edit)+int(click),n=APP->history->actionIndex;
    if(!count)return last()==prior;
    bool order=prior?(n>count && APP->history->actions[n-count-1]==prior):n==count;if(!order)return false;
    if(click){APP->history->undo();order &= raw()==edited;}
    if(edit){APP->history->undo();order &= raw()==before;APP->history->redo();order &= raw()==edited;}
    if(click){APP->history->redo();order &= raw()==after;}return order;
};
widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;
auto released=[&](ValueKnob* c){return !APP->window->isCursorLocked() && !c->adjusting && !c->adjustmentTip;};
bool grid=true,gridZero=true,control=true,relative=true,zero=true,preview=true,bounds=true;int gridCases=0,controlCases=0;
for(int track=0;track<2;++track)for(int stage=0;stage<8;++stage)for(int mode=0;mode<4;++mode){
    ValueCell* c=nullptr;for(auto* w:editor->children)if(auto* v=dynamic_cast<ValueCell*>(w))if(!v->compact && v->stageRow==M::CV && v->stageColumn==stage)c=v;
    if(!c){grid=false;continue;}
    for(float dy:{-.2f,-2.f,2.f,-6.f}){
        clean(track,stage,mode);int id=M::stageParam(track,stage,M::CV);c->step();auto before=raw();auto* prior=last();c->onDragStart(start);move.mouseDelta=Vec(0,dy);c->onDragMove(move);auto edited=raw(),expected=edited;
        if(mode==metro::GateCv && std::abs(dy)<3)expected[id]=m->getParamQuantity(id)->getValue()>=1.5f?0:5;
        for(int i=0;i<M::NUM_PARAMS;++i)if(i!=id)grid &= edited[i]==before[i];
        c->onDragEnd(end);grid &= raw()==expected && sequence(prior,before,edited,raw()) && released(c) && !c->dragStarted;++gridCases;
    }
    clean(track,stage,mode);int id=M::stageParam(track,stage,M::CV);c->step();auto before=raw(),expected=before;auto* prior=last();c->onDragStart(start);move.mouseDelta=Vec(0,0);c->onDragMove(move);gridZero &= raw()==before;
    if(mode==metro::GateCv)expected[id]=m->getParamQuantity(id)->getValue()>=1.5f?0:5;c->onDragEnd(end);gridZero &= raw()==expected && sequence(prior,before,before,raw()) && released(c);
}
check(grid && gridZero,string::f("%d actual grid CV edits across all stages, tracks and types retain exact raw undo, genuine Gate clicks and zero-movement values",gridCases));
for(const std::string label:{std::string("Encoder"),std::string("Metropolix display")})for(int track=0;track<2;++track)for(int stage=0;stage<8;++stage)for(int mode=0;mode<4;++mode)for(bool alt:{false,true}){
    auto* c=find(label);int id=M::stageParam(track,stage,M::CV);
    for(float dy:{-.2f,-2.f,2.f,-6.f}){
        clean(track,stage,mode,alt);auto before=raw();auto* prior=last();c->onDragStart(start);move.mouseDelta=Vec(0,dy);c->onDragMove(move);auto edited=raw(),expected=edited;
        for(int i=0;i<M::NUM_PARAMS;++i){bool allowed=false;for(int s=0;s<8;++s)allowed |= i==M::stageParam(track,s,M::CV) && (alt || s==stage);if(!allowed)control &= edited[i]==before[i];}
        if(alt && mode!=metro::GateCv){auto type=m->effectiveCvType(track);float delta=edited[id]-metro::trackCvVoltage(before[id],type);
            for(int s=0;s<8;++s){int next=M::stageParam(track,s,M::CV);float value=metro::clamp(metro::trackCvVoltage(before[next],type)+delta,type==metro::BipolarCv?-5.f:0.f,5.f);relative &= std::abs(edited[next]-value)<.00001f;}}
        if(std::abs(dy)<3){
            if(mode==metro::GateCv){for(int s=0;s<8;++s)if(alt || s==stage){int next=M::stageParam(track,s,M::CV);expected[next]=metro::trackCvVoltage(edited[next],metro::GateCv)>=1.5f?0:5;}}
            else for(int s=0;s<8;++s)if(alt || s==stage)expected[M::cvRampParam(track,s)]=1;
        }
        c->onDragEnd(end);control &= raw()==expected && sequence(prior,before,edited,raw()) && released(c) && !c->dragging;++controlCases;
    }
    clean(track,stage,mode,alt);auto before=raw(),expected=before;auto* prior=last();c->onDragStart(start);move.mouseDelta=Vec(0,0);c->onDragMove(move);zero &= raw()==before;
    for(int s=0;s<8;++s)if(alt || s==stage){int next=M::stageParam(track,s,M::CV);if(mode==metro::GateCv)expected[next]=metro::trackCvVoltage(before[next],metro::GateCv)>=1.5f?0:5;else expected[M::cvRampParam(track,s)]=1;}
    c->onDragEnd(end);zero &= raw()==expected && sequence(prior,before,before,raw()) && released(c);
    // Tiny confirming movements apply their draft once; they must not also
    // cycle a transition or toggle a Gate after confirming it.
    if(mode!=metro::GateCv)for(float dy:{-.2f,-2.f}){
        clean(track,stage,mode,alt);m->params[M::ENC_CONFIRM].setValue(1);before=raw();prior=last();c->onDragStart(start);move.mouseDelta=Vec(0,dy);c->onDragMove(move);preview &= raw()==before && m->uiDraftParam==id;
        c->onDragEnd(end);auto after=raw();preview &= after!=before && m->uiDraftParam<0 && sequence(prior,before,before,after) && released(c);
        for(int s=0;s<8;++s)preview &= after[M::cvRampParam(track,s)]==0;
    }
    // At the upper bound a tiny movement adds only its genuine click action.
    if(mode!=metro::GateCv){clean(track,stage,mode,alt);for(int s=0;s<8;++s)m->params[M::stageParam(track,s,M::CV)].setValue(5);before=raw();prior=last();c->onDragStart(start);move.mouseDelta=Vec(0,-2);c->onDragMove(move);bounds &= raw()==before;c->onDragEnd(end);auto after=raw();bounds &= sequence(prior,before,before,after) && released(c);}
}
check(control && zero,string::f("%d actual encoder/display CV drags preserve edit-before-click history, ALT scope and zero-delta values",controlCases));
check(relative,"ALT CV drags use the effective sounding range, including saved Bipolar forced to Unipolar by Velocity CV");
check(preview && bounds,"tiny CV confirmation and upper-bound clicks retain only their genuine action, with exact raw undo/redo and released cursor");
for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->cancelDraft();m->uiAlt.store(false);m->uiMod.store(false);m->selectedTrack.store(0);screenPage(m,M::HOME);
for(auto* action:APP->history->actions)delete action;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
