void resetClockOutput(Metropolix* m,bool record=true) {
    if(!m)return;
    std::vector<int> ids{Metropolix::CLOCK_OUT_DIV,Metropolix::CLOCK_OUT_OFFSET};
    std::vector<float> before;
    for(int id:ids){before.push_back(m->params[id].getValue());m->params[id].setValue(m->getParamQuantity(id)->defaultValue);}
    if(record)parameterHistory(m,ids,before,"reset clock output");
}
void appendClockMenu(ui::Menu* menu,Metropolix* m) {
    if(!m)return;
    menu->addChild(createMenuLabel("Global CLK output"));
    menu->addChild(createMenuItem("Open CLK output settings","",[m]{m->uiPage.store(Metropolix::CLOCK_OUT_PAGE);m->uiField.store(0);m->uiAlt.store(false);m->uiChooseFunction.store(false);}));
    menu->addChild(createSubmenuItem("Clock division","",[m](ui::Menu* child){
        for(int div=1;div<=256;++div)child->addChild(createCheckMenuItem("/"+std::to_string(div),"",[m,div]{return m->integer(Metropolix::CLOCK_OUT_DIV)==div;},[m,div]{setWithHistory(m->getParamQuantity(Metropolix::CLOCK_OUT_DIV),div);}));
    }));
    menu->addChild(createSubmenuItem("Clock offset","",[m](ui::Menu* child){
        for(int shift=0;shift<m->integer(Metropolix::CLOCK_OUT_DIV);++shift)child->addChild(createCheckMenuItem(std::to_string(shift),"",[m,shift]{return m->getParamQuantity(Metropolix::CLOCK_OUT_OFFSET)->getValue()==shift;},[m,shift]{setWithHistory(m->getParamQuantity(Metropolix::CLOCK_OUT_OFFSET),shift);}));
    }));
    menu->addChild(createMenuItem("Reset CLK output","",[m]{resetClockOutput(m);}));
}
