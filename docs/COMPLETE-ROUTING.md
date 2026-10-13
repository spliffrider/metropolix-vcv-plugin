# Complete AUX/MOD routing — 2.19.0

All **38 destinations plus None** listed on manual pp.154 and 157–163 are
available on AUX X/Y/Z and all eight MOD lanes. The original destination IDs
are unchanged, so patches and internal preset format 7 retain their selections.
The hardware panel still uses its black assignment buttons and encoder;
right-click menus, held vertical dragging, hover help, numeric entry and undo
provide desktop editing.

This release adds these twenty destinations:

| Destination | Operation |
| --- | --- |
| BPM (INT) | Offsets the internal master tempo, limited to 0.1–400 BPM. External clocks take precedence. The BPM screen shows the effective tempo beside its programmed value. |
| Clock In Div | Offsets track clock division, /1–/64. |
| Direction | A high gate reverses the effective playback direction; low restores it. |
| Gate Scale | Offsets gate scaling between 0–100%. |
| Loopy | A high gate starts Loopy at Stage 1 using the LOOPY target and pulse settings; low releases it. |
| Loopy Pulses / Offset | Change the loop pulse count and starting stage without rewriting the LOOPY settings. |
| Play Order | Moves through the 21 orders, saturating at the ends of the list. |
| Pulse Count | Adds/subtracts pulses after the track's Switch/uniform source, limited to 1–8. |
| Pulse Div | Offsets pulse-count division, /1–/8. |
| Pulses Len | Offsets fixed track pulse length, up to 64. An Auto base can return to Auto; a fixed base bottoms out at one. |
| Reset | A rising trigger resets the assigned track's sequence and accumulator, while the other track, MOD lanes, master clock and chain continue. |
| Root | Transposes the scale root at 1 V/octave, limited to one octave either way. |
| Skip Invert | High complements skips and restarts the affected traversal at the next master pulse, retaining accumulated offsets. |
| Slide | High enables slide on all stages; low restores the individual marks. Works with Tempo, Analog and Acid. |
| Slide Amount | Offsets slide amount between 0–100%. |
| Slider Invert / Range | Inverts programmed slider orientation with a gate, or offsets its 1–4 octave range with CV. |
| Stages Len / Offset | Offset the 1–8 stage window and its 0–7 starting offset. |

The other eighteen destinations remain available; see [Accumulator routing](ACCUMULATOR-ROUTING.md),
[AUX](AUX-INPUTS.md), [Scales](SCALES.md), [Swing](SWING.md) and
[Output routing](OUTPUT-ROUTING.md). Gate Length CV now reaches **100% Hold**
as specified in the manual, as well as its zero-length Rest endpoint. Strong
positive Gate Length modulation can therefore sound more connected than in
older builds, which capped it at 99% unless the base was already Hold.

Gate/trigger destinations use the established 1.5 V threshold and AUX logical
inversion. MOD stage readouts and numeric entry now use **On/Off** for gates;
continuous destinations retain volts. BPM, the three Loopy destinations and
Track Out Swap are global. Their assignment readout says Global and retains
the inactive track selection for reuse with a track destination. Global sources
are counted once, including a source previously assigned to Both.

Routing offsets the effective values. Returning CV to zero, lowering a gate or
removing a route restores the programmed setting. Both source types use the
same engine operations; a route never rewrites the shared sliders/switches.
Targeted Reset acts on its rising edge, holding pitch until the next played
note; it cancels that track's pending swing pulse. Accum Reset instead waits
for the next track pulse and clears only accumulated pitches.

Gate-driven Loopy leaves the normal sequence advancing underneath. Manual
held/latched Loopy takes priority when both are requested. Offset shifts a
manual range without losing its width, saturating at panel edges. Loopy pulse
CV changes Auto to a finite count while nonzero; removing it restores Auto.
These collision and edge rules need hardware-owner comparison.

## CV gain choices

The manual specifies full-scale ±5 V, bounds and direction, without every
intermediate voltage threshold. This implementation uses rounded integer
offsets and these documented gains:

| Quantity | Gain per volt |
| --- | --- |
| BPM | 80 BPM |
| Clock division | 12.6 divisions |
| Gate Scale / Slide Amount | 20 percentage points |
| Loopy Pulses / Pulses Len | 12.8 pulses |
| Loopy Offset / Pulse Count / Pulse Div / Stages Len / Stages Offset | 1.4 steps |
| Play Order | 4 order positions |
| Slider Range | 0.6 octaves |
| Root | 12 semitones, capped at ±12 |

Existing CTRL offsets combine with AUX/MOD where applicable. Direction and
Slider Invert gates toggle the direction obtained from the programmed setting
and CTRL; Skip Invert combines with CTRL as a positive request. Exact gains,
live divider/length changes and conflicting performance gestures still need
physical-module feedback. This is a manual-based implementation, not a
hardware-verified firmware port.

## Demo and checks

[Complete-routing-performance.vcv](../examples/Complete-routing-performance.vcv)
uses twenty new routes across four chained scenes, with a velocity-shaped bass
and independent percussion. It opens stopped; choose AUDIO and press RUN.
Try the previous simpler demos when isolating an individual destination.

The portable suite compares offsets and gates with independently programmed
equivalents across all 21 orders, including all three slide types, and checks
tempo at 44.1/48/96 kHz, targeted reset and Loopy. Native Rack tests cover real
clock/pitch ports, live BPM, gate entry units, assignment scope and saving.
Callback tests exercise destination reachability, global fields, undo and
native save/reopen. The demo recording verifies finite signals and scene
transitions; it is not a listening judgment.
