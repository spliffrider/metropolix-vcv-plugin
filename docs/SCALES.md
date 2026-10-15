# Scales and chord changes — 2.63.0

The SCALE screen offers all **49 factory scales/chord shapes** shown in the
[firmware 1.6 manual, pp.62–68](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf),
plus **100 editable user scales**, in ten banks of ten slots. Factory note sets
were transcribed from the keyboard diagrams on pp.63–64.

## Screen and confirmation

Factory/user selection, lock/search/MOD-lock symbols and the larger note editor
follow the [native reference layout](UI-FIDELITY.md). The arrow follows the
selected note; ALT switches the active frame to Root. Hover help and right-click
menus retain full scale names. The OLED shortens Whole half diminished and Half
whole diminished to Whole Half Dim and Half Whole Dim when displaying them.

Encoder Confirm Off applies browsing immediately. With it On, browsing previews
the same hardware bank order and shows a question mark until you click to apply.
For example, Major advances to Dorian in both modes. Before 2.63, confirmed
browsing instead advanced through legacy numeric IDs and previewed Minor.
Existing stored IDs and older patches remain compatible.

If a question mark is visible, the first click confirms that value. A following
click performs the ordinary Scale Lock action. A pre-existing lock remains
separate from the selected scale and continues to control the sounding scale
until you apply a new lock or unlock it. Root and note-cursor editing also keep
the current confirmation preference. Their previews, toggles and cancellations
retain Rack Undo/Redo.

## Choose a scale

Click SCALE, then drag the encoder or display up/down to browse in the manual's
bank order. ALT switches between Scale and Root. Right-click SCALE, the encoder,
or the display for factory-bank menus, all ten user banks and direct Root choices.
The keyboard shows enabled notes, disabled hashed keys and the root marker.

The original seven numerical scale IDs stay unchanged for existing patches and
internal presets. Encoder browsing and the bank menus use the hardware order;
Rack's numerical automation IDs retain their historical order.

## Lock and browse

With Scale selected, click the encoder to lock the displayed scale. Browsing now
changes the screen without changing the audible scale. The lock becomes a search
icon when viewing another scale. Click to apply and lock that new scale; click
again to unlock. A user-scale lock also establishes its modulation bank and 0V
starting slot. Locks survive saving and reopening a Rack patch.

## Edit a user scale

1. Choose a user slot, such as Scale 1-1, with **Scale** selected rather than Root.
2. Hold the encoder for more than one second to enter note editing.
3. Drag to move the keyboard arrow; click the encoder to toggle that note.
4. ALT selects the user's Root field. Changing Root transposes the note pattern.
5. Hold the encoder again to exit from either the notes or Root field. The definition is saved with the Rack patch.

A ROOT-selected hold outside editing does not enter the note editor. The
explicit right-click **Edit user scale notes** choice opens directly from Root
and discards a pending Root preview. Selecting a factory scale through Rack
automation automatically closes an existing note editor. Inactive holds
use the control's existing short-click action when released.

Menus provide the same note toggles, and can copy a factory note pattern into the
current user slot. Each definition contains twelve note-enable bits, relative to
its own root. User slots recall their root along with their notes; factory scales
use the shared factory Root setting. New user slots start as C major.

At least one note must remain enabled. The editor ignores attempts to remove the
last note. A malformed empty definition falls back to the root note in the engine.
Note toggles, root drags and factory-pattern copies support Rack undo/redo.

## Copy and paste a scale

Following printed page 165, ALT + a short SCALE press copies the displayed
scale. Browse to a user slot, then hold ALT + SCALE for more than one second
to paste. The clipboard takes a snapshot of its notes and root; later source
edits and repeated pastes do not change it. A locked sounding scale does not
replace the displayed scale as the copy source.

On the computer, hold the keyboard Alt key while clicking/holding SCALE, or
right-click SCALE, the encoder or the display for **Copy displayed scale** and
**Paste scale into displayed user slot**. The panel's latched ALT from another
screen and a held panel ALT also work. Clicking panel ALT on the SCALE screen
continues to switch between Scale and Root.

Paste replaces only the selected user slot's notes and root, with one Rack
undo action. It keeps Scale selection, Scale Lock, shortcuts and other user
slots intact. Identical pastes add no undo action. Factory destinations and
an empty clipboard reject the paste; menus disable that action. Explicit
copy/paste cancels pending encoder previews and uses the applied definition.

The scale clipboard is separate from stage/lane copy. It is transient and
clears on Initialize or patch reopening; pasted definitions remain saved with
the patch. This clipboard also copies the root. The manual calls this a scale
copy without enumerating the copied fields, so root inclusion still needs a
physical-module comparison.

## Sequence chord changes

Lock a user scale and assign **Scale (User)** to a MOD lane, AUX input or CTRL knob.
Assignments can affect TRK 1, TRK 2 or both. Each +1V advances one user slot; each
-1V moves back one slot. Selection wraps inside the locked ten-slot bank. The
chosen slot supplies both its notes and root. Browsing another bank does not move
the locked starting point. Factory locks and unlocked user scales ignore this route.

CTRL uses a -5V to +5V equivalent around noon. MOD/AUX voltages and the CTRL offset
combine before choosing the slot. In this implementation, fractional voltages
truncate toward zero: +0.99V/-0.99V remain at the starting slot, +1V/-1V change it.
The manual specifies whole-volt changes but does not define intermediate boundaries;
this threshold choice still needs comparison with hardware.

Open [Scale-progression.vcv](../examples/Scale-progression.vcv), select an AUDIO
device and press RUN. MOD 1 selects C major, A minor, F major and G major triads on
both voices. CTRL 1 adds a live scale offset. Other user slots in that bank remain
editable, so a large CTRL offset can intentionally select a different chord.
Switch HOME to its note overview to see each track's current scale and pitches.

## Eight shortcuts

Hold SCALE and click a stage button to recall its saved Scale/Root combination.
Hold the stage for more than one second while holding SCALE to store the current
combination. For a mouse, right-click SCALE for the shortcut menu, or right-click
a stage on the SCALE screen for its individual Save/Recall commands.

Recalling a shortcut applies the new scale even if another scale was locked.
Shortcut gestures and menu actions group their parameter changes into one undo.
The eight shortcuts and all user definitions belong to the Rack module instance
and survive patch saving. They are not stored inside the 64 internal sequence presets.

## Compatibility and validation

Scale IDs, routing destinations and ports retain their identities. Custom note
definitions, custom roots and shortcuts remain global. Scale Lock is stored
with each musical preset as of 2.33, following manual p.169. The selected Scale
and factory Root were already stored. Locks load even when physical control
recall is Off, and participate in preset undo, Init, chains and MIDI PC.

Format10 appends Scale Lock and both Loopy settings after the frozen 518-value
prefix. Old banks still import with their old fields intact and neutral new
values (Unlocked/Auto/Current Track); recalling a migrated old preset can
therefore unlock the previously sounding scale. Format10 banks require 2.33.
Version2.34 appended independent MOD group mute as ID786 (787 parameters/schema33);
the lock/Loopy snapshot positions remain fixed in format11. See [Preset storage](PRESET-STORAGE.md).

Scale changes requantize slider pitches and chromatic override anchors.
Accumulation continues in the selected scale's degrees, preserving its stored
degree offsets. Loopy and stopped audition use the same quantizer.

Validation covers every nonempty twelve-note set, nearest-note ties and negative
octaves, factory mode relationships, manual World/chord diagrams, all user-bank
wraps, per-track AUX/MOD/CTRL routing, accumulation and overrides. Rack host tests
cover old patch migration, all 100 definitions, global preset boundaries and real
pitch outputs. Widget tests exercise editing, locks, held shortcuts, undo/redo and
native save/reopen. The supplied two-voice patch also verifies all four chord
changes during recorded Rack playback. Native OS mouse behavior and physical
hardware fidelity remain user-test items.
