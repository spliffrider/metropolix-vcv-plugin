bool cvPage(Metropolix* m) {return m && !m->uiMod.load() && m->uiPage.load()==Metropolix::STAGE_PAGE && m->selectedRow.load()==Metropolix::CV;}
bool cvGate(Metropolix* m) {return m && !m->uiMod.load() && m->integer(Metropolix::cvTypeParam(m->selectedTrack.load()))==metro::GateCv;}
std::vector<int> cvParams(Metropolix* m,bool all=false) {
    std::vector<int> ids;if(!m)return ids;
    for(int s=0;s<8;++s)if(all || m->uiAlt.load() || s==m->selectedStage.load())ids.push_back(Metropolix::stageParam(m->selectedTrack.load(),s,Metropolix::CV));
    return ids;
}
void resetCv(Metropolix* m,bool all=false,bool record=true) {
    auto ids=cvParams(m,all);std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->params[id].setValue(0);}
    if(record)parameterHistory(m,ids,before,"reset stage CV");
}
void invertCv(Metropolix* m) {
    auto ids=cvParams(m,true);std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->params[id].setValue(m->getParamQuantity(id)->getValue()>=1.5f?0:5);}
    parameterHistory(m,ids,before,"invert CV gates");
}
void rotateCv(Metropolix* m,int turns,bool record=false) {
    auto ids=cvParams(m,true);std::vector<float> before;
    for(int id:ids)before.push_back(m->params[id].getValue());
    for(int s=0;s<8;++s)m->params[ids[s]].setValue(before[metro::modulo(s-turns,8)]);
    if(record)parameterHistory(m,ids,before,"rotate CV gates");
}
void cvEncoderPress(Metropolix* m) {
    if(cvGate(m)){if(m->uiAlt.load())invertCv(m);else {auto* q=m->getParamQuantity(Metropolix::stageParam(m->selectedTrack.load(),m->selectedStage.load(),Metropolix::CV));setWithHistory(q,q->getValue()>=1.5f?0:5);}return;}
    int t=m->selectedTrack.load(),selected=m->selectedStage.load();int next=(m->integer(Metropolix::cvRampParam(t,selected))+1)%3;
    std::vector<int> ids;std::vector<float> before;
    for(int s=0;s<8;++s)if(m->uiAlt.load() || s==selected){int id=Metropolix::cvRampParam(t,s);ids.push_back(id);before.push_back(m->params[id].getValue());m->params[id].setValue(next);}
    parameterHistory(m,ids,before,"change CV transition");
}
void appendCvMenu(ui::Menu* menu,Metropolix* m) {
    if(!m || m->uiMod.load())return;
    int t=m->selectedTrack.load(),s=m->selectedStage.load();
    menu->addChild(new ui::MenuSeparator);menu->addChild(createMenuLabel("Track CV"));
    const char* types[]={"Bipolar ±","Unipolar +","Gate Toggle"};
    for(int type=0;type<3;++type)menu->addChild(createCheckMenuItem(types[type],"",[m,t,type]{return int(m->effectiveCvType(t))==type;},[m,t,type]{setWithHistory(m->getParamQuantity(Metropolix::cvTypeParam(t)),type);},type==0 && m->integer(Metropolix::outputTrackParam(t,Metropolix::VELOCITY_SOURCE))==3));
    if(!cvPage(m))return;
    if(cvGate(m)) {
        menu->addChild(createMenuItem("Invert gate pattern","",[m]{invertCv(m);}));
        menu->addChild(createMenuItem("Rotate gates forward","",[m]{rotateCv(m,1,true);}));
        menu->addChild(createMenuItem("Rotate gates backward","",[m]{rotateCv(m,-1,true);}));
    } else {
        const char* ramps[]={"Step","sRamp (whole stage)","pRamp (one pulse)"};
        for(int r=0;r<3;++r)menu->addChild(createCheckMenuItem(ramps[r],"",[m,t,s,r]{return m->integer(Metropolix::cvRampParam(t,s))==r;},[m,t,s,r]{setWithHistory(m->getParamQuantity(Metropolix::cvRampParam(t,s)),r);}));
        menu->addChild(createMenuItem("Apply this transition to all stages","",[m,t,s]{std::vector<int> ids;std::vector<float> before;int r=m->integer(Metropolix::cvRampParam(t,s));for(int k=0;k<8;++k){int id=Metropolix::cvRampParam(t,k);ids.push_back(id);before.push_back(m->params[id].getValue());m->params[id].setValue(r);}parameterHistory(m,ids,before,"set all CV transitions");}));
    }
    menu->addChild(createMenuItem("Reset this stage CV","",[m]{bool alt=m->uiAlt.load();m->uiAlt.store(false);resetCv(m);m->uiAlt.store(alt);}));
    menu->addChild(createMenuItem("Reset all stage CV","",[m]{resetCv(m,true);}));
}
