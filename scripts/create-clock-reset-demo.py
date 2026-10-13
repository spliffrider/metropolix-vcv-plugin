"""Create a VCV Core/Free example of stopped clock output and protected reset."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
patch=json.loads((root/'examples/Clock-output-demo.vcv').read_text())
module=next(m for m in patch['modules'] if m['id']==1)
params={p['id']:p['value'] for p in module['params']}
params.update({1:0,719:0,720:0,721:0,722:1,723:0,724:12,725:0,726:0})
module['params']=[dict(id=i,value=v) for i,v in sorted(params.items())]
module['data'].update(schemaVersion=23,uiPage=25,uiField=0,globalCursor=1,setupCursor=3)
(root/'examples/Clock-reset-performance.vcv').write_text(json.dumps(patch,indent=2)+'\n',encoding='utf-8',newline='\n')
print('Created Clock-reset-performance.vcv: Always CLK, sequenced left voice, protected TRK 2')
