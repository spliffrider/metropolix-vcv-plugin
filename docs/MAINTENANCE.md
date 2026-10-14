# Maintenance — 2.30.0

Open **SETUP → Version / Factory**, or right-click SETUP and use its
**Version / Factory** submenu. The header shows the current **Lab software
version**, independently of the hardware firmware. Drag the encoder/display
to select an action and click to open confirmation.

Confirmation always starts on **Cancel**. Click to cancel, or drag upward once
to select **Apply** and click. EXIT also cancels. Opening a menu, cancelling,
or closing a patch while confirmation is visible never performs the action.
A saved patch reopens at the maintenance menu with confirmation disarmed.

| Action | Changes | Preserves |
| --- | --- | --- |
| Factory Reset | All 787 settings return to plugin defaults, including stage/track/MOD parameters, routing, custom scale definitions, shortcuts, pickup and global preferences | All 64 saved musical presets, working chain links and eight saved chains |
| Clear All Presets | Empties all 64 stored slots | Current live parameter settings, custom scales, shortcuts, pickup, working chain links and eight saved chains |

Both actions stop RUN, reset playback position, cancel pending preset loads
and disable chain playback. They preserve local MIDI devices, module identity,
cables, neighbours and optional editor size. An already high external Run gate
cannot restart the module until it goes low/high or RUN is explicitly pressed.
Neither action overwrites another module in your patch.

Each action creates **one Rack Undo** entry. Undo restores the entire previous
bank/settings and saved RUN state; Redo restores the stopped result. Playback
phase itself is reset, as when loading a patch. Export a backup before a
maintenance action if you want a separate copy beyond Rack's history.

Factory Reset uses this plugin's defaults, including the software-specific
Run on Boot “Recall Rack patch” and legacy clock-output choice. It does not
claim to reproduce physical slider positions or every hardware factory default.
Saved chain preservation and disabling chain playback are explicit desktop
behavior; those maintenance details are not specified by the hardware manual.

**Init Preset** is different: it initializes musical settings through the
configured preset-loading policies and retains global preferences. See
[Editing workflows](EDITING-WORKFLOWS.md). Factory Reset resets global settings
and custom shortcuts while keeping stored musical presets, as described in the
[firmware 1.6 manual, pp.59 and 189](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
The hardware's power-on, EEPROM tests and physical calibration have no virtual
equivalent here.

Schema31 remembers the maintenance menu selection. Confirmation choice is
temporary and is never serialized. Parameter IDs and format9/518-value musical
presets remain unchanged. Full/settings backups from schema29 or30 still import;
current settings backups need 2.30.0 or later, while selective format2 backups
remain compatible with 2.29.0.
