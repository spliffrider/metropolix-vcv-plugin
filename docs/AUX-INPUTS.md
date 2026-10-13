# AUX X/Y/Z — 2.4.0

The three jacks below A, B, and CLK are now CV/gate inputs. Each has its own
attenuverter knob and assignment button, at the positions in the hardware photo.
The display follows the AUX settings page in manual pp.154–156.

## Assign and play

1. Patch a CV source into X, Y, or Z. The first polyphonic channel is used.
2. Right-click the small black button below its knob. Choose **Destination**
   and **Target tracks** (Track 1, Track 2, or Both tracks).
3. Drag its knob upward from noon to increase the amount, or downward to invert
   it. Noon contributes zero. Right-click the knob for percentage entry/reset.
4. Click the assignment button to view the destination and live meter. Drag the
   encoder or display to edit; click either to switch between destination and
   track assignment. The held assignment button also edits the destination.
5. Choose **Clear assignment** to stop that input affecting playback. Changes
   support Rack undo/redo, automation, and patch/module-preset saving.

Available destinations are **Octave, Pitch Pre, Pitch Post, Pitch Offset,
Gate Length, Probability, Ratchets, and Mute**. They use the same destination
semantics as the internal MOD lanes. They offset programmed values; they do not
rewrite them. Removing a route restores the programmed setting (probability
takes effect at its next decision; rest/rejected stages retain pitch as before).

## CV and gate behavior

As of 2.9.0, **Swing** is a ninth destination: ±5 V adds/subtracts up to
28 percentage points from the programmed amount, limited to 50–78%.
The AUX/MOD sum is sampled at the beginning of each track's pulse pair.
See the [Swing guide](SWING.md) for timing and CTRL interaction.

Continuous CV is multiplied by the knob's -100% to +100% amount, then limited
to -5..+5 V. For example, +10 V at +25% produces +2.5 V; at -25%, -2.5 V.
Unpatched continuous inputs contribute zero.

**Mute is a gate assignment.** An incoming voltage of at least 1.5 V is high.
Clockwise from noon follows the gate; counterclockwise logically inverts it.
The amount's magnitude does not scale this gate. With no cable, clockwise is
a manual high gate and counterclockwise is low. At exact noon the gate is low.
A high gate reverses the track's programmed mute state; it can therefore unmute
a track whose base Mute setting is on.

AUX and MOD voltages targeting the same destination/track are summed before
the combined -5..+5 V limit. For example, +4 V +4 V from MOD and -5 V from AUX
produce +3 V. This combination rule, exact-noon gate behavior, and unpatched
continuous-input normalization are implementation choices where the manual
does not give a precise specification. The established Gate Length/Probability
and Ratchet gains are also inferred; see [validation](VALIDATION.md).

External voltage is read every audio sample. Knob/routing parameters and the
display meters refresh every 32 samples. Meters remain live while stopped;
stopping silences gates/CV outputs and holds pitch as in the existing engine.

## Ready-patched example

Open `examples/Aux-inputs-demo.vcv` and select an output device in AUDIO. It uses
the existing two voices and internal MOD lanes, with three additional cables:

| Cable | Assignment | Amount | Effect |
| --- | --- | --- | --- |
| A → X | Pitch Post, Track 1 | +25% | -2..+2 V becomes -6..+6 chromatic semitones |
| B → Y | Pitch Post, Track 2 | -10% | 0..+5 V ramp becomes 0..-6 chromatic semitones |
| Track 1 GATE → Z | Mute, Track 2 | -100% | Track 2 is muted while Track 1's gate is low |

Turn X/Y to noon to hear the pitch modulation disappear. Change Z's destination
to None to hear Track 2 without that gate mask. Restore each edit with undo.
The public demo has no selected audio device and uses only VCV Free/Core.

## Scope and compatibility

2.4.0 appends nine parameters and three input IDs. The previous 404 parameter
IDs and nine working ports retain their identities. Loading an older patch or
module preset clears the new AUX settings, including when loaded over a module
that already had assignments. New patches should be opened with 2.4.0 or later.

This implements the eight destinations above. Additional hardware destinations,
ALT+AUX selective randomization, hardware long-press gestures, CTRL assignments,
Loopy, and swing remain unfinished. There has been no physical-hardware check.

Scale (User), added in 2.12.0, uses whole-volt offsets inside the locked user
scale bank and can target either or both tracks. See [Scales](SCALES.md).

Since 2.16.0, Velocity adds −5…+5 V and Track Out Swap is a global gate
destination with the same gate inversion/threshold handling as Mute. See
[Output routing](OUTPUT-ROUTING.md) for all track-based A/B signals.

Version 2.18.0 adds six [accumulator routes](ACCUMULATOR-ROUTING.md), bringing the
shared AUX/MOD count to 18. All six use gate logic; Accum Reset uses its rising
edge. An assigned low Accumulate gate pauses rather than removing the route.

Version 2.19.0 completes all 38 manual destinations; see [Complete routing](COMPLETE-ROUTING.md)
for the added voltage/gate routes, Global scope, Loopy and targeted reset.
