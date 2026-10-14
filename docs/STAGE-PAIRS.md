# PROB and ACCUM gestures — 2.36.0

The paired controls follow printed pages 99 and 101 of the
[firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
Select a note track, press EDIT, choose PROB or ACCUM, then select a stage.

| Gesture | PROB | ACCUM |
| --- | --- | --- |
| Turn/drag encoder | Probability amount | Signed accumulation amount, in scale degrees |
| Click encoder | Cycle Stage / Pulse target | Cycle Stage / Pulse / Ratchet trigger |
| Hold encoder for more than one second | Reset amount to 100% and target to Stage | Reset amount to 0 and trigger to Stage |
| ALT + turn/drag encoder | Change all eight amounts | Change all eight amounts |
| ALT + click encoder | Cycle selected target, then apply it to all eight stages | Cycle selected trigger, then apply it to all eight stages |
| ALT + hold encoder | Reset all eight amounts and targets | Reset all eight amounts and triggers |

ALT amount editing applies the selected stage's change to the other stages,
preserving their relative values until an endpoint is reached. It leaves
targets/triggers unchanged. Reversing direction during the drag uses the
original pattern, so values at an endpoint recover their original differences.
The operation affects the selected track only.

The manual says ALT + encoder press gives every stage the same target/trigger,
but does not explicitly specify whether that press advances the selected
choice first. This implementation cycles the selected choice and applies it
to the other stages, matching the existing CV transition gesture. That detail
and relative editing at endpoints still need physical-module comparison.

Right-click the encoder, display or a stage for target/trigger choices, applying
the current choice to all stages, and selected/all paired resets. Native Rack
numeric entry remains available for the amount. These menus provide direct
desktop access to the same parameters.

Each drag, reset or target/trigger operation creates one Rack undo action.
Undo restores signed amounts and secondary choices exactly, including values
clipped during an ALT edit. Unrelated fields and the other track stay intact;
CTRL-owned parameters remain protected.

Since 2.51.0, amount drags capture only amounts in Undo. An independent
target/trigger change during the drag survives undoing that amount edit,
including ALT edits and changes to any of the eight stages. Clicks and long
paired resets retain their own target/trigger history.

With Enc Confirm enabled, an ALT drag previews a change across all eight amounts.
Press the encoder to apply it in one action. Targets/triggers stay unchanged
during confirmation. Paired reset cancels any pending preview and applies the
defaults directly. These stage gestures are separate from the track-wide LIM
accumulator settings.

## Compatibility and checks

No parameter IDs or saved fields changed: 787 parameters, schema33 and
format11/522 remain unchanged from 2.34/2.35. Amounts and target/trigger choices
save in musical presets and Rack patches as before.

The dedicated Windows/Linux Rack callback suite exercises every stage on both
tracks, all target/trigger choices, selected/all reset thresholds, relative ALT
edits with saturation/reversal, exact undo/redo, menus, pending confirmation and
save/reopen. Existing engine and production tests cover the underlying
probability and accumulator signal behavior. OS mouse/focus playtests and a
physical-module comparison remain useful.
