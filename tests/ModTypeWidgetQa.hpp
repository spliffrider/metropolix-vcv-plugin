using M=Metropolix;APP->engine->bypassModule(m,true);
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->actionIndex=APP->history->savedIndex=0;
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};auto original=raw();
auto last=[&]()->history::Action*{int n=APP->history->actionIndex;return n?APP->history->actions[n-1]:nullptr;};
auto exact=[&](history::Action* previous,const std::vector<float>& before,const std::vector<float>& after){int n=APP->history->actionIndex;if(before==after)return last()==previous;bool one=previous?(n>1 && APP->history->actions[n-2]==previous):n==1;if(!one)return false;APP->history->undo();bool ok=raw()==before;APP->history->redo();return ok && raw()==after;};
auto clean=[&]{for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);m->params[M::ENC_CONFIRM].setValue(0);m->cancelDraft();m->uiMod.store(true);m->uiAlt.store(false);m->uiChooseFunction.store(false);m->selectedLane.store(0);m->selectedStage.store(0);screenPage(m,M::HOME);};
auto* encoder=find("Encoder");auto* display=find("Metropolix display");auto* typeButton=find("Stage 8 / CV");auto* stageButton=find("Stage 1 / SLIDE");
widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
auto tap=[&](HardwareControl* c){c->onDragStart(start);c->onDragEnd(end);};
clean();m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModPitchOffset);m->params[M::modParam(0,M::MOD_TYPE)].setValue(metro::ModUnipolar);m->params[M::modParam(0,M::MOD_VALUE)].setValue(-3.25f);
check(m->getParamQuantity(M::modParam(0,M::MOD_VALUE))->getMinValue()==-5 && m->effectiveModVoltage(0,-3.25f)==-3.25f,"continuous internal MOD destination uses bipolar editing and sounding voltage despite saved Unipolar");
clean();m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModPitchOffset);m->params[M::modParam(0,M::MOD_TYPE)].setValue(metro::ModGate);m->params[M::modParam(0,M::MOD_VALUE)].setValue(2.25f);tap(stageButton);
check(m->uiPage.load()==M::STAGE_PAGE && m->params[M::modParam(0,M::MOD_VALUE)].getValue()==2.25f,"continuous internal destination stage selection does not toggle a saved Gate pattern");
clean();m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModNone);m->uiAlt.store(true);tap(typeButton);
check(m->uiPage.load()==M::CV_TYPE_PAGE && !m->uiAlt.load(),"MOD ALT+TYPE opens the actual OLED CV TYPE page");closeMenus();
// A type preview becomes obsolete when an external internal assignment fixes
// its effective type. Confirm it immediately, without an intervening UI step.
clean();m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModNone);m->params[M::modParam(0,M::MOD_TYPE)].setValue(0);m->params[M::ENC_CONFIRM].setValue(1);screenPage(m,M::CV_TYPE_PAGE);encoder->onDragStart(start);move.mouseDelta=Vec(0,-44);encoder->onDragMove(move);encoder->onDragEnd(end);
check(m->uiDraftParam==M::modParam(0,M::MOD_TYPE),"prepare meaningful MOD type preview before external assignment");m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModPitchOffset);auto origin=raw();auto* previous=last();tap(encoder);
check(raw()==origin && m->uiDraftParam<0 && last()==previous,"external internal assignment consumes obsolete type confirmation after its binding becomes fixed");
bool entries=true,locked=true,stages=true;int entryCases=0,lockedCases=0,stageCases=0;
ValueCell* typeCell=nullptr;for(auto* w:modPanel->children)if(auto* c=dynamic_cast<ValueCell*>(w))if(c->modProperty==M::MOD_TYPE)typeCell=c;
check(typeCell,"effective MOD type probe finds actual optional grid type cell");
for(int lane=0;lane<8;++lane)for(int destination=0;destination<metro::ModDestinationCount;++destination)for(int saved=0;saved<3;++saved){
    clean();int tid=M::modParam(lane,M::MOD_TYPE),did=M::modParam(lane,M::MOD_DESTINATION),vid=M::modParam(lane,M::MOD_VALUE);m->selectedLane.store(lane);m->params[tid].setValue(saved);m->params[did].setValue(destination);m->params[vid].setValue(2.25f);m->uiAlt.store(true);tap(typeButton);entries &= m->uiPage.load()==M::CV_TYPE_PAGE && !m->uiAlt.load();++entryCases;closeMenus();
    if(destination!=metro::ModNone){
        screenPage(m,M::CV_TYPE_PAGE);for(auto* c:{encoder,display}){origin=raw();previous=last();c->onDragStart(start);move.mouseDelta=Vec(0,-44);c->onDragMove(move);c->onDragEnd(end);locked &= raw()==origin && last()==previous && !c->adjustmentTip && !APP->window->isCursorLocked();++lockedCases;}
        if(typeCell){typeCell->step();origin=raw();previous=last();typeCell->onDragStart(start);move.mouseDelta=Vec(0,-44);typeCell->onDragMove(move);typeCell->onDragEnd(end);locked &= raw()==origin && last()==previous && !typeCell->adjustmentTip && !APP->window->isCursorLocked();++lockedCases;}
        screenPage(m,M::CV_TYPE_PAGE);origin=raw();previous=last();encoder->onDragStart(start);encoder->heldSince=system::getTime()-1.1;encoder->step();encoder->onDragEnd(end);locked &= raw()==origin && last()==previous && !APP->window->isCursorLocked();++lockedCases;
    }
    m->params[tid].setValue(saved);m->params[vid].setValue(2.25f);screenPage(m,M::HOME);origin=raw();previous=last();bool gate=metro::modGateDestination(static_cast<metro::ModDestination>(destination)) || (destination==metro::ModNone && saved==metro::ModGate);auto expected=origin;if(gate)expected[vid]=0;tap(stageButton);stages &= m->uiPage.load()==M::STAGE_PAGE && raw()==expected && exact(previous,origin,expected);++stageCases;
}
check(entries,string::f("%d MOD ALT+TYPE entries use OLED page across all lanes/destinations/saved types",entryCases));
check(locked,string::f("%d forced MOD type encoder/OLED/grid drags and long resets preserve saved preference without history",lockedCases));
check(stages,string::f("%d stage selections/toggles follow effective MOD destination type with exact raw Undo/Redo",stageCases));
bool menus=true;int menuCases=0;
for(int lane=0;lane<8;++lane)for(int destination:{metro::ModNone,metro::ModPitchOffset,metro::ModDirection})for(int saved=0;saved<3;++saved){
    clean();m->selectedLane.store(lane);m->params[M::modParam(lane,M::MOD_TYPE)].setValue(saved);m->params[M::modParam(lane,M::MOD_DESTINATION)].setValue(destination);modSettingsMenu(m,lane);
    auto* overlay=APP->scene->children.back();auto* item=dynamic_cast<ui::MenuItem*>(seek(overlay,[](widget::Widget* w){auto* i=dynamic_cast<ui::MenuItem*>(w);return i && i->text=="CV type";}));
    if(item){item->step();const char* labels[]={"Bipolar","Unipolar","Gate"};int type=destination==metro::ModNone?saved:destination==metro::ModDirection?2:0;menus &= item->rightText.find(labels[type])!=std::string::npos && item->disabled==(destination!=metro::ModNone);}else menus=false;
    closeMenus();++menuCases;
}
check(menus,string::f("%d native MOD type menus display effective mode and lock only internal assignments",menuCases));
auto capture=[&](const std::string& name){auto pixels=dynamic_cast<HardwareDisplay*>(display)->render();std::ofstream file(dir+"/"+name+".pgm",std::ios::binary);file<<"P5\n128 64\n255\n";file.write(reinterpret_cast<char*>(pixels.pixels),128*64);};
clean();m->params[M::modParam(0,M::MOD_TYPE)].setValue(metro::ModUnipolar);m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModPitchPost);screenPage(m,M::CV_TYPE_PAGE);capture("mod-type-forced-bipolar");
m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModDirection);capture("mod-type-forced-gate");
m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModNone);capture("mod-type-restored-unipolar");
m->params[M::modParam(0,M::MOD_DESTINATION)].setValue(metro::ModPitchPost);for(int s=0;s<8;++s)m->params[M::modParam(0,M::MOD_VALUE+s)].setValue(s%2?2.25f:-1.25f);m->selectedStage.store(0);m->uiStageDetail.store(true);screenPage(m,M::STAGE_PAGE);capture("mod-type-negative-continuous");
for(int n=0;n<M::NUM_PARAMS;++n)m->params[n].setValue(original[n]);m->cancelDraft();m->uiMod.store(false);m->uiAlt.store(false);m->selectedTrack.store(0);screenPage(m,M::HOME);closeMenus();
for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
