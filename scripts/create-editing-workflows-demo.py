"""Create a portable, stopped two-voice patch for live editing workflows."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
patch=json.loads((root/'examples/Clock-reset-performance.vcv').read_text())
module=next(m for m in patch['modules'] if m['id']==1)
params={p['id']:p['value'] for p in module['params']}
params[1]=0
params[727]=0
module['params']=[dict(id=i,value=v) for i,v in sorted(params.items())]
module['data'].update(schemaVersion=24,uiPage=8,uiField=0,selectedRow=1,
                      selectedStage=0,selectedTrack=0,uiMod=False,uiAlt=False,
                      uiEdit=True,uiStageDetail=True)
(root/'examples/Editing-workflows-performance.vcv').write_text(
    json.dumps(patch,indent=2)+'\n',encoding='utf-8',newline='\n')
print('Created Editing-workflows-performance.vcv: two voices, editable probability, stopped transport')
