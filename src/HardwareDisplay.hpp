#include "OledCanvas.hpp"

struct HardwareDisplay : HardwareControl {
    // Rack can render into its normal and framebuffer NanoVG contexts. Each
    // needs its own image handle, released before the window context closes.
    std::map<NVGcontext*,int> displayImages;
    void clearDisplayImages() {
        for(auto& entry:displayImages)if(entry.second>0)nvgDeleteImage(entry.first,entry.second);
        displayImages.clear();
    }
    ~HardwareDisplay() { clearDisplayImages(); }
    void onContextDestroy(const ContextDestroyEvent& e) override {
        clearDisplayImages();HardwareControl::onContextDestroy(e);
    }
    int integer(int id,int fallback=0) const { return metroModule?int(std::round(metroModule->uiValue(id))):fallback; }
    float value(int id,float fallback=0.f) const { return metroModule?metroModule->uiValue(id):fallback; }
    bool effectiveSkip(int t,int s) const {
        bool invert=false;
        for(int k=0;k<2;++k) {
            int target=integer(Metropolix::ctrlParam(k,Metropolix::CTRL_ASSIGNMENT),2);
            if(integer(Metropolix::ctrlParam(k,Metropolix::CTRL_DESTINATION))==metro::CtrlSkipInvert && (target==2 || target==t))
                invert=value(Metropolix::ctrlParam(k,Metropolix::CTRL_VALUE))>0;
        }
        return bool(integer(Metropolix::stageParam(t,s,Metropolix::SKIP)))!=(invert || (metroModule && (metroModule->modSkipMask.load()&(1u<<t))));
    }
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
        const char* names[]={"RATCHET","PROBABILITY","ACCUM","SLIDE","SKIP","CV","PITCH","GATE"};
        return names[metroModule->selectedRow.load()];
    }
    std::string modValueLabel(const OledCanvas& c,int lane,float volts) const {
        auto destination=static_cast<metro::ModDestination>(integer(Metropolix::modParam(lane,Metropolix::MOD_DESTINATION)));
        std::string shown=metro::modDisplayText(volts,destination);if(c.width(shown)<=40)return shown;
        auto spec=metro::modDisplay(destination);std::string unit=spec.unit;
        if(unit==" pulses")unit="pl";else if(unit==" stages")unit="stg";else if(unit==" scales")unit="scl";else if(unit==" orders")unit="ord";else if(unit==" ratch")unit="rt";else unit.erase(std::remove(unit.begin(),unit.end(),' '),unit.end());
        float value=metro::modDisplayValue(volts,destination);if(std::abs(value)<.000001f)value=0;
        for(int precision=spec.decimals;precision>=0;--precision){char buffer[64];std::snprintf(buffer,sizeof(buffer),"%+.*f%s",precision,double(value),unit.c_str());shown=buffer;if(c.width(shown)<=40)break;}
        return shown;
    }
    void footer(OledCanvas& c,bool detailed=false) const {
        auto* m=metroModule;
        if(pendingConfirmation(m)){c.fill(0,54,128,10,0);c.text(1,56,"? PRESS ENCODER");return;}
        if(m && m->workflowUntil>system::getTime()){c.fill(0,54,128,10,0);c.text(1,56,m->workflowMessage);return;}
        if(m && m->workflowStageHeld){c.text(1,56,string::f(m->workflowLaneCopy?"COPY LANE %d":"COPY STAGE %d",m->workflowStageSource+1));return;}
        if(m && m->heldTrackReset.load() && !workflowTrackIds(m,m->selectedTrack.load()).empty()){c.text(1,56,string::f("COPY TRK %d",m->heldTrackReset.load()&1?1:2));return;}
        std::string left=m && m->uiMod.load()?string::f("MOD %d",m->selectedLane.load()+1):string::f("TRK %d",m?m->selectedTrack.load()+1:1);
        if(detailed && m)left+=string::f(":%d",m->selectedStage.load()+1);
        std::string right=m && m->uiPage.load()==Metropolix::ACCUM_PAGE?"ACCUMULATOR":
            m && m->uiPage.load()==Metropolix::CTRL_PAGE?string::f("CTRL %d",m->selectedCtrl.load()+1):m && m->uiPage.load()==Metropolix::CV_TYPE_PAGE?"CV TYPE":functionName();
        if((overridePage(m) || cvPage(m) || stagePairPage(m) || simpleStagePage(m)) && m->uiAlt.load())right="EDIT ALL / ALT";
        if(m && m->uiMod.load()) {
            int source=m->selectedLane.load()+2;
            bool a=integer(Metropolix::OUTPUT_SOURCE_A)==source,b=integer(Metropolix::OUTPUT_SOURCE_B)==source;
            if(a || b)right+=a?(b?":AB":":A"):":B";
        }
        if(m && (m->uiPage.load()==Metropolix::SCALE_PAGE || m->uiPage.load()==Metropolix::ACCUM_PAGE)) {
            bool wave=m->uiPage.load()==Metropolix::SCALE_PAGE && !m->uiMod.load() && m->selectedRow.load()==Metropolix::SLIDE;
            int edge=m->uiPage.load()==Metropolix::ACCUM_PAGE?127:wave?113:126;right=OledCanvas::glyphText(right);
            while(c.captionWidth(left)+c.captionWidth(right)+10>edge && !right.empty())right.pop_back();
            int rightX=edge-c.captionWidth(right);
            c.captionText(2,56,left);c.captionText(rightX,56,right);
            for(int x=2+c.captionWidth(left)+7;x<=rightX-4;x+=4)c.dot(x,58);
            if(wave){const uint16_t rows[]={120,72,72,72,79};c.bitmap(119,56,7,5,rows);}
            return;
        }
        int edge=127;
        if(m && m->uiPage.load()!=Metropolix::ACCUM_PAGE && m->uiPage.load()!=Metropolix::CTRL_PAGE && m->uiPage.load()!=Metropolix::CV_TYPE_PAGE && !m->uiMod.load() && m->selectedRow.load()==Metropolix::SLIDE) {
            c.line(120,61,120,56);c.line(120,56,123,56);c.line(123,56,123,61);c.line(123,61,127,61);edge=117;
        }
        while(c.width(left)+c.width(right)+8>edge && !right.empty())right.pop_back();
        c.text(1,56,left);c.right(edge,56,right);
        for(int x=c.width(left)+5;x<edge-c.width(right)-2;x+=3)c.dot(x,58);
    }
    void pages(OledCanvas& c,int selected) const {
        for(int i=0;i<4;++i)for(int x=i*32;x<i*32+28;++x)if(i==selected || x%3==0)c.dot(x,0);
    }
    // Four rows retain the eleven-pixel lowercase descenders and a separate
    // browsing/value highlight. Short aliases leave room for VCV extensions.
    static int listStart(int cursor,int count) { return cursor<4?0:metro::clamp(cursor-1,0,count-4); }
    static std::string listLabel(const OledCanvas& c,std::string label,const std::string& value) {
        if(c.menuWidth(label)+c.menuWidth(value)+4<=126)return label;
        if(label=="Slider Octaves:")label="Slider Oct:";
        else if(label=="Slide Toggle CC:")label="Slide Tog CC:";
        else if(label=="Slide Amount CC:")label="Slide Amt CC:";
        else if(label=="Clk/Rset Out:")label="Clock Mode:";
        else if(label=="Button LED Level:")label="Button LEDs:";
        else if(label=="Gate Type:")label="Gate:";
        if(c.menuWidth(label)+c.menuWidth(value)+4>126 && !label.empty() && label.back()==':')label.pop_back();
        return label;
    }
    static void listRow(OledCanvas& c,int row,const std::string& name,const std::string& value,bool selected,bool editing) {
        int y=12+row*12,valueX=127-c.menuWidth(value);std::string label=listLabel(c,name,value);
        uint8_t labelInk=255,valueInk=255;
        if(selected){if(editing){c.fill(valueX-2,y-1,130-valueX,12);valueInk=0;}else {c.fill(0,y-1,128,12);labelInk=valueInk=0;}}
        c.menuText(1,y,label,labelInk);c.menuText(valueX,y,value,valueInk);
    }
    std::string trackListValue(int item) const {
        auto* m=metroModule;if(!m)return {};int id=trackMenuParam(m,item);if(id<0)return {};
        int v=integer(id);std::string shown;
        if(item==0)shown=v?string::f("%+d",v):"--";
        else if(item==1)shown=v?std::to_string(v):"Shared";
        else if(item==4){const char* names[]={"Switch","Single","Sngl+Rst","Multi","Multi+Rst","Hold","Hold+Rst"};shown=names[metro::clamp(v,0,6)];}
        else shown=previewDisplayString(m,id);
        if(m->uiDraftParam==id && shown.find('?')==std::string::npos)shown+=" ?";
        return shown;
    }
    void field(OledCanvas& c,int y,const std::string& name,const std::string& v,bool selected,int w=128,int size=2,int pressOptions=1,int baselineShift=0) const {
        auto* m=metroModule;
        c.text(2,y+2,name);
        std::string shown=v;if(selected && pendingConfirmation(metroModule) && shown.find('?')==std::string::npos)shown+=" ?";
        c.valueFit(18,(y?y+22:23)+baselineShift,w-20,shown,size==2);
        if(selected){
            c.corners(0,y,w,y?27:28);
            int owner=metroModule?metroModule->ctrlOwner(screenParam(metroModule)):-1;
            if(owner>=0)c.ownerHint(1,y+14,owner);
            else {
                bool output=m && (m->uiPage.load()==Metropolix::A_PAGE || m->uiPage.load()==Metropolix::B_PAGE);
                bool editingScale=m && m->uiPage.load()==Metropolix::SCALE_PAGE && m->scaleEditing.load();
                bool queued=m && !editingScale && (output || requiresConfirmation(m,screenParam(m)));
                bool pending=pendingConfirmation(m) || (output && m->outputDraft[m->uiPage.load()==Metropolix::A_PAGE?0:1].load()>=0);
                int hintY=y+18-(m && m->uiPage.load()==Metropolix::SCALE_PAGE && !y?1:0);
                c.turnHint(3,hintY,queued,pending,255,m && m->uiPage.load()==Metropolix::CV_TYPE_PAGE);
            }
        }
        else c.pressHint(3,y+18,pressOptions);
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
                    int x=2+16*s,y=7+16*row;
                    if(metro::modulo(s-offset,8)>=length){c.dot(x+5,y+5);continue;}
                    if(!mod && effectiveSkip(row,s)) {
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
            for(int t=0;t<2;++t) {
                int locked=integer(Metropolix::SCALE_LOCK,-1),index=locked<0?integer(Metropolix::SCALE,1):locked;
                if(m && metro::userScale(locked)){int playing=m->soundingScale[t].load();if(metro::userScale(playing) && (playing-49)/10==(locked-49)/10)index=playing;}
                int root=integer(m?m->scaleRootParam(index):Metropolix::ROOT);
                auto scale=m?m->uiScale(index):metro::scaleAt(index);
                int y=5+t*25;
                c.fit(0,y,80,std::string(notes[root])+" "+metro::scaleName(index),1);
                int pre=m?metro::nearestDegree(m->pitchPre[t].load()*12.f,scale):0;
                int post=m?static_cast<int>(std::round(m->pitchPost[t].load()*12.f)):0;
                c.right(102,y,string::f("%+d",pre));c.right(127,y,string::f("%+d",post));
                for(int s=0;s<8;++s) {
                    int anchor=integer(Metropolix::stageParam(t,s,Metropolix::PITCH_OVERRIDE),-1);
                    float sliderPitch=m?m->uiSliderPosition(t,s)*12*m->uiSliderOctaves(t):value(Metropolix::PITCH+s)*36.f;
                    int deg=metro::nearestDegree(anchor<0?sliderPitch:float(anchor),scale);
                    int note=metro::modulo(metro::degreeToSemitone(deg,scale)+root+integer(Metropolix::trackParam(t,Metropolix::TRANSPOSE)),12);
                    c.text(s*16,y+10,notes[note],false);
                    if(used(t,s) && !effectiveSkip(t,s)) {
                        bool playing=m && m->playStage[t].load()==s;
                        for(int x=s*16;x<s*16+11;++x)if(playing || x%2==0)c.dot(x,y+19);
                    }
                }
            }
        } else if(h==2) {
            // Override dots distinguish sounding memories from physical controls.
            for(int i=0;i<2;++i){
                float a=value(Metropolix::ctrlParam(i,Metropolix::CTRL_VALUE))*2.61f;
                c.circle(8,14+i*25,6);c.line(8,14+i*25,8+std::round(std::sin(a)*5),14+i*25-std::round(std::cos(a)*5));
                c.text(7,22+i*25,std::to_string(i+1));
                if(m && m->waitingPickup(Metropolix::ctrlParam(i,Metropolix::CTRL_VALUE)))c.circle(18,14+i*25,1,true);
            }
            for(int s=0;s<8;++s) {
                int x=30+s*13;c.text(x-2,4,std::to_string(s+1));
                c.line(x,14,x,34);int y=34-static_cast<int>(value(Metropolix::PITCH+s)*20);c.line(x-3,y,x+3,y);
                if(m && m->waitingPickup(Metropolix::PITCH+s))c.circle(x+5,y,1,true);
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
                bool gate=j>=2 && m && m->effectiveModType(l)==metro::ModGate;
                if(gate){c.box(x,35,7,7);if(v>=1.5f)c.fill(x+1,36,5,5);}
                else { c.box(x,12,7,30);for(int k=1;k<6;k+=2)c.dot(x+k,26);int y=26-static_cast<int>(metro::clamp(v,-5.f,5.f)*2.7f);c.fill(x+2,std::min(y,26),3,std::max(1,std::abs(y-26))); }
                if(j>=2 && integer(Metropolix::modParam(l,Metropolix::MOD_DESTINATION))) {
                    int target=integer(Metropolix::modParam(l,Metropolix::MOD_ASSIGNMENT));c.text(x,44,metro::modGlobalDestination(static_cast<metro::ModDestination>(integer(Metropolix::modParam(l,Metropolix::MOD_DESTINATION))))?"G":target==2?"12":std::to_string(target+1));
                }
                bool a=integer(Metropolix::OUTPUT_SOURCE_A)==j,b=integer(Metropolix::OUTPUT_SOURCE_B,1)==j;
                c.text(x,50,a?(b?"AB":"A"):(b?"B":""));
                if(j==1)for(int y=10;y<53;y+=3)c.dot(25,y);
            }
        }
        footer(c);
    }
    #include "HardwareScaleDisplay.hpp"
    void stage(OledCanvas& c) const {
        auto* m=metroModule;if(!m)return;
        int t=m->selectedTrack.load(),s=m->selectedStage.load(),l=m->selectedLane.load(),row=m->selectedRow.load();
        bool mod=m->uiMod.load(),detail=m->uiStageDetail.load();
        auto id=[&](int stage){return mod?Metropolix::modParam(l,Metropolix::MOD_VALUE+stage):Metropolix::stageParam(t,stage,row);};
        bool gates=(mod && m->effectiveModType(l)==metro::ModGate) || (!mod && row==Metropolix::CV && cvGate(m));
        if(!mod && row>=Metropolix::PITCH_OVERRIDE) {
            bool pitch=row==Metropolix::PITCH_OVERRIDE;
            int selected=integer(id(s),-1);
            std::string label=selected<0?"--":pitch?previewDisplayString(m,id(s)):selected==0?"Rest":string::f("%d%%",selected);
            if(pitch && pendingConfirmation(m) && label.size()>2 && label.substr(label.size()-2)==" ?")label.resize(label.size()-2);
            c.valueFit(2,11,36,label,false,true);c.corners(0,0,40,28);c.turnHint(3,18,requiresConfirmation(m,id(s)),pendingConfirmation(m));
            if(pitch){c.valueFit(2,37,40,"Sync",false,true);c.pressHint(3,43);}
            for(int i=0;i<8;++i) {
                int x=44+i*10,v=integer(id(i),-1),h=pitch?int(std::max(0,v)*.7f):int(std::max(0,v)*.42f);
                bool active=i==s;
                if(v<0)c.box(x,47,8,3);
                else if(!pitch && v==0)c.box(x,47,8,3),c.fill(x,48,8,2);
                else {h=std::max(2,h);c.box(x,48-h,8,h);if(active)c.fill(x,48-h,8,h);}
                if(active)c.arrow(x+4,1);
            }
        } else if((!mod && (row==Metropolix::SLIDE || row==Metropolix::SKIP)) || gates) {
            for(int i=0;i<8;++i) {
                int x=8+i*16;bool on=value(id(i))>=(gates?1.5f:.5f);
                if(!mod && row==Metropolix::SKIP) { if(on){c.line(x-4,21,x+4,29);c.line(x+4,21,x-4,29);}else c.box(x-4,21,9,9); }
                else c.circle(x,25,5,on);
                if(mod || row!=Metropolix::CV)c.text(x-2,39,std::to_string(i+1));
                if(detail && s==i && (mod || row!=Metropolix::CV))c.arrow(x,13);
            }
        } else if(mod || row==Metropolix::CV) {
            for(int x=0;x<128;x+=3)c.dot(x,27);
            auto cv=[&](int stage){return metro::trackCvVoltage(value(id(stage)),m->effectiveCvType(t));};
            int previous=mod?27:27-static_cast<int>(cv(7)*4.4f);
            for(int i=0;i<8;++i) {
                float v=mod?m->effectiveModVoltage(l,value(id(i))):cv(i);
                int x=7+i*16,y=27-static_cast<int>(v*4.4f);
                int ramp=mod?integer(Metropolix::modParam(l,Metropolix::MOD_RAMP+i)):integer(Metropolix::cvRampParam(t,i));
                if(ramp) {
                    int end=x;
                    if(!mod && ramp==metro::CvPulseRamp)end=x-16+std::max(2,16/std::max(1,integer(Metropolix::PULSES+i,1)));
                    c.line(x-16,previous,end,y);if(end<x)c.line(end,y,x,y);
                }
                else {c.line(x-16,previous,x-16,y);c.line(x-16,y,x,y);}
                if((detail && i==s) || m->uiAlt.load())c.box(x-2,y-2,5,5);
                if(detail && i==s)c.arrow(x,48,false);
                previous=y;
            }
            if(detail) {
                c.fill(0,0,43,43,0);c.valueFit(2,11,39,mod?modValueLabel(c,l,m->effectiveModVoltage(l,value(id(s)))):string::f("%+.2fV",cv(s)),false,true);c.corners(0,0,43,28);c.turnHint(3,18,requiresConfirmation(m,id(s)),pendingConfirmation(m));
                int ramp=mod?integer(Metropolix::modParam(l,Metropolix::MOD_RAMP+s)):integer(Metropolix::cvRampParam(t,s));
                const char* ramps[]={"Step","sRmp","pRmp"};c.valueFit(2,37,40,mod?(ramp?"Ramp":"Step"):ramps[metro::clamp(ramp,0,2)],false,true);
                c.pressHint(3,43,mod?2:3);
            }
        } else {
            bool accum=row==Metropolix::ACCUMULATION,prob=row==Metropolix::PROBABILITY;
            if(detail) {
                c.valueFit(2,11,36,prob?string::f("%d%%",integer(id(s))):accum?string::f("%d°",integer(id(s))):std::to_string(integer(id(s))),false,true);
                c.corners(0,0,40,28);c.turnHint(3,18,requiresConfirmation(m,id(s)),pendingConfirmation(m));
                if(prob || accum){int trigger=integer(Metropolix::stageOption(t,s,prob?Metropolix::PROB_TARGET:Metropolix::ACCUM_TRIGGER));c.valueFit(2,37,40,trigger==0?"Stage":trigger==1?"Pulse":"Ratch",false,true);c.pressHint(3,43,prob?2:3);}
            }
            for(int i=0;i<8;++i) {
                int x=44+i*10;bool selected=detail && (m->uiAlt.load() || i==s);int n=integer(id(i));
                if(row==Metropolix::RATCHETS){for(int j=0;j<n;++j){c.box(x,46-j*5,8,3);if(selected)c.fill(x,46-j*5,8,3);}}
                if(prob){int h=static_cast<int>(n*.42f);if(h>0){c.box(x,48-h,8,h);if(selected)c.fill(x,48-h,8,h);}c.fill(x,50,8,2);if(integer(Metropolix::stageOption(t,i,Metropolix::PROB_TARGET)))c.fill(x+3,50,2,2,0);}
                if(accum){int h=static_cast<int>(std::abs(n)*.7f);int y=n>=0?27-h:27;c.dot(x+4,27);if(h){c.box(x,y,8,h);if(selected)c.fill(x,y,8,h);}}
                if(selected)c.arrow(x+4,1);
            }
        }
        int playing=mod?m->modStage[l].load():m->playStage[t].load();
        if(playing>=0 && integer(Metropolix::SHOW_POSITION,1)) {
            bool narrow=!mod && row!=Metropolix::SLIDE && row!=Metropolix::SKIP && row!=Metropolix::CV;
            int x=narrow?44+playing*10:2+playing*16;c.line(x,53,x+(narrow?7:10),53);
        }
        if(pendingConfirmation(m))c.text(32,19,"?");
        if(detail && (mod || row==Metropolix::CV)){
            int cc=integer(mod?Metropolix::midiModParam(l,Metropolix::MIDI_MOD_CC):Metropolix::midiTrackParam(t,Metropolix::MIDI_CV_CC));
            if(cc>0){float voltage=mod?m->effectiveModVoltage(l,value(id(s))):metro::trackCvVoltage(value(id(s)),m->effectiveCvType(t));bool bipolar=mod?(m->effectiveModType(l)==metro::ModBipolar):m->effectiveCvType(t)==metro::BipolarCv;c.text(1,48,string::f("CC %d:%d",cc-1,metro::midiVoltage(voltage,bipolar)));}
        }
        footer(c,detail || (!mod && row>=Metropolix::PITCH_OVERRIDE));
    }
    OledCanvas render() const {
        OledCanvas c;auto* m=metroModule;int page=m?m->uiPage.load():Metropolix::HOME;
        int t=m?m->selectedTrack.load():0,l=m?m->selectedLane.load():0,f=m?m->uiField.load():0;bool mod=m && m->uiMod.load();
        auto tm=[&](int tp,int mp){return mod?Metropolix::modParam(l,mp):Metropolix::trackParam(t,tp);};
        if(m && m->uiChooseFunction.load()) {
            c.text(1,3,mod?"SELECT MOD LANE":"SELECT EDIT FUNCTION");
            const char* labels[]={"SLIDE","SKIP","PITCH","GATE","RATCH","PROB","ACCUM","CV"};
            for(int i=0;i<8;++i){int x=(i%4)*32,y=17+(i/4)*17;c.text(x+2,y,mod?string::f("MOD %d",i+1):labels[i]);c.text(x+2,y+8,std::to_string(i+1));}
            if(m && m->loopyMuteHeld.load()){c.fill(0,54,128,10,0);c.text(1,56,"MUTE TRK/LANE");return c;}
        footer(c);return c;
        }
        if(page==Metropolix::HOME){home(c);return c;}
        if(page==Metropolix::STAGE_PAGE){stage(c);return c;}
        if(page==Metropolix::BPM_PAGE) {
            bool ext=m && m->externalClock.load(),midi=m && m->integer(Metropolix::CLOCK_SOURCE)==3;
            field(c,0,midi?"MIDI CLOCK":ext?"EXTERNAL":"INTERNAL",string::f("%.1f",ext?m->displayBpm.load():value(Metropolix::TEMPO,120)),!ext,128,2,0);
            if(!ext && m && std::abs(m->displayBpm.load()-m->params[Metropolix::TEMPO].getValue())>.05f){c.fill(20,9,107,18,0);c.valueText(19,15,string::f("%.1f",m->displayBpm.load()));std::string base=string::f("%.1f",value(Metropolix::TEMPO,120));if(pendingConfirmation(m))base+=" ?";c.valueFit(56,23,70,base,true);}
            field(c,28,midi?"CLOCK RESOLUTION":ext?"EXT CLOCK DIV":"TAP TEMPO",midi?"24 PPQN":ext?"/"+std::to_string(integer(Metropolix::EXT_CLOCK_DIV,1)):"Tap BPM",ext && !midi,128,1,0);
        } else if(page==Metropolix::CLOCK_OUT_PAGE) {
            field(c,0,"CLK OUT","/"+std::to_string(integer(Metropolix::CLOCK_OUT_DIV,1)),f==0);
            field(c,28,"CLK OFFSET",std::to_string(metro::ClockOutput::offset(integer(Metropolix::CLOCK_OUT_OFFSET),integer(Metropolix::CLOCK_OUT_DIV,1))),f==1);
        } else if(page==Metropolix::SCALE_PAGE) {
            scale(c);
        } else if(page==Metropolix::ORDER_PAGE) {
            field(c,0,"PLAY ORDER",Metropolix::orderLabels()[integer(tm(Metropolix::ORDER,Metropolix::MOD_ORDER))],true,128,1);
            int id=mod?Metropolix::modParam(l,Metropolix::MOD_REVERSED):Metropolix::extraParam(t,Metropolix::REVERSED);
            field(c,28,"DIRECTION",integer(id)?"Reverse":"Forward",false,128,1,2);
        } else if(page==Metropolix::LENGTH_PAGE) {
            bool offset=m && m->uiAlt.load();
            field(c,0,"STAGES",std::to_string(integer(tm(Metropolix::LENGTH,Metropolix::MOD_LENGTH),8)),f==0 && !offset,74);
            c.text(82,3,"OFFSET");c.valueRight(126,15,std::to_string(integer(tm(Metropolix::OFFSET,Metropolix::MOD_STAGE_OFFSET))));
            c.box(81,16,17,7);c.text(83,17,"ALT");if(offset)c.corners(78,0,50,28);
            int id=mod?Metropolix::modParam(l,Metropolix::MOD_PULSE_LIMIT):Metropolix::extraParam(t,Metropolix::PULSE_LIMIT);
            int n=integer(id);field(c,28,"PULSES",n?std::to_string(n):"Auto",f==1 && !offset,74,1);
        } else if(page==Metropolix::DIV_PAGE) {
            field(c,0,"CLOCK DIV","/"+std::to_string(integer(tm(Metropolix::DIVISION,Metropolix::MOD_DIVISION),1)),f==0);
            if(!mod)field(c,28,"PULSE COUNT DIV","/"+std::to_string(integer(Metropolix::extraParam(t,Metropolix::PULSE_DIV),1)),f==1);
        } else if(page==Metropolix::CV_TYPE_PAGE) {
            const char* types[]={"Bipolar ±","Unipolar +","Gate Toggle"};
            int type=mod?int(m->effectiveModType(l)):integer(Metropolix::cvTypeParam(t));
            if(!mod && type==metro::BipolarCv && integer(Metropolix::outputTrackParam(t,Metropolix::VELOCITY_SOURCE))==3)type=metro::UnipolarCv;
            bool forced=mod && integer(Metropolix::modParam(l,Metropolix::MOD_DESTINATION))!=metro::ModNone;
            field(c,0,"CV TYPE",types[metro::clamp(type,0,2)],!forced,128,1,0,-1);
            if(forced){c.text(1,34,"TYPE FROM DEST");c.valueFit(1,52,126,m->getParamQuantity(Metropolix::modParam(l,Metropolix::MOD_DESTINATION))->getDisplayValueString());}
            if(!mod && m && integer(Metropolix::outputTrackParam(t,Metropolix::VELOCITY_SOURCE))==3)c.text(1,40,"CV = VELOCITY");
        } else if(page==Metropolix::SLIDE_PAGE) {
            if(mod){c.text(2,3,"SLIDE");c.text(2,23,"NOTE TRACKS ONLY");}
            else {
            field(c,0,"SLIDE AMOUNT",string::f("%.0f",value(Metropolix::trackParam(t,Metropolix::SLIDE_TIME),25)),true);
            const char* types[]={"Tempo","Analog","Acid"};
            field(c,28,"SLIDE TYPE",types[metro::clamp(integer(Metropolix::slideTypeParam(t)),0,2)],false,128,1,3);
            }
        } else if(page==Metropolix::SWING_PAGE) {
            if(mod){c.text(2,3,"SWING");c.text(2,23,"NOTE TRACKS ONLY");}
            else {
                int owner=m?m->ctrlOwner(Metropolix::swingParam(t)):-1;
                c.text(2,2,"SWING",true,1,owner>=0?155:255);
                std::string shown=string::f("%.0f%%",value(Metropolix::swingParam(t),50));if(pendingConfirmation(m))shown+=" ?";
                c.valueFit(18,23,108,shown,true);
                c.corners(0,0,128,28);
                if(owner>=0)c.ownerHint(1,14,owner);else c.turnHint(3,18,requiresConfirmation(m,Metropolix::swingParam(t)),pendingConfirmation(m));
            }
        } else if(page==Metropolix::GATE_PAGE) {
            if(mod){c.text(2,3,"GATE");c.text(2,23,"NOTE TRACKS ONLY");}
            else {
                float length=value(Metropolix::trackParam(t,Metropolix::GATE_LENGTH),50);
                field(c,0,"GATE LENGTH",length>=100?"Hold":string::f("%.0f%%",length),f==0);
                field(c,28,"GATE SCALE",string::f("%.0f%%",value(Metropolix::gateScaleParam(t),100)),f==1);
            }
        } else if(page==Metropolix::TRACK_MENU_PAGE) {
            int cursor=m?m->trackMenuCursor.load():0,start=listStart(cursor,TrackMenuCount);
            c.text(1,1,string::f("TRACK %d SETUP",t+1));
            for(int row=0;row<4;++row){int item=start+row,id=m?trackMenuParam(m,item):-1;std::string label=trackMenuName(item);if(id>=0)label+=":";listRow(c,row,label,trackListValue(item),item==cursor,f && id>=0);}
            if(m && m->workflowUntil>system::getTime())c.text(1,59,m->workflowMessage);
            else if(m && m->heldTrackReset.load())c.text(1,59,string::f("COPY TRK %d MENU",m->heldTrackReset.load()&1?1:2));
            return c;
        } else if(page==Metropolix::A_PAGE || page==Metropolix::B_PAGE) {
            int source=m?shownOutputSource(m):0;auto selection=metro::outputSelection(source);
            bool pending=m && m->outputDraft[outputIndex(m)].load()>=0;
            std::string name=metro::outputKindName(selection.kind);if(pending && f==0)name+=" ?";
            field(c,0,page==Metropolix::A_PAGE?"OUTPUT A":"OUTPUT B",name,f==0,128,1);
            std::string assignment=metro::outputTargetCount(selection.kind)?string::f(metro::outputIsMod(selection.kind)?"Mod %d":"Trk %d",selection.target+1):"Global";
            if(metro::outputTargetCount(selection.kind))assignment+=page==Metropolix::A_PAGE?" > Out A":" > Out B";
            if(pending && f==1)assignment+=" ?";
            field(c,28,"OUTPUT ASSIGN",assignment,f==1 && metro::outputTargetCount(selection.kind),128,1);
        } else if(page==Metropolix::AUX_PAGE || page==Metropolix::MOD_ASSIGN_PAGE) {
            int input=m?m->selectedAux.load():0;
            bool internal=page==Metropolix::MOD_ASSIGN_PAGE;int lane=m?m->selectedLane.load():0;
            int dest=integer(internal?Metropolix::modParam(lane,Metropolix::MOD_DESTINATION):Metropolix::auxParam(input,Metropolix::AUX_DESTINATION));
            int target=integer(internal?Metropolix::modParam(lane,Metropolix::MOD_ASSIGNMENT):Metropolix::auxParam(input,Metropolix::AUX_ASSIGNMENT));
            float voltage=m?(internal?m->modVoltage[lane].load():m->auxVoltage[input].load()):0.f;
            field(c,0,internal?string::f("MOD %d PARAM",lane+1):string::f("AUX %c PARAM",'X'+input),Metropolix::modDestinations()[dest],f==0,128,1);
            if(metro::modGateDestination(static_cast<metro::ModDestination>(dest))) {
                c.box(116,2,9,7);if(voltage>=1.5f)c.fill(118,4,5,3);
            } else {
                c.box(83,2,42,6);c.line(104,1,104,8);
                int end=104+static_cast<int>(metro::clamp(voltage,-5.f,5.f)*3.7f);
                if(end!=104)c.fill(std::min(end,104),4,std::abs(end-104),2);
            }
            bool global=metro::modGlobalDestination(static_cast<metro::ModDestination>(dest));
            field(c,28,internal?"MOD ASSIGN":"AUX ASSIGN",global?"Global":(internal?string::f("Mod %d > ",lane+1):string::f("CV %c > ",'X'+input))+(target==2?"Trk 1+2":string::f("Trk %d",target+1)),f==1 && !global,128,1);
        } else if(page==Metropolix::LOOPY_PAGE) {
            int count=integer(Metropolix::LOOPY_PULSES),both=integer(Metropolix::LOOPY_TARGET);
            field(c,0,"LOOP PULSES",count?std::to_string(count):"Auto",true,128,2);
            field(c,28,"LOOP TARGET",both?"1+2":"Current Trk",false,128,1,2);
            if(m && m->loopyLatched.load()){c.box(1,58,6,5);c.box(2,55,4,5);c.text(11,57,"LATCHED");}
            else c.text(1,57,m && m->loopyMuteHeld.load()?"MUTE TRK/LANE":string::f("TRK %d :",t+1)+(integer(Metropolix::RUN)?"LOOP STAGES":"PLAY STAGES"));
            return c;
        } else if(page==Metropolix::CTRL_PAGE) {
            int knob=m?m->selectedCtrl.load():0;
            int dest=integer(Metropolix::ctrlParam(knob,Metropolix::CTRL_DESTINATION));
            int target=integer(Metropolix::ctrlParam(knob,Metropolix::CTRL_ASSIGNMENT),2);
            field(c,0,string::f("CTRL %d PARAM",knob+1),Metropolix::ctrlDestinations()[dest],f==0,128,1);
            float a=value(Metropolix::ctrlParam(knob,Metropolix::CTRL_VALUE))*2.61f;
            c.circle(118,5,4);c.line(118,5,118+std::round(std::sin(a)*3),5-std::round(std::cos(a)*3));
            bool global=dest==metro::CtrlTempo || dest==metro::CtrlOutputSwap;
            field(c,28,"CTRL ASSIGN",global?"Global":string::f("Ctrl %d > ",knob+1)+(target==2?"Trk 1+2":string::f("Trk %d",target+1)),f==1,128,1);
        } else if(page==Metropolix::ACCUM_PAGE) {
            const char* labels[]={"Positive Limit","Negative Limit","Mode","Order","Polarity","Reset"};
            int first=metro::clamp(f,0,5)/3*3;
            c.captionText(1,2,"LIMITS + SETUP");
            for(int i=first;i<first+3;++i) {
                int id=Metropolix::accumScreenParam(t,i),n=integer(id,i<2?9:0);
                std::string v;
                if(i<2)v=string::f("%c%d",i?'-':'+',n);
                else if(i==2)v=n?"Track":"Stage";
                else if(i==3){const char* orders[]={"Wrap","Pendulum","Random","Hold"};v=orders[metro::clamp(n,0,3)];}
                else if(i==4)v=n?"Bipolar":"Unipolar";
                else v=n?"Auto":"Manual";
                if(m && m->uiDraftParam==id)v+=" ?";
                int y=10+(i-first)*15;
                int owner=m?m->ctrlOwner(id):-1;
                uint8_t ink=owner>=0?155:255;
                if(i==f){c.fill(0,y-3,128,15,ink);ink=0;}
                c.menuText(owner>=0?14:2,y,labels[i],ink);c.menuRight(124,y,v,ink);
                if(owner>=0)c.ownerHint(0,y-2,owner,ink);
            }
        } else if(page==Metropolix::LOAD_PAGE || page==Metropolix::SAVE_PAGE) {
            int bank=m?m->presetBankIndex.load():0;
            const char* colors[]={"Red","Orange","Yellow","Green","Cyan","Blue","Magenta","Pink"};
            field(c,0,"PRESET BANK",string::f("%d ",bank+1)+colors[bank],true,128,1);
            bool save=page==Metropolix::SAVE_PAGE;
            const char* delays[]={"Instant","Queue Pulses","Total Pulses"};
            const char* actions[]={"Save + Exit","Save","Clear"};
            field(c,save?28:27,save?"ACTION":"LOAD DELAY",save?actions[m?m->presetSaveAction.load():0]:delays[integer(Metropolix::LOAD_DELAY)],false,128,1,3);
            int pending=m?m->pendingSlot.load():-1,notice=m?m->presetNotice.load():0;
            if(pending>=0)c.text(1,57,string::f("QUEUED %d-%d",pending/8+1,pending%8+1));
            else if(notice) {
                const char* messages[]={"","SAVED","CLEARED","EMPTY SLOT","QUEUE CANCELLED","CANCEL QUEUE FIRST","DISABLE CHAIN FIRST"};
                c.text(1,57,messages[metro::clamp(notice,0,6)]);
            } else c.text(1,57,save?"SELECT SLOT TO SAVE":"SELECT SLOT TO LOAD");
            return c;
        } else if(page==Metropolix::PRESET_SETUP_PAGE) {
            int selected=presetSetupDisplayRow(f),start=metro::clamp(selected-1,0,5);if(selected<4)start=0;c.text(1,1,"PRESETS SETUP");
            for(int row=0;row<4;++row){int item=presetSetupDisplayItem(start+row);if(item<0){c.text(1,14+row*12,"RACK");continue;}listRow(c,row,std::string(presetSetupName(item))+":",presetSetupValue(m,item),item==f,m && m->presetSetupEditing.load());}
            return c;
        } else if(page==Metropolix::CHAIN_PAGE && m) {
            const char* colors[]={"Red","Orange","Yellow","Green","Cyan","Blue","Magenta","Pink"};
            auto state=m->getChainState();int n=state.working.size;
            int cursor=m->chainCursor.load(),playing=m->chainPlayingLink.load(),edit=m->chainEdit.load();
            if(state.enabled && m->chainFollow.load() && playing>=0)cursor=2*playing+1;
            auto icon=[&](int x,int y,uint8_t v=255){const uint16_t rows[]={8176,4112,4567,4369,7537,257,511};for(int j=0;j<7;++j)for(int i=0;i<13;++i)if(rows[j]&(1<<(12-i)))c.dot(x+i,y+j,v);};
            auto plus=[&](int x,int y,uint8_t v=255){c.line(x,y+2,x+4,y+2,v);c.line(x+2,y,x+2,y+4,v);};
            auto bar=[&](int x,int y,int width){c.fill(x,y-2,width,11);};
            auto caption=[&](int x,int y,const std::string& text){c.chainText(x,y,text,255,true);};
            caption(2,1,"#");icon(20,0);caption(73,1,"X");caption(104,1,"MUTE");
            // An insertion line occupies a row only while it is selected.
            int selected=cursor/2,first=std::max(0,selected-1);
            if(cursor>=2*n+1)first=std::max(0,n-2);
            int y=12;
            for(int i=first;i<=n && y<=45;++i) {
                if(cursor==2*i && !state.enabled && !edit) {
                    if(!n){bar(1,y,127);c.chainText(19,y,"New Chain",0);plus(3,y+1,0);}
                    else {c.line(1,y+3,127,y+3);c.fill(57,y,9,7,0);plus(59,y+1);}
                    y+=11;
                }
                if(i==n || y>45)break;
                const auto& link=state.working.links[i];bool active=cursor==2*i+1;
                if(active)bar(edit==1?18:edit==2?73:1,y,edit==1?54:edit==2?29:127);
                c.chainText(2,y,std::to_string(i+1),active && !edit?0:255);
                std::string preset=std::string(colors[link.slot/8])+" "+std::to_string(link.slot%8+1);
                if(c.chainWidth(preset)>53)preset="Mag "+std::to_string(link.slot%8+1);
                c.chainText(19,y,preset,active && edit!=2?0:255);
                c.chainText(73,y,"x"+std::to_string(link.repeats),active && edit!=1?0:255);
                for(int t=0;t<3;++t){const uint8_t solid[]={14,31,31,31,14},ring[]={14,17,17,17,14};const auto* rows=link.mutes&(1u<<t)?ring:solid;for(int j=0;j<5;++j)for(int k=0;k<5;++k)if(rows[j]&(1<<(4-k)))c.dot(103+9*t+k,y+1+j,active && !edit?0:255);}
                if(m->chainQuick && i==n-1)plus(96,y+1,active && edit!=1?0:255);
                if(i==playing && state.enabled && !active)c.fill(0,y+1,1,5);
                y+=11;
            }
            auto action=[&](int position,const char* name){
                if(y>45)return;
                bool active=cursor==position;if(active)bar(1,y,127);
                c.chainText(19,y,name,active?0:255);
                if(!n)for(int j=y;j<y+9;++j)for(int x=19;x<128;++x)if((x+j)%2==0)c.dot(x,j,0);
                y+=11;
            };
            action(2*n+1,state.enabled?"Disable Chain":"Enable Chain");action(2*n+2,"Clear Chain");
            if(state.enabled) {
                if(std::fmod(system::getTime(),.8)<.5)icon(1,56);
                caption(15,58,playing<0?"-":string::f("%d:%d",playing+1,m->chainPlayingRepeat.load()));
                caption(43,58,m->chainFollow.load()?"FOLLOWING CHAIN":"CHAIN PLAYBACK");
            } else {
                caption(2,58,m->uiMod.load()?string::f("MOD %d",m->selectedLane.load()+1):string::f("TRK %d",m->selectedTrack.load()+1));
                c.dot(34,60);c.dot(38,60);
                caption(43,58,m->chainQuick?"ADD TO CHAIN":edit?"EDIT LINK":"SAVE/LOAD CHAIN");
            }
            return c;
        } else if(page==Metropolix::BACKUP_PAGE || page==Metropolix::BACKUP_PRESET_PAGE){
            bool presets=page==Metropolix::BACKUP_PRESET_PAGE;
            int cursor=m?(presets?m->backupPresetCursor.load():m->backupCursor.load()):0;
            int count=presets?BackupPresetCount:BackupCount,start=listStart(cursor,count);
            c.text(1,1,presets?"PRESET EXPORT":"BACKUP FILES");
            for(int row=0;row<4 && start+row<count;++row){int item=start+row;listRow(c,row,presets?backupPresetName(item):backupName(item),"",item==cursor,false);}
            c.text(1,59,m && system::getTime()<m->backupNoticeUntil?m->backupNotice:presets?"CLICK TO EXPORT":"JSON BACKUP");return c;
        } else if(page==Metropolix::FACTORY_PAGE){
            c.text(1,1,"LAB "+labVersion());int cursor=m?m->factoryCursor.load():0;
            for(int action=0;action<2;++action)listRow(c,action,factoryName(action),"",action==cursor,false);
            c.text(1,59,m && system::getTime()<m->factoryNoticeUntil?m->factoryNotice:"CONFIRM / UNDO");return c;
        } else if(page==Metropolix::FACTORY_CONFIRM_PAGE){
            int action=m?m->factoryCursor.load():0,choice=m?m->factoryConfirmCursor:0;
            c.text(1,1,action?"CLEAR 64 PRESETS?":"RESET SETTINGS?");
            c.text(1,14,action?"KEEP LIVE SETTINGS":"KEEP 64 PRESETS");
            for(int n=0;n<2;++n)listRow(c,n+2,n?"Apply":"Cancel","",n==choice,false);return c;
        } else if(page==Metropolix::MX_SETUP_PAGE){
            int mode=integer(Metropolix::MX_MODE),assign=m?mxAssignment(m):-1;
            field(c,0,"MX - EXPANDER",m?previewDisplayString(m,Metropolix::MX_MODE):"Transport A",!f,128,1);c.right(126,2,"GATE");
            field(c,28,"MX ASSIGN",assign<0?(mode==metro::MxModClocks?"All MOD > MX":"Global/All > MX"):previewDisplayString(m,assign)+" > MX",f && assign>=0,128,1,assign<0?0:1);
            c.text(1,56,m && m->mxConnected.load()?"MX CONNECTED":"PLACE MX AT RIGHT");return c;
        } else if(page==Metropolix::SETUP_PAGE){
            int cursor=m?m->setupCursor.load():0,selected=setupDisplayRow(cursor);
            int start=metro::clamp(selected-1,0,setupDisplayRow(SetupMenuCount-1)-3);
            // Keep the first four reference options together at the top.
            if(cursor<4)start=0;
            c.text(1,1,"SETUP");
            for(int row=0;row<4;++row){int displayRow=start+row,item=setupDisplayItem(displayRow),y=12+row*12;
                if(item<0){c.text(1,y+2,setupCategoryName(displayRow));continue;}
                int id=m?setupMenuParam(m,item):-1;std::string label=setupMenuName(item);if(id>=0)label+=":";
                listRow(c,row,label,setupMenuValue(m,id),item==cursor,f && id>=0);
            }
            return c;
        } else if(page==Metropolix::GLOBAL_SETUP_PAGE || page==Metropolix::UI_SETUP_PAGE || page==Metropolix::MIDI_SETUP_PAGE) {
            bool global=page==Metropolix::GLOBAL_SETUP_PAGE,ui=page==Metropolix::UI_SETUP_PAGE;
            int cursor=m?(global?m->globalCursor.load():ui?m->uiSetupCursor.load():m->midiSetupCursor.load()):0,count=global?GlobalMenuCount:ui?UiSetupCount:MidiSetupCount,start=listStart(cursor,count);
            c.text(1,1,global?"GLOBAL SETUP":ui?"UI SETUP":"MIDI SETUP");
            for(int row=0;row<4;++row){int item=start+row,id=m?(global?globalMenuParam(m,item):ui?uiSetupParam(m,item):midiSetupParam(m,item)):-1;std::string label=global?globalMenuName(item):ui?uiSetupName(item):midiSetupName(item);if(id>=0)label+=":";else if(label=="Backup files…")label="Backup Files...";listRow(c,row,label,setupMenuValue(m,id),item==cursor,f && id>=0);}
            return c;
        } else if(page==Metropolix::MOD_MENU_PAGE){
            int cursor=m?m->modMenuCursor.load():0;c.text(1,1,string::f("MOD %d SETUP",l+1));
            for(int item=0;item<4;++item){int id=m?modMenuParam(m,item):-1;std::string label=modMenuName(item);if(id>=0)label+=":";listRow(c,item,label,id>=0?previewDisplayString(m,id):"",item==cursor,f && id>=0);}
            return c;
        } else if(page==Metropolix::TUNER_PAGE){
            field(c,0,"TUNER PITCHES","C"+std::to_string(integer(Metropolix::TUNER_OCTAVE)),true,128,2,0,-1);
            field(c,28,"TUNER GATES",integer(Metropolix::TUNER_GATE)?"Clock":"High",false,128,1,2);c.text(1,56,"EXIT TO RESUME");return c;
        }
        else {c.text(8,13,"NOT IMPLEMENTED");c.text(8,30,"HOVER FOR DETAILS");}
        if(m && m->loopyMuteHeld.load()){c.fill(0,54,128,10,0);c.text(1,56,"MUTE TRK/LANE");return c;}
        footer(c);return c;
    }
    void draw(const DrawArgs& args) override {
        auto* vg=args.vg;
        rect(vg,0,0,box.size.x,box.size.y,nvgRGB(23,25,25),.8f);
        rect(vg,1,1,box.size.x-2,box.size.y-2,nvgRGB(0,5,7),.3f);
        nvgSave(vg);nvgTranslate(vg,3,3);nvgScale(vg,(box.size.x-6)/128.f,(box.size.y-6)/64.f);
        nvgScissor(vg,0,0,128,64);render().paint(vg,displayImages[vg],value(Metropolix::SCREEN_LEVEL,4)*.25f);nvgRestore(vg);
    }
};
