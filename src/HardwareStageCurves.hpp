// Included inside HardwareDisplay. The detailed graph starts beside the value
// sidebar. Voltages use the model's real CV/MOD range and destination units.
void stageCurveGraph(OledCanvas& c,int track,int selected,int lane,bool mod,bool detail) const {
    auto* m=metroModule;
    auto id=[&](int stage){return mod?Metropolix::modParam(lane,Metropolix::MOD_VALUE+stage):Metropolix::stageParam(track,stage,Metropolix::CV);};
    auto voltage=[&](int stage){return mod?m->effectiveModVoltage(lane,value(id(stage))):metro::trackCvVoltage(value(id(stage)),m->effectiveCvType(track));};
    auto ordinate=[&](float volts){return metro::clamp(int(25.f-volts*5.f),0,51);};
    int start=detail?38:0,width=detail?11:16,previous=ordinate(voltage(7));
    for(int x=start;x<128;x+=2)c.dot(x,25);
    for(int stage=0;stage<8;++stage) {
        int x=start+width*stage,end=std::min(127,x+width),y=ordinate(voltage(stage));
        int ramp=mod?integer(Metropolix::modParam(lane,Metropolix::MOD_RAMP+stage)):integer(Metropolix::cvRampParam(track,stage));
        int target=!mod && ramp==metro::CvPulseRamp?x+std::max(1,width/std::max(1,integer(Metropolix::PULSES+stage,1))):end;
        if(ramp){c.line(x,previous,target,y);if(target<end)c.line(target,y,end,y);}
        else{c.line(x,previous,x,y);c.line(x,y,end,y);}
        if((detail && stage==selected) || m->uiAlt.load())c.box(target-2,y-2,5,5);
        if(detail && stage==selected)c.arrow(x+width/2,48,false);
        previous=y;
    }
    if(!detail)return;
    std::string label=mod?modValueLabel(c,lane,voltage(selected)):string::f("%+.2f",voltage(selected));
    c.valueFit(0,11,37,label,false,true);c.fieldCorners(0,0,37,26);stageTurnHint(c,id(selected));
    int ramp=mod?integer(Metropolix::modParam(lane,Metropolix::MOD_RAMP+selected)):integer(Metropolix::cvRampParam(track,selected));
    const char* names[]={"Step","sRmp","pRmp"};c.valueFit(1,37,36,mod?(ramp?"Ramp":"Step"):names[metro::clamp(ramp,0,2)],false,true);
    // The native two/three-choice helper has a filled middle circle. It marks
    // the press action, rather than claiming the circle is a selected enum.
    const uint16_t outline[]={14,17,17,17,14},solid[]={14,31,31,31,14};
    for(int n=0;n<(mod?2:3);++n)c.bitmap(3+n*8,43,5,5,n==1?solid:outline);
    int cc=integer(mod?Metropolix::midiModParam(lane,Metropolix::MIDI_MOD_CC):Metropolix::midiTrackParam(track,Metropolix::MIDI_CV_CC));
    if(cc>0 && !pendingConfirmation(m)){
        bool bipolar=mod?m->effectiveModType(lane)==metro::ModBipolar:m->effectiveCvType(track)==metro::BipolarCv;
        std::string midi=std::to_string(metro::midiVoltage(voltage(selected),bipolar));c.captionText(35-c.captionWidth(midi),17,midi);
    }
}
