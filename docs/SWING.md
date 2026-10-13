# Swing — 2.9.0 public alpha

Select **TRK 1** or **TRK 2**, then click **SWING**. Each track has its own
setting from **50%** (straight time) to **78%**. Drag the button, encoder, or
display up/down to change it. Right-click for numeric entry, reset, or shortcuts.
Hold the encoder for more than one second to reset the selected track to 50%.
These edits support Rack undo/redo and live adjustment readouts.

On the Swing screen, the eight stage buttons choose **50, 54, 58, 62, 66, 70,
75, or 78%**. This desktop shortcut replaces the hardware's held-button chord;
the screen stays open until another setting or EXIT is selected. The selected
shortcut lights up. EDIT still enters ordinary stage editing.

The OLED follows the manual's Swing page: selected track, SWING label, large
percentage on the right, encoder indicator, and current stage-function footer.
When a CTRL knob owns Swing, its numbered indicator replaces the encoder icon.
MOD lanes have no Swing setting of their own.

## Knobs and modulation

Assign **Swing** to CTRL 1 or CTRL 2 and choose Track 1, Track 2, or both.
CTRL changes the panel value directly: fully left = 50%, noon = 64%, fully
right = 78%. Direct Swing editing is locked while that CTRL owns it. Clearing
the assignment leaves the last panel value in place.

Assign **Swing** to AUX X/Y/Z or a MOD lane to offset the panel value instead.
0 V preserves the programmed value; +5 V adds 28 percentage points and -5 V
subtracts 28, with the final amount limited to 50–78%. AUX attenuverters scale
or invert the offset. Contributions from AUX and MOD sum before saturation.
They never change the saved base setting. A MOD lane assigned to Swing remains
on its straight clock while it swings its destination note track(s).

## Timing and persistence

Swing operates after each track's Clock Div. For a straight track interval P,
66% produces alternating intervals of 1.32P and 0.68P; 78% gives 1.56P and
0.44P. The first note stays on the downbeat, the second is delayed, and the
pair retains its original total length. CLK output, MOD clocks, Queue/Total
Pulses, and chain boundaries continue to count the straight global clock.

Gate-length, ratchet subdivision, and Tempo slide use each long/short track
interval. Swing parity continues through skips, stage changes, pulse counts,
and automatic fixed track-length wraps. Explicit reset, normal transport
restart, preset recall, chain phrase reset, or a clock-source change starts a
new pair. Chain Run/Pause freezes a pending delay until playback resumes.

Changes are sampled at the first pulse of each pair, so moving a knob or
ramping CV cannot retract a pulse already scheduled. External timing uses the
measured input interval (the tempo setting supplies the initial estimate).
A stalled clock can finish its one pending delayed pulse but creates no
future pulses. If an abrupt input tempo increase overtakes that pulse, it is
processed before the newly arrived track pulse to preserve traversal order.

Rack patches, internal presets, and chain references store both Swing values.
Old patches and old preset banks migrate to 50%. Existing parameter, port,
destination, and page IDs retain their meanings. Patch schema is 12; compact
preset format 2 appends two Swing values to format 1's 422 values.

The [Swing performance patch](../examples/Swing-performance.vcv) uses VCV Free
and Core. Select your AUDIO device and start playback; it alternates straight
and swung phrases, with independent bass/melody amounts and a small MOD offset.
It starts stopped so you can choose your output level before listening.

## Reference and comparison limits

Reference: [Metropolix firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf),
pp.79, 147, 153, 163, 168, and 170. The page-79 overview says even-numbered
pulses are delayed, while its bullet says odd-numbered pulses. This version
keeps the first pulse straight and delays the second. The manual names the MPC
shortcuts and matches Tête; the exact eight values are confirmed by the
[official Tête manual](https://intellijel.com/downloads/manuals/tete_manual_v1.2_2021.07.01.pdf).

Pair latching, parity at automatic track wraps, gate/slide treatment, and
extreme external tempo changes are documented software interpretations.
Sample-level timing and Rack output tests cover them, but hardware-owner
comparison is still needed before claiming exact firmware equivalence.
