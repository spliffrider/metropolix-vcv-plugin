"""Build the stopped two-voice CV demonstration from the slide performance."""
import json
from pathlib import Path
root=Path(__file__).resolve().parent.parent
patch=json.loads((root/'examples/Slide-performance.vcv').read_text())
m=patch['modules'][0]
values={a['id']:a['value'] for a in m['params']}
types=[(0,0),(1,1),(2,2),(0,1)]
def musical(v,slot):
    v[1]=0;v[402]=0;v[403]=1;v[685]=0;v[686]=0
    v[687],v[688]=types[slot]
    for track in range(2):
        for stage in range(8):
            # Original row-major CV parameters: TRACK + t*57 + 9 + 5*8 + s.
            cv=[-2,4,0,2,1,3,-1,0][(stage+slot)%8]
            if types[slot][track]==2:cv=5 if stage in [0,2,3,6] else 0
            v[79+track*57+stage]=cv
            v[689+track*8+stage]=(stage+slot)%3
    v[421]=32
musical(values,0)
m['params']=[dict(id=k,value=v) for k,v in sorted(values.items())]
m['data']['schemaVersion']=17;m['data']['presetFormat']=5
m['data']['uiPage']=8;m['data']['selectedRow']=5;m['data']['uiStageDetail']=True
for link in m['data']['chains']['working']:link['mutes'] &= ~3
ids=list(range(422))+list(range(429,465))+[685,686]+list(range(687,705))
assert len(ids)==478
for slot,array in enumerate(m['data']['presets']):
    if array is None:continue
    assert len(array)==460
    v={id:value for id,value in zip(ids,array+[0]*18)}
    musical(v,slot);array[:]=[v[id] for id in ids]
for cable in patch['cables']:
    if cable['id']==25:cable['outputModuleId']=1;cable['outputId']=6
(root/'examples/Stage-CV-performance.vcv').write_text(json.dumps(patch,indent=2)+'\n',encoding='utf-8')
print('PASS stage CV demonstration generated')
