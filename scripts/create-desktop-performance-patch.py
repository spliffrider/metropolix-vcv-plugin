"""Open the accepted bass-and-lead groove in the optional desktop view."""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
patch = json.loads((root / 'examples/Pulse-Pressure-Sidechain-Opening-Loop.vcv').read_text(encoding='utf-8'))
for module in patch['modules']:
    if module['id'] == 1:
        module['version'] = '2.71.0'
        module['data'].update(expanded=True, performanceEditor=True, modEditor=False)
    elif module['pos'][1] == 0:
        module['pos'][0] += 32
patch['zoom'] = .65
path = root / 'examples/Pulse-Pressure-Desktop.vcv'
path.write_text(json.dumps(patch, indent=2) + '\n', encoding='utf-8', newline='\n')
print(path)
