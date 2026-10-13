"""Four related performance presets using Night Current's existing synth voices.

Run with Python from any directory. No installed plugins or private paths are
read. The compact older demo omits defaults; expand them for bank format 1.
"""
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
patch = json.loads((root / 'examples/CTRL-performance.vcv').read_text())
version = json.loads((root / 'plugin.json').read_text())['version']
module = next(m for m in patch['modules'] if m['plugin'] == 'MetropolixLab')
p = dict.fromkeys(range(428), 0)
p.update({0: 108, 3: 9, 4: 2, 5: 3, 403: 1, 422: 32,
          423: 2, 424: 1, 425: 1, 426: 2, 427: 1})
for t in range(2):
    for i, value in enumerate([8, 0, 1, 0, 0, 50, 25, 9, 0]):
        p[30 + t * 57 + i] = value
    for row, value in enumerate([1, 100, 0, 0, 0, 0]):
        for stage in range(8):
            p[30 + t * 57 + 9 + row * 8 + stage] = value
    p[144 + t * 6 + 1] = 1
    p[156 + t * 3] = 9
for lane in range(8):
    p[194 + lane * 26] = 8
    p[194 + lane * 26 + 2] = 1
p.update({v['id']: v['value'] for v in module['params']})
# A consistent eight-bar maximum lets the variation lanes develop naturally.
# The QA capture overrides Total to one bar to exercise several loads in 12 s.
p[421] = 128
slots = []
for pattern in range(4):
    q = p.copy()
    if pattern == 1:  # Open register / syncopated response, same harmony.
        q[87 + 4] += 12
        q[87 + 3] = 3  # Odd / Even traversal
        q[87 + 9 + 0 * 8 + 4] = 2
        q[87 + 9 + 4 * 8 + 3] = 1  # Skip a middle stage
    elif pattern == 2:  # Sparse breakdown, short melody and absent bass.
        q[30 + 8] = 1
        q[87 + 5] = 34
        q[87 + 9 + 4 * 8 + 2] = 1
        q[87 + 9 + 4 * 8 + 5] = 1
        q[87 + 9 + 4 * 8 + 7] = 1
    elif pattern == 3:  # Return with a longer held answer and a small fill.
        q[87 + 3] = 0
        q[14 + 4] = 3
        q[22 + 4] = 3  # Hold
        q[87 + 9 + 0 * 8 + 6] = 3
        q[87 + 9 + 3 * 8 + 4] = 1  # Slide
    slots.append([q[i] for i in range(422)])
module['version'] = version
module['params'] = [dict(id=i, value=p[i]) for i in range(428)]
module['data'].update(schemaVersion=10, presetFormat=1,
                      presets=slots + [None] * 60, presetBank=0,
                      uiPage=16, uiField=0, uiMod=False)
patch.pop('path', None)
for m in patch['modules']:
    if m['plugin'] == 'Core' and 'audio' in m.get('data', {}):
        assert m['data']['audio'].get('deviceId', -1) == -1
output = root / 'examples/Preset-performance.vcv'
output.write_text(json.dumps(patch, indent=2) + '\n')
print(output)
