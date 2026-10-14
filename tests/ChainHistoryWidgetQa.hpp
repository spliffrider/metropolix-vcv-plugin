// Exhaustive actual callbacks, separate from the musical chain fixture below.
if(qaFrame==3){
    using M=Metropolix;APP->engine->bypassModule(m,true);
    auto initial=m->getChainState();int originalCursor=m->chainCursor.load(),originalBank=m->presetBankIndex.load();bool originalFollow=m->chainFollow.load();std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());
    m->params[M::RUN].setValue(0);m->params[M::ENC_CONFIRM].setValue(0);
    auto same=[](const M::ChainState& a,const M::ChainState& b){return a.working==b.working && a.saved==b.saved && a.enabled==b.enabled && a.loaded==b.loaded;};
    auto last=[]()->history::Action*{return APP->history->actionIndex?APP->history->actions[APP->history->actionIndex-1]:nullptr;};
    auto exact=[&](history::Action* prior,const M::ChainState& before){auto after=m->getChainState();if(same(before,after))return last()==prior;int index=APP->history->actionIndex;bool ok=last()!=prior && (prior?(index>=2 && APP->history->actions[index-2]==prior):index==1);APP->history->undo();ok &= same(m->getChainState(),before);APP->history->redo();return ok && same(m->getChainState(),after);};
    auto fixture=[&](int size){M::ChainState state;state.loaded=5;for(int i=0;i<size;++i){metro::ChainLink link;link.slot=(i*7)%64;link.repeats=1+i;link.mutes=i%8;state.working.insert(i,link);}for(int slot=0;slot<8;++slot){metro::ChainLink link;link.slot=slot*8+3;link.repeats=slot+1;link.mutes=7-slot;state.saved[slot].insert(0,link);}m->setChainState(state);m->chainQuick=false;openChainPage(m);return state;};
    auto* encoder=find("Encoder");auto* display=find("Metropolix display");bool inserts=true,deletes=true,clears=true,threshold=true,guards=true;
    for(int size=0;size<=32;++size){
        for(int at=0;at<=size;++at){auto before=fixture(size);m->chainCursor.store(2*at);m->presetBankIndex.store(at%8);auto* prior=last();click(encoder);auto after=m->getChainState();
            inserts &= after.working.size==std::min(size+1,32) && exact(prior,before) && after.saved==before.saved && (size==32?after.loaded==before.loaded:after.loaded==-1);
            if(size<32)inserts &= after.working.links[at].slot==8*(at%8) && after.working.links[at].repeats==1 && after.working.links[at].mutes==0;
        }
        for(int at=0;at<size;++at){auto before=fixture(size);m->chainCursor.store(2*at+1);auto* prior=last();hold(encoder,.9);threshold &= same(m->getChainState(),before) && last()==prior && !encoder->longFired;
            m->chainEdit.store(0);hold(encoder,1.1);auto after=m->getChainState();deletes &= encoder->longFired && after.working.size==size-1 && after.loaded==-1 && after.saved==before.saved && exact(prior,before);
            auto expected=before.working;expected.erase(at);deletes &= after.working==expected;
        }
        auto before=fixture(size);m->chainCursor.store(2*size+2);auto* prior=last();click(encoder);auto after=m->getChainState();clears &= after.working.size==0 && after.loaded==-1 && after.saved==before.saved && exact(prior,before);
    }
    check(inserts,"every insertion position through 32 links adds one exact full-state undo and preserves all eight saved chains; full capacity is a no-op");
    check(deletes && threshold,"every selected link through 32 links deletes after the hold threshold with one exact undo/redo and preserved saved chains");
    check(clears,"Clear Chain at every size records one exact undo/redo and preserves all saved chains");
    bool turns=true,banks=true,reversal=true,browse=true;
    for(auto* c:{encoder,display})for(int repeats=1;repeats<=64;++repeats){auto before=fixture(3);before.working.links[1].repeats=repeats;m->setChainState(before);m->chainCursor.store(3);m->chainEdit.store(2);auto* prior=last();drag(c,-44);auto after=m->getChainState();turns &= after.working.links[1].repeats==std::min(64,repeats+2) && exact(prior,before);}
    for(auto* c:{encoder,display})for(int bank=0;bank<8;++bank){auto before=fixture(3);before.working.links[1].slot=bank*8+5;m->setChainState(before);m->presetBankIndex.store(bank);m->chainCursor.store(3);m->chainEdit.store(1);auto* prior=last();drag(c,-22);auto after=m->getChainState();banks &= after.working.links[1].slot==((bank+1)%8)*8+5 && exact(prior,before);}
    widget::EventContext context;event::DragStart start;start.context=&context;start.button=GLFW_MOUSE_BUTTON_LEFT;event::DragMove move;move.context=&context;move.button=GLFW_MOUSE_BUTTON_LEFT;event::DragEnd end;end.context=&context;end.button=GLFW_MOUSE_BUTTON_LEFT;
    for(auto* c:{encoder,display}){auto before=fixture(3);m->chainCursor.store(3);m->chainEdit.store(2);auto* prior=last();c->onDragStart(start);move.mouseDelta=Vec(0,-44);c->onDragMove(move);move.mouseDelta=Vec(0,44);c->onDragMove(move);c->onDragEnd(end);reversal &= same(before,m->getChainState()) && last()==prior && !APP->window->isCursorLocked();
        before=fixture(3);m->chainCursor.store(1);prior=last();drag(c,-22);browse &= same(before,m->getChainState()) && last()==prior;
        before=fixture(3);m->chainCursor.store(0);prior=last();hold(encoder,1.1);guards &= !encoder->longFired && m->getChainState().working.size==4 && exact(prior,before);
        before=fixture(3);before.enabled=true;m->setChainState(before);m->chainCursor.store(1);prior=last();hold(encoder,1.1);guards &= same(before,m->getChainState()) && last()==prior;
    }
    check(turns && banks,"encoder and display bank/repeat drags retain one exact full-state undo across all banks, counts and clipping endpoints");
    check(reversal && browse,"reversed repeat drag and cursor-only navigation leave chain state/history unchanged and release cursor capture");
    check(guards,"inactive insertion-line hold retains short insertion action; enabled playback prevents deletion");
    m->resetChainRuntime();for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->setChainState(initial);m->chainCursor.store(originalCursor);m->presetBankIndex.store(originalBank);m->chainFollow.store(originalFollow);screenPage(m,M::HOME);APP->engine->bypassModule(m,false);
}
