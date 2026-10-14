# Portable backups — 2.34.0

Open **SETUP → MIDI → Backup files…**, or use the module's right-click menu.
Drag the encoder or display up/down to select an action. Click the encoder to
open the computer's file chooser. **Export Presets** first opens a selection
submenu; EXIT returns to Backup files. Right-click offers the same choices.

| Action | Saved contents |
| --- | --- |
| Export All | Current settings/state, all 64 preset slots, the working chain and eight saved chains |
| Export Settings | All 787 parameter settings, custom scales/shortcuts, global preferences, MIDI assignments, MX assignments, panel pickup targets and screen selection |
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

## Selective file format and compatibility

Format 2 contains exactly `type`, `format`, `scope`, `selection` and `bank`.
`scope` is `presets`. `selection` is either `{"kind":"filled"}` or
`{"kind":"bank","bank":8}` (public bank numbers 1–8). `bank` contains
`presetFormat: 11` and a `slots` array. Each entry has an absolute `slot` index
0–63 and `values`, the 522-value musical snapshot. Legacy format9/518 files
gain Unlocked/Auto/Current Track for the three lock/Loopy fields; formats9–10
gain group mute Off. Individual lane masks remain unchanged. A Bank entry may
use `null` for an empty slot; Filled entries must contain a snapshot.
Duplicate IDs, foreign bank IDs, missing bank slots, wrong snapshot sizes and
out-of-range values are rejected before import. The files contain no chains.

Use **2.55.0 or later** for new **All** or **Settings** backups: schema34
stores the expanded SETUP cursor and its direct child parent. **Presets-only**,
**Filled** and **Bank** files still need **2.34.0 or later** for format11 musical
banks. Older format9–10
selective files still import. See [Preset storage](PRESET-STORAGE.md) and
[MOD group mute](MOD-GROUP-MUTE.md). Existing 2.26–2.29 All/Settings/All Presets backups
continue to import, including settings schemas29–31 with 770 parameters.
Schema32 settings retain their released 786-parameter count; schema33
added the independent group mute, for 787. Schema34 retains these IDs and migrates old SETUP positions without
changing musical values. Schemas29–31 initialize missing ORDER
shortcuts to defaults, and schemas29–32 initialize missing group mute Off while
preserving individual lane masks. The first 521 musical snapshot positions and
all earlier parameter IDs remain fixed. Format10 added Scale Lock and both
Loopy settings; format11 appends group mute. Formats1–10 full banks and
formats9–10 selective files still import. The
[example Bank 8 backup](../examples/Backup-bank-8.json) intentionally replaces
Bank 8 with three occupied slots and five empty slots; try it in a spare patch.

The hardware manual's **Send All / Send Settings-State / Send Presets →
All, Filled, 1–8** scopes inspired these export options
([firmware 1.6 manual, p.53](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf)).
Chains are included as a desktop convenience. These files are **Metropolix Lab
JSON**, and cannot be transferred to the hardware's Receive Any screen. The
manual does not specify a hardware SysEx packet format; hardware SysEx
interchange remains unimplemented.
