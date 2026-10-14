bool simpleStagePage(Metropolix* m) {
    if(!m || m->uiChooseFunction.load() || m->uiMod.load() || m->uiPage.load()!=Metropolix::STAGE_PAGE)return false;
    int row=m->selectedRow.load();return row==Metropolix::RATCHETS || row==Metropolix::SLIDE || row==Metropolix::SKIP;
}
bool binaryStagePage(Metropolix* m){return simpleStagePage(m) && m->selectedRow.load()!=Metropolix::RATCHETS;}
std::vector<int> simpleStageIds(Metropolix* m,bool all=false) {
    std::vector<int> ids;if(!simpleStagePage(m))return ids;
    for(int s=0;s<8;++s)if(all || m->uiAlt.load() || s==m->selectedStage.load()) {
        int id=Metropolix::stageParam(m->selectedTrack.load(),s,m->selectedRow.load());if(m->ctrlOwner(id)<0)ids.push_back(id);
    }return ids;
}
bool simpleStageLongEnabled(Metropolix* m){return simpleStagePage(m) && (m->selectedRow.load()==Metropolix::RATCHETS || m->uiAlt.load());}
void resetSimpleStages(Metropolix* m,bool all=false,bool record=true) {
    if(!simpleStagePage(m))return;m->cancelDraft();auto ids=simpleStageIds(m,all);std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(m->getParamQuantity(id)->defaultValue);}
    if(record)parameterHistory(m,ids,before,"reset stage pattern");
}
void rotateStagePattern(Metropolix* m,int shift,bool record=true) {
    if(!binaryStagePage(m))return;m->cancelDraft();std::array<float,8> pattern;
    int track=m->selectedTrack.load(),row=m->selectedRow.load();
    for(int s=0;s<8;++s)pattern[s]=m->params[Metropolix::stageParam(track,s,row)].getValue();
    auto ids=simpleStageIds(m,true);std::vector<float> before;
    for(int id:ids)before.push_back(m->params[id].getValue());
    for(int s=0;s<8;++s){int id=Metropolix::stageParam(track,s,row);if(m->ctrlOwner(id)<0)m->getParamQuantity(id)->setValue(pattern[metro::modulo(s-shift,8)]);}
    if(record)parameterHistory(m,ids,before,"rotate stage pattern");
}
void invertStagePattern(Metropolix* m) {
    if(!binaryStagePage(m))return;m->cancelDraft();auto ids=simpleStageIds(m,true);std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->getParamQuantity(id)->setValue(1-m->integer(id));}
    parameterHistory(m,ids,before,"invert stage pattern");
}
void ratchetStageDrag(Metropolix* m,int selected,const std::vector<int>& ids,const std::vector<float>& before) {
    if(!simpleStagePage(m) || binaryStagePage(m) || !m->uiAlt.load())return;
    auto origin=std::find(ids.begin(),ids.end(),selected);if(origin==ids.end())return;
    float delta=m->params[selected].getValue()-before[origin-ids.begin()];
    for(int id:simpleStageIds(m))if(id!=selected){auto it=std::find(ids.begin(),ids.end(),id);if(it!=ids.end())m->getParamQuantity(id)->setValue(before[it-ids.begin()]+delta);}
}
void appendSimpleStageMenu(ui::Menu* menu,Metropolix* m) {
    if(!simpleStagePage(m))return;menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuLabel(m->selectedRow.load()==Metropolix::RATCHETS?"Stage ratchets":m->selectedRow.load()==Metropolix::SLIDE?"Stage slide pattern":"Stage skip pattern"));
    if(binaryStagePage(m)) {
        menu->addChild(createMenuItem("Rotate pattern forward","ALT + encoder turn",[m]{rotateStagePattern(m,1);}));
        menu->addChild(createMenuItem("Rotate pattern backward","ALT + encoder turn",[m]{rotateStagePattern(m,-1);}));
        menu->addChild(createMenuItem("Invert pattern","ALT + encoder press",[m]{invertStagePattern(m);}));
    }
    menu->addChild(createMenuItem("Reset this stage","",[m]{bool alt=m->uiAlt.load();m->uiAlt.store(false);resetSimpleStages(m);m->uiAlt.store(alt);}));
    menu->addChild(createMenuItem("Reset all stages","ALT + hold encoder",[m]{resetSimpleStages(m,true);}));
}
