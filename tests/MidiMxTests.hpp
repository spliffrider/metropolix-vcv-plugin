{
    auto mxCheck=[&](bool ok,const char* label){if(!ok)std::cerr<<"MX MIDI: "<<label<<'\n';check(ok);};
    std::array<float,8> gates{};std::array<int,8> levels{};levels.fill(127);gates.fill(5);
    MidiMxOutput disabled;messages.clear();disabled.process(0,0,0,gates,levels,emit);mxCheck(messages.empty(),"default channel off sends no messages without a physical expander");
    for(int channel=1;channel<=16;++channel){
        MidiMxOutput mx;messages.clear();for(int lane=0;lane<8;++lane){gates[lane]=lane%2?0:5;levels[lane]=20+lane*13;}
        mx.process(channel,0,7,gates,levels,emit);mxCheck(messages.size()==8,"CC mode initializes all eight controllers");
        for(int lane=0;lane<8;++lane)mxCheck(messages[lane]==Packet{{0xb0+channel-1,102+lane,lane%2?0:levels[lane],3}},"CC102..109 use the selected channel and stage-derived values");
        messages.clear();mx.process(channel,0,7,gates,levels,emit);mxCheck(messages.empty(),"unchanged CC values are not spammed");levels[0]=12;mx.process(channel,0,7,gates,levels,emit);mxCheck(messages.size()==1 && messages[0][1]==102 && messages[0][2]==12,"held modulation gate updates its CC value");
        messages.clear();mx.process(0,0,7,gates,levels,emit);mxCheck(messages.size()==4,"disabling the bus releases each active CC on its old channel");for(auto p:messages)mxCheck(p[0]==0xb0+channel-1 && p[2]==0,"CC release sends zero to the previous channel");
        messages.clear();mx.process(0,0,7,gates,levels,emit);mxCheck(messages.empty(),"disabled bus remains quiet");
        mx.process(channel,1,8,gates,levels,emit);mxCheck(messages.size()==4,"Note mode starts only high gates");
        int note=0;for(int lane=0;lane<8;lane+=2)mxCheck(messages[note++]==Packet{{0x90+channel-1,35+lane,levels[lane],3}},"notes35..42 retain independent stage velocities");
        messages.clear();levels[0]=99;mx.process(channel,1,8,gates,levels,emit);mxCheck(messages.empty(),"held notes do not retrigger when only velocity changes");
        mx.silence(emit,true);mxCheck(messages.size()==4,"panic releases all active notes");messages.clear();mx.process(channel,1,8,gates,levels,emit);mxCheck(messages.empty(),"panic consumes held gates until they fall");
        gates.fill(0);mx.process(channel,1,8,gates,levels,emit);gates[7]=5;mx.process(channel,1,8,gates,levels,emit);mxCheck(messages.size()==1 && messages[0][1]==42,"a fresh gate after panic starts its own note");
        messages.clear();int other=channel==16?1:channel+1;mx.process(other,1,8,gates,levels,emit);mxCheck(messages.size()==2 && messages[0][0]==0x80+channel-1 && messages[1][0]==0x90+other-1,"changing channel releases the old note before starting the new channel");
        messages.clear();mx.process(other,0,8,gates,levels,emit);mxCheck(messages.size()==9 && messages[0][0]==0x80+other-1,"switching Note to CC clears old notes before initializing controllers");
        messages.clear();mx.process(other,1,8,gates,levels,emit);mxCheck(messages.size()==2 && messages[0]==Packet{{0xb0+other-1,109,0,3}} && messages[1][0]==0x90+other-1,"switching CC to Note clears the active controller first");
        messages.clear();mx.process(other,1,0,gates,levels,emit);mxCheck(messages.size()==2 && messages[0][0]==0x80+other-1 && messages[1][0]==0x90+other-1,"changing MX function rebinds the current gate without hanging the previous voice");
        messages.clear();levels[7]=0;mx.process(other,1,0,gates,levels,emit);mxCheck(messages.size()==1 && messages[0][0]==0x80+other-1,"zero velocity releases a note rather than emitting an audible zero-velocity Note On");
        messages.clear();mx.silence(emit);mxCheck(messages.empty(),"silencing an already silent note is a no-op");
    }
    for(int rate:{44100,48000,96000}){
        MidiMxOutput mx;std::array<float,8> pulse{};std::array<int,8> velocity{};velocity.fill(127);messages.clear();int on=-1,off=-1,width=int(std::ceil(.005*rate-1e-9));
        for(int frame=0;frame<width+2;++frame){pulse[3]=frame<width?5:0;size_t before=messages.size();mx.process(16,1,8,pulse,velocity,emit);if(messages.size()!=before){if(messages.back()[0]==0x9f)on=frame;else if(messages.back()[0]==0x8f)off=frame;}}
        mxCheck(messages.size()==2 && on==0 && off==width,"five-ms MX gates produce one exactly timed Note On/Off pair at every sample rate");
    }
}
