int screenParam(Metropolix* m);
int setupMenuParam(Metropolix* m,int item);
bool requiresConfirmation(Metropolix* m,int id){
    if(!m || id<0 || m->ctrlOwner(id)>=0)return false;
    if(auto* type=dynamic_cast<ModTypeQuantity*>(m->getParamQuantity(id)))if(type->forced())return false;
    if(binaryStagePage(m) && m->uiAlt.load())return false;
    if(m->uiPage.load()==Metropolix::SETUP_PAGE && m->uiField.load() && id==setupMenuParam(m,-1))return true;
    if(id>=Metropolix::MX_MODE && id<=Metropolix::MX_LANE)return true;
    if(m->uiPage.load()==Metropolix::MX_SETUP_PAGE && id==Metropolix::MIDI_MX_CHANNEL)return true;
    for(int input=0;input<3;++input)if(id==Metropolix::auxParam(input,Metropolix::AUX_DESTINATION) || id==Metropolix::auxParam(input,Metropolix::AUX_ASSIGNMENT))return true;
    for(int knob=0;knob<2;++knob)if(id==Metropolix::ctrlParam(knob,Metropolix::CTRL_DESTINATION) || id==Metropolix::ctrlParam(knob,Metropolix::CTRL_ASSIGNMENT))return true;
    for(int lane=0;lane<8;++lane)if(id==Metropolix::modParam(lane,Metropolix::MOD_DESTINATION) || id==Metropolix::modParam(lane,Metropolix::MOD_ASSIGNMENT))return true;
    if(m->uiAlt.load() && m->uiPage.load()==Metropolix::STAGE_PAGE){bool gate=m->uiMod.load()?m->effectiveModType(m->selectedLane.load())==metro::ModGate:m->selectedRow.load()==Metropolix::CV && m->integer(Metropolix::cvTypeParam(m->selectedTrack.load()))==metro::GateCv;if(gate)return false;}
    return m->integer(Metropolix::ENC_CONFIRM)!=0;
}
bool pendingConfirmation(Metropolix* m){return m && m->uiDraftParam>=0 && m->uiDraftParam==screenParam(m);}
std::vector<int> draftContextParams(Metropolix* m,int id){
    std::vector<int> ids;auto* q=m->getParamQuantity(id);
    auto add=[&](int param){if(param>=0 && param<Metropolix::NUM_PARAMS)ids.push_back(param);};
    if(auto* cv=dynamic_cast<TrackCvQuantity*>(q)){add(cv->typeParam);add(cv->velocityParam);}
    if(auto* mod=dynamic_cast<ModVoltageQuantity*>(q)){add(mod->typeParam);add(mod->destinationParam);}
    if(auto* clock=dynamic_cast<ClockOffsetQuantity*>(q))add(clock->divisionParam);
    if(auto* type=dynamic_cast<ModTypeQuantity*>(q))add(type->destinationParam);
    return ids;
}
bool validDraft(Metropolix* m){
    if(!pendingConfirmation(m) || m->uiDraftAlt!=m->uiAlt.load() || !requiresConfirmation(m,m->uiDraftParam))return false;
    if(m->uiDraftIds.empty() || m->uiDraftIds.size()!=m->uiDraftRawBefore.size())return false;
    // Compare stored parameters, not their effective values: unipolar/gate CV
    // quantities can hide a negative voltage that must survive cancellation.
    for(size_t i=0;i<m->uiDraftIds.size();++i){int id=m->uiDraftIds[i];if(m->ctrlOwner(id)>=0 || m->params[id].getValue()!=m->uiDraftRawBefore[i])return false;}
    if(draftContextParams(m,m->uiDraftParam)!=m->uiDraftContextIds || m->uiDraftContextIds.size()!=m->uiDraftContextBefore.size())return false;
    for(size_t i=0;i<m->uiDraftContextIds.size();++i)if(m->params[m->uiDraftContextIds[i]].getValue()!=m->uiDraftContextBefore[i])return false;
    return true;
}
void invalidateDraft(Metropolix* m){if(m && m->uiDraftParam>=0 && !validDraft(m))m->cancelDraft();}
std::string previewDisplayString(Metropolix* m,int id){
    auto* q=m->getParamQuantity(id);if(id!=m->uiDraftParam)return q->getDisplayValueString();
    float v=m->uiDraftValue;
    if(auto* pitch=dynamic_cast<PitchOverrideQuantity*>(q)){if(v<-.5f)return "-- ?";int note=metro::degreeToSemitone(metro::nearestDegree(v,pitch->scale()),pitch->scale());const char* names[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};return string::f("%s%d ?",names[metro::modulo(note,12)],note/12);}
    if(auto* choice=dynamic_cast<SwitchQuantity*>(q)){int index=metro::clamp(int(std::round(v-q->getMinValue())),0,int(choice->labels.size())-1);return choice->labels[index]+" ?";}
    if(auto* cv=dynamic_cast<TrackCvQuantity*>(q))if(cv->type()==metro::GateCv)return v>=1.5f?"On ?":"Off ?";
    if(auto* cv=dynamic_cast<ModVoltageQuantity*>(q))return cv->gate()?(v>=1.5f?"On ?":"Off ?"):metro::modDisplayText(v,cv->destination(),false)+" ?";
    float shown=v;if(q->displayBase>0)shown=std::pow(q->displayBase,shown);else if(q->displayBase<0)shown=std::log(shown)/std::log(-q->displayBase);
    shown=shown*q->displayMultiplier+q->displayOffset;
    return string::f(q->snapEnabled?"%.0f ?":"%.3g ?",shown);
}
bool confirmDraft(Metropolix* m){
    if(!pendingConfirmation(m))return false;
    // Consume an obsolete confirmation without falling through to a field or
    // secondary action. The external edit remains the applied value.
    if(!validDraft(m)){m->cancelDraft();return true;}
    int id=m->uiDraftParam;float value=m->uiDraftValue,base=m->uiDraftOrigin;auto ids=m->uiDraftIds;auto before=m->uiDraftBefore;m->cancelDraft();
    if(m->ctrlOwner(id)<0){
        if(ids.size()<=1)setWithHistory(m->getParamQuantity(id),value);
        else {std::vector<float> values;auto* pitch=dynamic_cast<PitchOverrideQuantity*>(m->getParamQuantity(id));
            if(pitch){auto scale=pitch->scale();int off=metro::nearestDegree(0,scale)-1,max=metro::nearestDegree(60,scale);auto degree=[&](float v){return v<0?off:metro::nearestDegree(v,scale);};int delta=degree(value)-degree(base);for(float v:before){int d=metro::clamp(degree(v)+delta,off,max);values.push_back(d==off?-1.f:float(metro::clamp(metro::degreeToSemitone(d,scale),0,60)));}}
            else for(float v:before)values.push_back(v+value-base);
            workflowWrite(m,ids,values,"confirm screen values");
        }
    }
    return true;
}
void turnDraft(Metropolix* m,int id,float dy,float& remainder,bool fine){
    if(m->uiDraftParam>=0 && !validDraft(m)){m->cancelDraft();remainder=0;}
    if(id!=screenParam(m) || !requiresConfirmation(m,id))return;
    auto* q=m->getParamQuantity(id);if(m->uiDraftParam!=id){m->cancelDraft();m->uiDraftParam=id;m->uiDraftBase=m->params[id].getValue();m->uiDraftAlt=m->uiAlt.load();m->uiDraftOrigin=m->uiDraftValue=q->getValue();m->uiDraftIds.push_back(id);
        if(m->uiAlt.load() && m->uiPage.load()==Metropolix::STAGE_PAGE && (m->uiMod.load() || m->selectedRow.load()==Metropolix::CV || m->selectedRow.load()>=Metropolix::PITCH_OVERRIDE)){m->uiDraftIds.clear();for(int s=0;s<8;++s)m->uiDraftIds.push_back(m->uiMod.load()?Metropolix::modParam(m->selectedLane.load(),Metropolix::MOD_VALUE+s):Metropolix::stageParam(m->selectedTrack.load(),s,m->selectedRow.load()));}
        if(stagePairPage(m) && m->uiAlt.load())m->uiDraftIds=stagePairIds(m,false,true);
        if(simpleStagePage(m) && !binaryStagePage(m) && m->uiAlt.load())m->uiDraftIds=simpleStageIds(m);
        for(int next:m->uiDraftIds){m->uiDraftBefore.push_back(m->getParamQuantity(next)->getValue());m->uiDraftRawBefore.push_back(m->params[next].getValue());}
        m->uiDraftContextIds=draftContextParams(m,id);for(int next:m->uiDraftContextIds)m->uiDraftContextBefore.push_back(m->params[next].getValue());}
    bool gate=false;if(auto* cv=dynamic_cast<TrackCvQuantity*>(q))gate=cv->type()==metro::GateCv;if(auto* cv=dynamic_cast<ModVoltageQuantity*>(q))gate=cv->gate();
    if(q->snapEnabled || gate){
        remainder-=dy/(fine?10.f:1.f);int steps=int(remainder/22.f);
        if(steps){
            if(gate)m->uiDraftValue=steps>0?5:0;
            else if(id==Metropolix::SCALE && m->uiPage.load()==Metropolix::SCALE_PAGE){
                // Preview the hardware bank order while keeping legacy stored IDs.
                int position=metro::scaleBrowsePosition(int(std::round(m->uiDraftValue)));
                m->uiDraftValue=float(metro::scaleBrowseId(metro::clamp(position+steps,0,148)));
            }
            else if(auto* pitch=dynamic_cast<PitchOverrideQuantity*>(q)){
                auto scale=pitch->scale();int off=metro::nearestDegree(0,scale)-1;
                int degree=m->uiDraftValue<0?off:metro::nearestDegree(m->uiDraftValue,scale);
                degree=metro::clamp(degree+steps,off,metro::nearestDegree(60,scale));
                m->uiDraftValue=degree==off?-1.f:float(metro::degreeToSemitone(degree,scale));
            }
            else m->uiDraftValue+=steps;
            remainder-=steps*22.f;
        }
    }
    else m->uiDraftValue-=dy*q->getRange()*.0007f*(fine?.1f:1.f);
    m->uiDraftValue=metro::clamp(m->uiDraftValue,q->getMinValue(),q->getMaxValue());
    if(m->uiDraftValue==q->getValue())m->cancelDraft();
}
