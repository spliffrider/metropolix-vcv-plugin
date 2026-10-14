using M=Metropolix;APP->engine->bypassModule(m,true);
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->actionIndex=APP->history->savedIndex=0;
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};auto original=raw();
auto clean=[&]{for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->cancelDraft();m->uiMod.store(true);m->uiAlt.store(false);m->uiChooseFunction.store(false);screenPage(m,M::HOME);};
ValueCell* typeCell=nullptr;for(auto* w:modPanel->children)if(auto* c=dynamic_cast<ValueCell*>(w))if(c->modProperty==M::MOD_TYPE)typeCell=c;check(typeCell,"locked MOD menu probe finds actual grid type cell");
auto overlay=[&]()->widget::Widget*{for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if(dynamic_cast<ui::MenuOverlay*>(*it))return *it;return nullptr;};
auto label=[&](widget::Widget* menu,const std::string& text){return menu && seek(menu,[&](widget::Widget* w){auto* l=dynamic_cast<ui::MenuLabel*>(w);return l && l->text.find(text)!=std::string::npos;});};
if(typeCell){
    clean();m->selectedLane.store(0);m->params[M::modParam(0,M::MOD_TYPE)].setValue(metro::ModUnipolar);m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModPitchPost);m->params[M::accumModeParam(0)].setValue(1);typeCell->step();auto before=raw();int prior=APP->history->actionIndex;press(typeCell,GLFW_MOUSE_BUTTON_RIGHT);auto* menu=overlay();
    check(!label(menu,"CTRL 0"),"forced MOD type right-click never presents a nonexistent CTRL 0");
    auto* clear=menu?dynamic_cast<ui::MenuItem*>(seek(menu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Clear assignment";})):nullptr;if(clear){event::Action action;clear->onAction(action);}
    check(raw()==before && APP->history->actionIndex==prior,"locked type menu does not expose a false controller action that clears track 1 accumulation mode");closeMenus();
    bool menus=true;int cases=0;
    for(int lane=0;lane<8;++lane)for(int saved=0;saved<3;++saved)for(int dest=1;dest<int(M::modDestinations().size());++dest){
        clean();m->selectedLane.store(lane);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(saved);m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(dest);typeCell->step();auto* q=m->getParamQuantity(M::modParam(lane,M::MOD_TYPE));before=raw();prior=APP->history->actionIndex;
        widget::EventContext context;event::Button button;button.context=&context;button.button=GLFW_MOUSE_BUTTON_RIGHT;button.action=GLFW_PRESS;button.mods=0;typeCell->onButton(button);menu=overlay();
        menus &= context.consumed && context.target==typeCell && menu && !label(menu,"CTRL 0") && label(menu,q->getDisplayValueString()) && label(menu,M::modDestinations()[dest]) && raw()==before && APP->history->actionIndex==prior;
        closeMenus();++cases;
    }
    check(menus,string::f("%d assigned MOD type grid menus explain the effective type/destination, consume right-click and preserve all raw fields/history",cases));
    bool settings=true;int settingsCases=0;
    for(int lane=0;lane<8;++lane)for(int saved=0;saved<3;++saved)for(auto dest:{metro::ModPitchPost,metro::ModDirection,metro::ModRoot}){
        clean();m->selectedLane.store(lane);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(saved);m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(dest);typeCell->step();before=raw();prior=APP->history->actionIndex;
        widget::EventContext context;event::Button button;button.context=&context;button.button=GLFW_MOUSE_BUTTON_RIGHT;button.action=GLFW_PRESS;button.mods=0;typeCell->onButton(button);menu=overlay();
        auto* link=menu?dynamic_cast<ui::MenuItem*>(seek(menu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="MOD lane settings";})):nullptr;m->selectedLane.store((lane+1)%8);if(link){event::Action action;link->onAction(action);}menu=overlay();
        auto* destination=menu?dynamic_cast<ui::MenuItem*>(seek(menu,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="Internal destination";})):nullptr;auto* child=destination?destination->createChildMenu():nullptr;auto* none=child?dynamic_cast<ui::MenuItem*>(seek(child,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="None";})):nullptr;
        if(none){event::Action action;none->onAction(action);}auto after=before;after[M::modParam(lane,M::MOD_DESTINATION)]=metro::ModNone;
        settings &= link && destination && none && label(menu,"MOD lane "+std::to_string(lane+1)) && raw()==after && APP->history->actionIndex==prior+1 && m->effectiveModType(lane)==saved;
        if(APP->history->actionIndex>prior){APP->history->undo();settings &= raw()==before && m->effectiveModType(lane)==metro::effectiveModType(static_cast<metro::ModType>(saved),dest);APP->history->redo();settings &= raw()==after;}
        delete child;closeMenus();++settingsCases;
    }
    check(settings,string::f("%d locked type menu settings actions remain bound to the original lane after selection changes; clearing its destination restores the saved type with exact Undo/Redo",settingsCases));
    bool guards=true;for(int knob:{-1,2,9}){ui::Menu menu;before=raw();prior=APP->history->actionIndex;appendCtrlMenu(&menu,m,knob);guards &= menu.children.empty() && raw()==before && APP->history->actionIndex==prior;}check(guards,"invalid CTRL menu indices create no mislabeled options or parameter/history changes");
}
for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(false);m->selectedTrack.store(0);screenPage(m,M::HOME);closeMenus();
for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
