using M=Metropolix;APP->engine->bypassModule(m,true);
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->actionIndex=APP->history->savedIndex=0;
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};auto original=raw();
auto dismiss=[&]{APP->event->setSelectedWidget(nullptr);std::vector<widget::Widget*> menus;for(auto* w:APP->scene->children)if(dynamic_cast<ui::MenuOverlay*>(w))menus.push_back(w);for(auto* w:menus){APP->scene->removeChild(w);delete w;}};
auto clean=[&]{dismiss();for(auto* a:APP->history->actions)delete a;APP->history->actions.clear();APP->history->actionIndex=APP->history->savedIndex=0;for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::ENC_CONFIRM].setValue(0);m->cancelDraft();m->uiAlt.store(false);m->uiChooseFunction.store(false);m->selectedRow.store(M::CV);screenPage(m,M::STAGE_PAGE);};
std::array<ValueCell*,8> tracks{},mods{};ValueCell* typeCell=nullptr;ValueCell* assignment=nullptr;
for(auto* w:editor->children)if(auto* c=dynamic_cast<ValueCell*>(w))if(!c->compact && c->stageRow==M::CV && c->stageColumn>=0)tracks[c->stageColumn]=c;
for(auto* w:modPanel->children)if(auto* c=dynamic_cast<ValueCell*>(w)){if(c->modProperty>=M::MOD_VALUE && c->modProperty<M::MOD_VALUE+8)mods[c->modProperty-M::MOD_VALUE]=c;if(c->modProperty==M::MOD_TYPE)typeCell=c;if(c->modProperty==M::MOD_ASSIGNMENT)assignment=c;}
bool cells=typeCell && assignment;for(int s=0;s<8;++s)cells &= tracks[s] && mods[s];check(cells,"native value matrix finds all stage/type/assignment controls");
auto overlay=[&]()->widget::Widget*{for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if(dynamic_cast<ui::MenuOverlay*>(*it))return *it;return nullptr;};
widget::EventContext context;
auto open=[&](widget::Widget* c){if(auto* cell=dynamic_cast<ValueCell*>(c))cell->step();event::Button b;b.context=&context;b.button=GLFW_MOUSE_BUTTON_RIGHT;b.action=GLFW_PRESS;b.mods=0;c->onButton(b);return overlay();};
auto init=[&](widget::Widget* menu){auto* item=menu?dynamic_cast<ui::MenuItem*>(seek(menu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Initialize";})):nullptr;if(item && !item->disabled){event::Action e;item->onAction(e);return true;}return false;};
auto enter=[&](widget::Widget* menu,const std::string& text){auto* f=menu?dynamic_cast<ui::TextField*>(seek(menu,[](widget::Widget* w){return dynamic_cast<ui::TextField*>(w)!=nullptr;})):nullptr;if(!f)return false;f->text=text;event::SelectKey e;e.context=&context;e.key=GLFW_KEY_ENTER;e.action=GLFW_PRESS;e.mods=0;f->onSelectKey(e);return true;};
bool values=true,locked=true;int valueCases=0,lockCases=0;
if(cells){
    for(bool mod:{false,true})for(int target=0;target<(mod?8:2);++target)for(int stage=0;stage<8;++stage)for(int saved=0;saved<3;++saved)for(float stored:{-3.75f,.75f,2.25f})for(int action=0;action<7;++action){
        clean();m->uiMod.store(mod);m->selectedTrack.store(target%2);m->selectedLane.store(target);m->selectedStage.store(stage);int id=mod?M::modParam(target,M::MOD_VALUE+stage):M::stageParam(target,stage,M::CV);m->params[id].setValue(stored);m->params[mod?M::modParam(target,M::MOD_TYPE):M::cvTypeParam(target)].setValue(saved);
        auto destination=(stage+target)%3==0?metro::ModNone:(stage+target)%3==1?metro::ModPitchPost:metro::ModDirection;
        if(mod)m->params[M::modParam(target,M::MOD_DESTINATION)].setValue(destination);else m->params[M::outputTrackParam(target,M::VELOCITY_SOURCE)].setValue(2);
        widget::Widget* c=action<3?static_cast<widget::Widget*>(mod?mods[stage]:tracks[stage]):static_cast<widget::Widget*>(find(action<5?"Encoder":"Metropolix display"));if(auto* cell=dynamic_cast<ValueCell*>(c))cell->step();auto before=raw(),after=before;
        bool numeric=action==1 || action==4 || action==6,gate=mod?(destination==metro::ModDirection || (destination==metro::ModNone && saved==metro::ModGate)):saved==metro::GateCv;
        after[id]=numeric?(gate?5.f:mod && destination==metro::ModPitchPost?.125f:1.25f):0;
        bool acted=true;if(action==2){event::DoubleClick e;e.context=&context;c->onDoubleClick(e);}else {auto* menu=open(c);acted=numeric?enter(menu,gate?"On":mod && destination==metro::ModPitchPost?"1.5":"1.25"):init(menu);}
        bool pass=acted && raw()==after && APP->history->actionIndex==1;if(APP->history->actionIndex){APP->history->undo();pass &= raw()==before;APP->history->redo();pass &= raw()==after;}values &= pass;++valueCases;
        if(!pass && valueCases<8)check(false,"native value action matrix exact raw Undo/Redo");
    }
    for(int lane=0;lane<8;++lane)for(int saved=0;saved<3;++saved)for(auto dest:{metro::ModPitchPost,metro::ModDirection,metro::ModTempo})for(int action=0;action<4;++action){
        clean();m->uiMod.store(true);m->selectedLane.store(lane);screenPage(m,M::CV_TYPE_PAGE);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(saved);m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(dest);typeCell->step();auto before=raw();if(action==3){event::DoubleClick e;e.context=&context;typeCell->onDoubleClick(e);}else init(open(action==0?static_cast<widget::Widget*>(typeCell):static_cast<widget::Widget*>(find(action==1?"Encoder":"Metropolix display"))));locked &= raw()==before && APP->history->actionIndex==0;++lockCases;
    }
    for(int lane=0;lane<8;++lane)for(int saved=0;saved<3;++saved)for(auto dest:{metro::ModOutputSwap,metro::ModTempo,metro::ModLoopy,metro::ModLoopyPulses,metro::ModLoopyOffset})for(bool menu:{false,true}){
        clean();m->uiMod.store(true);m->selectedLane.store(lane);m->params[M::modParam(lane,M::MOD_ASSIGNMENT)].setValue(saved);m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(dest);assignment->step();auto before=raw();if(menu)init(open(assignment));else{event::DoubleClick e;e.context=&context;assignment->onDoubleClick(e);}locked &= raw()==before && APP->history->actionIndex==0;++lockCases;
    }
}
check(values,string::f("%d native numeric/Initialize/double-click value cases preserve exact raw Undo/Redo across every track/lane/stage/type and effective destination modes",valueCases));
check(locked,string::f("%d assigned type/global assignment context and double-click resets retain locked stored preferences and add no Undo",lockCases));
dismiss();for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(false);m->selectedTrack.store(0);screenPage(m,M::HOME);
for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
