# Clock output — 2.48.0

2.15.0 adds the global CLK output settings from manual pp.120–121.
Press **ALT**, then **CLK** (stage 5). Drag the encoder or the display up/down
to change the selected field. Click the encoder to switch fields:

- **CLK OUT** divides the master clock by any integer from `/1` to `/256`.
- **CLK OFFSET** delays the trigger by `0` to `division−1` master pulses.

With `/4` and offset `2`, output triggers occur on master pulses 2, 6, 10,
and so on, counting the first pulse after reset as zero. Track clock division,
Swing, stage pulse counts, skipping, mutes and MOD lane clocks do not change
this output. Internal and external master clocks use the same rule.

Long-press the encoder to reset both fields to `/1`, offset `0`. That reset
has one Rack undo action. Right-click the display or encoder for numeric entry,
division/offset choices and reset. At `/1`, offset is fixed at zero; the context
menu and long-press reset remain available. Ctrl dragging gives Rack's fine
adjustment. Existing CLK cables keep the same jack ID.

These are **global settings**, saved in the Rack patch and excluded from all
64 internal presets. Preset recall cannot change the selected divider/offset.
Older patches retain `/1`, offset `0`; preset formats 1–5 remain supported.
If a smaller division masks a previously saved offset, the effective value is
clamped to the valid range. Raising the divider restores that stored offset,
so undo and automation do not destroy its previous value. Explicitly editing
offset stores the new value.

## Editing masked offsets

If a smaller division hides a stored offset, an encoder or OLED drag now
undoes to that exact stored offset, including movement below three pixels.
For a small edit, the value action is recorded before the normal click that
switches fields. Field navigation adds no parameter action. At `/1`, the
disabled offset remains untouched; confirmed previews apply once, and
unconfirmed previews do not enter Undo or a saved patch. Clock timing is
unchanged.

## Demo

Open [Clock-output-demo.vcv](../examples/Clock-output-demo.vcv), select your
audio device and press RUN. Core and VCV Free are the only dependencies.
The left voice follows TRK 1 gates. The right voice follows CLK `/4` with
offset `2`; Scope shows that clock and the regular track gates. Try offsets
0–3, then `/3` or `/8`, while the note tracks continue to play.

## Timing choices and comparison limits

CLK sends 5 V triggers lasting approximately 1 ms, rounded to the sample rate.
This preserves the plugin's earlier undivided output. Stop closes any active
trigger; Run/Stop restarts the master count, while chain Run/Pause preserves it.
Global reset and preset/chain transport restarts start the count at zero.
Changing the divider while running applies to the continuing master pulse
count; it does not reset the note tracks or add a synthetic clock.

The manual specifies divided clock positions, but does not fully specify
trigger width or phase during live edits/transport changes. The rules above
are explicit implementation choices, tested in Rack. Physical-hardware
comparison remains outstanding.
