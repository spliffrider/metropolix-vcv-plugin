# Modulation playtest — 2.54.0

Open `examples/Hardware-panel-demo.vcv` in Rack. It is already running and patched. Right-click **MOD**, then choose **Open MOD editor** to expose the grid used in these checks. The older `Modulation-demo.vcv` uses the same musical settings. Use **File > Save As** to keep your own test copy. Choose your output device in the AUDIO module to hear the two voices. The patch's two VCAs are set to 32%.

Metropolix Lab produces control voltages, not audio. Its pitch and gate outputs drive the oscillators, envelopes, and VCAs below it. The first Scope monitors OUT A and OUT B; the second monitors PITCH 1 and GATE 1. Signals should appear even without an audio device selected.

## Select lanes on ORDER, LEN and DIV

With MOD selected, open ORDER, LEN or DIV. Click a stage button 1–8 without
ALT or another held shortcut button to select MOD lane 1–8. The settings
screen and its active field stay open; the lit button shows the selected lane.
The encoder or OLED then edits that lane's setting. Lane selection leaves
stage voltages, gate toggles and Undo untouched, including during mouse movement.

Switching to a different lane cancels the previous lane's pending setting.
Selecting the same lane retains it. Hover a button for lane-specific help;
right-click selects the lane and offers **Lane settings** and **Edit stage
values**. Those menu links retain the clicked lane if selection later changes.
EDIT lane selection, ALT functions and held timing/order shortcuts remain
available. This follows the supplied firmware 1.6 manual, printed p145.

## Destination-controlled CV type

The firmware 1.6 manual (printed pp.138–140 and 192) says an internal MOD
destination determines the lane's type. A continuous destination uses
Bipolar ± values and Step/Ramp transitions; a gate destination uses Gate
toggles. This now applies consistently to editing, playback, graphs, A/B
outputs and MIDI CC scaling. A saved Unipolar or Gate choice no longer clips
or quantizes an internal continuous modulation pattern.

Press MOD, select the lane, then ALT + TYPE to open its OLED CV TYPE page.
For an internal assignment it shows **TYPE FROM DEST** and its destination.
The encoder, OLED, grid and type menus preserve the saved choice while this
type is fixed. Clear the internal destination to edit the lane's type again;
its saved choice returns. A/B-only and unassigned lanes retain all three modes.

Raw stage voltages and transitions stay stored. Switching destinations or
types does not overwrite them or alter preset/patch formats. If assignment
changes during an unconfirmed type preview, its obsolete confirmation is
discarded. Confirmed voltage edits and existing raw Undo remain supported.

The optional MOD grid's assigned type cell also has a read-only right-click
menu. It explains the type/destination and opens the clicked lane's settings.
Clearing its internal destination there restores the saved choice, without
changing another lane or Track 1's accumulation mode. Lane settings remain
bound to the clicked lane if selection changes while the menu is open.

## Stored voltages, ALT and Undo

For an unassigned or output-only lane, a Unipolar or Gate setting can hide a negative voltage previously entered in
Bipolar mode. Editing that stage now records the exact stored origin, so Undo
restores it even when its displayed value was zero. This applies to the main
encoder, OLED and optional MOD grid, including very small drags.

ALT continuous voltage edits shift all eight effective voltages by the same
amount, clamping each at its allowed limits. A negative stored value hidden
by a positive-only type starts that shift at zero; it no longer makes the
other stages jump. Undo restores all eight original stored values together.

The existing three-pixel click threshold stays in place. A smaller drag can
edit the value and then perform its genuine Gate/transition click; Undo
reverses the click before the edit. Zero-distance movement makes no voltage
write. With Enc. Confirm enabled, a voltage preview applies once without also
changing a transition. ALT Gate rotation/inversion keeps its existing workflow.

## Expected starting behavior

| Lane | Starting pattern | Destination |
| --- | --- | --- |
| MOD 1 | -2, -1, 0, 1, 2, 1, 0, -1 V; stepped; division 4 | OUT A |
| MOD 2 | Alternating 0 and 5 V; ramps; division 4 | OUT B |
| MOD 3 | 0, 0, 1, 1, 0, 0, -1, -1 V; division 8 | Octave, track 1 |
| MOD 4 | Occasional positive steps; division 4 | Ratchets, track 2 |
| MOD 5 | 0, -1, 0, 1, 0, -1, 0, 1 V; division 2 | Gate Length, both tracks |
| MOD 6–8 | Zero volts | Unassigned |

The active stage moves through the selected lane's graph. Its live voltage is shown at the top of the MOD editor. The first Scope shows five seconds, enough for a full MOD cycle; its second trace is centered with a -2.5 V display offset. Scope time/scale controls can be adjusted to inspect a longer pattern or a single transition.

## Suggested checks

1. **Transport and sound.** Listen for two separate voices. RUN should stop both voices and lower routed MOD voltages to zero (global CTRL/Root and static Velocity outputs can remain active); pitch outputs hold. RUN starts a fresh traversal. RESET restarts all note and MOD sequences.
2. **Edit a voltage.** Select MOD 1. Drag a stage value vertically, or right-click for numeric entry. Watch the change on A when that stage plays. Rack Undo should restore the edit. Changes to MOD 1 should not rewrite MOD 2's values.
3. **Steps and ramps.** Select MOD 2. Change a RAMP cell to STEP. That stage should jump directly to its target instead of taking the stage duration to get there. Change it back and vary DIV: a larger division makes the stage and ramp longer.
4. **Independent timing.** Change MOD 1's LENGTH, OFFSET, or ORDER. Its traversal should change while MOD 2 keeps its own settings. Right-click a MOD selector to try reverse direction or a fixed pulse length. RESET makes comparisons easier.
5. **Internal pitch modulation.** Select MOD 3 and compare muted/unmuted playback. Track 1 should lose/regain the octave pattern, while its saved transpose and stage pitches remain unchanged. Track 2 should keep its original pitch pattern.
6. **Rhythm modulation.** Mute/unmute MOD 4 to compare track 2's extra ratchets. Mute/unmute MOD 5 to compare the gate lengths on both tracks. Note probability already present in the musical patch means successive loops can differ.
7. **Output assignment.** Set OUTPUT A SOURCE to MOD 3. A should now show that lane's 0/+1/-1 V pattern, while MOD 3 continues to change track 1's octave internally. Return A to MOD 1. The two output selectors are independent.
8. **Gate mode.** Use an unused lane, choose Gate as its CV type, and click stage values to toggle 0/5 V. Route B to that lane. It should emit low/high steps; gates do not ramp. Return B to MOD 2 afterward.
9. **Save and reopen.** Save your test copy, close it, and reopen it. Check selected MOD lane, editor visibility, stage values, timing, destinations, and A/B assignments. Playback restarts rather than resuming at the exact saved clock position.
10. **Earlier controls.** Right-click EDIT to open the stage editor and try pitch sliders, pulse counts, gate types, ratchets, probability, accumulation, skip, and slide on each track. Reopen the MOD editor through MOD's right-click menu; it should retain its settings. Try compact/expanded views with neighboring modules and ordinary mouse dragging. The new panel workflow is detailed in `HARDWARE-UI.md`.

If something behaves unexpectedly, save the patch and note the selected track/lane, control changed, expected result, and actual result. That gives us a reproducible case without needing the physical module.

## Already checked automatically

Engine tests, Rack parameter save/load, editor events and undo, and a 12-second Rack signal recording pass. Through connected Rack cables, A measured **-2 to +2 V**, B measured **0 to +5 V**, and both audio voices were non-silent with finite, bounded output. `examples/Modulation-preview.wav` is the recording.

Physical mouse handling and listening on your selected audio device are still part of your playtest. As of 2.9.0 there are nine internal MOD destinations, including [Swing](SWING.md); further hardware destinations remain incomplete. See `VALIDATION.md` for scaling assumptions and fidelity limits.
