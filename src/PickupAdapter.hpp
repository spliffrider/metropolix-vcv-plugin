// Members of Metropolix. The audio thread owns pickup state; the UI receives
// atomic hints and reads complete serialized state through Rack's engine lock.
enum RecallMode {PitchOff=0,PitchThru=1,PitchJump=2,CtrlOff=0,CtrlAssigns=1,CtrlThru=2,CtrlJump=3};
metro::PanelPickup panelPickup{};
std::array<std::atomic<float>,10> pickupTargets{};
std::atomic<unsigned> pickupActiveMask{0};
static int panelParam(int index){return index<8?PITCH+index:ctrlParam(index-8,CTRL_VALUE);}
static int panelIndex(int id){if(id>=PITCH && id<PITCH+8)return id-PITCH;for(int k=0;k<2;++k)if(id==ctrlParam(k,CTRL_VALUE))return k+8;return -1;}
std::array<float,10> physicalControls(){std::array<float,10> values;for(int i=0;i<10;++i)values[i]=params[panelParam(i)].getValue();return values;}
void clearPickup(){panelPickup={};pickupActiveMask.store(0,std::memory_order_release);}
void publishPickup(){unsigned mask=0;for(int i=0;i<10;++i){pickupTargets[i].store(panelPickup[i].target,std::memory_order_relaxed);if(panelPickup[i].active)mask|=1u<<i;}pickupActiveMask.store(mask,std::memory_order_release);}
bool waitingPickup(int id){int i=panelIndex(id);return i>=0 && (pickupActiveMask.load(std::memory_order_acquire)&(1u<<i));}
float panelValue(int id){int i=panelIndex(id);return i>=0 && waitingPickup(id)?pickupTargets[i].load(std::memory_order_relaxed):params[id].getValue();}
float readPanel(int index){bool pitch=index<8;int mode=integer(pitch?LOAD_PITCH:LOAD_CTRL);return panelPickup[index].process(params[panelParam(index)].getValue(),mode==(pitch?PitchThru:CtrlThru),mode>=(pitch?PitchThru:CtrlThru));}
void recallMusical(const Snapshot& snapshot){
    for(int i=ROOT;i<SNAPSHOT_PARAMS;++i){int id=presetParam(i);if(!recallParam(id))continue;int panel=panelIndex(id);
        if(panel>=0){bool thru=integer(panel<8?LOAD_PITCH:LOAD_CTRL)==(panel<8?PitchThru:CtrlThru);if(thru){panelPickup[panel].recall(params[id].getValue(),snapshot.values[i]);continue;}panelPickup[panel].active=false;}
        params[id].setValue(snapshot.values[i]);
    }
}
json_t* pickupToJson(){json_t* array=json_array();for(const auto& p:panelPickup)json_array_append_new(array,json_pack("{s:b,s:f}","active",p.active,"target",double(p.target)));return array;}
void pickupFromJson(json_t* root){
    clearPickup();json_t* array=json_object_get(root,"pickup");if(!json_is_array(array) || json_array_size(array)!=10)return;
    for(int i=0;i<10;++i){json_t* p=json_array_get(array,i),*target=json_object_get(p,"target");if(!json_is_true(json_object_get(p,"active")) || !json_is_number(target))continue;double value=json_number_value(target);auto* q=paramQuantities[panelParam(i)];if(!std::isfinite(value) || value<q->minValue || value>q->maxValue)continue;panelPickup[i].recall(params[panelParam(i)].getValue(),float(value));}
    publishPickup();
}
void snapshotPickupJson(Snapshot& snapshot,json_t* data){
    json_t* array=json_object_get(data,"pickup");if(!json_is_array(array) || json_array_size(array)!=10)return;
    for(int i=0;i<10;++i){json_t* p=json_array_get(array,i),*target=json_object_get(p,"target");if(json_is_true(json_object_get(p,"active")) && json_is_number(target))snapshot.values[presetIndex(panelParam(i))]=sanitizePresetValue(panelParam(i),json_number_value(target));}
}
