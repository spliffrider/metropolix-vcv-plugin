# Portable backups — 2.26.0

Open **SETUP → MIDI → Backup files…**, or use the module's right-click menu.
Drag the encoder or display up/down to select an action. Click the encoder to
open the computer's file chooser; the same actions are available by right-click.

| Action | Saved contents |
| --- | --- |
| Export All | Current settings/state, all 64 preset slots, the working chain and eight saved chains |
| Export Settings | All 770 parameter settings, custom scales/shortcuts, global preferences, MIDI assignments, MX assignments, panel pickup targets and screen selection |
| Export Presets | All 64 slots, including empty slots, the working chain and eight saved chains |
| Import Backup | Restore the scope recorded in the selected file |

Files use readable JSON with `type: MetropolixLabBackup`, `format: 1`, and a
scope of `all`, `settings` or `presets`. Keep the complete file. Sharing an All
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
RUN/RESET. Preset imports replace all slots and chains in that scope.

Pitch/CTRL physical positions and remembered pickup targets are restored
together. Signed CV settings and dormant clock offsets retain their stored
values, even when the current editing mode displays a constrained value.

Cancelling a file chooser changes no sound, bank or history. Unsupported
versions, missing fields, duplicate JSON keys, invalid values, malformed
chains and files over 4 MiB are rejected before any settings change. A failed
save preserves an existing destination file; a successful save replaces it
using a fully written temporary file in the same directory.

The hardware manual's **Send All / Send Settings-State / Send All Presets**
scopes inspired these export options ([firmware 1.6 manual, p.183](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf)).
Chains are included as a desktop convenience. These files are **Metropolix Lab
JSON**, and cannot be transferred to the hardware's Receive Any screen. The
manual does not specify a hardware SysEx packet format; hardware SysEx
interchange remains unimplemented.
