# UI setup, confirmation and external clock input — 2.63.0

## MIDI MX settings — 2.70.0

Primary SETUP now includes MIDI MX Ch and MIDI MX Output after MIDI PC Ch.
The channel is Off or 1–16; the output is CC or Note. They configure the same
independent MIDI bus as ALT → stage 6 → ALT on the MX screen and the desktop
MIDI Setup menu. The new global values stay outside musical presets. Old
menu positions migrate to their original named setting. [MIDI](MIDI.md) and
[MX](MX.md) explain operation; [Backups](BACKUPS.md) covers the new file versions.

## Native menu layout and complete descenders — 2.65.0

Track, primary SETUP, MOD, Global, UI, MIDI and Presets Setup menus use the
native fifteen-pixel row pitch, measured headings and margins, and inverted
row/value selections. Right alignment respects advance cells, including the
different trailing space of `t`. Zero transpose shows a single dash on the
OLED; its stored value remains zero. Long values retain compact label aliases
and full names in hover help and menus.

The fourth row moves upward when needed to keep lowercase tails visible.
The supplied SETUP image clips the `p` in Input; the desktop version retains
it. Backup/maintenance screens keep compact rows for their status lines, and
track copy feedback temporarily uses the compact layout. Browsing, binary
clicks, confirmation, navigation, native menus and exact Undo/Redo are unchanged.

The entire 8192-cell Track example matches the native p33 image. SETUP matches
6656 native cells across its heading and first three rows; its final 1536 cells
follow an independent bounded-row reference, using the native fourth row moved
up two pixels and restoring its clipped tail from observed glyphs. This is
explicitly a desktop adaptation, not a claim that every SETUP pixel matches
the cropped hardware image. No cells are excluded from either comparison.

Both complete reference arrays pass in actual Rack and a separate saved-image
comparison. All 194 current menu captures were inspected. Sixteen bottom-row
word/selection cases retain their tails, and the existing navigation, pending
value, stale-preview, reset and full-parameter Undo matrices pass. All 20
affected Windows groups pass; release CI separately checks native hosts and
eighteen Linux Rack groups. Font glyph tables, DSP, 787 parameter IDs, schema34, format11/522
snapshots and backup formats remain unchanged. Performance-field and gauge
refinements continue separately.


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
| CV Out Delay | 0–21 ms applied to the seven main outputs and MX outputs; MIDI stays immediate. Default 0. |

AUX destination/assignment, CTRL destination/assignment, MOD destination/assignment and A/B source choices
always require confirmation when browsed with the encoder/display. A confirmed
edit creates one normal Rack undo action. Leaving a page cancels its preview;
unconfirmed edits never enter a saved patch or preset. Direct named menus,
native numeric entry, panel sliders, CTRL knobs and desktop grid controls remain
immediate. ALT MOD voltage previews preserve relative offsets across all eight
stages and confirm as one undoable edit.

## Confirmed scale browsing

With Encoder Confirm On, [SCALE previews](SCALES.md) now follow the same hardware
bank order as immediate browsing. The displayed question mark remains until
confirmation. Stored scale IDs and lock state remain separate and unchanged.

## Menu lettering

Lists now use [proportional lettering](UI-FIDELITY.md) based on the native OLED
images. Full descenders fit inside each selected row, including pending values
on the bottom row. Long values can shorten their OLED label; full names remain
in help and native menus. This changes display rendering, not saved preferences.

## Primary SETUP list

SETUP opens a four-row scrolling list, with mixed-case labels, values on the
right and a white selection bar. Its first four entries follow the manual's
p48 screenshot: **Clock**, **Clock Mode**, **Run Btn**, **Reset Input**.
Clock Mode is the analog **Clk/Rset Out** option described by the p49 table.
Bottom Pitch and Tuner follow; PRESET, MIDI and ADVANCED headings separate
Total Pulses/Init/Chains, MIDI settings/Backup Files and the advanced submenus.
Auto clock selection and Legacy output scope retain the existing Rack choices.

Drag the encoder or display to browse. A binary setting toggles on one click.
For another setting, click to select its value, drag vertically, then click to
apply and return to browsing. These main-list choices always require explicit
confirmation, including when Enc. Confirm is Off. Native numeric/context-menu
choices still apply directly. An independent edit cancels an obsolete preview
without overwriting that edit or recording a false Undo; the value field may
stay selected so you can start a fresh edit.

Hold the encoder for a second to reset only the selected parameter, before or
during editing. One Undo restores its exact prior raw value. EXIT leaves a
submenu; Tuner and Backup opened here return to this list, including nested
Backup selection. The desktop Global/MIDI shortcuts retain their own parents.
Right-click SETUP for Slider Octaves, Global Setup, MIDI Setup and other native
shortcuts. Dragging the SETUP button only opens navigation.

Settings schema34 stores positions 0–16 and the direct child parent. Older
schemas29–33 map the previous eight shortcuts to their new destinations and
reopen main SETUP browsing, avoiding an edit of a different setting. Reopening
a tuner patch returns inactive to its correct parent. Parameters remain 787,
and musical snapshots remain format11/522 values. New **Settings** and **All**
backups require 2.55; musical-only exports retain their existing format11
compatibility. Preserve an old patch copy before resaving for older versions.
Backup Files uses the documented plugin JSON format; it is not hardware SysEx.
Version / Factory identifies the plugin version and its maintenance actions.

## Track and secondary lists

ALT → TRK 1/2 opens **TRACK 1/2 SETUP**; ALT → MOD opens **MOD n SETUP**.
These and the Global/UI/MIDI shortcuts use the same four-row spacing and
mixed-case labels as primary SETUP, following the manual's p33 track screenshot.
Browse with the encoder or display. A selected row is inverted; entering a
multi-choice value highlights only that value. Binary options toggle on one click.
Enc. Confirm retains its existing Off/On behavior on these secondary lists.

Slider Octaves shows **Shared** or **1–4**. A pending choice from Shared shows
its actual octave number and `?`; the previous readout incorrectly kept Shared.
Transpose shows a signed semitone count, or `--` at zero. Gate source names
use the manual's compact Single/Sngl+Rst/Multi/Multi+Rst/Hold/Hold+Rst choices.
Long labels shorten only where a value needs the space; hover and native menus
retain the full label and units. Track-copy feedback occupies a separate bottom
line. Selected-item resets, CTRL locks, native shortcuts and Undo stay intact.

This display update keeps schema34 and all saved parameter/cursor meanings.
Presets Setup now uses the same compact list; see [Preset settings](PRESETS.md).
[Backup](BACKUPS.md) and [maintenance](MAINTENANCE.md) now share the compact
list style, with separate status lines and Cancel-first confirmation. Exact
glyph/material matching and hardware comparison remain open.

## MOD lane navigation

On MOD ORDER/LEN/DIV, plain stage buttons select lanes while keeping the
current settings screen and field open. They do not edit the lane's stage
voltages or gates. Lane lights, hover text and right-click settings identify
the target. See [MOD lane editing](MOD-TEST.md).

## Numeric entry, Initialize and double-click

Right-click a value to enter a number or choose a named switch value. Numeric
entry uses the parameter's normal units and Rack's expression parser. Initialize
and double-click reset the clicked parameter. Each actual change adds one Undo;
a no-change entry adds none. Fine dragging and Unmap remain available.

The menu stays attached to the parameter you clicked if you select another
track, stage or page before applying it. It rechecks editability when an action
runs, so assigning a CTRL knob cannot leave an older menu able to overwrite its
now-owned value. Internal MOD destinations fix their lane type; global MOD
routing fixes assignment scope. These read-only states preserve the saved type
or track preference when Initialize or double-click is attempted.

Undo restores the actual stored value, including a negative voltage hidden by
Unipolar/Gate CV or a clock offset hidden by a smaller division. The entry itself
still follows the current effective range, display conversion and clamps.

## Assigned MOD type grid menu

Right-click the optional MOD grid's type cell while an internal destination
fixes its type. The menu explains the effective type and destination. **MOD
lane settings** opens settings for the clicked lane, even if another lane is
selected after the menu opens. Clearing that lane's destination restores its
saved type choice with one Undo action. Actual CTRL-owned cells retain their
controller settings menus.

## Paired-page dragging and Undo

On CLK, GATE, SLIDE, LEN, DIV and ORDER, an encoder or OLED drag edits its
selected field. Its Undo action leaves independent changes to the page's
other fields intact. PROB/ACCUM encoder, OLED and stage-button drags capture
amounts only; an ALT drag captures all eight amounts, keeping separate
target/trigger changes outside its Undo action.

Long encoder holds still reset the whole paired page or stage pair. ALT
PROB/ACCUM resets still cover all eight amounts and targets/triggers. These
reset scopes are separate from the fields captured by a turn. Held panel
shortcuts, confirmation and CTRL ownership retain their existing behavior.
This is a Rack editing addition; saved parameters and formats are unchanged.

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

## Type and range changes during an edit

Changing a Track CV type or Velocity source, a MOD type or destination, or the
CLK output division can change the meaning or range of its displayed value.
An unconfirmed preview created under the previous setting is now discarded.
Its obsolete confirmation or held release is consumed without a Gate toggle,
transition change or parameter Undo action. The externally applied setting
stays in place, including when the UI already cancelled the held preview.

Continuing intentional movement starts a new preview from the latest settings;
that preview can be confirmed normally. Movement at a value's bound that makes
no meaningful preview retains the normal short-click action.

For immediate voltage edits, changing a type during the drag no longer creates
an extra native Undo action. The voltage edit restores its exact raw origin,
while the externally changed type remains in place. This covers the encoder,
OLED and grid, including ALT groups and the existing tiny edit-before-click
workflow. Paired non-voltage page edits are a separate scope to audit.

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
to parameter IDs, musical format11 or the 522-value snapshot. Schema34 changes
only SETUP navigation; the pending edit itself remains transient.

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
