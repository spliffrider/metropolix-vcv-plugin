"""Build a portable Stage/Track comparison using the manual's C-major example."""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
patch = json.loads((root / 'examples/Hardware-panel-demo.vcv').read_text())
version = json.loads((root / 'plugin.json').read_text())['version']
for module in patch['modules']:
    if module['plugin'] == 'MetropolixLab':
        module['version'] = version
        # Deliberately sparse: omitted parameters use their documented defaults.
        params = {0: 120, 1: 1, 3: 0, 4: 1, 5: 3, 413: 0, 414: 1}
        for s, note in enumerate([0, 2, 4, 5, 0, 2, 4, 5]):
            params[6+s] = note/36
            params[14+s] = 1
            params[22+s] = 2
        for track in range(2):
            base = 30+track*57
            params[base] = 4  # length
            params[base+2] = 4  # quarter-note stages
            params[base+4] = -12  # comfortable C3 register
            params[base+7] = 3  # positive limit
            params[base+9+2*8] = 1  # stage 1 accumulation
            params[156+track*3] = 3  # negative limit
        module['params'] = [dict(id=k, value=v) for k,v in sorted(params.items())]
        module['data'] = dict(schemaVersion=8, selectedTrack=1, selectedStage=0,
                              selectedRow=2, uiPage=14, uiField=2, uiHome=1,
                              expanded=False, modEditor=False, uiMod=False)
    if module['plugin'] == 'Core':
        # Do not carry a local device selection into a portable example.
        assert 'audio' not in module.get('data', {}) or module['data']['audio'].get('deviceId', -1) == -1

for cable in patch['cables']:
    if cable['inputModuleId'] == 2:
        cable['outputId'] = cable['inputId']  # Track 1 pitch/gate
    if cable['inputModuleId'] == 10:
        cable['outputId'] = 2+cable['inputId']  # Track 2 pitch/gate

path = root / 'examples/Accumulator-modes.vcv'
path.write_text(json.dumps(patch, indent=2)+'\n', encoding='utf-8')
print(path)
