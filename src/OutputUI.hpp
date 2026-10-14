bool outputPage(Metropolix* m) {return m && (m->uiPage.load()==Metropolix::A_PAGE || m->uiPage.load()==Metropolix::B_PAGE);}
int outputIndex(Metropolix* m) {return m->uiPage.load()==Metropolix::B_PAGE?1:0;}
bool invalidateOutputDraft(Metropolix* m,int n) {
    if(!m || m->outputDraft[n].load()<0)return false;
    if(m->params[Metropolix::OUTPUT_SOURCE_A+n].getValue()==m->outputDraftBase[n].load())return false;
    m->outputDraft[n].store(-1);return true;
}
void invalidateOutputDrafts(Metropolix* m) {if(m)for(int n=0;n<2;++n)invalidateOutputDraft(m,n);}
int shownOutputSource(Metropolix* m) {int n=outputIndex(m),draft=m->outputDraft[n].load();return draft>=0?draft:m->integer(Metropolix::OUTPUT_SOURCE_A+n);}
void outputTurn(Metropolix* m,int turns) {
    if(!outputPage(m))return;
    int n=outputIndex(m);invalidateOutputDraft(m,n);
    if(m->outputDraft[n].load()<0)m->outputDraftBase[n].store(m->params[Metropolix::OUTPUT_SOURCE_A+n].getValue());
    auto selection=metro::outputSelection(shownOutputSource(m));
    if(!m->uiField.load())selection.kind=metro::clamp(selection.kind+turns,0,int(metro::OutputKindCount)-1);
    else {int count=metro::outputTargetCount(selection.kind);if(!count)return;selection.target=metro::clamp(selection.target+turns,0,count-1);}
    int source=metro::outputSource(selection.kind,selection.target);
    m->outputDraft[n].store(source==m->integer(Metropolix::OUTPUT_SOURCE_A+n)?-1:source);
}
void outputEncoderPress(Metropolix* m) {
    if(!outputPage(m))return;
    int n=outputIndex(m);
    if(invalidateOutputDraft(m,n))return; // Consume a stale confirmation without changing its field.
    int draft=m->outputDraft[n].load();
    if(draft>=0){setWithHistory(m->getParamQuantity(Metropolix::OUTPUT_SOURCE_A+n),draft);m->outputDraft[n].store(-1);}
    auto selection=metro::outputSelection(m->integer(Metropolix::OUTPUT_SOURCE_A+n));
    m->uiField.store(metro::outputTargetCount(selection.kind)?1-m->uiField.load():0);
}
void resetOutput(Metropolix* m,bool record=true) {
    if(!outputPage(m))return;
    int n=outputIndex(m),id=Metropolix::OUTPUT_SOURCE_A+n;
    std::vector<int> ids{id};std::vector<float> before{m->params[id].getValue()};
    m->params[id].setValue(m->getParamQuantity(id)->defaultValue);m->outputDraft[n].store(-1);m->uiField.store(0);
    if(record)parameterHistory(m,ids,before,"reset output assignment");
}
void appendOutputMenu(ui::Menu* menu,Metropolix* m) {
    if(!outputPage(m))return;
    int n=outputIndex(m),id=Metropolix::OUTPUT_SOURCE_A+n;
    menu->addChild(createMenuLabel(n?"Output B routing":"Output A routing"));
    auto choose=[m,n,id](int source){m->outputDraft[n].store(-1);setWithHistory(m->getParamQuantity(id),source);};
    for(int kind=0;kind<metro::OutputKindCount;++kind){
        int count=metro::outputTargetCount(kind);
        if(!count){int source=metro::outputSource(kind);menu->addChild(createCheckMenuItem(metro::outputKindName(kind),"",[m,id,source]{return m->integer(id)==source;},[choose,source]{choose(source);}));}
        else menu->addChild(createSubmenuItem(metro::outputKindName(kind),"",[m,id,kind,count,choose](ui::Menu* child){
            for(int target=0;target<count;++target){int source=metro::outputSource(kind,target);child->addChild(createCheckMenuItem(std::string(metro::outputIsMod(kind)?"MOD ":"Track ")+std::to_string(target+1),"",[m,id,source]{return m->integer(id)==source;},[choose,source]{choose(source);}));}
        }));
    }
    menu->addChild(new ui::MenuSeparator);
    menu->addChild(createMenuItem("Discard pending choice","",[m,n]{m->outputDraft[n].store(-1);}));
    menu->addChild(createMenuItem("Reset output assignment","",[m]{resetOutput(m);}));
}
