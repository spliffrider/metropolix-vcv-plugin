"""Save the two Pulse Pressure video openings as editable, continuous loops.

Keeps the published patches' opening parameters, modules, wiring and stored
presets. Disables chain playback and Total Pulses automatic resets only.
"""
import copy
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
for name in ['Pulse-Pressure', 'Pulse-Pressure-Sidechain']:
    source = ROOT / 'examples' / (name + '.vcv')
    patch = json.loads(source.read_text(encoding='utf-8'))
    opening = copy.deepcopy(patch)
    metro = next(m for m in opening['modules'] if m['plugin'] == 'MetropolixLab' and m['model'] == 'Metropolix')
    metro['data']['chains']['enabled'] = False
    next(p for p in metro['params'] if p['id'] == 421)['value'] = 0
    assert next(p for p in metro['params'] if p['id'] == 1)['value'] == 1
    assert not any(m.get('model') == 'AudioQA' for m in opening['modules'])
    path = ROOT / 'examples' / (name + '-Opening-Loop.vcv')
    path.write_text(json.dumps(opening, indent=2) + '\n', encoding='utf-8', newline='\n')
    print(path)
