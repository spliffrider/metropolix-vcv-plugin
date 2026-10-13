bool trackMenuPage(Metropolix* m){return m && m->uiPage.load()==Metropolix::TRACK_MENU_PAGE;}
constexpr int TrackMenuCount=16;
const char* trackMenuName(int item){const char* names[]={"Transpose","Slider Octaves","Slider Dir","Pulse Count","Gate Type","Gate Stretching","Ratchets","Rest Pitch","Global Reset","Velocity","MIDI Channel","CV to CC","Slide Toggle CC","Slide Amount CC","Slide Amount","Init Trk Defaults"};return names[metro::clamp(item,0,TrackMenuCount-1)];}
int trackMenuParam(Metropolix* m,int item=-1){
    if(!m)return -1;
    int t=m->selectedTrack.load();
    const int ids[]={Metropolix::trackParam(t,Metropolix::TRANSPOSE),Metropolix::outputTrackParam(t,Metropolix::SLIDER_OCTAVES),Metropolix::outputTrackParam(t,Metropolix::SLIDER_DIRECTION),Metropolix::trackBehaviorParam(t,Metropolix::PULSE_SOURCE),Metropolix::trackBehaviorParam(t,Metropolix::GATE_SOURCE),Metropolix::extraParam(t,Metropolix::GATE_STRETCH),Metropolix::extraParam(t,Metropolix::RATCHET_MODE),Metropolix::trackBehaviorParam(t,Metropolix::REST_PITCH),Metropolix::TRACK_GLOBAL_RESET_1+t,Metropolix::outputTrackParam(t,Metropolix::VELOCITY_SOURCE),-1};
    int n=metro::clamp(item<0?m->trackMenuCursor.load():item,0,TrackMenuCount-1);return n<10?ids[n]:n<15?Metropolix::midiTrackParam(t,n-10):-1;
}
std::vector<int> trackResetParams(Metropolix* m){
    int t=m->selectedTrack.load();std::vector<int> ids;
    for(int n=0;n<Metropolix::MIDI_TRACK_STRIDE;++n)ids.push_back(Metropolix::midiTrackParam(t,n));
    for(int k=0;k<Metropolix::TRACK_STRIDE;++k)ids.push_back(Metropolix::trackParam(t,k));
    for(int k=0;k<Metropolix::EXTRA_STRIDE;++k)ids.push_back(Metropolix::extraParam(t,k));
    for(int k=0;k<3;++k){ids.push_back(Metropolix::accumParam(t,k));ids.push_back(Metropolix::outputTrackParam(t,k));ids.push_back(Metropolix::trackBehaviorParam(t,k));}
    ids.push_back(Metropolix::TRACK_GLOBAL_RESET_1+t);ids.push_back(Metropolix::accumModeParam(t));ids.push_back(Metropolix::slideTypeParam(t));ids.push_back(Metropolix::cvTypeParam(t));ids.push_back(Metropolix::swingParam(t));ids.push_back(Metropolix::gateScaleParam(t));
    for(int s=0;s<8;++s){for(int k=0;k<2;++k){ids.push_back(Metropolix::stageOption(t,s,k));ids.push_back(Metropolix::stageParam(t,s,Metropolix::PITCH_OVERRIDE+k));}ids.push_back(Metropolix::cvRampParam(t,s));}
    return ids;
}
void resetTrackMenu(Metropolix* m,bool all,bool record=true){
    if(!m)return;
    auto ids=all?trackResetParams(m):std::vector<int>{trackMenuParam(m)};std::vector<float> before;
    for(int id:ids){before.push_back(id<0?0:m->params[id].getValue());if(id>=0 && m->ctrlOwner(id)<0)m->getParamQuantity(id)->setValue(m->getParamQuantity(id)->defaultValue);}
    if(record){std::vector<int> valid;std::vector<float> previous;for(size_t i=0;i<ids.size();++i)if(ids[i]>=0){valid.push_back(ids[i]);previous.push_back(before[i]);}parameterHistory(m,valid,previous,all?"initialize track":"reset track menu item");}
    m->uiField.store(0);
}
void trackMenuEncoderPress(Metropolix* m){
    int id=trackMenuParam(m);if(id<0){resetTrackMenu(m,true);return;}
    auto* q=m->getParamQuantity(id);
    if(m->ctrlOwner(id)>=0)return;
    if(q->maxValue-q->minValue==1){setWithHistory(q,q->getValue()==q->minValue?q->maxValue:q->minValue);m->uiField.store(0);}
    else m->uiField.store(1-m->uiField.load());
}
