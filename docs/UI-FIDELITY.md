# Panel and display refinement — 2.56.0

This pass uses the supplied firmware 1.6 manual, especially EXIT on p.27,
MOD editing on pp.138–141 and destination descriptions on pp.159–164.
The default panel remains the hardware arrangement with the independent
**sensiburner <3 intellijel** credit.

Primary SETUP and the Track/MOD/Global/UI/MIDI lists now use four compact
mixed-case rows, right-aligned values and inverted selection, following the
manual's pp33/48 screenshots. Value editing highlights only the value. The
Shared octave extension correctly displays a pending 1–4 choice and `?`.
See [UI setup](UI-SETUP.md) for gestures, compact labels and compatibility.
Presets Setup and Backup/maintenance layouts remain separate follow-up work.

## Values and confirmation

During a held drag, the adjustment popup shows the value you are previewing.
A pending value has **?** and **Press encoder to apply**. Releasing the mouse
keeps the preview pending; pressing the encoder applies it with one undo.
EXIT cancels it. Rack's Tooltips preference also controls this popup.

MOD stages now show destination offsets in both the OLED detail view and the
optional editor. Right-click numeric entry uses the same units. None and A/B
voltage output remain in volts; gate destinations show On/Off.

| Destination | Display |
| --- | --- |
| Pitch Pre / Post | Semitones, rounded |
| Pitch Offset | Semitones, continuous |
| Octave | Whole octaves, truncated toward zero |
| Gate Length / Scale, Probability, Slide Amount, Swing | Percentage offset |
| Internal BPM | BPM offset |
| Root | Semitone offset, limited to ±12 |
| Scale (User) | Whole-scale offset, truncated toward zero |
| Clock / pulse division, ratchet, play order | Offset in the corresponding steps |
| Stage / pulse length and Loopy | Stage or pulse offset |
| Slider Range | Octave-range offset |

These are modulation **offsets**, not the resulting base-plus-modulation
setting. The graph retains its voltage axis. The gains match this engine's
existing routing; numeric gains that the manual does not specify remain
implementation choices pending hardware comparison. Formatting does not
change stored voltages or the sound of existing patches.

The small OLED field abbreviates long units (pl/stg/scl/ord/rt) and reduces
decimal places when necessary so the unit stays visible. Hover help and numeric
entry retain the full units and precision.

ALT shows all MOD vertices while editing the entire lane. MOD gate destinations
send MIDI CC **0/127** even with a saved Bipolar type, including stopped CC
learning. Ordinary bipolar voltage CCs keep their bipolar mapping.

When Velocity Source is CV, a saved Bipolar track CV type operates as
Unipolar. Its tooltip explains the 0–5 V effective range and retains the saved
choice. Changing Velocity Source restores that choice. A pending TYPE change
is shown on the OLED before confirmation.

## EXIT and panel sync

A short EXIT goes back through Setup submenus. Backup files and tuner opened
from primary SETUP return there; desktop shortcuts retain MIDI/Global parents.
Leaving the tuner stops its audition. Other editing screens
return Home.

Hold EXIT for more than one second to **Sync to Panel**, or use its right-click
menu. This releases all recalled pitch/CTRL pickup targets together so the
visible controls determine the values. One undo restores the remembered
targets and one redo releases them again. Sync preserves sequence position;
it does not initiate a preset recall or reset. An already synced panel adds
no history action.

In Rack, Jump recall moves the visible virtual pitch, pulse-count, gate-type
and CTRL controls directly. They therefore already match their settings.
Thru recall keeps visible pitch/CTRL positions and remembers sounding targets;
those are the values released by Sync. This is a desktop adaptation of the
hardware's physically immovable controls. See [Preset pickup](PRESET-PICKUP.md).

## MX and optional editors

Opening the 66 HP editor beside an attached MX keeps the main panel anchored
and moves MX to its new right edge. Closing the editor brings MX back to the
34 HP edge. Rack moves conflicting neighboring modules as needed. The MX link
and its existing cables survive both changes.

## Verification and limits

Native Rack tests check units/numeric conversion, saved/effective CV type and
actual gate-destination MIDI bytes. Real Rack widget callbacks check held
preview popups, confirmation/cancel, submenu back, all ten Sync targets with
undo/redo, optional MOD fields, MX resizing and save/reopen. Bitmap display
captures cover units, ALT vertices, CV overrides/previews and Sync notices.

Visual material details, native OS mouse/focus behavior and physical Metropolix
comparisons still need human testing. The photo and manual guide the design;
they do not prove pixel-perfect appearance or hardware timing equivalence.
