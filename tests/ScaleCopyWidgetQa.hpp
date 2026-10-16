using M=Metropolix;
static_assert(M::FORMAT34_NUM_PARAMS==787 && M::NUM_PARAMS==789 && M::SNAPSHOT_PARAMS==522 && M::PRESET_FORMAT==12,"Scale clipboard adds no saved fields");
if(std::getenv("METROPOLIX_SCALECOPY_REOPEN_QA")) {
    if(qaFrame==6) {
        check(m->integer(M::SCALE)==148 && m->integer(M::userScaleParam(99))==0x91 && m->integer(M::userScaleParam(99,true))==9,"Rack reopen retains pasted notes/root in final user slot");
        check(m->integer(M::SCALE_LOCK)==1 && m->integer(M::MOD_GROUP_MUTE)==1,"Rack reopen retains unrelated lock/group mute");
        check(!m->scaleClipboardMask && !m->scaleCopyGesture && !m->scaleShortcutHeld.load(),"Rack reopen clears clipboard and held scale gestures");
        check(!canPasteScale(m),"reopened patch cannot paste stale scale clipboard");
        check(!qaFailed,"SCALECOPY REOPEN QA COMPLETE");APP->window->close();
    }return;
}
widget::EventContext copyContext;
event::DragStart copyStart;copyStart.context=&copyContext;copyStart.button=GLFW_MOUSE_BUTTON_LEFT;
event::DragEnd copyEnd;copyEnd.context=&copyContext;copyEnd.button=GLFW_MOUSE_BUTTON_LEFT;
auto beginCopy=[&](HardwareControl* c,int mods=0){event::Button e;e.context=&copyContext;e.pos=c->box.size.div(2);e.button=GLFW_MOUSE_BUTTON_LEFT;e.action=GLFW_PRESS;e.mods=mods;c->onButton(e);c->onDragStart(copyStart);};
auto endCopy=[&](HardwareControl* c){c->onDragEnd(copyEnd);};
auto shortCopy=[&](HardwareControl* c,int mods=0){beginCopy(c,mods);endCopy(c);};
auto longPaste=[&](int mods=0){auto* c=find("SCALE");beginCopy(c,mods);c->heldSince=system::getTime()-1.1;c->step();bool fired=c->longFired;endCopy(c);return fired;};
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};
auto lastAction=[&]()->history::Action*{return APP->history->actions.empty()?nullptr:APP->history->actions.back();};
auto pairedAction=[&](history::Action* previous){auto* action=dynamic_cast<history::ComplexAction*>(lastAction());return action && action!=previous && action->actions.size()==2;};
auto sameOther=[&](const std::vector<float>& before,const std::vector<int>& allowed){for(int id=0;id<M::NUM_PARAMS;++id)if(std::find(allowed.begin(),allowed.end(),id)==allowed.end() && before[id]!=m->params[id].getValue())return false;return true;};
auto menuItem=[&](const std::string& label)->ui::MenuItem*{for(auto it=APP->scene->children.rbegin();it!=APP->scene->children.rend();++it)if((*it)->visible && dynamic_cast<ui::MenuOverlay*>(*it))return dynamic_cast<ui::MenuItem*>(seek(*it,[&](widget::Widget* w){auto* item=dynamic_cast<ui::MenuItem*>(w);return item && item->text==label;}));return nullptr;};
auto activate=[&](const std::string& label){auto* item=menuItem(label);check(item && !item->disabled,"scale menu offers "+label);if(item && !item->disabled){event::Action e;item->onAction(e);}closeMenus();};
if(qaFrame==6) {
    m->params[M::RUN].setValue(0);m->params[M::RUN_ON_BOOT].setValue(2);m->params[M::ENC_CONFIRM].setValue(0);
    for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(metro::CtrlNone);
    m->workflowClipboard={4,6,-3};m->workflowClipKind=1;m->workflowClipTag=99;
    auto* scale=find("SCALE");bool copied=true,pasted=true,history=true,unrelated=true,independent=true;
    // Displayed scale deliberately differs from the sounding lock.
    m->params[M::SCALE_LOCK].setValue(1);m->params[M::MOD_GROUP_MUTE].setValue(1);
    for(int source=0;source<149;++source) {
        unsigned mask=source<49?metro::scaleMask(metro::scaleAt(source)):((1u<<(source%12))|(1u<<((source+5)%12)));
        int root=(source*7+3)%12;m->params[M::SCALE].setValue(source);m->params[m->scaleRootParam(source)].setValue(root);
        if(source>=49)m->params[M::userScaleParam(source-49)].setValue(float(mask));
        m->uiPage.store(M::SCALE_PAGE);m->uiField.store(0);m->uiAlt.store(true);auto before=raw();int h=APP->history->actionIndex;shortCopy(scale);
        copied &= m->scaleClipboardMask==mask && m->scaleClipboardRoot==root && raw()==before && APP->history->actionIndex==h;
        // Changing the source after copy must not modify the snapshot.
        m->params[m->scaleRootParam(source)].setValue((root+5)%12);
        if(source>=49)m->params[M::userScaleParam(source-49)].setValue(mask==1?3:1);
        for(int target=0;target<100;++target) {
            int maskId=M::userScaleParam(target),rootId=M::userScaleParam(target,true);
            m->params[M::SCALE].setValue(49+target);m->params[maskId].setValue(mask==1?3:1);m->params[rootId].setValue((root+1)%12);
            m->uiAlt.store(true);before=raw();auto* previous=lastAction();pasted &= longPaste();auto after=raw();
            pasted &= m->integer(maskId)==int(mask) && m->integer(rootId)==root && !m->scaleCopyGesture && !m->scaleShortcutHeld.load();
            unrelated &= sameOther(before,{maskId,rootId});
            history &= pairedAction(previous);APP->history->undo();history &= raw()==before;APP->history->redo();history &= raw()==after;
            independent &= m->scaleClipboardMask==mask && m->scaleClipboardRoot==root && m->workflowClipboard==std::vector<float>({4,6,-3}) && m->workflowClipKind==1 && m->workflowClipTag==99;
        }
    }
    check(copied,"ALT + SCALE copies all 49 factory and 100 user definitions/root from displayed scale while another scale stays locked");
    check(pasted,"long ALT + SCALE pastes 149 source snapshots into every one of 100 user slots across bank boundaries");
    check(history && unrelated,"all 14900 paired pastes have one exact undo/redo and leave selection/lock/mutes/unrelated parameters intact");
    check(independent,"scale snapshot survives source edits and repeated pastes independently of stage/lane clipboard");
    // Threshold, no-op, empty/factory rejection and no accidental short-copy.
    m->params[M::SCALE].setValue(49);m->params[M::userScaleParam(0)].setValue(3);m->params[M::userScaleParam(0,true)].setValue(0);m->scaleClipboardMask=0x91;m->scaleClipboardRoot=9;
    m->uiAlt.store(true);auto before=raw();int h=APP->history->actionIndex;auto* previous=lastAction();beginCopy(scale);scale->heldSince=system::getTime()-.9;scale->step();check(!scale->longFired && raw()==before,"scale paste waits for long threshold");scale->heldSince=system::getTime()-1.1;scale->step();endCopy(scale);check(scale->longFired && m->integer(M::userScaleParam(0))==0x91 && pairedAction(previous),"threshold crossing pastes notes/root once");
    m->uiAlt.store(true);h=APP->history->actionIndex;longPaste();check(APP->history->actionIndex==h,"pasting identical scale/root adds no undo");
    m->params[M::SCALE].setValue(3);before=raw();m->uiAlt.store(true);h=APP->history->actionIndex;longPaste();check(raw()==before && APP->history->actionIndex==h && m->scaleClipboardMask==0x91 && m->workflowMessage=="CHOOSE USER SCALE","factory paste is rejected without changing clipboard or musical values");
    m->params[M::SCALE].setValue(49);m->scaleClipboardMask=0;before=raw();m->uiAlt.store(true);longPaste();check(raw()==before && !m->scaleClipboardMask && m->workflowMessage=="COPY SCALE FIRST","empty long paste is rejected without falling through to short copy");
    // Actual held panel ALT and keyboard modifier callback paths.
    m->params[M::SCALE].setValue(49);m->params[M::userScaleParam(0)].setValue(0x91);m->params[M::userScaleParam(0,true)].setValue(9);m->uiField.store(0);m->uiAlt.store(false);auto* alt=find("ALT");beginCopy(alt);shortCopy(scale);endCopy(alt);
    check(m->scaleClipboardMask==0x91 && m->scaleClipboardRoot==9 && m->uiField.load()==1 && !alt->dragging,"held ALT copy consumes modifier release without switching Root field");
    m->params[M::SCALE].setValue(50);m->params[M::userScaleParam(1)].setValue(3);m->params[M::userScaleParam(1,true)].setValue(2);m->uiAlt.store(false);longPaste(GLFW_MOD_ALT);
    check(m->integer(M::userScaleParam(1))==0x91 && m->integer(M::userScaleParam(1,true))==9 && scale->pressMods==0,"keyboard Alt callback paste works and clears captured modifier");
    m->params[M::SCALE].setValue(3);m->params[M::ROOT].setValue(4);shortCopy(scale,GLFW_MOD_ALT);check(m->scaleClipboardMask==metro::scaleMask(metro::scaleAt(3)) && m->scaleClipboardRoot==4,"keyboard Alt callback copies displayed factory scale/root");
    // Native menus and disabled states.
    press(scale,GLFW_MOUSE_BUTTON_RIGHT);auto* paste=menuItem("Paste scale into displayed user slot");check(paste && paste->disabled,"paste menu disabled on factory scale");activate("Copy displayed scale");
    m->params[M::SCALE].setValue(148);m->params[M::userScaleParam(99)].setValue(1);m->params[M::userScaleParam(99,true)].setValue(0);previous=lastAction();press(find("Metropolix display"),GLFW_MOUSE_BUTTON_RIGHT);paste=menuItem("Paste scale into displayed user slot");if(!paste || paste->disabled)check(false,string::f("display paste diagnostic: page%d field%d shown%d clipboard%d quantity%d item%d",m->uiPage.load(),m->uiField.load(),m->integer(M::SCALE),int(m->scaleClipboardMask),screenParam(m),paste?1:0));activate("Paste scale into displayed user slot");check(m->integer(M::userScaleParam(99))==int(metro::scaleMask(metro::scaleAt(3))) && m->integer(M::userScaleParam(99,true))==4 && pairedAction(previous),"display menu pastes pair with one undo");APP->history->undo();APP->history->redo();
    m->scaleClipboardMask=0;press(scale,GLFW_MOUSE_BUTTON_RIGHT);paste=menuItem("Paste scale into displayed user slot");check(paste && paste->disabled,"paste menu disabled with empty clipboard");closeMenus();
    // Pending encoder preview is cancelled by explicit copy/paste.
    m->params[M::SCALE].setValue(49);m->params[M::ENC_CONFIRM].setValue(1);m->uiField.store(1);drag(find("Encoder"),-44);check(m->uiDraftParam>=0,"scale encoder produces pending browse preview");m->uiAlt.store(true);shortCopy(scale);check(m->uiDraftParam<0,"copy cancels pending browse and captures applied scale");m->params[M::ENC_CONFIRM].setValue(0);
    // Initialize clears clipboard; preserve final fixture after the check.
    M::ResetEvent reset;m->onReset(reset);check(!m->scaleClipboardMask && !m->scaleCopyGesture && m->workflowClipboard.empty(),"Initialize clears transient scale and stage/lane clipboards");
    m->params[M::RUN].setValue(0);m->params[M::RUN_ON_BOOT].setValue(2);m->params[M::SCALE].setValue(49);m->params[M::userScaleParam(0)].setValue(0x91);m->params[M::userScaleParam(0,true)].setValue(9);m->uiAlt.store(true);shortCopy(scale);m->params[M::SCALE].setValue(148);m->uiAlt.store(true);longPaste();m->params[M::SCALE_LOCK].setValue(1);m->params[M::MOD_GROUP_MUTE].setValue(1);
    APP->patch->save(dir+"/scalecopy-roundtrip.vcv");
}
if(qaFrame==8){metroqa::screenshot(dir+"/scale-pasted-user.png");auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));if(display){auto c=display->render();std::ofstream pgm(dir+"/scale-pasted.pgm",std::ios::binary);pgm<<"P5\n128 64\n255\n";pgm.write(reinterpret_cast<char*>(c.pixels),128*64);}}
if(qaFrame==10){check(!qaFailed,"SCALECOPY UI QA COMPLETE");APP->window->close();}
