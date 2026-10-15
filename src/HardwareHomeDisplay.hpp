// Included inside HardwareDisplay. These overviews never write musical state.
metrodisplay::MenuGlyph homeGlyph(unsigned char ch,bool caption) const {
    if(!caption)return metrodisplay::homeNoteGlyph(ch);
    auto glyph=metrodisplay::homeCaptionGlyph(static_cast<unsigned char>(std::toupper(ch)));
    return glyph.width<0?OledCanvas::chainCaption(ch):glyph;
}
int homeWidth(const std::string& text,bool caption=true) const {
    int cursor=0,edge=0;
    for(unsigned char ch:OledCanvas::glyphText(text)){auto glyph=homeGlyph(ch,caption);if(glyph.width)edge=cursor+(caption?metrodisplay::homeCaptionBearing(ch):1)+glyph.width;cursor+=glyph.advance;}
    return edge;
}
void homeText(OledCanvas& c,int x,int y,const std::string& text,bool caption=true) const {
    for(unsigned char ch:OledCanvas::glyphText(text)) {
        auto glyph=homeGlyph(ch,caption);
        int bearing=caption?metrodisplay::homeCaptionBearing(ch):1;
        for(int row=0;row<(caption?5:9);++row)for(int col=0;col<glyph.width;++col)if(glyph.rows[row]&(1<<(glyph.width-1-col)))c.dot(x+bearing+col,y+row);
        x+=glyph.advance;
    }
}
std::string homeScaleName(int shown) const {
    if(metro::userScale(shown))return string::f("USER %d-%d",(shown-49)/10+1,(shown-49)%10+1);
    // HARM MIN is observed in p29; the other compact aliases preserve meaning
    // within the header rather than truncating the full scale menu labels.
    const char* names[]={"CHROMATIC","IONIAN","AEOLIAN","DORIAN","MIXOLYD","MAJ PENT","MIN PENT","PHRYGIAN","LYDIAN","LOCRIAN","MEL MIN","DOR B2","LYD AUG","LYD DOM","MIX B6","AEOL B5","SUPER LOC","HARM MIN","LOC NAT6","ION #5","DOR #4","PHRYG DOM","LYD #2","ALTERED","WHOLE","WH HALF","HALF WH","MIN BLUES","BHAIRAV","HUNG MIN","CHINESE","HIRAJOSHI","IN-SEN","KUMOI","PELOG","MAJ TRIAD","MAJ 6","MAJ 7","MAJ7 B5","MAJ7 #5","DOM 7","MAJ 9","MIN TRIAD","MIN 6","MIN 7","MIN7 B5","DIM 7","MIN 9","MIN 11"};
    return names[metro::clamp(shown,0,48)];
}
std::string homeNoteName(int shown,int root,int note) const {
    auto* m=metroModule;auto scale=m?m->uiScale(shown):metro::scaleAt(shown);
    // Spell seven-note factory scales by their letter degrees (e.g. C harmonic
    // minor has Eb). This is a desktop spelling policy; a single source image
    // cannot establish the firmware's complete enharmonic policy.
    if(!metro::userScale(shown) && scale.count==7) {
        const char* letters="CDEFGAB";const int natural[]={0,2,4,5,7,9,11};int first=0;
        while(first<6 && letters[first]!=notes[metro::modulo(root,12)][0])++first;
        for(int degree=0;degree<7;++degree)if(metro::modulo(root+scale.notes[degree],12)==note) {
            int letter=(first+degree)%7,accidental=metro::modulo(note-natural[letter]+6,12)-6;
            if(std::abs(accidental)<=1)return std::string(1,letters[letter])+(accidental<0?"b":accidental>0?"#":"");
        }
    }
    return notes[note];
}
int homeNotePitch(int track,int stage,int shown) const {return baseSliderPitch(track,stage,shown,false,true);}
void homeKnob(OledCanvas& c,int x,int y,float position,bool waiting) const {
    const uint16_t arc[]={992,3096,4100,8194,8194,16385,16385,16385,16385,16385,8194,8194,4100};
    c.bitmap(x-7,y-7,15,13,arc);
    float angle=metro::clamp(position,-1.f,1.f)*2.35619449f;
    c.line(x,y,x+int(std::round(std::sin(angle)*6)),y-int(std::round(std::cos(angle)*6)));
    if(waiting)c.dot(x,y-10);
}
void home(OledCanvas& c) const {
    auto* m=metroModule;int h=m?m->uiHome.load():0,lane=m?m->selectedLane.load():0;
    pages(c,h);
    if(h==0) {
        for(int row=0;row<3;++row) {
            bool mod=row==2;
            int length=integer(mod?Metropolix::modParam(lane,Metropolix::MOD_LENGTH):Metropolix::trackParam(row,Metropolix::LENGTH),8);
            int offset=integer(mod?Metropolix::modParam(lane,Metropolix::MOD_STAGE_OFFSET):Metropolix::trackParam(row,Metropolix::OFFSET));
            int position=m?(mod?m->modStage[lane].load():m->playStage[row].load()):-1;
            bool mute=mod?(m && m->modLaneMuted(lane)):integer(Metropolix::trackParam(row,Metropolix::MUTE));
            for(int s=0;s<8;++s) {
                int x=2+16*s,y=6+17*row;bool first=s==offset;
                if(metro::modulo(s-offset,8)>=length){c.dot(x+5,y+5);continue;}
                if(!mod && effectiveSkip(row,s)) {
                    for(int width=first?-1:0;width<=(first?1:0);++width){c.line(x+1,y+1+width,x+9,y+9+width);c.line(x+1,y+9+width,x+9,y+1+width);}
                } else {
                    c.box(x,y,11,11);if(first)c.box(x+1,y+1,9,9);if(s==position)c.fill(x,y,11,11);
                }
                if(mute)c.hatch(x,y,11,11);
            }
        }
    } else if(h==1) {
        for(int track=0;track<2;++track) {
            int shown=m?previewScale(track):1,root=integer(m?m->scaleRootParam(shown):Metropolix::ROOT),y=6+track*24;
            auto scale=m?m->uiScale(shown):metro::scaleAt(shown);
            std::string title=std::string(notes[metro::modulo(root,12)])+" "+homeScaleName(shown);
            while(!title.empty() && homeWidth(title)>78)title.pop_back();homeText(c,2,y,title);
            int pre=m?metro::nearestDegree(m->pitchPre[track].load()*12.f,scale):0,post=m?int(std::round(m->pitchPost[track].load()*12.f)):0;
            std::string preLabel=string::f("%+d°",pre),postLabel=string::f("%+d",post);
            homeText(c,103-homeWidth(preLabel),y,preLabel);homeText(c,124-homeWidth(postLabel),y,postLabel);
            for(int stage=0;stage<8;++stage) {
                int pitch=m?homeNotePitch(track,stage,shown):0,note=metro::modulo(pitch,12),center=7+stage*16;
                auto label=homeNoteName(shown,root,note);homeText(c,center-homeWidth(label,false)/2,y+10,label,false);
                if(integer(Metropolix::stageParam(track,stage,Metropolix::PITCH_OVERRIDE),-1)>=0)c.fill(center,y+7,2,2);
                if(used(track,stage) && !effectiveSkip(track,stage))for(int x=stage*16+2;x<stage*16+13;++x)if((m && m->playStage[track].load()==stage) || (x-stage*16)%2)c.dot(x,y+19);
            }
        }
    } else if(h==2) {
        for(int i=0;i<2;++i) {
            int x=i?21:10,y=i?38:14,id=Metropolix::ctrlParam(i,Metropolix::CTRL_VALUE);
            homeKnob(c,x,y,value(id),m && m->waitingPickup(id));homeText(c,x-2,y+7,std::to_string(i+1));
        }
        for(int stage=0;stage<8;++stage) {
            int x=37+12*stage;homeText(c,x-2,4,std::to_string(stage+1));
            c.line(x,14,x,32);int handle=32-int(std::round(metro::clamp(value(Metropolix::PITCH+stage),0.f,1.f)*18));c.line(x-3,handle,x+3,handle);
            if(m && m->waitingPickup(Metropolix::PITCH+stage))c.dot(x,11);
            auto count=std::to_string(integer(Metropolix::PULSES+stage,1));homeText(c,x-homeWidth(count)/2,39,count);
            int gate=integer(Metropolix::GATES+stage,2),left=x-4;
            if(gate==3)c.fill(left,50,9,4);
            else if(gate==2){c.fill(left,50,4,4);c.fill(left+5,50,4,4);}
            else {c.box(left,50,9,4);if(gate==1)c.fill(left,50,4,4);}
        }
    } else {
        for(int column=0;column<10;++column) {
            bool cv=column<2;int laneIndex=column-2,x=cv?3+12*column:33+12*laneIndex;
            auto number=std::to_string(cv?column+1:laneIndex+1);homeText(c,x+4-homeWidth(number)/2,4,number);
            float voltage=m?(cv?m->playCv[column].load():m->modVoltage[laneIndex].load()):0;
            bool gate=!cv && m && m->effectiveModType(laneIndex)==metro::ModGate;
            if(gate){c.box(x,31,7,7);if(voltage>=1.5f)c.fill(x+2,33,3,3);}
            else {
                c.box(x,11,7,27);c.dot(x+2,24);c.dot(x+4,24);
                int height=int(std::round(std::abs(metro::clamp(voltage,-5.f,5.f))*2.4f));
                if(height)c.fill(x+2,voltage>0?25-height:24,3,height);
            }
            if(!cv && integer(Metropolix::modParam(laneIndex,Metropolix::MOD_DESTINATION))) {
                int target=integer(Metropolix::modParam(laneIndex,Metropolix::MOD_ASSIGNMENT));
                std::string label=metro::modGlobalDestination(static_cast<metro::ModDestination>(integer(Metropolix::modParam(laneIndex,Metropolix::MOD_DESTINATION))))?"G":target==2?"12":std::to_string(target+1);
                homeText(c,x+4-homeWidth(label)/2,41,label);
            }
            bool a=integer(Metropolix::OUTPUT_SOURCE_A)==column,b=integer(Metropolix::OUTPUT_SOURCE_B,1)==column;std::string label=a?(b?"AB":"A"):(b?"B":"");
            if(!label.empty())homeText(c,x+4-homeWidth(label)/2,48,label);
        }
        for(int y=10;y<52;y+=2)c.dot(26,y);
    }
    footer(c);
}
