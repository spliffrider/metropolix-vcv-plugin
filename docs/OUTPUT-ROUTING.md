# A/B output routing — 2.70.0

ALT → A or B opens the two-field routing screen (manual pp.115–119).
Drag the encoder or display up/down to choose a function or track/lane.
A question mark identifies an unconfirmed choice: the cable keeps its applied
signal until you press the encoder. Pressing also moves to the other field.
EXIT discards a pending choice. Right-click for named choices that apply directly.
Hold the encoder to reset the selected output: A = CV / Track 1;
B = First Pulse / Track 1. Confirmations and resets support Rack undo.

If the applied source changes elsewhere while a choice is pending, the obsolete
preview is discarded. This includes the expanded MOD editor, automation and
Undo/Redo. Confirmation checks the applied raw source before writing, even if
the display has not refreshed yet. Further dragging starts from the newest
applied function. Editing the other output does not cancel a valid preview.
Named menu choices already apply directly and clear their own pending choice.
Both output screens always require encoder confirmation, regardless of the
general Encoder Confirm setting. Their right-click choices and resets remain
direct desktop actions. The transient preview and its origin are never saved.

| Function | Signal | Assignment |
| --- | --- | --- |
| Ctrl 1/2 ± | Knob position as −5…+5 V; noon 0 V | Global |
| Ctrl 1/2 + | Knob position as 0…5 V; noon 2.5 V | Global |
| CV | Track CV, including its type and transitions | Track 1/2 |
| First Pulse | Trigger on the first track pulse after start/reset; repeats when LEN Pulses is fixed | Track 1/2 |
| Last Pulse | Trigger on the last fixed-LEN track pulse before restart; absent in Auto | Track 1/2 |
| MOD Clock | Trigger on each divided pulse of the chosen MOD lane | MOD 1…8 |
| Mod | MOD lane voltage, including Step/Ramp, type and mute | MOD 1…8 |
| New Stage | Trigger when traversal enters a played stage | Track 1/2 |
| Reset | Trigger when the global sequencer resets | Global |
| Root | Effective scale root in V/oct, including CTRL Root | Track 1/2 |
| Run | 5 V running, 0 V stopped/paused | Global |
| Slider (0–5V) | Current stage's physical pitch-slider position × 5 V | Track 1/2 |
| Slider (Range) | Slider position × the track's octave range, with its direction | Track 1/2 |
| Velocity | Selected track velocity, clamped to 0…5 V after offsets | Track 1/2 |
| Mod Trigs |5 ms trigger derived from track CV, independent of note gates | Track 1/2 |

The original CV/MOD assignment IDs and all cable endpoints remain fixed. Older
patches keep their A/B functions, including B = Track 2 CV when that was saved.
New modules use the hardware's A = CV / Track 1, B = First Pulse / Track 1 defaults.
Global functions have no track assignment. Both MOD functions select any of
the eight lanes. Track Out Swap exchanges every track-based A/B signal along
with PITCH/GATE; global and MOD functions stay on their selected source.

## Track settings and controllers

Right-click TRK 1/2 → Selected track settings for **Velocity** (50%, 75%, 100%
or Track CV), **Slider octaves** (1–4) and **Slider direction** (Normal/Inverted).
Velocity defaults to 100%, matching the firmware 1.6 change on manual p.192.
Track CV can create per-stage velocities. Negative CV is allowed as a base;
the final velocity is clamped only after CTRL/AUX/MOD offsets are added.

Older patches and bank formats 1–5 use **Follow shared range**, preserving
their existing range control and sound. Choose an explicit track range to
make the two tracks independent. These settings are stored in internal presets
(format 6); A/B functions and global CLK settings remain global across recalls.

CTRL gains three destinations: **Velocity** adds −5…+5 V; **Slider Range** directly
sets 1–4 octaves; **Gate Scale** directly sets 0–100%. The latter two own their
panel settings until unassigned. AUX/MOD **Velocity** adds −5…+5 V and **Track
Out Swap** accepts a gate. Positive CTRL Slider Invert reverses the programmed
track direction; noon/negative positions use that direction (manual p.152).
The pitch overview and pitch-override Sync follow the same direction/range.

## Play the example

Open [Output-routing-performance.vcv](../examples/Output-routing-performance.vcv),
select your AUDIO device and press RUN. It uses only Core and VCV Free:

- A carries Track 1 velocity into a second bass VCA. CV steps shape accents.
- B carries MOD 8's divided clock into a short envelope and sine percussion voice.
- Four scenes and seven chain links vary articulation and the percussion division,
  with a MOD/percussion break. Disable the chain to edit a single scene.

The patch starts stopped. Right-click A/B editing controls to explore other
signals; clock and stage triggers are useful for envelopes and other sequencers.

## Timing and fidelity limits

Triggers are 5 V and approximately 1 ms. Track triggers follow divided/swung
track clocks; MOD clocks stay on their independent straight divisions. Skipped
stages consume no New Stage trigger; rests and probability failures still enter
stages. All-skipped fixed-length tracks retain First/Last timing.
Manual RESET emits First Pulse immediately, without duplicating it on the next
clock, and never emits Last Pulse. Automatic global restart emits Reset;
ordinary track LEN wraps do not. During Loopy, First/Last follow the original
background track while New Stage and slider/CV signals follow the foreground.
Stopped Stage Player emits no clock/stage triggers.

The manual's general assignment paragraph is inconsistent with the individual
Reset and MOD Clock descriptions. This implementation treats Reset as global
and lets MOD Clock select eight lanes. Pulse width, live-edit event treatment,
Loopy routing and root-offset scope are documented interpretations requiring
hardware-owner feedback. No physical module comparison has been made.

## Mod Trigs — firmware 1.6.5

Select Mod Trigs and Track1/2 through the A/B screen or native menu. Rising CV
at the existing 1.5V gate threshold emits a full 5 ms pulse, including for a short
input. A held high CV does not create repeated pulses. Track swap follows the
source track; mute/stop clear the pulse. The stopped Stage Player can produce
it from its frozen CV. Note Rest/probability do not suppress a CV-derived
trigger. Trigger width is specified by the new manual p117; threshold, edge
and cancellation policy remain software interpretations.

The two new packed sources append at 38/39 after the unchanged 0–37 mapping.
Format 12 banks keep the same 522 value positions, but require 2.70 to distinguish
the new choices safely. Older banks and Rack patches retain their routes.
