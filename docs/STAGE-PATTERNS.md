# Slide, Skip and Ratchet gestures — 2.38.0

These stage editing gestures follow printed pages 87, 88 and 94 of the
[firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
Select a note track, press EDIT and choose SLIDE, SKIP or RATCH.

| Gesture | Slide / Skip | Ratchet |
| --- | --- | --- |
| Stage button | Toggle that stage's state | Select that stage's count |
| Encoder drag | Edit the selected state | Edit the selected count, 1–8 |
| ALT + encoder drag | Rotate/wrap the eight-stage pattern | Change all eight counts relatively |
| ALT + encoder click | Invert the entire pattern | No additional action |
| Long encoder, over one second | No ordinary reset gesture | Reset the selected count to 1 |
| ALT + long encoder | Reset all eight states to Off | Reset all eight counts to 1 |

Click ALT to enter its latched mode, perform an operation and click ALT again
to leave. Rotating forward moves a state to the next stage, wrapping stage 8
to stage 1; reversing moves it back. Each complete drag has one Rack undo
action. A full eight-stage wrap, an unchanged reset or any other unchanged
operation adds no history. This direction convention matches the existing
CV/MOD gate-pattern editor and remains a hardware-comparison detail.

Ratchet bulk editing applies the selected count's change to the original
counts, bounded to 1–8. Reversing during a drag restores differences lost at
an endpoint. The manual specifies bulk count editing without describing that
endpoint behavior, so this remains a documented interpretation.

Right-click the encoder, display or a stage for rotation/inversion and
selected/all reset actions. Native numeric entry remains available. Explicit
actions cancel pending previews. With Enc Confirm enabled, ALT Ratchet drags
preview all eight counts and the encoder applies them together. Slide/Skip
pattern operations apply directly, matching the existing gate-pattern workflow.

The display marks bulk editing as EDIT ALL / ALT and highlights all edited
Ratchet, Probability and Accumulation bars. These gestures affect only the
selected note track's current stage row. They keep the other track, other
rows, CTRL settings, shared sliders, preset bank and MOD group mute intact.
They are separate from the track-wide Slide amount/type screen.

## Compatibility and checks

No IDs or saved fields changed: 787 parameters, schema33 and format11/522.
Stage values save in Rack patches and musical presets as before. Reopening
retains the selected editor and clears transient ALT.

The dedicated Windows/Linux Rack callback suite exercises every eight-bit
Slide/Skip pattern on both tracks, forward/backward/multi-wrap rotation,
inversion, reset and exact undo/redo. Ratchet cases cover every selected stage,
selected/all reset, relative edits with clipping/reversal and eight-count
confirmation. Menus, the public EDIT/ALT workflow and save/reopen also have
checks. These are real Rack callback/render checks; OS mouse/focus behavior
and a physical-module comparison remain human playtest items.
