// Members of Metropolix; MIDI uses raw sequencer signals before analog delay.
MetropolixMidiInput midiInput;MetropolixMidiOutput midiOutput;
midi::Message midiInputMessage;std::array<midi::Message,3> midiOutputMessages;
metro::MidiClockInput midiClockInput;metro::MidiClockOutput midiClockOutput;
std::array<metro::MidiVoice,2> midiVoices;
metro::MidiMxOutput midiMxOutput;
std::array<int,8> midiMxLevels{{127,127,127,127,127,127,127,127}};
std::array<int,2> midiLastStage{{-1,-1}};std::array<uint64_t,2> midiEvents{};
std::array<uint64_t,2> midiLastSerial{{UINT64_MAX,UINT64_MAX}};
std::array<int,6> midiTrackLastCC;std::array<int,8> midiModLastCC;
std::array<uint64_t,2> midiTrackConfig{};std::array<int,8> midiModConfig{};
std::array<int,NUM_PARAMS> midiLastParameters{};
bool midiWasRun=false,midiResume=false,midiParametersSeen=false,midiEverStarted=false;
int midiPreviousSource=-1,midiLastOutputDevice=-1,midiLastOutputDriver=-1,midiPCRequest=-1;
bool midiInputReset=false;unsigned midiReceivedClock=0;
std::atomic<bool> midiPanicRequest{false};
std::array<bool,2> midiHeldSuppressed{{false,false}};
int64_t midiFrame=-1;double midiCCAge=0;

void configureMidi(){
    midiInput.setChannel(-1);midiOutput.setChannel(-1);
    midiOutputMessages[0].bytes.pop_back();midiOutputMessages[0].bytes.pop_back();midiOutputMessages[1].bytes.pop_back();
    for(int t=0;t<2;++t){std::string prefix="Track "+std::to_string(t+1)+" MIDI ";
        configSwitch(midiTrackParam(t,MIDI_CHANNEL),0,16,t+1,prefix+"channel",midiChannels());
        configSwitch(midiTrackParam(t,MIDI_CV_CC),0,128,2,prefix+"CV to CC",midiCCs());
        configSwitch(midiTrackParam(t,MIDI_SLIDE_CC),0,128,66,prefix+"slide toggle CC",midiCCs());
        configSwitch(midiTrackParam(t,MIDI_AMOUNT_CC),0,128,6,prefix+"slide amount CC",midiCCs());
        configSwitch(midiTrackParam(t,MIDI_AMOUNT_ALWAYS),0,1,0,prefix+"slide amount",{"Toggle","Always"});
    }
    for(int l=0;l<8;++l){std::string prefix="MOD "+std::to_string(l+1)+" MIDI ";
        configSwitch(midiModParam(l,MIDI_MOD_CHANNEL),0,16,1,prefix+"channel",midiChannels());
        configSwitch(midiModParam(l,MIDI_MOD_CC),0,128,21+l,prefix+"MOD to CC",midiCCs());
    }
    configSwitch(MIDI_CLOCK_OUT,0,1,0,"MIDI clock output",{"Off","On"});
    configSwitch(MIDI_TRANSPORT,0,1,0,"MIDI transport",{"Off","On"});
    configSwitch(MIDI_ROOT_CHANNEL,0,16,0,"MIDI root channel",midiChannels());
    configSwitch(MIDI_PC_CHANNEL,0,16,0,"MIDI program-change channel",midiChannels());
    configParam(MIDI_ZERO_NOTE,0,127,60,"MIDI note at 0 V")->snapEnabled=true;
    configSwitch(MIDI_MX_CHANNEL,0,16,0,"MIDI MX channel",midiChannels());
    configSwitch(MIDI_MX_OUTPUT,0,1,0,"MIDI MX output",{"CC","Note"});
    midiTrackLastCC.fill(-1);midiModLastCC.fill(-1);
}
void emitMidi(int status,int a,int b,int size){
    auto& midiOutputMessage=midiOutputMessages[metro::clamp(size,1,3)-1];midiOutputMessage.bytes[0]=uint8_t(status);
    if(size>1)midiOutputMessage.bytes[1]=uint8_t(a & 127);
    if(size>2)midiOutputMessage.bytes[2]=uint8_t(b & 127);
    midiOutputMessage.frame=midiFrame;
    if(status<0xf0)midiOutput.usedChannels.fetch_or(1u<<(status&15),std::memory_order_relaxed);
    midiOutput.sendMessage(midiOutputMessage);
}
void midiPanic(){auto emit=[this](int s,int a,int b,int n){emitMidi(s,a,b,n);};for(auto& voice:midiVoices)voice.silence(emit);midiMxOutput.silence(emit);midiTrackLastCC.fill(-1);midiModLastCC.fill(-1);midiLastStage.fill(-1);midiLastSerial.fill(UINT64_MAX);}
void resetMidiRuntime(){midiPanic();midiMxOutput.reset();midiHeldSuppressed.fill(false);midiPanicRequest.store(false);midiClockInput.align();midiClockOutput.align();midiWasRun=false;midiResume=false;midiEverStarted=false;midiPreviousSource=-1;midiPCRequest=-1;midiParametersSeen=false;midiCCAge=0;while(midiInput.tryPop(&midiInputMessage,INT64_MAX)){} }
void processMidiInput(const ProcessArgs& args){
    midiFrame=args.frame;midiInputReset=false;midiReceivedClock=0;midiPCRequest=-1;
    if(midiPanicRequest.exchange(false)){midiPanic();midiHeldSuppressed.fill(true);midiMxOutput.suppressHeld();}
    if(midiPreviousSource!=integer(CLOCK_SOURCE)){midiClockInput.align();midiClockOutput.align();externalDivider.reset();sequencer.resetExternalTiming();meterClockSeen=false;meterClockElapsed=0;midiPanic();midiResume=false;midiPreviousSource=integer(CLOCK_SOURCE);}
    while(midiBankUpdates.pop(midiAudioBankUpdate))midiPresetBank[midiAudioBankUpdate.slot]=midiAudioBankUpdate.snapshot;
    // Bound host work per audio frame; retain the rest in Rack's input queue.
    for(int count=0;count<256 && midiInput.tryPop(&midiInputMessage,args.frame);++count){
        const auto& bytes=midiInputMessage.bytes;if(bytes.empty())continue;
        int status=bytes[0],channel=(status&15)+1;
        if(status==0xf8 && integer(CLOCK_SOURCE)==3){midiClockInput.pulse();++midiReceivedClock;}
        else if(integer(CLOCK_SOURCE)==3 && integer(MIDI_TRANSPORT)){
            if(status==0xfa || status==0xfb){midiClockInput.align();params[RUN].setValue(1);midiResume=status==0xfb;midiInputReset=status==0xfa;}
            else if(status==0xfc)params[RUN].setValue(0);
            else if(status==0xf2 && bytes.size()>=3 && bytes[1]==0 && bytes[2]==0)midiInputReset=true;
        }
        if((status&0xf0)==0x90 && bytes.size()>=3 && bytes[1]<128 && bytes[2]>0 && bytes[2]<128 && channel==integer(MIDI_ROOT_CHANNEL)
            && !metro::userScale(integer(SCALE)) && !metro::userScale(integer(SCALE_LOCK))) {params[ROOT].setValue(bytes[1]%12);readSettings();}
        if((status&0xf0)==0xc0 && bytes.size()>=2 && bytes[1]<64 && channel==integer(MIDI_PC_CHANNEL))midiPCRequest=bytes[1];
    }
}
void processMidiPreset(bool run,bool reset){
    if(midiPCRequest<0 || audioChain.enabled)return;
    const auto& snapshot=midiPresetBank[midiPCRequest];if(!snapshot.occupied)return;
    pendingPreset=snapshot;audioPendingSlot=midiPCRequest;pendingSlot.store(midiPCRequest);audioPendingMidi=true;presetNotice.store(0);
    if(!run || reset || integer(LOAD_DELAY)==metro::LoadInstant)applyPendingPreset(false);
}
void midiCC(int channel,int cc,int value){if(channel>0 && cc>0)emitMidi(0xb0+channel-1,cc-1,value,3);}
int midiTrackChannel(int t){return integer(midiTrackParam(sequencer.swapped()?1-t:t,MIDI_CHANNEL));}
float midiSlideAmount(int t){return metro::clamp(sequencer.settings.tracks[t].slideTime+sequencer.modulation.routes[t][metro::ModSlideAmount]*.2f,0.f,1.f);}
bool midiSlide(int t){const auto& state=sequencer.sounding(t);if(state.stage<0)return false;return sequencer.settings.tracks[t].slideType==metro::AcidSlide?state.slideFromPrevious:sequencer.settings.tracks[t].stages[state.stage].slide || sequencer.modulation.routes[t][metro::ModSlide]>=1.5f;}
void processMidiOutput(const ProcessArgs& args,bool run,bool external,bool reset,bool update,bool clockEdge){
    auto emit=[this](int s,int a,int b,int n){emitMidi(s,a,b,n);};
    bool outputChanged=midiLastOutputDevice!=midiOutput.getDeviceId() || midiLastOutputDriver!=midiOutput.getDriverId();
    if(outputChanged){midiPanic();midiLastOutputDevice=midiOutput.getDeviceId();midiLastOutputDriver=midiOutput.getDriverId();}
    if(run!=midiWasRun){
        if(integer(MIDI_TRANSPORT) && integer(CLOCK_SOURCE)!=3)emit(run?(integer(RUN_MODE) && midiEverStarted?0xfb:0xfa):0xfc,0,0,1);
        if(run)midiEverStarted=true;
        if(run){midiClockOutput.align();midiPanic();}else midiPanic();
    }
    if(reset && integer(MIDI_TRANSPORT) && integer(CLOCK_SOURCE)!=3)emit(0xf2,0,0,3);
    // pp.49 and 193 disagree. Follow the later firmware correction: Running
    // suppresses stopped MIDI clocks; Always and Rack's legacy mode retain them.
    bool clockEnabled=integer(MIDI_CLOCK_OUT) && (run || integer(CLOCK_RUNNING_ONLY)!=1);
    bool sameLoopback=midiInput.getDriverId()==-12 && midiOutput.getDriverId()==-12 && midiInput.getDeviceId()==midiOutput.getDeviceId();
    if(integer(CLOCK_SOURCE)==3){if(clockEnabled && !sameLoopback)for(unsigned i=0;i<midiReceivedClock;++i)emit(0xf8,0,0,1);}
    else midiClockOutput.process(args.sampleTime,clockEdge,external,15./sequencer.effectiveTempo(),clockEnabled,emit);
    midiCCAge+=args.sampleTime;bool ccDue=midiCCAge>=.001;if(ccDue)midiCCAge=0;
    for(int t=0;t<2;++t){const auto& state=sequencer.sounding(t);int channel=midiTrackChannel(t);
        uint64_t signature=unsigned(channel);for(int f=MIDI_CV_CC;f<=MIDI_AMOUNT_ALWAYS;++f)signature=(signature<<8)|unsigned(integer(midiTrackParam(t,f)));
        if(signature!=midiTrackConfig[t]){for(int i=0;i<3;++i)midiTrackLastCC[t*3+i]=-1;midiTrackConfig[t]=signature;}
        bool stageChanged=state.stage!=midiLastStage[t] || state.stageSerial!=midiLastSerial[t];
        if(stageChanged){++midiEvents[t];midiLastStage[t]=state.stage;}
        if(state.gate<1.5f || (stageChanged && midiLastSerial[t]!=UINT64_MAX))midiHeldSuppressed[t]=false;
        midiLastSerial[t]=state.stageSerial;bool slide=midiSlide(t);
        int key=metro::midiNote(state.targetPitch,integer(MIDI_ZERO_NOTE));auto& voice=midiVoices[t];
        int velocity=metro::midiValue(sequencer.velocity(t)*.2f);
        bool began=channel && velocity>0 && state.gate>=1.5f && !midiHeldSuppressed[t] && (voice.note<0 || voice.channel!=channel || voice.note!=key || voice.event!=midiEvents[t]);
        if((run || sequencer.playerPlaying[t]) && channel){
            int cv=metro::midiVoltage(state.cv,sequencer.settings.tracks[t].cvType==metro::BipolarCv);
            int values[]={cv,slide?127:0,integer(midiTrackParam(t,MIDI_AMOUNT_ALWAYS)) || slide?metro::midiValue(midiSlideAmount(t)):0};
            const int fields[]={MIDI_CV_CC,MIDI_SLIDE_CC,MIDI_AMOUNT_CC};
            for(int i=0;i<3;++i){bool initial=i==2 && integer(midiTrackParam(t,MIDI_AMOUNT_ALWAYS))?midiTrackLastCC[t*3+i]<0:began;
                if(initial || (ccDue && values[i]!=midiTrackLastCC[t*3+i])){midiCC(channel,integer(midiTrackParam(t,fields[i])),values[i]);midiTrackLastCC[t*3+i]=values[i];}}
        }
        voice.process(state.gate>=1.5f && !midiHeldSuppressed[t],key,channel,velocity,midiEvents[t],slide,emit);
    }
    if(run && ccDue)for(int l=0;l<8;++l){int signature=integer(midiModParam(l,MIDI_MOD_CHANNEL))*256+integer(midiModParam(l,MIDI_MOD_CC));if(signature!=midiModConfig[l]){midiModLastCC[l]=-1;midiModConfig[l]=signature;}const auto& config=sequencer.settings.modLanes[l];int value=metro::midiVoltage(sequencer.modulation.outputs[l],metro::effectiveModType(config.type,config.destination)==metro::ModBipolar);if(value!=midiModLastCC[l]){midiCC(integer(midiModParam(l,MIDI_MOD_CHANNEL)),integer(midiModParam(l,MIDI_MOD_CC)),value);midiModLastCC[l]=value;}}
    if(update){
        if(midiParametersSeen && !run)for(int t=0;t<2;++t){int channel=midiTrackChannel(t);
            for(int s=0;s<8;++s){int id=stageParam(t,s,CV);if(midiLastParameters[id]!=int(std::lround(params[id].getValue()*10000)))midiCC(channel,integer(midiTrackParam(t,MIDI_CV_CC)),metro::midiVoltage(metro::trackCvVoltage(params[id].getValue(),effectiveCvType(t)),effectiveCvType(t)==metro::BipolarCv));
                id=stageParam(t,s,SLIDE);if(midiLastParameters[id]!=int(std::lround(params[id].getValue()*10000)))midiCC(channel,integer(midiTrackParam(t,MIDI_SLIDE_CC)),integer(id)?127:0);
            }
            int id=trackParam(t,SLIDE_TIME);if(midiLastParameters[id]!=int(std::lround(params[id].getValue()*10000)))midiCC(channel,integer(midiTrackParam(t,MIDI_AMOUNT_CC)),metro::midiValue(midiSlideAmount(t)));
        }
        if(midiParametersSeen && !run)for(int l=0;l<8;++l)for(int s=0;s<8;++s){int id=modParam(l,MOD_VALUE+s);const auto& config=sequencer.settings.modLanes[l];if(midiLastParameters[id]!=int(std::lround(params[id].getValue()*10000)))midiCC(integer(midiModParam(l,MIDI_MOD_CHANNEL)),integer(midiModParam(l,MIDI_MOD_CC)),metro::midiVoltage(effectiveModVoltage(l,params[id].getValue()),metro::effectiveModType(config.type,config.destination)==metro::ModBipolar));}
        for(int id=0;id<NUM_PARAMS;++id)midiLastParameters[id]=int(std::lround(params[id].getValue()*10000));
        midiParametersSeen=true;
    }
    midiWasRun=run;
}
