// Included inside HardwareDisplay. All methods only render current UI state.
std::string scaleValueName(int shown) const {
    if(shown==25)return "Whole Half Dim";
    if(shown==26)return "Half Whole Dim";
    return shown==1?"Ionian":shown==2?"Aeolian":metro::scaleName(shown);
}
std::string scaleTitle(int shown) const {
    if(metro::userScale(shown))return string::f("USER (BANK %d)",(shown-metro::FactoryScaleCount)/10+1);
    const char* banks[]={"MAJOR","MEL MIN","HARM MIN","MISC","WORLD","MAJ CHORD","MIN CHORD"};
    return std::string("SCALE (")+banks[metro::scaleBrowsePosition(shown)/7]+")";
}
void keyboard(OledCanvas& c,bool editing) const {
    auto* m=metroModule;
    int shown=integer(Metropolix::SCALE,1),root=integer(m?m->scaleRootParam(shown):Metropolix::ROOT);
    auto sc=m?m->uiScale(shown):metro::scaleAt(shown);
    const int whites[]={0,2,4,5,7,9,11},blacks[]={1,3,6,8,10},offsets[]={5,17,35,46,57};
    int x=editing?56:59,y=editing?21:29,height=editing?30:25,blackHeight=editing?18:16;
    auto enabled=[&](int note){for(int i=0;i<sc.count;++i)if(metro::modulo(note-root,12)==sc.notes[i])return true;return false;};
    auto shaded=[&](int left,int top,int w,int h,bool flip=false){for(int j=top;j<top+h;++j)for(int i=left;i<left+w;++i)c.dot(i,j,(((i+j)&1)^flip)?255:0);};
    for(int k=0;k<7;++k) {
        if(enabled(whites[k]))c.fill(x+k*10,y,9,height);else shaded(x+k*10,y,9,height);
        if(whites[k]==root)c.fill(x+k*10+2,y+height-7,5,5,0);
    }
    for(int k=0;k<5;++k) {
        int left=x+offsets[k];c.fill(left,y,7,blackHeight,0);
        // The native G-sharp key uses the opposite checker phase.
        if(!enabled(blacks[k]))shaded(left+1,y,5,blackHeight-1,blacks[k]==8);
        if(blacks[k]==root)c.fill(left+2,y+blackHeight-6,3,3);
    }
    if(editing) {
        int note=metro::modulo(integer(Metropolix::SCALE_NOTE)+root,12),center=x+4;
        for(int k=0;k<7;++k)if(whites[k]==note)center=x+k*10+4;
        for(int k=0;k<5;++k)if(blacks[k]==note)center=x+offsets[k]+3;
        c.arrow(center,y-5);
    }
}
void scale(OledCanvas& c) const {
    auto* m=metroModule;
    int shown=integer(Metropolix::SCALE,1),locked=integer(Metropolix::SCALE_LOCK,-1),field=m?m->uiField.load():1;
    bool editing=m && m->scaleEditing.load(),user=metro::userScale(shown),pending=pendingConfirmation(m);
    bool queued=m && requiresConfirmation(m,screenParam(m));
    if(editing) {
        c.captionText(2,4,"EDITING");
        c.captionText(57,4,metro::scaleName(shown)+(pending && field?" ?":""));
        c.box(3,16,24,9);c.captionText(5,18,"EXIT");c.scaleLongHint(29,16);
        if(field)c.scaleCorners(53,0,75,54);
    } else {
        c.captionText(2,4,scaleTitle(shown));
        int left=pending?24:queued?(user?52:38):(user?44:30);
        c.valueFit(left,22,126-left,scaleValueName(shown)+(pending && field?" ?":""));
        if(field) {
            c.scaleCorners(0,0,128,27);
            int owner=m?m->ctrlOwner(screenParam(m)):-1;
            if(owner>=0)c.ownerHint(1,14,owner);
            else {
                c.turnHint(3,17,queued,pending);
                if(!pending){c.scaleLockHint(queued?24:17,17);if(user)c.scaleLongHint(queued?39:32,16);}
            }
        } else c.pressHint(3,18);
        if(locked>=0) {
            c.scaleStatus(locked!=shown);
            if(locked==shown && metro::userScale(locked))c.captionText(96,4,"MOD");
        }
    }
    c.captionText(2,32,"ROOT");
    std::string root=notes[integer(m?m->scaleRootParam(shown):Metropolix::ROOT)];
    c.valueFit(23,51,27,root+(pending && !field?" ?":""));
    if(!field)c.scaleCorners(0,29,54,25);
    c.box(3,43,19,9);c.captionText(5,45,"ALT",255,1);
    keyboard(c,editing);
}
