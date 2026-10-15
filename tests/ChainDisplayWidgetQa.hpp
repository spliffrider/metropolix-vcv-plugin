// Native screen masks and actual interaction cases for the chain renderer.
if(qaFrame==4){
    using M=Metropolix;
    #include "ChainDisplayReference.hpp"
    APP->engine->bypassModule(m,true);
    auto originalState=m->getChainState();std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());
    int oldPage=m->uiPage.load(),oldTrack=m->selectedTrack.load(),oldLane=m->selectedLane.load(),oldCursor=m->chainCursor.load(),oldEdit=m->chainEdit.load(),oldPlaying=m->chainPlayingLink.load(),oldRepeat=m->chainPlayingRepeat.load();
    bool oldMod=m->uiMod.load(),oldFollow=m->chainFollow.load(),oldQuick=m->chainQuick,oldCreating=m->chainCreating,oldCancel=m->chainQuickCancel;
    auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;APP->history->savedIndex=APP->history->actionIndex=0;
    auto raw=[&]{std::vector<float> values;for(auto& p:m->params)values.push_back(p.getValue());return values;};
    auto same=[](const M::ChainState& a,const M::ChainState& b){return a.working==b.working && a.saved==b.saved && a.enabled==b.enabled && a.loaded==b.loaded;};
    auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));
    auto capture=[&](const std::string& name){auto c=display->render();std::ofstream out(dir+"/chain-display-"+name+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<char*>(c.pixels),sizeof(c.pixels));};
    auto fixture=[&](int size){M::ChainState state;for(int i=0;i<size;++i){metro::ChainLink link;link.slot=48+i%8;link.repeats=64-i%64;link.mutes=i%8;state.working.insert(i,link);}m->setChainState(state);openChainPage(m);m->uiMod.store(false);m->selectedTrack.store(0);m->chainQuick=false;m->chainPlayingLink.store(-1);return state;};
    m->params[M::RUN].setValue(0);m->params[M::ENC_CONFIRM].setValue(0);
    bool native=true;int nativeCount=0;
    for(const auto& ref:chainReferences){auto state=fixture(ref.variant==1?6:0);
        if(ref.variant==1){state.working.links[4].slot=44;state.working.links[4].repeats=1;state.working.links[4].mutes=0;state.working.links[5].slot=33;state.working.links[5].repeats=2;state.working.links[5].mutes=2;m->setChainState(state);m->chainCursor.store(13);}else m->chainCursor.store(0);
        auto before=raw();int history=APP->history->actionIndex;auto actual=display->render();bool match=true;
        // One footer dot has different luminance in the supplied native images
        // (219 versus 29). Do not turn that intensity sample into a shape test.
        for(int y=0;y<64;++y)for(int x=0;x<128;++x)if(x!=38 || y!=60)match &= bool(actual.pixels[y][x])==bool(ref.packed[y*16+x/8]&(1<<(7-x%8)));
        native &= match && raw()==before && history==APP->history->actionIndex && same(state,m->getChainState());if(!match)check(false,"native chain screen "+std::to_string(ref.variant));capture("native-"+std::to_string(ref.variant));++nativeCount;
    }
    check(native,std::to_string(nativeCount)+" native chain screen foreground masks match measured rows, captions, selection, hatching, icons and mutes; one varying-luminance dot excluded");
    bool metrics=true;int glyphCount=0;
    for(bool caption:{false,true})for(int code=1;code<=127;++code)if(code==1 || code>=32){auto glyph=caption?OledCanvas::chainCaption(static_cast<unsigned char>(code)):metrodisplay::chainGlyph(static_cast<unsigned char>(code));OledCanvas c;c.chainText(0,0,std::string(1,char(code)),255,caption);metrics &= glyph.width>=0 && glyph.width<=7 && glyph.advance>=glyph.width+1;for(int y=0;y<64;++y)for(int x=0;x<128;++x)if(c.pixels[y][x])metrics &= x>=1 && x<=glyph.width && y<(caption?5:9);++glyphCount;}
    check(metrics,std::to_string(glyphCount)+" chain/caption glyph metrics keep descenders within eleven-pixel rows");
    auto visibleNumber=[&](const OledCanvas& actual,int number,int edit){OledCanvas expected;if(!edit)expected.fill(1,0,16,9);expected.chainText(2,0,std::to_string(number),edit?255:0);for(int y:{12,23,34,45}){bool match=true;for(int j=0;j<9;++j)for(int x=3;x<17;++x)match &= actual.pixels[y+j][x]==expected.pixels[j][x];int edge=edit==1?71:edit==2?101:127;if(match && actual.pixels[y-2][edge])return true;}return false;};
    bool navigation=true;int navigationCount=0;
    for(int size:{1,2,3,4,8,16,32})for(int at:{0,size/2,size-1})for(int edit=0;edit<3;++edit){auto state=fixture(size);m->chainCursor.store(2*at+1);m->chainEdit.store(edit);auto before=raw();int history=APP->history->actionIndex;auto c=display->render();navigation &= visibleNumber(c,at+1,edit) && raw()==before && same(state,m->getChainState()) && history==APP->history->actionIndex;++navigationCount;if(size==32 && at==31)capture("last-link-edit-"+std::to_string(edit));}
    check(navigation,std::to_string(navigationCount)+" selected link/field cases stay visible through 32-link scrolling, all three edit modes and compact Magenta labels without state/history writes");
    bool mutes=true;
    for(int mask=0;mask<8;++mask){auto state=fixture(1);state.working.links[0].mutes=mask;m->setChainState(state);m->chainCursor.store(1);auto c=display->render();for(int t=0;t<3;++t)mutes &= bool(c.pixels[15][105+9*t])==bool(mask&(1<<t));capture("mute-"+std::to_string(mask));}
    check(mutes,"all eight mute combinations remain legible inside an inverted chain row");
    fixture(0);beginQuickChain(m);click(find("Stage 1 / SLIDE"));auto one=m->getChainState();auto plusPresent=[&]{auto c=display->render();int count=0;for(int y=13;y<18;++y)for(int x=96;x<101;++x)count+=bool(c.pixels[y][x]);return count==9;};bool quick=one.working.size==1 && one.working.links[0].repeats==1 && plusPresent();capture("quick-one");click(find("Stage 1 / SLIDE"));auto two=m->getChainState();quick &= two.working.links[0].repeats==2 && plusPresent();capture("quick-two");APP->history->undo();quick &= same(m->getChainState(),one) && plusPresent();APP->history->redo();quick &= same(m->getChainState(),two) && plusPresent();
    check(quick,"actual Quick Chain stage presses show the repeat-plus cue through repetition and exact Undo/Redo");
    auto state=fixture(8);state.enabled=true;m->setChainState(state);m->chainFollow.store(true);m->chainPlayingLink.store(3);m->chainPlayingRepeat.store(64);m->chainCursor.store(0);auto before=raw();int history=APP->history->actionIndex;auto following=display->render();bool follow=visibleNumber(following,4,0) && m->chainCursor.load()==0 && raw()==before && same(state,m->getChainState()) && history==APP->history->actionIndex;capture("following");click(find("Encoder"));follow &= !m->chainFollow.load() && m->chainCursor.load()==7;capture("static-playback");click(find("Encoder"));follow &= m->chainFollow.load();
    check(follow,"actual Chain Follow toggle preserves the playing-row highlight, stored cursor and read-only playback state");
    m->resetChainRuntime();for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->setChainState(originalState);m->uiPage.store(oldPage);m->selectedTrack.store(oldTrack);m->selectedLane.store(oldLane);m->uiMod.store(oldMod);m->chainCursor.store(oldCursor);m->chainEdit.store(oldEdit);m->chainPlayingLink.store(oldPlaying);m->chainPlayingRepeat.store(oldRepeat);m->chainFollow.store(oldFollow);m->chainQuick=oldQuick;m->chainCreating=oldCreating;m->chainQuickCancel=oldCancel;
    for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
}
