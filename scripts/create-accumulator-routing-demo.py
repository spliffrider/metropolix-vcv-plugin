import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
patch=json.loads((root/'examples/Track-options-performance.vcv').read_text());m=patch['modules'][0];d=m['data'];values={q['id']:q['value'] for q in m['params']}
def param(index):return index if index<=421 else 429+index-422 if index<458 else 685+index-458 if index<478 else 707+index-478
def snapshot(id):return id if id<=421 else 422+id-429 if id<465 else 458+id-685 if id<705 else 478+id-707
# First three MOD lanes become independent performance gates. Lane8's separate
# percussion clock and the velocity-controlled bass stay patched throughout.
destinations=[(13,14,15),(16,17,18),(14,15,13),(18,13,17)]
for slot,scene in enumerate(d['presets'][:4]):
    for lane,dest in enumerate(destinations[slot]):
        base=194+lane*26;scene[base+2]=[4,3,2][lane];scene[base+6]=2;scene[base+7]=dest;scene[base+8]=1;scene[base+9]=0
        for st in range(8):scene[base+10+st]=5 if (st+lane+slot)%4<2 else 0;scene[base+18+st]=0
    scene[415]=1;scene[416]=29;scene[417]=0;scene[418]=-.5 if slot==2 else 1;scene[419]=29;scene[420]=1
    scene[413]=0;scene[414]=1;scene[snapshot(430)]=[50,62,66,75][slot];scene[snapshot(431)]=100;scene[snapshot(432)]=70
    for t in range(2):
        scene[30+t*57+7]=7
        scene[156+t*3]=4
        scene[157+t*3]=0
        scene[158+t*3]=0
        for st in range(8):scene[55+t*57+st]=1 if t==1 and st in (0,3,5) else 0
    assert len(scene)==490
for index,value in enumerate(d['presets'][0]):
    id=param(index)
    if id not in [0,1,2,5]:values[id]=value
values[1]=0;d['schemaVersion']=21;d['uiPage']=15;d['selectedCtrl']=1;d['uiMod']=False;m['version']='2.18.0';patch.pop('path',None)
m['params']=[{'id':id,'value':value} for id,value in sorted(values.items())]
(root/'examples/Accumulator-routing-performance.vcv').write_text(json.dumps(patch,indent=2)+'\n',encoding='utf-8')
