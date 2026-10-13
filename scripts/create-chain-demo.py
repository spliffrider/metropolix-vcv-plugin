"""Original 32-bar chain arrangement using the existing two-voice preset demo."""
import copy
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
patch = json.loads((root / 'examples/Preset-performance.vcv').read_text())
module = next(m for m in patch['modules'] if m['plugin'] == 'MetropolixLab')
module['version'] = json.loads((root / 'plugin.json').read_text())['version']
params = {p['id']: p['value'] for p in module['params']}
params.update({1: 1, 421: 64, 428: 0})
for preset in module['data']['presets'][:4]:
    preset[421] = 64  # One repeat = four bars at sixteenth-note clock resolution.
    preset[38] = 0  # Bass breakdown comes from the chain's Track 1 mute flag.
    preset[194 + 2] = 8  # Four-bar filter swell, reset with each repeat.
    preset[220 + 2] = 16  # One bar per chord: Am / F / C / G.
    preset[272 + 9] = 1  # Register lifts now come from the chosen preset.
    preset[324 + 2] = 8  # Articulation follows each half bar.
    preset[376 + 9] = 1  # Bass breaks now come from per-link chain mutes.
params.update(enumerate(module['data']['presets'][0]))
params[1] = 1
module['params'] = [dict(id=i, value=params.get(i, 0)) for i in range(429)]
links = [dict(slot=slot, repeats=repeats, mutes=mutes) for slot, repeats, mutes in
         [(0, 2, 0), (1, 1, 0), (0, 1, 0), (2, 1, 1),
          (3, 1, 0), (1, 1, 4), (3, 1, 0)]]
module['data'].update(schemaVersion=11, chainFormat=1, uiPage=19, uiField=0,
                      uiMod=False, expanded=False,
                      chains=dict(working=links, saved=[copy.deepcopy(links)] + [[] for _ in range(7)],
                                  enabled=True, loaded=0, follow=True, cursor=1))
patch.pop('path', None)
for m in patch['modules']:
    if m['plugin'] == 'Core' and 'audio' in m.get('data', {}):
        assert m['data']['audio'].get('deviceId', -1) == -1
output = root / 'examples/Chain-performance.vcv'
output.write_text(json.dumps(patch, indent=2) + '\n')
print(output)
