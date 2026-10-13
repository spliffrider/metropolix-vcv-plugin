# Modulation playtest — 2.2.0 interface / 2.1.0 musical baseline

Open `examples/Hardware-panel-demo.vcv` in Rack. It is already running and patched. Right-click **MOD**, then choose **Open MOD editor** to expose the grid used in these checks. The older `Modulation-demo.vcv` uses the same musical settings. Use **File > Save As** to keep your own test copy. Choose your output device in the AUDIO module to hear the two voices. The patch's two VCAs are set to 32%.

Metropolix Lab produces control voltages, not audio. Its pitch and gate outputs drive the oscillators, envelopes, and VCAs below it. The first Scope monitors OUT A and OUT B; the second monitors PITCH 1 and GATE 1. Signals should appear even without an audio device selected.

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
