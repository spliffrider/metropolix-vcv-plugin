# Loopy and Stage Player — 2.34.0

For direct mouse range selection, visible mutes and keyboard stage playing,
open the optional [Desktop performance view](DESKTOP-PERFORMANCE.md) in 2.71.0
or newer. The original panel and menu gestures below remain available.

Reference: Metropolix 1.6 manual, pp.39–46 and 169. The panel keeps the hardware's
∞ button and eight stage buttons; desktop menus make two-boundary loops practical
with one mouse. No physical-module comparison has been performed.

See [MOD group mute](MOD-GROUP-MUTE.md): muting the group preserves each
lane's individual mute choice; the independent group bit is stored in presets.

## Temporary loops

1. Press RUN, select TRK 1 or TRK 2, then click **∞**.
2. Drag the encoder or OLED up/down to choose **Auto** or **1–64 pulses**.
   Ctrl gives fine adjustment; right-click offers numeric entry and named choices.
   Click the encoder to toggle **Current Trk** / **1+2**. MOD selection switches
   to TRK 1 on entry. Current follows the selected note track.
3. Hold a stage button. Playback jumps there on its next track-clock pulse.
   Release it to return to where the original pattern would currently be.

Auto repeats every pulse of that single stage. A fixed pulse count continues
linearly through further stages, then restarts at the held stage. The Direction
setting is respected; play order, stage window, fixed track length and Skip
are bypassed by the temporary loop. Gate types, probability, ratchets, pitch/gate
overrides, slide, CV and modulation continue to apply.

## Two boundaries and latching with a mouse

While running on the Loopy screen, **right-click a stage → Latch range from
stage N → Through stage M**. This starts a latched loop between the two
boundaries. Reverse starts at the upper boundary. Auto plays every assigned
pulse in the range. Fixed counts truncate or wrap that range:

| Stages 1–4, one pulse each | Forward | Reverse |
| --- | --- | --- |
| Auto | 1 2 3 4, repeat | 4 3 2 1, repeat |
| 3 pulses | 1 2 3, repeat | 4 3 2, repeat |
| 5 pulses | 1 2 3 4 1, repeat | 4 3 2 1 4, repeat |

Two simultaneous stage holds also define boundaries. Navigating to a Global or
Track settings page while holding a running stage latches the loop. The ∞ menu
can latch the current loop or release it. A lock appears on the Loopy display.
A latched loop continues on other pages. Holding another stage changes its
region and preserves the latch after release. Return with ∞, then click ∞ again or
click the encoder to unlatch. Leaving the page without a held or latched loop
returns to ordinary operation. Range menus require RUN; stopped audition is
momentary.

## Stage Player

Stop RUN and enter Loopy. Holding a stage outputs that track's pitch, a 5 V gate,
and stage CV through the existing A/B track-CV routing. It ignores physical
Gate Type, gate-length override, Skip and probability. Pitch overrides and the
existing pitch routing apply. The pitch/CV are sampled on press and frozen until
a fresh press. Releasing drops the gate/CV. The original playback position stays
unchanged; Resume run mode can continue it. Analog slide applies to marked stages
as of 2.13.0; its rate and target freeze on press. Tempo and Acid audition
immediately. See the [slide guide](SLIDES.md). When several stages are held
while stopped, this version auditions the lowest numbered stage.

## Mutes

Hold ∞ and click TRK 1/2, MOD, or a stage button. Stage buttons mute individual
MOD lanes. MOD toggles a separate group mute, preserving each lane's individual
mute choice. The MOD button shows the group state; Stage buttons show individual states.
With a single mouse, right-click ∞ and enable **Mute tracks / MOD lanes** instead;
click the desired buttons and disable that menu mode afterward, or open another
settings page. Unmuted controls show yellow; muted controls blink red.

These performance mute changes wait for the next base-clock pulse while running
and apply immediately while stopped. They update the existing track/lane mute
parameters or the independent group parameter and are stored in presets. Pitch slides already underway finish their
target while gates/CV are muted. The performance mute gesture itself does not
create Rack undo entries; the existing parameter context menus remain available.

## Saving, testing and limits

Loop pulse count and target are stored in each musical preset as of 2.33,
matching the preset column on manual p.169. They load with Instant/Queue/Total,
custom Init, chains and MIDI program changes, and participate in recall undo.
They also persist as live Rack patch settings. Held stages, latches and mouse
mute mode clear on patch reopen/module Initialize; a patch cannot retain a
held Stage Player gate. All earlier parameter/cable IDs stay fixed. Old banks gain
Auto / Current Track; format10 banks require 2.33; current format11 banks require 2.34. See [Preset storage](PRESET-STORAGE.md).

Open [Loopy-performance.vcv](../examples/Loopy-performance.vcv), select an AUDIO
device and press RUN. It starts stopped with Track 2 selected and the chain
disabled. The two voices, filters and stereo echoes use only Core and VCV Free.
Hold a stage, release it, then latch stages 2–5 with Auto and five pulses to compare.
The embedded four presets/seven-link chain remain available for further experiments.

Portable tests cover the printed sequences, pulse-count edits, skips, direction,
independent targets, divided/swung clocks, audition routing/freeze/release and
background continuation across every play order. Rack host/widget/file tests cover
actual output jacks, gestures, menus, undo for settings, global persistence and
preset compatibility. `scripts/test-ui.ps1 -LoopyAudio` performs temporary and
latched loops in that stereo patch and records twelve seconds of real output.

Foreground accumulator/probability state starts from the underlying track when
Loopy begins, then runs independently. On release the underlying state resumes;
foreground changes are not written back. This protects pattern continuation but
is an interpretation requiring hardware feedback, especially for Track accumulators
and random probability. Loopy AUX/MOD gate, pulse-count and offset routes and Queue & Loopy
options are implemented; see [Complete routing](COMPLETE-ROUTING.md).
Simultaneous native multi-touch and external hardware verification remain work. An automated callback test does not establish native
mouse/focus behavior on every computer.
