"""Prepare Pulse Pressure for the Axiom Pro factory GM Patch.

CC numbers follow Appendix A of M-Audio's Axiom Pro User Guide. Hardware
device selection, MIDI learn overrides and physical response testing are left
to the user's connected-controller session. No controller memory is written.
"""
import copy
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = ROOT / 'examples/Pulse-Pressure-Sidechain-Opening-Loop.vcv'
patch = copy.deepcopy(json.loads(source.read_text(encoding='utf-8')))
metro = next(m for m in patch['modules'] if m['id'] == 1)

# Both offsets remain at noon, preserving the opening groove until moved.
for param_id, value in {416: 13, 417: 1, 419: 16, 420: 1}.items():
    next(p for p in metro['params'] if p['id'] == param_id)['value'] = value

for module in patch['modules']:
    module['pos'][0] += 20

faders = [(cc, 1, 6 + stage) for stage, cc in enumerate([74, 71, 91, 93, 73, 72, 5, 84])]
knobs = [
    (10, 1, 415),   # E1: CTRL 1, lead pulse-count offset.
    (2, 1, 418),    # E2: CTRL 2, lead ratchet offset.
    (12, 4, 1),     # E3: bass filter cutoff.
    (13, 7, 1),     # E4: lead filter cutoff.
    (75, 4, 0),     # E5: bass filter resonance.
    (76, 7, 0),     # E6: lead filter resonance.
    (92, 12, 2),    # E7: delay tone; Mix does not affect the patched Wet output.
    (95, 30, 1),    # E8: reverb wet level.
]
for module_id, x, mappings in [(50, 0, faders), (51, 10, knobs)]:
    patch['modules'].append({
        'id': module_id, 'plugin': 'Core', 'model': 'MIDI-Map',
        'version': '2.6.6', 'pos': [x, 0], 'params': [],
        'data': {'smooth': True, 'midi': {'channel': 0}, 'maps': [
            {'cc': cc, 'moduleId': target, 'paramId': param}
            for cc, target, param in mappings
        ]},
    })
patch['modules'].append({
    'id': 52, 'plugin': 'Core', 'model': 'Notes', 'version': '2.6.6',
    'pos': [0, 1], 'params': [], 'data': {'text': (
        'AXIOM PRO / GM PATCH\n\n'
        'Prepared template; hardware not yet tested.\n'
        'MIDI-Map left = S1-S8 stage pitch.\n'
        'MIDI-Map right = E1-E8 performance.\n\n'
        'Later: select GM Patch 1 on Axiom.\n'
        'Use absolute MIDI CC, channel 1.\n'
        'Select Windows MIDI and the regular Axiom input on BOTH MIDI-Maps.\n'
        'Select your output device in AUDIO.\n\n'
        'E1: lead pulse count / CTRL 1\n'
        'E2: lead ratchets / CTRL 2\n'
        'E3/E4: bass/lead cutoff\n'
        'E5/E6: bass/lead resonance\n'
        'E7: delay tone\n'
        'E8: reverb level\n\n'
        'S9, pads, transport and keys await live setup.\n'
        'Smooth CC is on; pickup is not provided.\n'
        'Guide: docs/AXIOM-PRO.md'
    )},
})
patch['zoom'] = 0.8
path = ROOT / 'examples/Pulse-Pressure-Axiom-Pro.vcv'
path.write_text(json.dumps(patch, indent=2) + '\n', encoding='utf-8', newline='\n')
print(path)
