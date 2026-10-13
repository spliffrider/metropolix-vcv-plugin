"""Create a Core + VCV Free clock-output comparison patch."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
patch=json.loads((root/'examples/Two-voice-demo.vcv').read_text())
module=next(m for m in patch['modules'] if m['id']==1)
params={p['id']:p['value'] for p in module['params']}
params.update({0:120,1:0,705:4,706:2})
module['params']=[dict(id=i,value=v) for i,v in sorted(params.items())]
module['data'].update(schemaVersion=18,expanded=False,uiPage=23,uiField=0)
for cable in patch['cables']:
    if cable['id']==9:cable['outputId']=4 # Right ADSR follows divided CLK.
    if cable['id']==1:cable['outputId']=4 # Scope shows the divided clock.
for m in patch['modules']:
    if m['id']==6:m['params']=[dict(id=2,value=-24)]
    if m['id']==7:m['params']=[dict(id=i,value=v) for i,v in enumerate([0,.35,1,.6])]
(root/'examples/Clock-output-demo.vcv').write_text(json.dumps(patch,indent=2)+'\n',newline='\n')
print('Created Clock-output-demo.vcv: left sequenced voice, right /4 offset2 clock voice')
