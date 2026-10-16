# Preset storage — 2.34.0

## Current file compatibility — 2.70.0

New All/Settings backups require **2.70.0**: settings schema 35 contains789
parameters, appending MIDI MX Ch and MIDI MX Output after the frozen 787-ID
prefix. Old schema 34 SETUP/MIDI positions migrate to their same named setting;
new MIDI MX defaults to disabled CC. New Presets-only, Filled and Bank exports
also require **2.70.0** for musical format 12. The tuple still has 522 values and
all positions stay fixed; the version protects the new A/B routes38/39 from
being misread by older builds. Formats1–11 and supported settings schemas29–34
continue to import. Older format 9 example backups remain intentional migration
fixtures. Preserve an older patch copy if you need to return to an older plugin.

Musical presets now remember **Scale Lock**, **Loopy pulse count** and **Loopy
target**. The [firmware 1.6 manual's storage table, p.169](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf)
places all three in presets. Earlier plugin builds incorrectly treated them as
global settings; some older documentation repeated that mistake.

Save a pattern with a locked scale and a fixed Loopy pulse count, change those
settings, then load the pattern. The saved lock, pulse count and Current Track /
Tracks 1+2 choice return together. These values load even when Pitch, Switch,
CTRL and AUX loads are Off. An unlocked scale is also a saved choice.

The same recall applies to Instant, Queue Pulses and Total Pulses loads, custom
Init presets, preset chains and incoming MIDI program changes. Rack undo/redo
restores the fields changed by a recall. It does not rewind playback time.

Custom scale note definitions and their roots, both kinds of user shortcuts,
clock/setup preferences and MX assignments remain global. Held stages, active
Loopy regions, latches and pending preset requests are temporary performance
state and clear on patch reopen. Remembering the Loopy settings does not save a
held Stage Player gate or latched region.

## Files and compatibility

Version 2.33 added the three fields below in **format 10**, preserving the
518-value format 9 prefix. Version 2.34 appends an independent MOD group mute in
**format 11**, for 522 snapshot values and 787 Rack parameters (schema 33):

| Snapshot index | Stored value | Range |
| --- | --- | --- |
| 518 | Scale Lock | −1 = unlocked; 0–148 = scale ID |
| 519 | Loopy pulse count | 0 = Auto; 1–64 pulses |
| 520 | Loopy target | 0 = Current Track; 1 = Tracks 1+2 |
| 521 | All MOD group mute | 0 = Off; 1 = On |

Old full banks in formats1–9 and old selective Filled/Bank files in format 9 still
import. They never stored these three values, so imported snapshots receive
**Unlocked / Auto / Current Track**. Loading one of those migrated presets can
therefore change a previously active lock or Loopy setting. Presets-only import
itself keeps live settings until a preset is recalled.

For a selective import, only its listed slots are upgraded. Unrelated newer
slots retain their lock/Loopy values; explicit Bank empties clear only the
corresponding slots. All-Presets continues replacing the complete bank/chains.
Malformed formats, snapshot sizes and values are rejected before import.

Version 2.55 uses settings schema 34 for its expanded SETUP navigation. It keeps
all 787 parameter IDs and format 11/522 musical values. New All/Settings backups
require 2.55; Presets-only, Filled and Bank files still require 2.34. An older
Rack plugin does not preserve the new menu positions. Keep old patch copies if
you need to return to an older build.

| New file from 2.34 | Minimum receiving plugin |
| --- | --- |
| Rack patch with a saved musical bank, All or Presets backup, Filled/Bank export | 2.34, to read format 11 |
| Settings-only backup | 2.34, to read schema 33 with 787 settings |

Old formats1–10 and settings schemas29–32 gain group mute Off while retaining
the individual lane mask. Old selective formats9–10 upgrade only their listed
slots. See [MOD group mute](MOD-GROUP-MUTE.md) for the separate layers.

Preserve a copy of an old Rack patch before saving over it with a newer build
if you need to return to an older plugin. The included example backups remain
old format 9 files to exercise legacy import. See [Backups](BACKUPS.md).

## Validation

Native checks cover all frozen prefix mappings, every Scale Lock selection,
all Loopy pulse counts and both targets, old full formats1–9, old/new selective
files, exact history receipts, queued boundaries, Init, chains, MIDI program
change and the quantizer's actual pitch output. Actual Windows/Linux Rack
callbacks verify save/load, undo/redo, custom Init, legacy import and patch
save/reopen. All four production binaries run the runtime stress host.

This corrects the documented storage boundary. Hardware timing and live
performance behavior still need comparison with a physical module.
