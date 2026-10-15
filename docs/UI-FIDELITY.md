# Panel and display refinement — 2.64.0

This pass uses the supplied firmware 1.6 manual, especially EXIT on p.27,
MOD editing on pp.138–141 and destination descriptions on pp.159–164.
The default panel remains the hardware arrangement with the independent
**sensiburner <3 intellijel** credit.

Primary SETUP and the Track/MOD/Global/UI/MIDI lists now use four compact
mixed-case rows, right-aligned values and inverted selection, following the
manual's pp33/48 screenshots. Value editing highlights only the value. The
Shared octave extension correctly displays a pending 1–4 choice and `?`.
See [UI setup](UI-SETUP.md) for gestures, compact labels and compatibility.
Presets Setup now follows the manual's pp53–56 setting order, using this compact
list style. Pending Init choices and question marks display correctly; see
[Preset settings](PRESETS.md) for gestures and saved navigation. Backup/maintenance
lists now share the compact rows and inverted selection, with separate status
lines and Cancel-first maintenance confirmation. Further edit-page, glyph,
animation and material matching still require comparison with the reference.

## CTRL ownership symbols and LIM captions — 2.64.0

Owned fields use a thirteen-pixel circled controller number with the native
outline and seven-row digits. The complete CTRL 2 symbol matches the inverted
encoder legend on manual p26. CTRL 1 uses the same outline and a digit observed
elsewhere in the manual; its composition and placement are interpretations.
Markers retain the existing gray and selected inversion on LIM. This change
does not alter which destinations own a field or the controller priority.

The LIM heading and footer use the proportional caption face, including two
more observed glyphs (plus and P), bringing its observed coverage to 28. The
footer, dots, existing menu rows and selected Negative Limit reproduce every
foreground cell of the firmware 1.6 p126 image in actual Rack. No pixels are
excluded. This is a shape comparison, not a hardware brightness or animation
measurement. Positive/Negative Limit follows the main p126 chapter; the p135
illustration uses the older Upper/Lower wording.

Tests retain all six LIM fields, 132 applied states, 240 actual pending states,
48 owned states, 24 field-navigation clicks, 48 immediate/confirmed edits,
32 owned read-only cases and 24 stale-confirmation conflicts. Another 80 actual
selected-field cases cover both controllers, both tracks and individual/both
assignment across ten fields. The independent symbol mask and contextual
pixels pass, including the lower selection corner where it crosses the marker
region. Encoder/OLED drag, reset, menus, preview isolation and exact Undo/Redo
retain their existing behavior. All 18 affected Windows groups pass; the
packaged production host and native/Linux checks run separately for release.

The first independent comparison caught a one-pixel LIM footer shift. A second
test assumption was corrected to include the existing selection-corner pixels;
the frame remains intact. Sequencing, 787 parameter IDs, settings schema34,
format11/522 snapshots and backup formats are unchanged. Other edit headings,
diagrams and panel materials remain separate visual work.

## Scale screen and editor — 2.63.0

Factory and user scale views follow the native headings, value baselines,
keyboards, root marks, lock/search/MOD-lock indicators, encoder hints, ALT badge
and footer. Note editing has its own larger keyboard, note arrow, EDITING/name
header and EXIT/long-press cue. The active frame moves between notes and Root.
Eleven complete native-screen foreground masks reproduce the supplied firmware
1.6 images on pp62/66/67/68 inside Rack, without excluding any pixels. This is
a foreground-shape comparison, not a physical brightness or animation claim.

Six additional caption glyphs and three bold glyphs come from these images.
There are now 26 observed five-row captions and 55 observed bold glyphs; the
thirteen observed large glyphs retain their existing rendering. Remaining glyphs
are companions. Compact bank headings outside the sampled Major/User views,
the black-key root mark and the two Dim aliases remain interpretations. Whole
Half Dim and Half Whole Dim keep their full names in hover help and menus.

With Encoder Confirm enabled, previews follow the same hardware bank order as
immediate browsing. A forward turn from Major now previews Dorian; it previously
previewed Minor by advancing a legacy stored ID. The change remains pending
until confirmed. Existing IDs and formats are preserved. Queued/pending hints
leave room for lock/long-press symbols and value question marks. Root and note
editing retain their confirmation preferences and exact Undo/Redo.

All 52 captured states and both actual-source charsets were inspected. Tests
cover all names in ordinary/queued/pending layouts, every confirmed bank
transition in both directions, roots, note editing and existing copy/shortcut/
lock/storage workflows. Sequencing code, all 787 parameter IDs, schema34,
format11/522 snapshots and backup formats are unchanged. Further ownership,
other page diagrams/captions and material matching remain separate work.

## Preset-chain display — 2.62.0

The chain table uses seven-pixel capitals with two rows for descenders, plus a
separate five-row caption face. Twenty observed glyphs in each face reproduce
the native firmware 1.6 images on pp171/174. Other glyphs remain companions;
they are not a claim of complete native-font equivalence.

Selected rows fill and invert; Preset and Repeats invert separately when being
edited. Eleven-pixel rows, the chain icon, table columns, mute symbols and track
footer follow the sampled layout. Unavailable empty-chain actions are hatched.
Magenta shortens to Mag when its full name would enter the repeat column; full
bank names remain in desktop menus. Quick Chain shows a plus beside the last
link's repeats, indicating that another matching stage press can increase them.
This cue follows the workflow on p175; its exact placement is an interpretation.
Playback retains the desktop repeat count and follow/static status.

Three independent native-screen foreground masks pass in actual Rack, with
one footer dot excluded because the source images give it different luminance.
Both font charsets and eighteen captured states were inspected. The 32-link
scrolling, field selection, mutes, Quick Chain Undo/Redo and Chain Follow cases
pass without render-time parameter, chain or history changes. Existing input,
playback, storage, 787 parameter IDs, schema34 and format11/522 remain unchanged.
Brightness, animation and physical-module equivalence remain unverified.

## Performance readouts — 2.61.0

Performance fields use separate bold and large lettering based on native OLED
images in the supplied firmware 1.6 manual. Fifty-two bold glyphs and thirteen
large glyphs reproduce the sampled shapes, bearings and spacing. Companion
glyphs remain interpretations; this does not establish a complete native font.
Tiny headings and the proportional menu font retain their existing roles.

Shared fields, stage values, tempo, tuner and assignments use the measured
baselines. Immediate, queued, pending and one/two/three-choice encoder cues
follow the existing confirmation policy. Stage accumulator values include the
degree unit; the SLIDE amount readout is unitless while its desktop quantity
continues to show percent. The main scale readout uses Ionian/Aeolian aliases
without changing scale IDs or full menu names. A/B and MOD assignment context
is clearer. Desktop status/help lines remain available.

Pending BPM edits preserve their question mark during modulation. The smaller
actual-tempo overlay compares against the applied base tempo, so previewing a
new base value does not itself appear as modulation. Confirmation and exact
Undo/Redo retain the existing behavior.

Forty-three independent native text masks, 194 glyph metrics, seven helper
masks, 43 actual module text regions, four pending-tempo drags and fourteen
queued/pending helper states pass inside Rack. These compare foreground pixels,
not physical brightness or color. Whole-screen captures and both charsets were
inspected. Chain and scale-editor geometry, other diagrams, animation and panel
materials remain separate refinement work. Sequencing, all 787 parameters,
schema34, format11/522 snapshots and backup formats are unchanged.

## Menu lettering — 2.60.0

SETUP, Track/MOD/Global/UI/MIDI, Presets Setup, Backup/maintenance and LIM
use a separate proportional font. The regular lettering has nine-pixel capitals
and eleven-pixel descenders. Thirty-eight observed glyphs reproduce native
OLED samples from the firmware 1.6 manual pp33/48/126/135; the remaining printable
glyphs follow those metrics and remain interpretations. Tiny headings/footer
lettering and the performance/numeric fonts keep their existing rendering.

Four-row lists retain their compact desktop spacing and separate status line.
Their selection bar now includes the full descenders, including the bottom row.
LIM retains its three fifteen-pixel rows. Existing pending markers, inverted
values, CTRL dimming/knob numbers and brightness behavior remain available.

Labels shorten only when their current value needs room: Slider Octaves becomes
Slider Oct, Slide Toggle/Amount CC becomes Slide Tog/Amt CC, and Gate Type
becomes Gate for its longest pending choice. Full names remain in hover help
and native menus. The input gestures, Undo/Redo, sequencing, all 787 parameters,
schema34 and format11/522 musical snapshots are unchanged.

Fourteen independent golden text rows check shapes and proportional spacing
against the embedded images; every printable glyph is checked for bounded
metrics and right alignment. Whole-screen captures include applied/pending
values, both CTRL owners and bottom-row descenders. This does not establish
complete typeface, animation, brightness or physical-module equivalence.

## Accumulator/LIM display — 2.59.0

The embedded OLED on manual p126 uses three rows. LIM now follows its title,
15-pixel row spacing, selection bar and track/ACCUMULATOR footer. The existing
six-field cycle and first/last groups of three remain. Labels use mixed case;
CTRL-owned Mode/Polarity labels and values are dimmed and identify CTRL 1/2.
Pending values show an inline `?` alongside the existing confirmation footer.
All six fields remain readable at their limits without abbreviated labels.
See [LIM controls](HARDWARE-UI.md) for the unchanged gestures and ownership rules.
The manual's +7/-7 example is used for comparison; the plugin's +9/-9 defaults
remain. Fonts, animation/material details and physical comparison remain open.

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
