"""Compose Night Current: a deterministic two-voice patch for Metropolix Lab 2.4.1.

Run from any directory with Python 3. Only Rack Core and VCV Free are required.
Fundamental parameter/port mappings match its official v2.6.4 sources.
"""
import json
import math
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--variations', action='store_true', help='Write the evolving performance version')
VARIATIONS = parser.parse_args().variations
VERSION = json.loads((ROOT / 'plugin.json').read_text())['version']
BPM = 108
modules, cables = [], []


def module(id, model, pos, params=None, *, plugin='Fundamental', data=None):
    entry = dict(id=id, plugin=plugin, model=model,
                 version=VERSION if plugin == 'MetropolixLab' else
                 '2.6.6' if plugin == 'Core' else '2.6.4', pos=pos,
                 params=[dict(id=k, value=v) for k, v in sorted((params or {}).items())])
    if data is not None:
        entry['data'] = data
    modules.append(entry)


def cable(source, output, target, input, color='#398d93'):
    cables.append(dict(id=len(cables) + 1, outputModuleId=source, outputId=output,
                       inputModuleId=target, inputId=input, color=color))


def envelope(attack_ms, decay_ms, sustain, release_ms):
    return {0: math.log10(attack_ms) / 4, 1: math.log10(decay_ms) / 4,
            2: sustain, 3: math.log10(release_ms) / 4}


def cutoff(hz):
    return (math.log2(hz / 261.625565) + 5) / 10


# Shared pitch sliders, in semitones above A. A two-stage bass loop uses A/E;
# the full melody rises to A, descends via G/E/C, then leaves room after D.
notes = [0, 7, 12, 10, 7, 3, 5, 0]
pulses = [3, 1, 2, 2, 3, 1, 2, 2]  # one 16-tick bar for the melody
gates = [2, 1, 1, 1, 1, 1, 1, 0]  # repeated opening note; silent last stage
params = {0: BPM, 1: 1, 3: 9, 4: 6, 5: 2, 402: 2, 403: 3}
for stage in range(8):
    params.update({6 + stage: notes[stage] / 24, 14 + stage: pulses[stage],
                   22 + stage: gates[stage]})
for track, length, division, transpose, gate in [(0, 2, 4, -24, 52), (1, 8, 1, -12, 62)]:
    base = 30 + track * 57
    params.update({base: length, base + 1: 0, base + 2: division,
                   base + 3: 0, base + 4: transpose, base + 5: gate})
    for stage in range(8):
        for row, value in enumerate([1, 100, 0, 0, 0, 0]):
            params[base + 9 + row * 8 + stage] = value

# Eight-bar timbre cycle. All pitch and gate events remain deterministic.
# A gentle one-bar ramp per MOD stage opens the lead filter by up to 1.25 octaves.
for lane, values in enumerate([[0, .25, .65, 1.1, 1.25, .8, .35, 0], [0] * 8]):
    base = 194 + lane * 26
    params.update({base: 8, base + 2: 16, base + 6: 1, base + 7: 0})
    for stage, value in enumerate(values):
        params[base + 10 + stage] = value
        params[base + 18 + stage] = 1

if VARIATIONS:
    # The lead traverses forward and backward at eighth-note speed. Its 54-tick
    # pendulum cycle crosses the bass's 16-tick bar, continually moving pickups.
    params[87 + 2] = 2
    params[87 + 3] = 2
    params[4] = 2  # Natural minor: keep all harmonic and melodic shifts in key.
    # Eight independent lanes make phrases, not just a repeated one-bar loop.
    # The Am / F / C / G root movement changes every two bars. A three-bar
    # melody-shaping lane moves passing notes within the same diatonic scale.
    # Over 32 bars: introduce, lift register, return, bass break, rebuild, settle.
    lanes = [
        # lane, division, destination, CV type, assignment, eight voltages
        (1, 32, 2, 0, 2, [0, -4/12, 3/12, -2/12, 0, -4/12, 3/12, -2/12]),
        (2, 6, 2, 0, 1, [0, 0, 2/12, 0, -2/12, 0, 3/12, -2/12]),
        (3, 64, 1, 1, 1, [0, 0, 1, 0, 0, 1, 0, 0]),
        (4, 4, 7, 1, 1, [0, 0, 0, 0, 0, 5/7, 0, 5/7]),
        (5, 16, 5, 0, 1, [0, -1, -.6, .3, 0, -.8, .6, -1.4]),
        (6, 8, 8, 2, 1, [0, 0, 0, 5, 0, 0, 5, 0]),
        (7, 64, 8, 2, 0, [0, 0, 0, 0, 5, 0, 0, 0]),
    ]
    for lane, division, destination, cv_type, assignment, values in lanes:
        base = 194 + lane * 26
        params.update({base: 8, base + 2: division, base + 6: cv_type,
                       base + 7: destination, base + 8: assignment})
        for stage, value in enumerate(values):
            params[base + 10 + stage] = value
            params[base + 18 + stage] = 0
    # A brief slide into the descending E gives that one gesture a softer edge.
    params[87 + 6] = 35
    params[87 + 9 + 3 * 8 + 4] = 1
module(1, 'Metropolix', [0, 0], params, plugin='MetropolixLab',
       data=dict(schemaVersion=7, expanded=False, modEditor=False, selectedTrack=1,
                 uiPage=0, uiHome=0, uiField=0, uiStageDetail=False, uiMod=False, uiEdit=False))
module(2, 'Scope', [35, 0])

# Bass: A1 / E2, filtered square, short attack and a little sustain.
module(3, 'VCO', [0, 1], {2: -12})
module(4, 'ADSR', [9, 1], envelope(4, 155, .22, 85))
module(10, 'VCF', [18, 1], {0: cutoff(185), 2: .2, 3: .22, 4: 0})
module(5, 'VCA-1', [25, 1], {0: .36})

# Lead: A3-E4-A4-G4-E4-C4-D4, plucked triangle with a slowly moving filter.
module(6, 'VCO', [31, 1], {2: 0})
module(7, 'ADSR', [40, 1], envelope(3, 170, .06, 115))
module(11, 'VCF', [49, 1], {0: cutoff(1150), 2: .17, 3: 1, 4: 0})
module(8, 'VCA-1', [56, 1], {0: .38})

# Clock input is one pulse per sixteenth. A 1.5 / 2.5 second knob setting
# becomes three / five clock ticks at any BPM: dotted eighth / five sixteenths.
for id, x, time, feedback in [(12, 61, 1.5, .3), (13, 71, 2.5, .27)]:
    module(id, 'Delay', [x, 1], {0: math.log10(time * 1000) / 4,
                               1: feedback, 2: .36, 3: 1})
    cable(8, 0, id, 4)
    cable(1, 4, id, 5, '#8a72aa')

# Bass stays centered; the lead's quiet, different echoes create stereo width.
for id, x, dry, wet in [(15, 50, .80, .46), (16, 60, .78, .55)]:
    module(id, 'VCMixer', [x, 0], {0: .65, 1: 1, 2: dry, 3: wet, 4: 0},
           data=dict(chExp=False, mixExp=False))
    cable(5, 0, id, 1, '#d28c49')
    cable(8, 0, id, 2)
    cable(12 if id == 15 else 13, 1, id, 3, '#829553')
module(9, 'AudioInterface2', [70, 0], {0: 1}, plugin='Core')
cable(15, 0, 9, 0)
cable(16, 0, 9, 1)
cable(15, 0, 2, 0)
cable(16, 0, 2, 1)

for pitch, gate, osc, env, filt, vca in [(0, 1, 3, 4, 10, 5), (2, 3, 6, 7, 11, 8)]:
    color = '#d28c49' if osc == 3 else '#398d93'
    cable(1, pitch, osc, 0, color)
    cable(1, gate, env, 4, color)
    cable(osc, 3 if osc == 3 else 1, filt, 3, color)
    cable(filt, 0, vca, 1, color)
    cable(env, 0, vca, 0, color)
cable(4, 0, 10, 0, '#d28c49')
cable(1, 5, 11, 0, '#8a72aa')

patch = dict(version='2.6.6', zoom=1.1, gridOffset=[-1, -.05],
             modules=modules, cables=cables)
path = ROOT / ('examples/Musical-variations.vcv' if VARIATIONS else 'examples/Musical-demo.vcv')
path.write_text(json.dumps(patch, indent=2) + '\n')
print(f'Wrote {path.name}: Night Current{" / Variations" if VARIATIONS else ""}, {BPM} BPM, A minor{"" if VARIATIONS else " pentatonic"}')
