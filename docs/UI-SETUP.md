# UI setup, confirmation and external clock input — 2.22.0

Open SETUP, drag the encoder to **UI Setup**, and press it. Drag to browse;
press to toggle a two-choice option or enter a value. Hold the encoder for a
second to restore the selected preference. Right-click SETUP/display for named
choices. These preferences are global to this module instance and stay outside
the 64 musical presets.

| Preference | Behavior |
| --- | --- |
| Run on Boot | Off / On / Recall Rack patch. The software default preserves the patch's saved transport. |
| Run Btn | Pause retains position; Stop restarts on the next run. |
| Enc. Confirm | Off applies ordinary screen edits immediately. On previews them with `?` until the encoder is pressed. Default Off. |
| Show Pos. | Show or hide playback position in stage editing diagrams. HOME and panel lights still show transport. |
| Button / Slider LED / Screen Level | Four brightness levels; pitch lights continue using Rack's Bloom setting. Default 4. |
| CV Out Delay | 0–21 ms applied equally to all seven pitch, gate, clock and A/B outputs. Default 0. |

AUX destination/assignment, MOD destination/assignment and A/B source choices
always require confirmation when browsed with the encoder/display. A confirmed
edit creates one normal Rack undo action. Leaving a page cancels its preview;
unconfirmed edits never enter a saved patch or preset. Direct named menus,
native numeric entry, panel sliders, CTRL knobs and desktop grid controls remain
immediate. ALT MOD voltage previews preserve relative offsets across all eight
stages and confirm as one undoable edit.

With an external clock selected, BPM shows the measured divided tempo and input
division. Drag its button/encoder to choose `/1`–`/256`. Hold BPM and click stages
1–8 for `/1`, `/2`, `/3`, `/4`, `/6`, `/12`, `/24`, `/48`. With internal clock,
the same gesture selects 20, 40, …, 160 BPM. `/6` converts a 24 ppq clock to the
module's sixteenth-note timing. The first edge starts the divided clock; changing
division or a global reset restarts that phase. The manual does not state its
maximum divider; 256 is this implementation's upper limit.

MOD alternates between voltage-stage editing and its confirmed assignment page.
ALT → MOD opens Init Lane / Init All. Those actions reset stage voltages and
transitions, retaining routing and traversal. ALT encoder changes all voltages,
or rotates a gate pattern; encoder press changes all transitions or inverts gates.
Long encoder resets selected/all voltages. Menus expose the same actions.

Hold TRK 1, TRK 2 or MOD while pressing RESET to reset only that selection,
including a track protected by Global Reset Off. The other selections and master
clock phase continue. This corrects the earlier held-track behavior that also
reset the other unprotected track and all MOD lanes.

Try [Setup-performance.vcv](../examples/Setup-performance.vcv), select your AUDIO
device and press RUN. It opens the delay preference at 8 ms with two voices;
changes to that delay shift pitch and gates together. This is a software
interpretation of manual pp. 37–38, 57–61 and 139–144, without physical-module
comparison. MIDI/MX interfaces and physical pickup are subsequent work.
