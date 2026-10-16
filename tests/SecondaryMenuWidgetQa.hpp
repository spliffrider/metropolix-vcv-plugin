// Manual pp33–37: four mixed-case rows, browse/value selection and named choices.
using M=Metropolix;APP->engine->bypassModule(m,true);
std::vector<float> original;for(auto& p:m->params)original.push_back(p.getValue());metrobackup::Json originalData(m->dataToJson());
auto savedHistory=std::move(APP->history->actions);int savedIndex=APP->history->savedIndex,savedActionIndex=APP->history->actionIndex;
APP->history->savedIndex=APP->history->actionIndex=0;
struct Entry{int page,item,track,lane,id,count;std::string label;};std::vector<Entry> entries;
const char* trackNames[]={"Transpose","Slider Octaves","Slider Dir","Pulse Count","Gate Type","Gate Stretching","Ratchets","Rest Pitch","Global Reset","Velocity","MIDI Channel","CV to CC","Slide Toggle CC","Slide Amount CC","Slide Amount","Init Trk Defaults"};
for(int t=0;t<2;++t){const int ids[]={M::trackParam(t,M::TRANSPOSE),M::outputTrackParam(t,M::SLIDER_OCTAVES),M::outputTrackParam(t,M::SLIDER_DIRECTION),M::trackBehaviorParam(t,M::PULSE_SOURCE),M::trackBehaviorParam(t,M::GATE_SOURCE),M::extraParam(t,M::GATE_STRETCH),M::extraParam(t,M::RATCHET_MODE),M::trackBehaviorParam(t,M::REST_PITCH),M::TRACK_GLOBAL_RESET_1+t,M::outputTrackParam(t,M::VELOCITY_SOURCE),M::midiTrackParam(t,0),M::midiTrackParam(t,1),M::midiTrackParam(t,2),M::midiTrackParam(t,3),M::midiTrackParam(t,4),-1};for(int n=0;n<16;++n)entries.push_back({M::TRACK_MENU_PAGE,n,t,0,ids[n],16,trackNames[n]});}
const int globalIds[]={M::CLOCK_SOURCE,M::CLOCK_RUNNING_ONLY,M::RESET_INPUT_MODE,M::BOTTOM_PITCH,M::RUN_MODE,-1};const char* globalNames[]={"Clock Src","Clk/Rset Out","Reset Input","Bottom Pitch","Run Btn","Tuner..."};for(int n=0;n<6;++n)entries.push_back({M::GLOBAL_SETUP_PAGE,n,0,0,globalIds[n],6,globalNames[n]});
const int uiIds[]={M::RUN_ON_BOOT,M::RUN_MODE,M::ENC_CONFIRM,M::SHOW_POSITION,M::BUTTON_LEVEL,M::SLIDER_LEVEL,M::SCREEN_LEVEL,M::CV_OUT_DELAY};const char* uiNames[]={"Run on Boot","Run Btn","Enc. Confirm","Show Pos.","Button LED Level","Slider LED Level","Screen Level","CV Out Delay"};for(int n=0;n<8;++n)entries.push_back({M::UI_SETUP_PAGE,n,0,0,uiIds[n],8,uiNames[n]});
const int midiIds[]={M::MIDI_CLOCK_OUT,M::MIDI_TRANSPORT,M::MIDI_ROOT_CHANNEL,M::MIDI_PC_CHANNEL,M::MIDI_MX_CHANNEL,M::MIDI_MX_OUTPUT,M::MIDI_ZERO_NOTE,-1};const char* midiNames[]={"MIDI Clock Out","MIDI Transport","MIDI Root Ch","MIDI PC Ch","MIDI MX Ch","MIDI MX Output","Note at 0V","Backup Files..."};for(int n=0;n<8;++n)entries.push_back({M::MIDI_SETUP_PAGE,n,0,0,midiIds[n],8,midiNames[n]});
const char* modNames[]={"MIDI Channel","MIDI Mod to CC","Init Lane Defaults","Init All Lanes"};for(int l=0;l<8;++l)for(int n=0;n<4;++n)entries.push_back({M::MOD_MENU_PAGE,n,0,l,n<2?M::midiModParam(l,n):-1,4,modNames[n]});
auto raw=[&]{std::vector<float> v;for(auto& p:m->params)v.push_back(p.getValue());return v;};
auto clean=[&](Entry e){closeMenus();for(auto* a:APP->history->actions)delete a;APP->history->actions.clear();APP->history->savedIndex=APP->history->actionIndex=0;for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->clearWorkflows();m->cancelDraft();m->params[M::RUN].setValue(0);for(int k=0;k<2;++k)m->params[M::ctrlParam(k,M::CTRL_DESTINATION)].setValue(0);m->uiMod.store(e.page==M::MOD_MENU_PAGE);m->uiAlt.store(false);m->selectedTrack.store(e.track);m->selectedLane.store(e.lane);screenPage(m,e.page);m->trackMenuCursor.store(e.item);m->globalCursor.store(e.item);m->uiSetupCursor.store(e.item);m->midiSetupCursor.store(e.item);m->modMenuCursor.store(e.item);};
auto* display=dynamic_cast<HardwareDisplay*>(find("Metropolix display"));auto* encoder=find("Encoder");
auto expectedValue=[&](Entry e,int v,bool pending){std::string text;if(e.id<0)return text;
    if(e.page==M::TRACK_MENU_PAGE && e.item==0)text=v?string::f("%+d",v):"-";
    else if(e.page==M::TRACK_MENU_PAGE && e.item==1)text=v?std::to_string(v):"Shared";
    else if(e.page==M::TRACK_MENU_PAGE && e.item==4){const char* names[]={"Switch","Single","Sngl+Rst","Multi","Multi+Rst","Hold","Hold+Rst"};text=names[v];}
    else if(e.id==M::CLOCK_SOURCE){const char* names[]={"Auto","Internal","External","MIDI"};text=names[v];}
    else if(e.id==M::CLOCK_RUNNING_ONLY){const char* names[]={"Always","Running","Legacy"};text=names[v];}
    else if(e.id==M::RESET_INPUT_MODE){const char* names[]={"Reset","Run","Run-C"};text=names[v];}
    else if(e.id==M::RUN_MODE)text=v?"Run/Pause":"Run/Stop";
    else if(e.id==M::RUN_ON_BOOT){const char* names[]={"Off","On","Patch"};text=names[v];}
    else if(e.id==M::CV_OUT_DELAY)text=std::to_string(v)+" ms";
    else if(auto* q=dynamic_cast<SwitchQuantity*>(m->getParamQuantity(e.id)))text=q->labels[v-int(q->getMinValue())];
    else text=std::to_string(v);
    if(pending)text+=" ?";return text;};
auto rowY=[](Entry e){int start=e.item<4?0:std::min(e.item-1,e.count-4);return 10+15*(e.item-start);};
auto pixels=[&](Entry e,const std::string& value,bool editing){auto c=display->render();int y=rowY(e),x=125-c.menuAdvanceWidth(value);auto label=HardwareDisplay::listLabel(c,e.label+":",value);y=std::min(y,64-std::max(c.menuInkHeight(label),c.menuInkHeight(value)));OledCanvas glyph;glyph.menuText(x,y,value);bool ok=true;
    for(int yy=y;yy<std::min(64,y+11);++yy)for(int xx=std::max(0,x);xx<127;++xx)ok &= c.pixels[yy][xx]==(glyph.pixels[yy][xx]?0:255);
    for(int xx=0;xx<128;++xx)ok &= c.pixels[y-1][xx]==(editing && xx<x-2?0:255);
    return ok;};
auto capture=[&](const std::string& name){auto c=display->render();std::ofstream out(dir+"/setup-secondary-"+name+".pgm",std::ios::binary);out<<"P5\n128 64\n255\n";out.write(reinterpret_cast<char*>(c.pixels),128*64);};
bool nav=true;int navCount=0;
for(auto e:entries)for(auto* control:{encoder,static_cast<HardwareControl*>(display)}){auto first=e;first.item=0;clean(first);auto before=raw();if(e.item)drag(control,-22.f*e.item);int selected=e.page==M::TRACK_MENU_PAGE?m->trackMenuCursor.load():e.page==M::GLOBAL_SETUP_PAGE?m->globalCursor.load():e.page==M::UI_SETUP_PAGE?m->uiSetupCursor.load():e.page==M::MIDI_SETUP_PAGE?m->midiSetupCursor.load():m->modMenuCursor.load();nav &= selected==e.item && !m->uiField.load() && raw()==before && APP->history->actionIndex==0;++navCount;}
check(nav,std::to_string(navCount)+" secondary menu encoder/OLED navigation cases preserve all parameters and history");
bool values=true,spacing=true;int valueCount=0,previewCount=0;
for(auto e:entries){clean(e);if(e.id<0){capture(string::f("page%d-item%d-trk%d-lane%d",e.page,e.item,e.track,e.lane));continue;}auto* q=m->getParamQuantity(e.id);
    for(int v=int(q->getMinValue());v<=int(q->getMaxValue());++v){m->cancelDraft();m->uiField.store(0);m->params[e.id].setValue(float(v));std::string text=expectedValue(e,v,false);values &= pixels(e,text,false);std::string name=e.label+":";auto label=HardwareDisplay::listLabel(OledCanvas{},name,text);spacing &= OledCanvas{}.menuWidth(label)+OledCanvas{}.menuWidth(text)+5<=124;++valueCount;}
    if(q->getRange()>1)for(auto* control:{encoder,static_cast<HardwareControl*>(display)}){clean(e);m->params[M::ENC_CONFIRM].setValue(1);int base=int(q->getMinValue());m->params[e.id].setValue(float(base));auto before=raw();click(control);
        for(int v=base+1;v<=int(q->getMaxValue());++v){drag(control,-22);std::string text=expectedValue(e,v,true);bool ok=m->uiDraftParam==e.id && m->uiValue(e.id)==v && raw()==before && !APP->history->actionIndex && pixels(e,text,true);values &= ok;auto label=HardwareDisplay::listLabel(OledCanvas{},e.label+":",text);spacing &= OledCanvas{}.menuWidth(label)+OledCanvas{}.menuWidth(text)+5<=124;++previewCount;}
    }
    clean(e);m->params[e.id].setValue(q->getMinValue());capture(string::f("page%d-item%d-trk%d-lane%d-min",e.page,e.item,e.track,e.lane));m->params[e.id].setValue(q->getMaxValue());capture(string::f("page%d-item%d-trk%d-lane%d-max",e.page,e.item,e.track,e.lane));
}
check(values,std::to_string(valueCount)+" applied and "+std::to_string(previewCount)+" actual pending secondary readouts show the expected value, question mark, selection and descenders");
check(spacing,"all secondary values and pending suffixes fit without overlapping mixed-case labels");
bool edits=true;int editCount=0;
for(auto e:entries)if(e.id>=0)for(int confirm=0;confirm<2;++confirm)for(auto* control:{encoder,static_cast<HardwareControl*>(display)}){clean(e);m->params[M::ENC_CONFIRM].setValue(float(confirm));auto* q=m->getParamQuantity(e.id);m->params[e.id].setValue(q->getMinValue());auto before=raw();click(control);auto after=before;after[e.id]=q->getMinValue()+1;
    if(q->getRange()>1){drag(control,confirm?-22.f:-220.f);if(!confirm){after[e.id]=m->params[e.id].getValue();edits &= after[e.id]>q->getMinValue() && after[e.id]<=q->getMaxValue();}if(confirm){edits &= raw()==before && pendingConfirmation(m) && !APP->history->actionIndex;click(control);}else edits &= !pendingConfirmation(m);}
    bool applied=raw()==after && !pendingConfirmation(m) && APP->history->actionIndex==1;if(!applied)check(false,string::f("secondary edit page%d item%d track%d lane%d confirm%d applied=%d history=%d pending=%d",e.page,e.item,e.track,e.lane,confirm,raw()==after,APP->history->actionIndex,pendingConfirmation(m)));edits &= applied;APP->history->undo();bool undo=raw()==before;if(!undo)check(false,string::f("secondary Undo page%d item%d confirm%d",e.page,e.item,confirm));edits &= undo;APP->history->redo();bool redo=raw()==after;if(!redo)check(false,string::f("secondary Redo page%d item%d confirm%d",e.page,e.item,confirm));edits &= redo;++editCount;}
check(edits,std::to_string(editCount)+" secondary immediate/confirmed edits and binary clicks retain exact raw Undo/Redo");
bool resets=true;int resetCount=0;
for(auto e:entries)if(e.id>=0)for(int field=0;field<2;++field){clean(e);auto* q=m->getParamQuantity(e.id);q->setValue(q->defaultValue==q->getMinValue()?q->getMaxValue():q->getMinValue());m->uiField.store(field);auto before=raw();widget::EventContext ctx;event::DragStart start;start.context=&ctx;start.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragStart(start);encoder->heldSince=system::getTime()-1.1;encoder->step();event::DragEnd end;end.context=&ctx;end.button=GLFW_MOUSE_BUTTON_LEFT;encoder->onDragEnd(end);auto after=before;after[e.id]=q->defaultValue;resets &= raw()==after && APP->history->actionIndex==1;APP->history->undo();resets &= raw()==before;APP->history->redo();resets &= raw()==after;++resetCount;}
check(resets,std::to_string(resetCount)+" selected secondary settings long resets preserve all other raw values and exact Undo/Redo");
Entry reference=entries[1];clean(reference);m->params[M::outputTrackParam(0,M::SLIDER_OCTAVES)].setValue(3);capture("reference-track1");click(encoder);m->params[M::ENC_CONFIRM].setValue(1);drag(encoder,22);capture("reference-track1-preview");
clean(entries[0]);m->workflowUntil=system::getTime()+2;m->workflowMessage="COPIED TRK 1";capture("track-copy-notice");auto c=display->render();OledCanvas text;text.text(1,59,"COPIED TRK 1");bool message=true;for(int y=59;y<64;++y)for(int x=0;x<128;++x)message &= c.pixels[y][x]==text.pixels[y][x];check(message,"track copy feedback stays below the four menu rows without clipping");
clean(entries[0]);for(int id=0;id<M::NUM_PARAMS;++id)m->params[id].setValue(original[id]);m->dataFromJson(originalData.get());for(auto* a:APP->history->actions)delete a;APP->history->actions=std::move(savedHistory);APP->history->savedIndex=savedIndex;APP->history->actionIndex=savedActionIndex;APP->engine->bypassModule(m,false);
