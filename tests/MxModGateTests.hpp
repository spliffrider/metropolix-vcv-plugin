check(MxModGate==7 && MxModTrig==8 && MxModeCount==9);
for(int rate:{44100,48000,96000})for(int lane=0;lane<8;++lane){
    double dt=1./rate;int width=int(std::ceil(rate*.005-1e-9));MxLogic mx;std::array<float,8> values{};values[lane]=5;
    int highs=0;for(int frame=0;frame<width+5;++frame){auto out=mx.process(dt,MxModTrig,false,.125,true,false,true,true,{},-1,false,false,-1,false,false,values);for(int i=0;i<8;++i){check(out[i]==0 || out[i]==5);if(i!=lane)check(out[i]==0);}highs+=out[lane]>0;values[lane]=0;}
    check(highs==width);values[lane]=1.499f;check(mx.process(dt,MxModTrig,false,.125,true,false,true,true,{},-1,false,false,-1,false,false,values)[lane]==0);
    values[lane]=1.5f;check(mx.process(dt,MxModTrig,false,.125,true,false,true,true,{},-1,false,false,-1,false,false,values)[lane]==5);
    check(mx.process(dt,MxModTrig,false,.125,true,false,false,true,{},-1,false,false,-1,false,false,values)[lane]==0);
    auto gate=mx.process(dt,MxModGate,false,.125,true,false,true,true,{},-1,false,false,-1,false,false,values);for(int i=0;i<8;++i)check(gate[i]==(i==lane?5:0));
    auto restart=mx.process(dt,MxModTrig,false,.125,true,false,true,true,{},-1,false,false,-1,false,false,values);check(restart[lane]==5);
    values[lane]=0;auto reset=mx.process(dt,MxModTrig,false,.125,true,true,true,true,{},-1,false,false,-1,false,false,values);for(float v:reset)check(v==0);
}
