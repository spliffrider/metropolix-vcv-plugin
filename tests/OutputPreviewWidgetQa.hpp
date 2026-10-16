using M=Metropolix;
static_assert(M::FORMAT34_NUM_PARAMS==787 && M::NUM_PARAMS==789 && M::SNAPSHOT_PARAMS==522 && M::PRESET_FORMAT==12,"output previews add no saved fields");
APP->engine->bypassModule(m,true);
auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
auto original=raw();m->params[M::RUN].setValue(0);
auto* encoder=find("Encoder");auto* display=find("Metropolix display");
auto open=[&](int n,int field,int source){screenPage(m,n?M::B_PAGE:M::A_PAGE);m->uiAlt.store(false);m->uiMod.store(false);m->uiField.store(field);m->params[M::OUTPUT_SOURCE_A+n].setValue(source);};
auto lastAction=[&]()->history::Action*{return APP->history->actions.empty()?nullptr:APP->history->actions.back();};
auto exactUndo=[&](history::Action* previous,const std::vector<float>& before,const std::vector<float>& after){if(before==after)return lastAction()==previous;auto* action=lastAction();auto& actions=APP->history->actions;bool one=previous?(actions.size()>1 && actions[actions.size()-2]==previous):actions.size()==1;bool ok=one && action && action!=previous && (dynamic_cast<history::ComplexAction*>(action) || dynamic_cast<history::ParamChange*>(action));APP->history->undo();ok &= raw()==before;APP->history->redo();return ok && raw()==after;};
auto only=[&](const std::vector<float>& before,int allowed){for(int id=0;id<M::NUM_PARAMS;++id)if(id!=allowed && before[id]!=m->params[id].getValue())return false;return true;};
widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;move.mouseDelta=Vec(0,-22);
event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
auto hold=[&](double seconds){press(encoder,GLFW_MOUSE_BUTTON_LEFT);encoder->onDragStart(start);encoder->heldSince=system::getTime()-seconds;encoder->step();bool fired=encoder->longFired;encoder->onDragEnd(end);return fired;};
auto menuItem=[&](const std::string& label)->ui::MenuItem*{for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if((*it)->visible && dynamic_cast<ui::MenuOverlay*>(*it))return dynamic_cast<ui::MenuItem*>(seek(*it,[&](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text==label;}));return nullptr;};
bool preview=true,confirmation=true,history=true,cancel=true,conflict=true,refresh=true,continued=true,unrelated=true;
for(int setting:{0,1})for(int n=0;n<2;++n) {
    int id=M::OUTPUT_SOURCE_A+n;m->params[M::ENC_CONFIRM].setValue(setting);
    for(int kind=0;kind<metro::OutputKindCount;++kind) {
        int neighbor=kind?kind-1:1;open(n,0,metro::outputSource(neighbor));auto before=raw();auto* previous=lastAction();
        auto* c=kind%2?display:encoder;drag(c,-22.f*(kind-neighbor));
        preview &= raw()==before && lastAction()==previous && shownOutputSource(m)==metro::outputSource(kind) && m->outputDraft[n].load()>=0;
        preview &= c->navigationValue && c->navigationValue().find("Press encoder to confirm")!=std::string::npos;
        click(encoder);confirmation &= m->integer(id)==metro::outputSource(kind) && m->outputDraft[n].load()<0 && m->uiField.load()==(metro::outputTargetCount(kind)?1:0);
        history &= exactUndo(previous,before,raw()) && only(before,id);
        int count=metro::outputTargetCount(kind);
        if(!count){open(n,1,metro::outputSource(kind));before=raw();previous=lastAction();drag(display,-220);click(encoder);confirmation &= raw()==before && lastAction()==previous && m->uiField.load()==0;}
        for(int target=0;target<count;++target) {
            int origin=(target+1)%count;open(n,1,metro::outputSource(kind,origin));before=raw();previous=lastAction();drag(target%2?display:encoder,-22.f*(target-origin));
            preview &= raw()==before && shownOutputSource(m)==metro::outputSource(kind,target) && lastAction()==previous;
            click(encoder);confirmation &= m->integer(id)==metro::outputSource(kind,target) && m->uiField.load()==0 && m->outputDraft[n].load()<0;
            history &= exactUndo(previous,before,raw()) && only(before,id);
        }
    }
    for(int source=0;source<metro::OutputSourceCount;++source) {
        open(n,0,source);auto selection=metro::outputSelection(source);int turn=selection.kind==metro::OutputKindCount-1?-1:1;
        auto before=raw();auto* previous=lastAction();drag(display,-22.f*turn);click(find("EXIT"));cancel &= raw()==before && m->outputDraft[n].load()<0 && lastAction()==previous;
        open(n,0,source);drag(encoder,-22.f*turn);int external=(source+1)%metro::OutputSourceCount;m->getParamQuantity(id)->setValue(external);before=raw();previous=lastAction();int field=m->uiField.load();click(encoder);
        conflict &= raw()==before && lastAction()==previous && m->outputDraft[n].load()<0 && m->uiField.load()==field;
        open(n,0,source);drag(display,-22.f*turn);m->getParamQuantity(id)->setValue(external);before=raw();previous=lastAction();display->step();refresh &= raw()==before && lastAction()==previous && m->outputDraft[n].load()<0;
        open(n,0,source);encoder->onDragStart(start);move.mouseDelta=Vec(0,-22.f*turn);encoder->onDragMove(move);m->getParamQuantity(id)->setValue(external);before=raw();previous=lastAction();encoder->onDragEnd(end);
        conflict &= raw()==before && lastAction()==previous && !APP->window->isCursorLocked();click(encoder);conflict &= raw()==before && lastAction()==previous && m->outputDraft[n].load()<0;
        int restart=source==29?0:29,next=source==29?14:31;
        open(n,0,source);encoder->onDragStart(start);encoder->onDragMove(move);m->getParamQuantity(id)->setValue(restart);move.mouseDelta=Vec(0,-22);encoder->onDragMove(move);encoder->onDragEnd(end);before=raw();previous=lastAction();
        continued &= m->integer(id)==restart && m->outputDraft[n].load()==next && !APP->window->isCursorLocked();click(encoder);continued &= m->integer(id)==next && exactUndo(previous,before,raw());
        open(n,0,source);drag(encoder,-22.f*turn);int expected=shownOutputSource(m),other=M::OUTPUT_SOURCE_A+1-n;m->getParamQuantity(other)->setValue(37);before=raw();previous=lastAction();click(encoder);
        unrelated &= m->integer(id)==expected && m->integer(other)==37 && exactUndo(previous,before,raw()) && only(before,id);
    }
}
check(preview && confirmation,"A/B preview and mandatory confirmation cover all sixteen functions and all 38 source/target choices with ENC_CONFIRM off/on");
check(history,"encoder/display confirmations restore exact raw state with one undo and preserve unrelated parameters");
check(cancel,"EXIT discards every pending A/B source without changing parameters or history");
check(conflict && refresh,"external edits across every A/B source survive confirmation, active-drag release and control refresh without false history");
check(continued && unrelated,"continued turns restart from the external source while unrelated output changes retain valid previews");
bool menus=true,resets=true,noops=true;
for(int n=0;n<2;++n) {
    int id=M::OUTPUT_SOURCE_A+n;
    for(int kind=0;kind<metro::OutputKindCount;++kind)for(int target=0;target<std::max(1,metro::outputTargetCount(kind));++target) {
        int source=metro::outputSource(kind,target);open(n,0,(source+1)%metro::OutputSourceCount);drag(display,-22);auto before=raw();auto* previous=lastAction();press(encoder,GLFW_MOUSE_BUTTON_RIGHT);
        auto* item=menuItem(metro::outputKindName(kind));auto* child=item && metro::outputTargetCount(kind)?item->createChildMenu():nullptr;
        if(child){std::string label=std::string(metro::outputIsMod(kind)?"MOD ":"Track ")+std::to_string(target+1);item=nullptr;for(auto* w:child->children)if(auto* candidate=dynamic_cast<ui::MenuItem*>(w))if(candidate->text==label)item=candidate;}
        menus &= item && !item->disabled;if(item){event::Action action;item->onAction(action);}delete child;closeMenus();
        menus &= m->integer(id)==source && m->outputDraft[n].load()<0 && exactUndo(previous,before,raw()) && only(before,id);
    }
    for(int field=0;field<2;++field) {
        open(n,field,37);auto before=raw();auto* previous=lastAction();noops &= !hold(.9) && raw()==before && lastAction()==previous;
        open(n,field,37);drag(display,22);before=raw();previous=lastAction();resets &= hold(1.1) && m->integer(id)==(n?14:0) && m->uiField.load()==0 && m->outputDraft[n].load()<0 && exactUndo(previous,before,raw()) && only(before,id);
        before=raw();previous=lastAction();noops &= hold(1.1) && raw()==before && lastAction()==previous;
    }
    for(int source:{10,metro::OutputSourceCount-1}){open(n,0,source);auto before=raw();auto* previous=lastAction();drag(display,source==10?22:-22);click(encoder);noops &= raw()==before && lastAction()==previous && m->outputDraft[n].load()<0;}
    open(n,0,0);drag(display,-22);m->params[id].setValue(.25f);auto before=raw();auto* previous=lastAction();click(encoder);conflict &= raw()==before && m->outputDraft[n].load()<0 && lastAction()==previous;
    open(n,0,0);drag(encoder,-22);before=raw();previous=lastAction();press(display,GLFW_MOUSE_BUTTON_RIGHT);auto* item=menuItem("Discard pending choice");menus &= item!=nullptr;if(item){event::Action action;item->onAction(action);}closeMenus();menus &= raw()==before && m->outputDraft[n].load()<0 && lastAction()==previous;
    open(n,0,37);drag(encoder,22);before=raw();previous=lastAction();press(display,GLFW_MOUSE_BUTTON_RIGHT);item=menuItem("Reset output assignment");menus &= item!=nullptr;if(item){event::Action action;item->onAction(action);}closeMenus();resets &= m->integer(id)==(n?14:0) && m->outputDraft[n].load()<0 && exactUndo(previous,before,raw());
}
check(menus,"all named A/B functions and targets apply through real menus, discard pending choices and undo exactly");
check(resets && noops,"A/B hold thresholds, long/menu resets, clipped choices and unchanged actions retain correct single/no-op history");
check(conflict,"output preview validity compares the raw applied source, including changes within one rounded source");
for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);screenPage(m,M::HOME);m->uiAlt.store(false);APP->engine->bypassModule(m,false);
