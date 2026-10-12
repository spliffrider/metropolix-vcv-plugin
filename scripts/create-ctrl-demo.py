"""Add playable CTRL assignments to the existing evolving two-voice arrangement."""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
patch = json.loads((root / 'examples/Musical-variations.vcv').read_text(encoding='utf-8'))
version = json.loads((root / 'plugin.json').read_text(encoding='utf-8'))['version']
patch.pop('path', None)
for module in patch['modules']:
    if module['plugin'] == 'MetropolixLab':
        module['version'] = version
        params = {p['id']: p['value'] for p in module['params']}
        params.update({415: 0, 416: 13, 417: 1, 418: 0, 419: 16, 420: 1})
        module['params'] = [dict(id=k, value=v) for k, v in sorted(params.items())]
        module['data'].update(schemaVersion=9, uiPage=15, uiField=0,
                              selectedCtrl=0, uiMod=False, expanded=False)
    if module['plugin'] == 'Core':
        assert 'audio' not in module.get('data', {}) or module['data']['audio'].get('deviceId', -1) == -1
path = root / 'examples/CTRL-performance.vcv'
path.write_text(json.dumps(patch, indent=2) + '\n', encoding='utf-8')
print(path)
