require(OutputKindCount==17 && OutputSourceCount==40 && outputSource(OutModTrigs,0)==38 && outputSource(OutModTrigs,1)==39,"1.6.5 appends two track Mod Trigs routes after the frozen38-source prefix");
for(int rate:{44100,48000,96000}) {
    double dt=1./rate;int width=int(std::ceil(.005*rate-1e-9));
    for(bool held:{false,true}){
        GateTrigger trigger;int high=0;
        for(int frame=0;frame<width+8;++frame){float v=trigger.process((frame==0 || held)?5.f:0.f,dt);require(v==0 || v==5,"Mod Trigs stay digital");high+=v>0;}
        require(high==width,"1.6.5 trigger lasts five milliseconds for short and held gates at every sample rate");
        trigger.process(0,dt);require(trigger.process(1.499f,dt)==0,"subthreshold CV does not trigger");require(trigger.process(1.5f,dt)==5,"the established1.5V boundary triggers");
        trigger.clear();require(trigger.process(-5,dt)==0,"clear removes a previous pulse");
    }
    {GateTrigger trigger;int high=0;double rackTime=float(1.f/rate);for(int f=0;f<rate/100;++f)high+=trigger.process(f?0.f:5.f,rackTime)>0;require(high==width,"Rack float sampleTime retains the same5ms sample count as double engine time");}
    Sequencer sequence;sequence.settings.outputSources={{38,39}};
    const float cv[2][4]={{5,5,0,5},{0,5,0,-5}};
    for(int t=0;t<2;++t){sequence.settings.tracks[t].length=4;for(int stage=0;stage<4;++stage){sequence.settings.tracks[t].stages[stage].cv=cv[t][stage];sequence.settings.tracks[t].stages[stage].probability=0;sequence.settings.stages[stage].gate=Rest;}}
    bool previous[2]={false,false};int period=rate/100;
    for(int pulse=0;pulse<8;++pulse){bool rising[2];for(int t=0;t<2;++t){bool high=cv[t][pulse%4]>=1.5f;rising[t]=high && !previous[t];previous[t]=high;}
        for(int frame=0;frame<period;++frame){sequence.process(dt,true,true,frame==0,false);for(int t=0;t<2;++t){near(sequence.aux[t],rising[t] && frame<width?5.f:0.f,"real A/B Mod Trigs follow only CV rising edges, independent of note rests/probability");near(sequence.sounding(t).gate,0,"CV triggers do not invent note gates");}}
    }
    Sequencer swapped;swapped.settings.outputSources={{38,39}};swapped.settings.tracks[0].stages[0].cv=5;swapped.process(dt,true,true,true,false);
    near(swapped.aux[0],5,"Track1 Mod Trigs reaches A");near(swapped.aux[1],0,"Track2 remains independent");
    swapped.settings.auxInputs[0].destination=ModOutputSwap;swapped.settings.auxInputs[0].connected=true;swapped.settings.auxInputs[0].amount=1;swapped.settings.auxInputs[0].voltage=5;swapped.process(dt,true,true,false,false);
    near(swapped.aux[0],0,"swap changes the trigger source track");near(swapped.aux[1],5,"swap preserves the already running source pulse");swapped.process(dt,false,true,false,false);near(swapped.aux[1],0,"stopping cancels active note-track triggers");
    Sequencer player;player.settings.outputSources={{38,39}};player.settings.tracks[0].stages[2].cv=5;player.loopy.stages=1u<<2;
    int highs=0;for(int frame=0;frame<width+5;++frame){player.process(dt,false,true,false,false);highs+=player.aux[0]>0;}
    require(highs==width,"stopped Stage Player CV produces one complete Mod Trigs pulse");player.loopy.stages=0;player.process(dt,false,true,false,false);player.loopy.stages=1u<<2;player.process(dt,false,true,false,false);near(player.aux[0],5,"a fresh Stage Player press re-arms the track CV trigger");
}
{
    Sequencer s;s.settings.outputSources={{38,39}};for(int t=0;t<2;++t)s.settings.tracks[t].stages[0].cv=5;s.settings.tracks[0].globalReset=false;
    const double dt=1./48000;s.process(dt,true,true,true,false);for(int i=0;i<99;++i)s.process(dt,true,true,false,false);
    s.process(dt,true,true,false,true);near(s.aux[0],5,"manual reset preserves a protected track's active CV trigger");near(s.aux[1],0,"manual reset cancels the reset track's trigger before its next note");
    for(int i=0;i<240;++i)s.process(dt,true,true,false,false);
    near(s.aux[0],0,"protected pulse expires normally without reset retrigger");
    s.process(dt,true,true,true,false);near(s.aux[1],5,"the reset track's next CV rising edge generates a new five-ms pulse");
}
