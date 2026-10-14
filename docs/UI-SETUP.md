# UI setup, confirmation and external clock input — 2.48.0

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

AUX destination/assignment, CTRL destination/assignment, MOD destination/assignment and A/B source choices
always require confirmation when browsed with the encoder/display. A confirmed
edit creates one normal Rack undo action. Leaving a page cancels its preview;
unconfirmed edits never enter a saved patch or preset. Direct named menus,
native numeric entry, panel sliders, CTRL knobs and desktop grid controls remain
immediate. ALT MOD voltage previews preserve relative offsets across all eight
stages and confirm as one undoable edit.

## Stationary clicks and Undo

A press by itself, with no mouse movement, performs its normal click: field navigation,
a toggle or a choice menu. If another control or automation changes a value
during that press, its write is not recorded as your drag. An actual toggle or
chosen menu value records its own Undo from the latest applied value.

This applies to panel controls, the OLED and optional grid cells. Cursor
capture and adjustment tips are released normally. Pointer movement retains
its existing drag workflow, including small continuous edits. CTRL ownership
still cancels a held edit when a controller takes over.
Held GATE/SLIDE plus a stage button is an intentional shortcut edit. Those
edits retain their Undo even when the held mouse stays still.

Small CV drags also retain their exact stored-value Undo. A drag below the
existing three-pixel click threshold can perform an edit and its normal click;
Undo reverses the click first, then the voltage edit. See [CV editing](STAGE-CV.md).
A movement callback with zero distance does not write a displayed/clamped value
back into a stored parameter. Its normal release click still acts once.

MOD voltage and clock-offset drags now have the same stored-value protection.
Undo restores a negative MOD voltage hidden by a positive-only type, or an
offset hidden by a smaller clock division. ALT MOD voltage editing shifts
every stage from its effective voltage and preserves its relative offsets
until a stage reaches its limit. See [MOD editing](MOD-TEST.md) and
[CLK output](CLOCK-OUTPUT.md). A small edit is recorded before its genuine
transition/Gate click or field navigation. Pure confirmation previews add no
edit action until applied.

## Long-press resets and Undo

Hold the encoder for more than one second to reset the current page or item
where that page offers a reset. A whole-page reset still skips CTRL-owned
fields. Reset behavior follows each page: for example GATE resets Length and
Scale together, PROB/ACCUM resets amount and target/trigger, and A/B restores
that output's default source. SCALE's hold opens its user editor instead.

The reset records one Rack Undo action **when it fires**, using the latest
applied raw values and current track/stage/ALT scope. Changes made by another
control or automation during the hold are therefore the values restored by
Undo. A reset that changes nothing adds no history. Dormant signed CV values
also restore exactly, even when a Unipolar display shows zero.

After a hold fires, its remaining movement and release are consumed. Release
before editing again, especially if the hold opened a different page. Later
external writes are not added to the reset's history. Long actions that already
record their own storage/chain/pickup history keep that history. Ordinary drags,
short presses and confirmation previews retain their existing workflows.

## Pending edits and desktop value actions

With Enc Confirm enabled, an encoder or OLED drag previews a value; it does
not write a parameter or create an undo entry until confirmed. If external
automation changes any value captured by that preview, it is discarded.
Bulk previews check all eight captured stages. This preserves stored negative
CV values even when a Unipolar quantity displays zero. A new CTRL assignment
that owns the value also discards the preview.

Continuing a drag after an external change starts a new preview from the latest
applied value. Changing page, selected parameter or ALT scope cancels the old
preview. An unrelated external parameter change leaves it valid. Opening a
matching control's right-click menu cancels it, including when Initialize
would leave the applied value unchanged. Native choices and numeric entry
apply immediately and retain Rack's normal undo. Merely opening that menu or
releasing a preview drag adds no undo entry. A confirmed bulk edit remains one
action, and undo restores the exact stored values.

These pending states are UI-only and are not serialized. There are no changes
to parameter IDs, settings schema33, musical format11 or the 522-value snapshot.

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
comparison. See the current [MIDI](MIDI.md), [MX](MX.md) and [pickup](PRESET-PICKUP.md) guides for those implemented interfaces.
