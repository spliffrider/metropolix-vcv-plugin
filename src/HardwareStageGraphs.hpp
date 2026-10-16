// Included inside HardwareDisplay. Bar diagrams share the native nine-cell
// columns; their value/trigger ranges remain the model's existing parameters.
void stageTurnHint(OledCanvas& c,int id) const {
    if(requiresConfirmation(metroModule,id)){c.turnHint(3,16,true,pendingConfirmation(metroModule));return;}
    for(int i=0;i<4;++i){c.line(3+i,19-i,3+i,19+i);c.line(10+i,16+i,10+i,22-i);}
}
void stageValueField(OledCanvas& c,int id,const std::string& text) const {
    c.valueFit(2,11,36,text,false,true);c.fieldCorners(0,0,40,26);stageTurnHint(c,id);
}
void stageSelectionArrow(OledCanvas& c,int center,int top) const {
    // Retain contrast when the selected bar reaches the top of the graph.
    for(int row=0;row<3;++row)for(int dx=-2+row;dx<=2-row;++dx)c.dot(center+dx,top+row,c.pixels[top+row][center+dx]?0:255);
}
void stageOverrideGraph(OledCanvas& c,int track,int selected,int row) const {
    auto* m=metroModule;bool pitch=row==Metropolix::PITCH_OVERRIDE;
    auto id=[&](int stage){return Metropolix::stageParam(track,stage,row);};
    int current=integer(id(selected),-1);
    std::string label=current<0?"--":pitch?previewDisplayString(m,id(selected)):current==0?"Rest":string::f("%d%%",current);
    if(pitch && pendingConfirmation(m) && label.size()>2 && label.substr(label.size()-2)==" ?")label.resize(label.size()-2);
    stageValueField(c,id(selected),label);
    if(pitch){c.valueFit(2,37,40,"Sync",false,true);c.pressHint(3,43);}
    for(int stage=0;stage<8;++stage) {
        int x=42+stage*11,n=integer(id(stage),-1);bool active=stage==selected;
        if(n<0){for(int dx=0;dx<9;dx+=2)c.dot(x+dx,50);}
        else if(!pitch && n==0){c.line(x,50,x+8,50);c.line(x,49,x,51);c.line(x+8,49,x+8,51);}
        else {
            int top=pitch?(60-metro::clamp(n,0,60))*51/61:51-std::max(1,metro::clamp(n,1,100)*51/100);
            int height=51-top;c.box(x,top,9,height);if(active)c.fill(x,top,9,height);
        }
        if(active)stageSelectionArrow(c,x+4,1);
    }
}
void stageValueGraph(OledCanvas& c,int track,int selected,int row,bool detail) const {
    auto* m=metroModule;bool accum=row==Metropolix::ACCUMULATION,prob=row==Metropolix::PROBABILITY;
    auto id=[&](int stage){return Metropolix::stageParam(track,stage,row);};
    if(detail) {
        stageValueField(c,id(selected),prob?string::f("%d%%",integer(id(selected))):accum?string::f("%d°",integer(id(selected))):std::to_string(integer(id(selected))));
        if(prob || accum){int trigger=integer(Metropolix::stageOption(track,selected,prob?Metropolix::PROB_TARGET:Metropolix::ACCUM_TRIGGER));c.valueFit(1,37,40,trigger==0?"Stage":trigger==1?"Pulse":"Ratch",false,true);c.pressHint(3,43,prob?2:3);}
    }
    for(int stage=0;stage<8;++stage) {
        int x=42+stage*11,n=integer(id(stage));bool active=detail && (m->uiAlt.load() || stage==selected);
        if(row==Metropolix::RATCHETS)for(int step=0;step<metro::clamp(n,1,8);++step){c.box(x,47-step*6,9,4);if(active)c.fill(x,47-step*6,9,4);}
        if(prob) {
            int height=std::max(3,metro::clamp(n,0,100)*46/100);c.box(x,46-height,9,height);if(active)c.fill(x,46-height,9,height);
            c.fill(x,48,9,3);if(integer(Metropolix::stageOption(track,stage,Metropolix::PROB_TARGET)))c.fill(x+4,48,1,3,0);
        }
        if(accum) {
            int height=std::abs(metro::clamp(n,-28,28))*22/28,y=n>=0?22-height:29;
            if(height){c.box(x,y,9,height);if(active)c.fill(x,y,9,height);}
            int trigger=metro::clamp(integer(Metropolix::stageOption(track,stage,Metropolix::ACCUM_TRIGGER)),0,2);const uint16_t marks[]={341,495,511};const uint16_t rows[]={marks[trigger],marks[trigger],marks[trigger]};c.bitmap(x,24,9,3,rows);
        }
        if(active)stageSelectionArrow(c,x+4,accum?0:1);
    }
}
