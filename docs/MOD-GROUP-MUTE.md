# MOD group mute — 2.34.0

Hold **Loopy (∞)** and click **MOD** to mute or unmute all eight MOD lanes
as a group. The group now has an independent state: it leaves each lane's
individual mute choice intact. Hold Loopy and click a Stage button to change
that lane's own mute, including while the group is silent.

For example, mute lanes 4 and 6, mute the group, then unmute the group. Lanes
4 and 6 remain muted; the other lanes resume at their current sequence
positions. Group mute does not restart MOD playback, change destinations,
erase lane values or mute the two note tracks.

The [firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf)
describes these performance gestures on p.46 and lists **All Mod Mute State**
in the preset column on p.169, separately from each lane's **Mute State** on
p.170. Earlier plugin versions implemented the group gesture by rewriting
all eight individual bits, losing an existing mix of muted and unmuted lanes.

## Desktop controls and indicators

Right-click **MOD** or **Loopy** and choose **Mute all MOD lanes** to control
the same group state. Each menu change supports one Rack undo/redo action.
Held performance gestures retain their existing pulse synchronization:
they apply on the next base clock pulse while running, or on the next engine
frame while stopped. Two group presses before that pulse cancel each other.
Direct parameter/menu changes follow Rack's usual parameter update path.

In held mute mode, the MOD button indicates the group bit: yellow when
unmuted, blinking red when muted. Stage buttons show individual lane choices,
so you can still see and edit the underlying mask while the group is muted.
HOME's selected MOD lane row is hatched when either layer silences it.
The manual does not explicitly describe combined indicator priority or
restoration of a previously mixed lane mask; these separate-state and
indicator choices need comparison with physical hardware.

Group, individual and preset-chain MOD mutes combine. Removing one layer
does not clear the others. During playback, a muted MOD lane contributes zero
voltage and no MOD clock, including when routed to A/B or MX. MIDI CC follows
that neutral voltage: 64 for bipolar CV, 0 for unipolar/gate CV. Its playback
position continues advancing. Other AUX sources and note-track CV remain independent.

## Saving and compatibility

Group mute is musical preset state. Save/Load, custom Init, chains and MIDI
program changes use it with the individual lane mask. Preset recall undo/redo
restores both layers. Rack patches and portable backups also remember it.
Held gestures and pending performance commands clear on reopen.

The first 786 Rack parameter IDs remain fixed; **All MOD lanes mute** appends
at ID786, for 787 parameters. Settings schema33 introduced the new parameter; schema34 retains it and changes SETUP navigation only.
Musical format11 appends group mute (0/1) at snapshot index521, after the
unchanged 521-value format10 prefix, for 522 values.

Old full musical banks (formats1–10), selective files (formats9–10) and
settings (schemas29–32) import with the missing group bit **Off**. Their
individual lane mutes remain exactly as stored. There is no way to recover a
previous group state from the old eight-bit mask. A presets-only import keeps
the live group state until you recall an imported slot; selective import also
preserves every unrelated slot and chain.

New musical banks and settings exports require **2.34 or later**. Preserve an
old patch copy if you need to return to an older plugin. Included example
backups deliberately remain legacy files. See [Backups](BACKUPS.md) and
[Preset storage](PRESET-STORAGE.md).

## Validation

Native Rack checks cover all 256 individual masks in all three MOD CV types,
real A/B voltages, internal routes, MIDI bytes and reciprocal MX outputs, continuing playback position,
pulse-synchronized gestures, independent edits under group mute, preset
history, Init and old full/selective bank and settings migration.
Actual Rack widget checks cover held gestures, desktop menus, undo/redo,
save/load, older selective import and reopen. The production runtime host
also exercises the appended parameter. Physical hardware comparison remains
outstanding.
