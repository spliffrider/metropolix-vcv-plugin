#include "OledCanvas.hpp"

struct HardwareDisplay : HardwareControl {
    int integer(int id,int fallback=0) const { return metroModule?metroModule->integer(id):fallback; }
    float value(int id,float fallback=0.f) const { return metroModule?metroModule->params[id].getValue():fallback; }
    bool used(int t,int s) const {
        int length=integer(Metropolix::trackParam(t,Metropolix::LENGTH),8);
        int offset=integer(Metropolix::trackParam(t,Metropolix::OFFSET));
        return metro::modulo(s-offset,8)<length;
    }
    std::string functionName() const {
        if(!metroModule)return "SLIDE";
        if(metroModule->uiMod.load()) {
            int d=integer(Metropolix::modParam(metroModule->selectedLane.load(),Metropolix::MOD_DESTINATION));
            return d?Metropolix::modDestinations()[d]:"UNASSIGNED";
        }
        const char* names[]={"RATCHET","PROBABILITY","ACCUM","SLIDE","SKIP","CV"};
        return names[metroModule->selectedRow.load()];
    }
    void footer(OledCanvas& c,bool detailed=false) const {
        auto* m=metroModule;
        std::string left=m && m->uiMod.load()?string::f("MOD %d",m->selectedLane.load()+1):string::f("TRK %d",m?m->selectedTrack.load()+1:1);
        if(detailed && m)left+=string::f(":%d",m->selectedStage.load()+1);
        std::string right=functionName();
        if(m && m->uiMod.load()) {
            int source=m->selectedLane.load()+2;
            bool a=integer(Metropolix::OUTPUT_SOURCE_A)==source,b=integer(Metropolix::OUTPUT_SOURCE_B)==source;
            if(a || b)right+=a?(b?":AB":":A"):":B";
        }
        int edge=127;
        if(m && !m->uiMod.load() && m->selectedRow.load()==Metropolix::SLIDE) {
            c.line(120,61,120,56);c.line(120,56,123,56);c.line(123,56,123,61);c.line(123,61,127,61);edge=117;
        }
        while(c.width(left)+c.width(right)+8>edge && !right.empty())right.pop_back();
        c.text(1,56,left);c.right(edge,56,right);
        for(int x=c.width(left)+5;x<edge-c.width(right)-2;x+=3)c.dot(x,58);
    }
    void pages(OledCanvas& c,int selected) const {
        for(int i=0;i<4;++i)for(int x=i*32;x<i*32+28;++x)if(i==selected || x%3==0)c.dot(x,0);
    }
    void field(OledCanvas& c,int y,const std::string& name,const std::string& v,bool selected,int w=128,int size=2) const {
        c.text(2,y+2,name);
        c.fit(18,y+10,w-22,v,size);
        if(selected){c.corners(0,y,w,27);c.turn(3,y+17);}
        else c.circle(5,y+21,2,true);
    }
    void home(OledCanvas& c) const {
        auto* m=metroModule;int h=m?m->uiHome.load():0,lane=m?m->selectedLane.load():0;
        pages(c,h);
        if(h==0) {
            for(int row=0;row<3;++row) {
                bool mod=row==2;
                int length=integer(mod?Metropolix::modParam(lane,Metropolix::MOD_LENGTH):Metropolix::trackParam(row,Metropolix::LENGTH),8);
                int offset=integer(mod?Metropolix::modParam(lane,Metropolix::MOD_OFFSET):Metropolix::trackParam(row,Metropolix::OFFSET));
                int position=m?(mod?m->modStage[lane].load():m->playStage[row].load()):-1;
                bool mute=integer(mod?Metropolix::modParam(lane,Metropolix::MOD_MUTE):Metropolix::trackParam(row,Metropolix::MUTE));
                for(int s=0;s<8;++s) {
                    int x=2+16*s,y=7+16*row;
                    if(metro::modulo(s-offset,8)>=length){c.dot(x+5,y+5);continue;}
                    if(!mod && integer(Metropolix::stageParam(row,s,Metropolix::SKIP))) {
                        c.line(x,y,x+9,y+9);c.line(x+9,y,x,y+9);
                    } else {
                        c.box(x,y,10,10);
                        if(s==offset)c.box(x+1,y+1,8,8);
                        if(s==position)c.fill(x,y,10,10);
                    }
                    if(mute)c.hatch(x,y,10,10);
                }
            }
        } else if(h==1) {
            int scale=integer(Metropolix::SCALE,1),root=integer(Metropolix::ROOT);
            const char* scaleNames[]={"CHROMATIC","MAJOR","MINOR","DORIAN","MIXOLYD","MAJ PENT","MIN PENT"};
            for(int t=0;t<2;++t) {
                int y=5+t*25;
                c.text(0,y,std::string(notes[root])+" "+scaleNames[scale]);
                int pre=m?metro::nearestDegree(m->pitchPre[t].load()*12.f,scale):0;
                int post=m?static_cast<int>(std::round(m->pitchPost[t].load()*12.f)):0;
                c.right(102,y,string::f("%+d",pre));c.right(127,y,string::f("%+d",post));
                for(int s=0;s<8;++s) {
                    int deg=metro::nearestDegree(value(Metropolix::PITCH+s)*12*integer(Metropolix::RANGE,3),scale);
                    int note=metro::modulo(metro::degreeToSemitone(deg,scale)+root+integer(Metropolix::trackParam(t,Metropolix::TRANSPOSE)),12);
                    c.text(s*16,y+10,notes[note],false);
                    if(used(t,s) && !integer(Metropolix::stageParam(t,s,Metropolix::SKIP))) {
                        bool playing=m && m->playStage[t].load()==s;
                        for(int x=s*16;x<s*16+11;++x)if(playing || x%2==0)c.dot(x,y+19);
                    }
                }
            }
        } else if(h==2) {
            // Rack presets restore the physical controls themselves. There are
            // no separate hardware/preset values, so no override dots appear.
            for(int i=0;i<2;++i){c.circle(8,14+i*25,6);c.line(8,14+i*25,4,18+i*25);c.text(7,22+i*25,std::to_string(i+1));}
            for(int s=0;s<8;++s) {
                int x=30+s*13;c.text(x-2,4,std::to_string(s+1));
                c.line(x,14,x,34);int y=34-static_cast<int>(value(Metropolix::PITCH+s)*20);c.line(x-3,y,x+3,y);
                c.text(x-2,40,std::to_string(integer(Metropolix::PULSES+s,1)));
                int gate=integer(Metropolix::GATES+s,2);
                c.box(x-4,49,9,3);if(gate==1)c.fill(x-4,49,3,3);if(gate==2){c.fill(x-4,49,2,3);c.fill(x,49,2,3);}if(gate==3)c.fill(x-4,49,9,3);
            }
        } else {
            for(int j=0;j<10;++j) {
                int x=j<2?j*12:29+(j-2)*12;
                int l=j-2;
                c.text(x+2,4,std::to_string(j<2?j+1:l+1));
                float v=m?(j<2?m->playCv[j].load():m->modVoltage[l].load()):0;
                bool gate=j>=2 && (integer(Metropolix::modParam(l,Metropolix::MOD_TYPE))==metro::ModGate || metro::modGateDestination(static_cast<metro::ModDestination>(integer(Metropolix::modParam(l,Metropolix::MOD_DESTINATION)))));
                if(gate){c.box(x,35,7,7);if(v>=1.5f)c.fill(x+1,36,5,5);}
                else { c.box(x,12,7,30);for(int k=1;k<6;k+=2)c.dot(x+k,26);int y=26-static_cast<int>(metro::clamp(v,-5.f,5.f)*2.7f);c.fill(x+2,std::min(y,26),3,std::max(1,std::abs(y-26))); }
                if(j>=2 && integer(Metropolix::modParam(l,Metropolix::MOD_DESTINATION))) {
                    int target=integer(Metropolix::modParam(l,Metropolix::MOD_ASSIGNMENT));c.text(x,44,target==2?"12":std::to_string(target+1));
                }
                bool a=integer(Metropolix::OUTPUT_SOURCE_A)==j,b=integer(Metropolix::OUTPUT_SOURCE_B,1)==j;
                c.text(x,50,a?(b?"AB":"A"):(b?"B":""));
                if(j==1)for(int y=10;y<53;y+=3)c.dot(25,y);
            }
        }
        footer(c);
    }
    void keyboard(OledCanvas& c,int x,int y,int w) const {
        int root=integer(Metropolix::ROOT),scale=integer(Metropolix::SCALE,1);
        const auto& sc=metro::scaleAt(scale);const int white[]={0,2,4,5,7,9,11};
        auto enabled=[&](int note){for(int i=0;i<sc.count;++i)if(metro::modulo(note-root,12)==sc.notes[i])return true;return false;};
        int kw=w/7;
        for(int k=0;k<7;++k){c.fill(x+k*kw,y,kw-1,23);if(!enabled(white[k]))c.hatch(x+k*kw,y,kw-1,23);if(white[k]==root)c.fill(x+k*kw+kw/2-1,y+19,2,2,0);}
        const int after[]={0,1,3,4,5},black[]={1,3,6,8,10};
        for(int k=0;k<5;++k){int bx=x+(after[k]+1)*kw-2;c.fill(bx,y,4,14,0);if(!enabled(black[k])){c.fill(bx,y,4,14);c.hatch(bx,y,4,14);}if(black[k]==root)c.dot(bx+1,y+11);}
    }
    void stage(OledCanvas& c) const {
        auto* m=metroModule;if(!m)return;
        int t=m->selectedTrack.load(),s=m->selectedStage.load(),l=m->selectedLane.load(),row=m->selectedRow.load();
        bool mod=m->uiMod.load(),detail=m->uiStageDetail.load();
        auto id=[&](int stage){return mod?Metropolix::modParam(l,Metropolix::MOD_VALUE+stage):Metropolix::stageParam(t,stage,row);};
        bool gates=mod && (integer(Metropolix::modParam(l,Metropolix::MOD_TYPE))==metro::ModGate || metro::modGateDestination(static_cast<metro::ModDestination>(integer(Metropolix::modParam(l,Metropolix::MOD_DESTINATION)))));
        if((!mod && (row==Metropolix::SLIDE || row==Metropolix::SKIP)) || gates) {
            for(int i=0;i<8;++i) {
                int x=8+i*16;bool on=value(id(i))>=(gates?1.5f:.5f);
                if(!mod && row==Metropolix::SKIP) { if(on){c.line(x-4,21,x+4,29);c.line(x+4,21,x-4,29);}else c.box(x-4,21,9,9); }
                else c.circle(x,25,5,on);
                c.text(x-2,39,std::to_string(i+1));
                if(detail && s==i)c.arrow(x,13);
            }
        } else if(mod || row==Metropolix::CV) {
            for(int x=0;x<128;x+=3)c.dot(x,27);
            int previous=27;
            for(int i=0;i<8;++i) {
                float v=value(id(i));if(mod && integer(Metropolix::modParam(l,Metropolix::MOD_TYPE))==metro::ModUnipolar)v=std::max(0.f,v);
                int x=7+i*16,y=27-static_cast<int>(v*4.4f);
                if(mod && integer(Metropolix::modParam(l,Metropolix::MOD_RAMP+i)))c.line(x-16,previous,x,y);
                else {c.line(x-16,previous,x-16,y);c.line(x-16,y,x,y);}
                if(detail && i==s){c.box(x-2,y-2,5,5);c.arrow(x,48,false);}
                previous=y;
            }
            if(detail) {
                c.fill(0,0,43,43,0);c.fit(1,4,40,string::f("%+.2fV",value(id(s))),1);c.corners(0,0,43,25);c.turn(3,16);
                c.text(1,29,mod && integer(Metropolix::modParam(l,Metropolix::MOD_RAMP+s))?"Ramp":"Step",false);
                if(mod){c.circle(5,41,2,true);c.circle(12,41,2,false);}
            }
        } else {
            bool accum=row==Metropolix::ACCUMULATION,prob=row==Metropolix::PROBABILITY;
            if(detail) {
                c.fit(1,4,39,prob?string::f("%d%%",integer(id(s))):accum?string::f("%+d",integer(id(s))):std::to_string(integer(id(s))));
                c.corners(0,0,40,25);c.turn(3,16);
                if(prob || accum){int trigger=integer(Metropolix::stageOption(t,s,prob?Metropolix::PROB_TARGET:Metropolix::ACCUM_TRIGGER));c.text(0,30,trigger==0?"Stage":trigger==1?"Pulse":"Ratch",false);for(int k=0;k<(prob?2:3);++k)c.circle(5+k*7,42,2,k==trigger);}
            }
            for(int i=0;i<8;++i) {
                int x=44+i*10;bool selected=detail && i==s;int n=integer(id(i));
                if(row==Metropolix::RATCHETS){for(int j=0;j<n;++j){c.box(x,46-j*5,8,3);if(selected)c.fill(x,46-j*5,8,3);}}
                if(prob){int h=static_cast<int>(n*.42f);if(h>0){c.box(x,48-h,8,h);if(selected)c.fill(x,48-h,8,h);}c.fill(x,50,8,2);if(integer(Metropolix::stageOption(t,i,Metropolix::PROB_TARGET)))c.fill(x+3,50,2,2,0);}
                if(accum){int h=static_cast<int>(std::abs(n)*.7f);int y=n>=0?27-h:27;c.dot(x+4,27);if(h){c.box(x,y,8,h);if(selected)c.fill(x,y,8,h);}}
                if(selected)c.arrow(x+4,1);
            }
        }
        int playing=mod?m->modStage[l].load():m->playStage[t].load();
        if(playing>=0) {
            bool narrow=!mod && row!=Metropolix::SLIDE && row!=Metropolix::SKIP && row!=Metropolix::CV;
            int x=narrow?44+playing*10:2+playing*16;c.line(x,53,x+(narrow?7:10),53);
        }
        footer(c,detail);
    }
    OledCanvas render() const {
        OledCanvas c;auto* m=metroModule;int page=m?m->uiPage.load():Metropolix::HOME;
        int t=m?m->selectedTrack.load():0,l=m?m->selectedLane.load():0,f=m?m->uiField.load():0;bool mod=m && m->uiMod.load();
        auto tm=[&](int tp,int mp){return mod?Metropolix::modParam(l,mp):Metropolix::trackParam(t,tp);};
        if(m && m->uiChooseFunction.load()) {
            c.text(1,3,mod?"SELECT MOD LANE":"SELECT EDIT FUNCTION");
            const char* labels[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
            for(int i=0;i<8;++i){int x=(i%4)*32,y=17+(i/4)*17;c.text(x+2,y,mod?string::f("MOD %d",i+1):labels[i]);c.text(x+2,y+8,std::to_string(i+1));}
            footer(c);return c;
        }
        if(page==Metropolix::HOME){home(c);return c;}
        if(page==Metropolix::STAGE_PAGE){stage(c);return c;}
        if(page==Metropolix::BPM_PAGE) {
            bool ext=m && m->externalClock.load();
            field(c,0,ext?"EXTERNAL":"INTERNAL",string::f("%.1f",ext?m->displayBpm.load():value(Metropolix::TEMPO,120)),!ext);
            field(c,29,ext?"EXT CLOCK DIV":"TAP TEMPO",ext?"/1":"Tap BPM",false,128,1);
        } else if(page==Metropolix::SCALE_PAGE) {
            const char* names[]={"Chromatic","Major","Minor","Dorian","Mixolydian","Major pent.","Minor pent."};
            field(c,0,"SCALE",names[integer(Metropolix::SCALE,1)],f==1,128,1);
            c.text(2,32,"ROOT");c.text(37,45,notes[integer(Metropolix::ROOT)],false);
            if(f==0)c.corners(0,29,51,26);
            c.box(2,45,17,7);c.text(4,46,"ALT");keyboard(c,58,29,70);
        } else if(page==Metropolix::ORDER_PAGE) {
            field(c,0,"PLAY ORDER",Metropolix::orderLabels()[integer(tm(Metropolix::ORDER,Metropolix::MOD_ORDER))],true,128,1);
            int id=mod?Metropolix::modParam(l,Metropolix::MOD_REVERSED):Metropolix::extraParam(t,Metropolix::REVERSED);
            field(c,29,"DIRECTION",integer(id)?"Reverse":"Forward",false,128,1);c.circle(12,50,2,false);
        } else if(page==Metropolix::LENGTH_PAGE) {
            bool offset=m && m->uiAlt.load();
            field(c,0,"STAGES",std::to_string(integer(tm(Metropolix::LENGTH,Metropolix::MOD_LENGTH),8)),f==0 && !offset,74);
            c.text(82,3,"OFFSET");c.fit(98,14,27,std::to_string(integer(tm(Metropolix::OFFSET,Metropolix::MOD_OFFSET))),1);
            c.box(81,16,17,7);c.text(83,17,"ALT");if(offset)c.corners(78,0,50,27);
            int id=mod?Metropolix::modParam(l,Metropolix::MOD_PULSE_LIMIT):Metropolix::extraParam(t,Metropolix::PULSE_LIMIT);
            int n=integer(id);field(c,29,"PULSES",n?std::to_string(n):"Auto",f==1 && !offset,74,1);
        } else if(page==Metropolix::DIV_PAGE) {
            field(c,0,"CLOCK DIV","/"+std::to_string(integer(tm(Metropolix::DIVISION,Metropolix::MOD_DIVISION),1)),f==0);
            if(!mod)field(c,29,"PULSE COUNT DIV","/"+std::to_string(integer(Metropolix::extraParam(t,Metropolix::PULSE_DIV),1)),f==1);
        } else if(page==Metropolix::SLIDE_PAGE) {
            field(c,0,"SLIDE AMOUNT",string::f("%.0f%%",value(Metropolix::trackParam(t,Metropolix::SLIDE_TIME),25)),true);
            c.text(2,31,"SLIDE TYPE");c.fit(20,41,105,"Tempo",1);
        } else if(page==Metropolix::GATE_PAGE) {
            field(c,0,"GATE LENGTH",string::f("%.0f%%",value(Metropolix::trackParam(t,Metropolix::GATE_LENGTH),50)),true);
            c.text(2,31,"GATE SCALE");c.fit(20,41,105,"100%",1);
        } else if(page==Metropolix::A_PAGE || page==Metropolix::B_PAGE) {
            int source=integer(page==Metropolix::A_PAGE?Metropolix::OUTPUT_SOURCE_A:Metropolix::OUTPUT_SOURCE_B);
            c.text(2,2,page==Metropolix::A_PAGE?"OUTPUT A":"OUTPUT B");c.right(125,12,source<2?"CV":"Mod",false);
            field(c,29,"OUTPUT ASSIGN",source<2?string::f("Trk %d",source+1):string::f("Mod %d",source-1),true,128,1);
        } else if(page==Metropolix::SETUP_PAGE)field(c,0,"SLIDER OCTAVES",std::to_string(integer(Metropolix::RANGE,3)),true);
        else {c.text(8,13,"NOT IMPLEMENTED");c.text(8,30,"HOVER FOR DETAILS");}
        footer(c);return c;
    }
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg;
        rect(vg,0,0,box.size.x,box.size.y,nvgRGB(23,25,25),.8f);
        rect(vg,1,1,box.size.x-2,box.size.y-2,nvgRGB(0,5,7),.3f);
        nvgSave(vg);nvgTranslate(vg,3,3);nvgScale(vg,(box.size.x-6)/128.f,(box.size.y-6)/64.f);
        nvgScissor(vg,0,0,128,64);render().paint(vg);nvgRestore(vg);
    }
};
