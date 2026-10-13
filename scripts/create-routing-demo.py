import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
patch=json.loads((root/'examples/Accumulator-routing-performance.vcv').read_text());m=patch['modules'][0];d=m['data']
def param(index):return index if index<=421 else 429+index-422 if index<458 else 685+index-458 if index<478 else 707+index-478
for slot,scene in enumerate(d['presets'][:4]):
    for lane in range(5):
        base=194+lane*26;dest=19+slot*5+lane;gate=dest in (21,23,30,32,33,35)
        scene[base+2]=[4,3,2,4,2][lane];scene[base+6]=2 if gate else 0;scene[base+7]=dest;scene[base+8]=1;scene[base+9]=0
        for st in range(8):
            scene[base+10+st]=5 if gate and (st+lane)%4<2 else 0 if gate else [0,1,-1,.75,0,-.75,1,0][(st+lane)%8]
            if dest==19:scene[base+10+st]*=.15
            if dest==20:scene[base+10+st]*=.2
            scene[base+18+st]=0
    # Gate-driven Loopy starts at the current lead track and has two pulses.
    # Skip inversion alternates two complementary playable stage groups.
    for st in range(8):scene[128+st]=1 if slot==2 and st in (1,4,7) else 0
    assert len(scene)==490
values={q['id']:q['value'] for q in m['params']}
for index,value in enumerate(d['presets'][0]):
    id=param(index)
    if id not in (0,1,2,5):values[id]=value
values[1]=0;values[465]=2;values[466]=0
m['params']=[{'id':id,'value':value} for id,value in sorted(values.items())];m['version']='2.19.0'
d.update(schemaVersion=22,uiPage=0,uiHome=3,uiMod=False,selectedTrack=1,selectedLane=0)
patch.pop('path',None)
(root/'examples/Complete-routing-performance.vcv').write_text(json.dumps(patch,indent=2)+'\n',encoding='utf-8')
print('PASS four complete-routing scenes, percussion and velocity bass retained; saved stopped')
