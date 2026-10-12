# CTRL 1 and CTRL 2 — 2.6.0

The two large white knobs are assignable performance controls. Their black
buttons open the manual-style CTRL page: destination above, target tracks below,
with a live knob gauge. Click the encoder or display to switch fields, then
drag up/down. Right-click either knob or button for named destinations and
Track 1 / Track 2 / Both assignments. The hardware reference is manual pp.146–153.

Knob dragging supports fine adjustment with Ctrl, numeric entry, and Rack undo.
Numeric position is 0% fully left, 50% at noon, 100% fully right. The context
menu includes **Centre knob (noon)** and **Clear assignment**. Double-click
resets that knob's position to its new-module default. Assignments are saved
with Rack patches and module presets.

New modules start with CTRL 1 controlling **Gate Length**, both tracks, at 50%;
CTRL 2 controls **Slide Amount**, both tracks, at 25%. Slide is audible only
on stages with slide enabled. Patches from 2.5.0 and earlier load with both
knobs unassigned so that their saved settings and sound remain intact.

## Direct settings and offsets

**Direct destinations** change the actual panel setting. For example, assigning
CTRL 1 to Gate Length makes that knob own the selected tracks' gate-length
parameters. The display marks the active field with a circled controller number;
the encoder and optional editor cannot change it independently. Right-click
the field to change or clear the controller assignment. Accumulator Mode and
Polarity also show ownership on LIM. Unassigning retains the last panel value.

**Offset destinations** preserve programmed settings. Noon gives no offset;
turning left subtracts and right adds. Returning to noon or clearing the
assignment restores the programmed value. AUX and MOD continue to operate
alongside CTRL. The HOME panel view now shows the actual knob positions.

| Destination | Operation in this version |
| --- | --- |
| Accum Mode | Direct: left/noon Stage; right Track; applied on the next track pulse |
| Accum Polar | Direct: left Unipolar with inverted increments; exact noon Bipolar; right normal Unipolar |
| Tempo | Direct: 0.1–400 BPM, global; external clock still takes precedence |
| Clock Div | Direct: /1–/64 per track |
| Gate Length | Direct: existing 1–99% range |
| Play Order | Direct: the 21 supported play orders |
| Pulse Div | Direct: /1–/8 pulse-count division |
| Pulses Len | Direct: Auto at minimum, then 1–64 pulses |
| Slide Amount | Direct: 0–100% of one track pulse |
| Stages Len / Stages Offset | Direct: 1–8 stages / offset 0–7 |
| Octave | Offset: -2 to +2 octaves, whole octaves |
| Pitch Pre | Offset: -12 to +12 semitones before scale quantization |
| Pitch Post | Offset: -12 to +12 semitones, chromatic, after quantization |
| Pitch Offset | Offset: -1 to +1 V, continuous |
| Probability | Offset: -100 to +100 percentage points across all stages |
| Pulse Count | Offset: -7 to +7 relative to shared switches; effective count clamped to 1–8 per track |
| Ratchet | Offset: -7 to +7; effective count clamped to 1–8 |
| Root | Offset: -12 to +12 semitones per track |
| Direction | Left Reverse; noon programmed direction; right Forward |
| Skip Invert | Right inverts all skips for the assigned tracks; left/noon normal |
| Slider Invert | Right reverses pitch-slider interpretation; left/noon normal |
| Track Out Swap | Right swaps physical Pitch/Gate 1 and 2 plus track-CV assignments at A/B; left/noon normal. MOD sources at A/B remain unchanged. Global. |

There are 23 destinations plus None. **Accum Dir, Gate Scale, Scale (User),
Slider Range, Swing, and Velocity** are listed as not implemented in the menu.
In particular, reversing an accumulated sequence requires more than changing
the sign of its increments, so Accum Dir is not presented as available.

## Try it

Open `examples/CTRL-performance.vcv` and select your device in AUDIO. This uses
the existing evolving Night Current arrangement with CTRL 1 = Pulse Count and
CTRL 2 = Ratchet, both on Track 2 at noon. Try CTRL 1 near 57% or 64% to add one
or two pulses per stage, then return to 50%. Turn CTRL 2 up for denser fills.
Use the centre menu for exact noon. The bass remains unchanged. Right-click
and try Stages Len, Pitch Pre, or Skip Invert next; reset between comparisons.

## Interpretations awaiting hardware comparison

The manual distinguishes direct settings from offsets but does not specify
every knob gain or conflict rule. Pitch-offset gains, octave range, switch
thresholds, and direct stepped-value spacing above are implementation choices.
Two offsets to the same destination add, then clamp to one full knob range.
CTRL 2 wins conflicting direct or switch assignments on overlapping tracks.
Direction uses the existing forward/reversed play-order traversal.

Changing Skip Invert restarts the assigned track traversal at the next base
clock, at the first playable stage in its order. It preserves accumulator
banks and does not restart the other track or MOD lanes. The manual says that
skip inversion resets the sequence but does not fully define that reset scope.
An order whose visited stages are all skipped produces silence. HOME reflects
the effective skip mask; the SKIP editor retains the programmed mask.

Gate Length retains the engine's existing 1–99% range; the hardware's wider
endpoint behavior is unfinished. No physical module comparison has been made.
