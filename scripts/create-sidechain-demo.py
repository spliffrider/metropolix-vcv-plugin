"""Create a 45-second Pulse Pressure sidechain A/B companion performance.

Requires the same plugins as Pulse Pressure plus Bogaudio PRESSOR. All musical
events and the A/B control originate in the sequencer, not the capture harness.
"""
import copy
import json
import math
import runpy
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
base = runpy.run_path(str(ROOT / 'scripts/create-pulse-pressure-demo.py'))
patch = copy.deepcopy(base['patch'])
modules = {m['id']: m for m in patch['modules']}
metro = modules[1]
presets = []
states = [False, True, False, True, True, True, False]
for i, enabled in enumerate(states):
    values = copy.deepcopy(base['slots'][2])
    values[421] = 64  # Four bars per link; six musical links, then effects tail.
    values[403] = 9   # B = MOD 8, used only for the compressor ratio control.
    values[430] = 56
    values[378] = 1
    values[385] = 0
    for stage in range(8):
        values[386 + stage] = 5 if enabled else 0
        values[394 + stage] = 0
    # Repeat the same four-bar timbral curve for a useful on/off comparison.
    values[326] = 8
    for stage, volts in enumerate([.6, .8, 1.1, 1.5, 1.3, 1.1, .8, .6]):
        values[334 + stage] = volts
    if i >= 4:
        for stage in [1, 4]:
            values[96 + stage] = 2
        for lane in [3, 4]:
            values[194 + lane * 26 + 9] = 0
    if i == 6:
        values[38] = values[95] = 1
        for lane in range(8):
            values[194 + lane * 26 + 9] = 1
    presets.append(values)

metro['params'] = [dict(id=k, value=v) for k, v in sorted(presets[0].items())]
next(p for p in metro['params'] if p['id'] == 422)['value'] = 64
metro['data']['presets'] = [[p[k] for k in base['preset_ids']] for p in presets] + [None] * 57
links = [dict(slot=i, repeats=1, mutes=0) for i in range(7)]
metro['data']['chains'] = dict(working=links, saved=[copy.deepcopy(links)] + [[] for _ in range(7)],
                            enabled=True, loaded=0, follow=True, cursor=1)

# Replace the upper pitch scope with a real stereo sidechain compressor. The
# lower scope becomes the final stereo mix; both new buses fit the same camera.
patch['modules'] = [m for m in patch['modules'] if m['id'] not in [10, 25]]
for id, x in [(15, 93), (16, 102), (9, 111), (12, 84)]:
    modules[id]['pos'][0] = x
for id in [15, 16]:
    next(p for p in modules[id]['params'] if p['id'] == 3)['value'] = .5

pressor = dict(id=31, plugin='Bogaudio', model='Bogaudio-Pressor', version='2.3.43', pos=[78, 0],
               params=[dict(id=k, value=v) for k, v in {
                   0: 1 / 3, 1: 1, 2: math.sqrt(2 / 500), 3: math.sqrt(.180 / 2),
                   4: 0, 5: 0, 6: 1, 7: 1, 8: 0, 9: 1}.items()],
               data={'threshold_range': 1.0})
patch['modules'].append(pressor)
for id, x in [(40, 93), (41, 102)]:
    patch['modules'].append(dict(id=id, plugin='Fundamental', model='VCMixer', version='2.6.4',
        pos=[x, 1], params=[dict(id=k, value=v) for k, v in
            {0: 1, 1: 1, 2: math.sqrt(.5 * .74 ** 2), 3: 0, 4: 0}.items()],
        data={'chExp': False, 'mixExp': False}))

patch['cables'] = [c for c in patch['cables'] if
    c['inputModuleId'] not in [9, 10, 25] and
    not (c['outputModuleId'] == 23 and c['inputModuleId'] in [15, 16]) and
    not (c['outputModuleId'] == 12 and c['inputModuleId'] == 28) and
    not (c['outputModuleId'] == 1 and c['outputId'] == 6)]

def cable(src, out, dst, inp, color):
    assert not any(c['inputModuleId'] == dst and c['inputId'] == inp for c in patch['cables'])
    patch['cables'].append(dict(outputModuleId=src, outputId=out, inputModuleId=dst, inputId=inp, color=color))

for mix, channel in [(15, 0), (16, 4)]:
    cable(12, 1, mix, 3, '#55d7c4')
    cable(mix, 0, 31, channel, '#55d7c4')
cable(20, 0, 31, 1, '#ff687f')  # Actual kick AUDIO drives the detector.
cable(1, 6, 31, 3, '#89baf5')   # MOD 8 sets 1:1 or approximately 1.8:1 ratio.
for master, output, channel in [(40, 1, 0), (41, 2, 1)]:
    cable(31, output, master, 1, '#55d7c4')
    cable(23, 0, master, 2, '#ff687f')  # Drums bypass the compressor.
    cable(master, 0, 9, channel, '#62bfc1')
for i, c in enumerate(patch['cables'], 1):
    c['id'] = i

path = ROOT / 'examples/Pulse-Pressure-Sidechain.vcv'
path.write_text(json.dumps(patch, indent=2) + '\n', newline='\n')
plan = dict(bpm=138, bar_seconds=240 / 138, musical_end=24 * 240 / 138, capture_seconds=45,
            sections=[dict(start_seconds=i * 4 * 240 / 138, sidechain=state) for i, state in enumerate(states)],
            threshold_db=-14, attack_ms=2, release_ms=180,
            on_ratio=1 / math.tan((1 - .5 ** 1.5) * math.pi / 4),
            routing='Kick audio → PRESSOR sidechain; bass/lead/delay/reverb → stereo audio inputs; drums summed afterward')
(ROOT / 'tmp/sidechain-plan.json').write_text(json.dumps(plan, indent=2) + '\n')
print(path)
print(len(patch['modules']), 'modules,', len(patch['cables']), 'cables, 24 musical bars plus effect tail')
