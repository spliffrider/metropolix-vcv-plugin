# Per-stage CV — 2.47.0

Each note track has an additional eight-stage CV pattern. It follows that
track's stages and pulse counts, including its play order, skips, division,
Swing and Loopy performance. Note gates, rests and probability do not mask this
lane. Track mute and stopping silence it.

Select a track, then ALT → TYPE (stage 8). Drag the encoder to choose
**Bipolar ±**, **Unipolar +**, or **Gate Toggle**. Hold the encoder for one
second to restore Bipolar. Right-click the encoder or a track button for named
choices. Type changes apply immediately.

Assign A or B to that track's CV: ALT → A/B, or use the output source menu.
The pattern must be connected to another module to hear its effect. The two
PITCH/GATE outputs remain the note sequence.

## Small edits and Undo

Encoder, display and optional grid voltage drags preserve the exact stored
voltage on Undo, including a negative value hidden by Unipolar or Gate mode.
A zero-distance movement does not clamp that stored value. Release still
performs its ordinary click: a Gate toggle or an encoder transition change.

The existing click threshold is three pixels. A smaller genuine drag can edit
the voltage and perform that click. They are recorded in that order, so Undo
reverses the click first and then the voltage edit. A movement at the voltage
limit adds no edit action. With Enc. Confirm enabled, a tiny voltage preview
confirms once without also changing its transition.

Selecting Track CV as the MIDI velocity source forces saved Bipolar CV to
sound in the Unipolar 0–5 V range. ALT relative editing now uses that same
effective range for every stage. The saved Bipolar choice returns when another
velocity source is selected. Single-stage, Gate pattern and confirmation
workflows retain their existing scope.

## Voltage patterns

Click EDIT → CV, then a stage. Drag the encoder, display or held stage button
up/down to change its voltage. Bipolar spans -5 to +5 V; Unipolar spans 0 to
+5 V. Click the encoder to cycle its incoming transition:

| Transition | Behavior |
| --- | --- |
| Step | Changes to the new value at the start of its stage |
| sRamp | Reaches the value across all pulses in that stage |
| pRamp | Reaches the value across its first pulse, then holds it |

For example, with four pulses, sRamp takes four pulses; pRamp takes one and
holds for three. With one pulse they agree. A skipped stage supplies no value
or ramp source. The source is the last actual CV value, preserving continuity
if a live edit or fixed pulse length ends a ramp early.

The OLED shows the three transitions, selected vertex, voltage and playing
position. Hold/toggle ALT to show every vertex. ALT + encoder drag shifts all
eight effective voltages together, clamping each at its type's limits. ALT +
encoder click assigns the selected stage's next transition to all eight. A
long encoder press resets the selected voltage to zero; ALT resets all eight.
These operations group their Rack undo. Menus provide individual transitions,
applying one transition to every stage, and single/all-stage resets.

## Gate patterns

Gate Toggle outputs exactly 0 or +5 V and ignores stored ramp choices. Clicking
a stage toggles its gate. The OLED uses filled/hollow dots and the buttons light
pink for enabled stages. The encoder can toggle the selected gate too.

ALT + upward/downward encoder turns rotate the pattern forward/backward. ALT +
encoder click inverts it. Long encoder press clears the selected gate; ALT
clears the entire pattern. Right-click offers inversion and rotation as well.
Each gesture is one undo action.

## Persistence and interpretations

Types, voltages and transitions are stored in Rack patches and internal
presets/chains. IDs 687–704 append two types and sixteen transitions; all earlier
IDs are unchanged. Schema 17 and compact preset format 5 have 478 musical fields.
Formats 1–4 load with Bipolar/Step defaults, preserving the earlier plugin's
output.

The manual's storage table lists CV values and transitions but does not
explicitly classify the track CV type; saving the type with its pattern is a
plugin choice that also allows the demonstration to compare types in a chain.

Changing type alone retains the stored numerical voltages and ramp choices.
Unipolar clamps negative values to zero without erasing them; switching back
restores them. Gate Toggle interprets values at least 1.5 V as On, matching the
plugin's existing gate lanes. Gate edits write 0/5 V. The threshold and preserved
representation are local choices where the manual does not define conversion.
The main controls and optional grid preserve raw stored values in undo.

Ramps start from zero after reset. A stalled external clock allows the current
pulse's ramp progress to finish, then holds; it does not invent additional
sRamp pulses. Swung/divided clocks determine each pulse's duration. These
first-ramp, stopped-clock and live-count behaviors need owner comparison.
Stopped Stage Player sends the chosen typed value immediately and freezes it
until a fresh press; it does not run its stored ramp.

The reference is the 1.6 manual pp.103–106/136. No physical module was available
for comparison. Its p.104 bulk-ramp instruction names EDIT, while p.105 and
the rest of the shortcuts name ALT; this version consistently uses ALT.

## Try it

Open [Stage-CV-performance.vcv](../examples/Stage-CV-performance.vcv), choose
your AUDIO device and press RUN. It starts stopped and uses Core and VCV Free.
A drives the lead filter; B drives the bass filter. Four related presets compare
Bipolar ramps, Unipolar ramps, Gate Toggle and mixed types. The seven-link chain
keeps both note tracks audible. Disable chaining to edit a single CV pattern.

`scripts/test-ui.ps1 -StageCv` exercises type/transition controls, ALT offsets,
gate inversion/rotation, resets, stored-value undo, the optional grid and native
save/reopen. `-CvAudio` records twelve seconds of stereo playback plus the actual
A/B cable voltages in `stage-cv.csv`, using shortened eight-pulse preset phrases
to cover the complete chain. Recreate the public demo with
`python scripts/create-stage-cv-demo.py`.
