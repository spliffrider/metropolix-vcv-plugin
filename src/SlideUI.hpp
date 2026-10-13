#pragma once
void resetSlide(Metropolix* m,bool record=true) {
    if(!m || m->uiMod.load())return;
    int t=m->selectedTrack.load();std::vector<int> ids;std::vector<float> before;
    for(int id:{Metropolix::trackParam(t,Metropolix::SLIDE_TIME),Metropolix::slideTypeParam(t)}) {
        if(m->ctrlOwner(id)>=0)continue;
        ids.push_back(id);before.push_back(m->params[id].getValue());m->params[id].setValue(m->getParamQuantity(id)->defaultValue);
    }
    if(record)parameterHistory(m,ids,before,"reset slide amount and type");
}
void appendSlideMenu(ui::Menu* menu,Metropolix* m) {
    if(!m || m->uiMod.load())return;
    menu->addChild(new ui::MenuSeparator);menu->addChild(createMenuLabel("Slide"));int t=m->selectedTrack.load();
    const char* names[]={"Tempo","Analog","Acid"};
    for(int type=0;type<3;++type)menu->addChild(createCheckMenuItem(names[type],"",[m,t,type]{return m->integer(Metropolix::slideTypeParam(t))==type;},[m,t,type]{setWithHistory(m->getParamQuantity(Metropolix::slideTypeParam(t)),type);}));
    menu->addChild(createMenuItem("Reset amount and type","",[m]{resetSlide(m);}));
}
