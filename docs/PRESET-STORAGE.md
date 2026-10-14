# Preset storage correction — 2.33.0

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

The 786 Rack parameter IDs and settings schema32 are unchanged. Musical bank
**format10** appends three values after the frozen 518-value format9 prefix:

| Snapshot index | Stored value | Range |
| --- | --- | --- |
| 518 | Scale Lock | −1 = unlocked; 0–148 = scale ID |
| 519 | Loopy pulse count | 0 = Auto; 1–64 pulses |
| 520 | Loopy target | 0 = Current Track; 1 = Tracks 1+2 |

Old full banks in formats1–9 and old selective Filled/Bank files in format9 still
import. They never stored these three values, so imported snapshots receive
**Unlocked / Auto / Current Track**. Loading one of those migrated presets can
therefore change a previously active lock or Loopy setting. Presets-only import
itself keeps live settings until a preset is recalled.

For a selective import, only its listed slots are upgraded. Unrelated newer
slots retain their lock/Loopy values; explicit Bank empties clear only the
corresponding slots. All-Presets continues replacing the complete bank/chains.
Malformed formats, snapshot sizes and values are rejected before import.

| New file from 2.33 | Minimum receiving plugin |
| --- | --- |
| Rack patch with a saved musical bank, All or Presets backup, Filled/Bank export | 2.33, to read format10 |
| Settings-only backup | 2.32; it remains schema32 with 786 settings |

Preserve a copy of an old Rack patch before saving over it with a newer build
if you need to return to an older plugin. The included example backups remain
old format9 files to exercise legacy import. See [Backups](BACKUPS.md).

## Validation

Native checks cover all frozen prefix mappings, every Scale Lock selection,
all Loopy pulse counts and both targets, old full formats1–9, old/new selective
files, exact history receipts, queued boundaries, Init, chains, MIDI program
change and the quantizer's actual pitch output. Actual Windows/Linux Rack
callbacks verify save/load, undo/redo, custom Init, legacy import and patch
save/reopen. All four production binaries run the runtime stress host.

This corrects the documented storage boundary. Hardware timing and live
performance behavior still need comparison with a physical module.
