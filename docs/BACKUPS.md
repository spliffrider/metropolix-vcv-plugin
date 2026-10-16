# Portable backups — 2.58.0

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

Open **SETUP → Backup Files**, or use the module's right-click menu. The
desktop MIDI Setup shortcut also retains its Backup files entry.
Drag the encoder or display up/down to select an action. Click the encoder to
open the computer's file chooser. **Export Presets** first opens a selection
submenu; EXIT returns to Backup files. Right-click offers the same choices.

| Action | Saved contents |
| --- | --- |
| Export All | Current settings/state, all 64 preset slots, the working chain and eight saved chains |
| Export Settings | All789 parameter settings, custom scales/shortcuts, global preferences, MIDI assignments, MX assignments, panel pickup targets and screen selection |
| Export Presets → All Presets | All 64 slots, including empty slots, the working chain and eight saved chains |
| Export Presets → Filled Presets | Only occupied slots, keeping their original bank/slot addresses; no chains or live settings |
| Export Presets → Bank 1–8 | Exactly eight slots in the selected bank, including explicit empty slots; no chains or live settings |
| Import Backup | Restore the scope recorded in the selected file |

Files use readable JSON with `type: MetropolixLabBackup`. All, Settings and All
Presets retain `format: 1` and a scope of `all`, `settings` or `presets`. Filled
and Bank selections use `format: 2`, described below. Keep the complete file. Sharing an All
backup lets another tester reproduce your module settings and musical bank.
They still need a Rack patch to supply oscillators, envelopes and cables.
An [example All backup](../examples/Backup-example.json) is included for trying
the restore workflow in a spare test patch.

An import **stops RUN** and resets sequence position. Press RUN or use your
configured external transport to resume. An already high external Run/Run-C
gate stays inhibited until it goes low and high again, or RUN is explicitly
pressed. Rack **Undo** restores the previous settings, bank and RUN state;
**Redo** restores the imported stopped state.
Playback phase is not part of a backup or undo receipt. As with loading a Rack
patch, playback starts from the reset sequence position.

MIDI input/output **device selections stay local**. Channel, CC, clock and
transport assignments are included in Settings. Module identity, cable
connections, bypass state, neighbours, and optional editor size stay as they
are in the receiving patch. Settings-only import keeps its existing bank;
presets-only import keeps its current parameter settings, apart from stopping
RUN/RESET. **All Presets replaces every slot and chain**, including empties.
**Filled merges only the occupied slots listed in the file**. **Bank replaces
exactly its eight slots**, so empty source slots clear the corresponding target
slots. Both selective imports preserve other banks and all chains.

Slots retain their addresses: importing Bank 8 affects slots 8-1 through 8-8;
it does not copy them into the currently selected bank. Filled is useful for
adding a sparse shared collection. Bank is useful for replacing a complete
bank. A file with no filled slots changes no presets but still stops playback
like every import. Merge/replacement semantics are explicit desktop behavior;
they have not been verified against a hardware SysEx receiver.

Pitch/CTRL physical positions and remembered pickup targets are restored
together. Signed CV settings and dormant clock offsets retain their stored
values, even when the current editing mode displays a constrained value.

Cancelling a file chooser changes no sound, bank or history. Unsupported
versions, missing fields, duplicate JSON keys, invalid values, malformed
chains and files over 4 MiB are rejected before any settings change. A failed
save preserves an existing destination file; a successful save replaces it
using a fully written temporary file in the same directory.

## Compact lists — 2.58.0

Backup Files and Preset Export show four mixed-case actions with an inverted
selection bar. The first four export selections stay together; later banks
scroll into view. The title stays at the top. Success, failure and imported/
stopped notices occupy a separate bottom line and expire back to the page's
normal hint. Navigation, native file choosers and right-click choices retain
their existing behavior. EXIT returns from selection to Backup Files, then
to the SETUP or desktop MIDI parent from which it was opened.

These are this plugin's JSON files. The hardware manual's MIDI SysEx options
are a reference for scope; hardware SysEx interchange is not implemented here.
The display change does not alter schema 34, 787 parameter IDs, format 11/522
musical snapshots, backup validation, file scope or Undo/Redo.

## Selective file format and compatibility

Format 2 contains exactly `type`, `format`, `scope`, `selection` and `bank`.
`scope` is `presets`. `selection` is either `{"kind":"filled"}` or
`{"kind":"bank","bank":8}` (public bank numbers 1–8). `bank` contains
`presetFormat: 11` and a `slots` array. Each entry has an absolute `slot` index
0–63 and `values`, the 522-value musical snapshot. Legacy format 9/518 files
gain Unlocked/Auto/Current Track for the three lock/Loopy fields; formats9–10
gain group mute Off. Individual lane masks remain unchanged. A Bank entry may
use `null` for an empty slot; Filled entries must contain a snapshot.
Duplicate IDs, foreign bank IDs, missing bank slots, wrong snapshot sizes and
out-of-range values are rejected before import. The files contain no chains.

Historical schema 32 settings have 786 parameters and schema 33/34 have 787.
Schemas29–31 initialize missing ORDER shortcuts; schemas29–32 initialize group
mute Off while preserving individual masks. Format 10 added Scale Lock/Loopy;
format 11 added group mute. Current compatibility is described above. The
[example Bank 8 backup](../examples/Backup-bank-8.json) intentionally replaces
Bank 8 with three occupied slots and five empty slots; try it in a spare patch.

The hardware manual's **Send All / Send Settings-State / Send Presets →
All, Filled, 1–8** scopes inspired these export options
([firmware 1.6 manual, p.53](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf)).
Chains are included as a desktop convenience. These files are **Metropolix Lab
JSON**, and cannot be transferred to the hardware's Receive Any screen. The
manual does not specify a hardware SysEx packet format; hardware SysEx
interchange remains unimplemented.
