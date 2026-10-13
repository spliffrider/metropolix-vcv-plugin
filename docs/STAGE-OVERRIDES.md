# Stage pitch and gate overrides — 2.10.0

Both note tracks have their own eight pitch overrides and eight gate-length
overrides. The shared panel sliders and switches still define the underlying
pattern. These features follow the firmware 1.6 manual, pp.82–84 and 89–92.

## Pitch

Select **TRK 1** or **TRK 2**, press **EDIT**, then the printed **PITCH** stage
button. Select a stage and drag the encoder, OLED, or held stage button up/down.
The note is quantized to the current scale, from C0 to C5. Fully down, **--**
restores the shared pitch slider. Assigned stages light yellow; the selected
stage flashes. The OLED shows the note, stage arrow, override bars and position.

- Click the encoder to **Sync** this override to its corresponding slider.
- Latch **ALT**, then click the encoder to sync all eight sliders individually.
- With ALT latched, dragging changes all eight overrides in scale-degree steps.
- Long-press the encoder to remove this override; with ALT, remove all eight.
- Right-click the encoder/display/stage for numeric entry, Sync or Remove.
  Note entry accepts `E2`, `C#3`, etc.; `--` returns to the slider.
- Every drag, Sync and Remove operation has one Rack undo/redo action, including
  operations on all eight stages. Ctrl-drag retains Rack fine adjustment.

The override replaces the slider's base pitch. Root, scale, track transposition,
pitch modulation, accumulation and slide remain active. Moving the shared
slider or changing Slider Range does not change an assigned override. Sync
includes slider inversion and range, but excludes live modulation/accumulation.

Overrides store a chromatic note anchor and are requantized in the current
scale on playback. Changing scale can therefore change the sounding note.
The displayed note excludes root/track transposition, as a base pitch setting;
the HOME note view includes those settings. This interpretation still needs a
hardware owner's comparison, particularly when changing between scale sizes.

## Stage gate

Select a track, press **EDIT**, then the printed stage **GATE** button. Select
a stage and edit its override:

| Value | Result |
| --- | --- |
| **--** | Follow the track's gate length |
| **Rest** | No gate from this stage |
| **1–99%** | Fraction of each pulse/ratchet |
| **100%** | Hold throughout the stage, with no forced low before the next stage |

Assigned stages light green; the selected stage flashes. The OLED shows the
value and all eight override bars, including separate Rest and unused markers.
ALT edits all stages together. Long encoder press removes one override; ALT
plus long press removes all. Right-click offers direct entry and removal;
entry accepts `Rest`, `hold`, `--`, or a percentage.

Gate overrides affect only gate articulation. Even **Rest** continues the
underlying accumulator triggers, including ratchet accumulation, and updates
pitch. A skipped stage still consumes no pulses and performs no accumulation.
The physical Rest switch, probability, and mute retain their existing rules.
Physical Hold remains a held gate unless the override is Rest; percentages
adjust pulse lengths for Single/Multi modes. These combinations are tested
against the documented rules, but have not been checked on actual hardware.

## Track gate and scaling

Press the separate **GATE** button on the lower left. Click the encoder to
switch between **Gate Length** and **Gate Scale**. Gate Length has a **Hold**
endpoint at 100. Gate Scale ranges from 0–100%, default 100%, and multiplies
both inherited and overridden lengths. For example an 88% stage override
with Gate Scale 50% produces a 44% gate. Scaling a length Hold below 100%
returns it to pulse/ratchet gating. The physical Hold switch stays held.
At Gate Scale 0%, gate output is silent.

Long encoder press resets both track fields to Length 50%, Scale 100%, in
one undo action. A CTRL-owned length remains controlled by that knob; edit
Gate Scale separately. Existing CTRL Gate Length and AUX/MOD Gate Length
ranges are unchanged. AUX/MOD Gate Length affects the inherited track length;
assigned stage lengths remain their own values.
Hold track GATE and press stage buttons 1–8 for 1,15,30,45,60,75,90,100%.
The last shortcut is Hold for Length. Right-click offers the same shortcuts.

## Patches, presets and testing

Rack saves all 32 overrides and both Gate Scale values in the patch and its
64 internal presets. Chains recall them before the first output of a new link.
Overrides recall even if shared slider/switch recall is Off. The global
slider range and recall preferences retain their existing behavior.

Older patches and format 1/2 preset banks migrate to no overrides and
Gate Scale 100%, preserving their sound. Parameter IDs and cable endpoints
from earlier versions remain unchanged. Keep original patches if testing
older plugin versions: older builds cannot retain the new preset-bank format.

`examples/Stage-overrides-performance.vcv` contrasts two melodies over shared
sliders and changes stage articulation across preset-chain sections. It starts
stopped; choose an AUDIO device, then RUN. Test the yellow/green edit pages,
Sync, ALT edits, Rest with ACCUM, and undo while the patch plays. Hardware
owners' comparisons are particularly useful for scale changes and Hold/Scale.
Disable the chain before editing individual presets; otherwise the next
section recalls its saved values. Hold SETUP to open the chain page.
