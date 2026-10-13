# Hardware panel and desktop interaction — 2.7.1

Open `examples/Hardware-panel-demo.vcv`. Its musical settings and signal connections come from the accepted MOD demo, with a new 34 HP front panel. Select an output device in the AUDIO module to hear its two voices.

The panel follows the supplied photograph at 34 HP. This revision refines lettering, component proportions, slider grips, button colors, metal sockets, secondary legends, mounting details, and the logo. The display uses a native 128 × 64 pixel canvas with lettering and diagrams based on the manual's screen images. Further comparison is still needed before calling it an exact recreation.

The bottom credit reads **sensiburner <3 intellijel** at the user's request.
sensiburner uses matching bold lowercase lettering and a red i dot; the red
<3 separates the creator's name from the original hardware maker. Rack's brand
and author metadata also identify sensiburner. This is an independent homage.

## Using the controls

- **Drag upward to increase; downward to decrease.** This applies to pitch faders, pulse-count/gate switches, the display's active value, the encoder, numeric buttons such as BPM/LEN/DIV, and values in the optional editors. Integer and toggle settings snap to valid values. Ctrl-drag provides fine adjustment. Release ends one undoable edit.
- **Hover for help.** Controls identify their value and purpose. Ports retain Rack cable behavior and tooltips. Controls for unfinished features explain their status.
- **Read values while dragging.** A compact popup follows the edited parameter on the panel, display, encoder and optional grids. It stays visible if the pointer leaves the control while held. Rack's Tooltips preference controls both hover help and adjustment readouts.
- **Gentler sliders.** At Rack's default linear sensitivity, pitch takes approximately 400 mouse pixels across its range; pulse-count and gate switches take about 24 pixels per step. Rack's global sensitivity and fine-adjustment modifiers still apply. Pitch-slider lights follow **View → Bloom** and remain illuminated when room brightness is reduced.
- **Right-click for direct access.** Numeric controls provide Rack value entry and reset; named settings such as ORDER offer their choices. TRK 1/2 expose track settings. MOD offers lane selection and its expanded editor. EDIT opens the stage editor; EXIT offers either editor and a close action.
- **HOME:** turn the encoder with a vertical drag, or click the encoder/display, to browse four views: stage positions, base notes, panel values, and CV/MOD activity. EXIT returns to HOME. The note view excludes accumulation and modulation from its stage notes; the pre/post readouts show the corresponding live routes.
- **Click a settings button** to put its setting on the display. Drag the encoder or display to change its active value. Click the encoder/display to switch root/scale, stages/pulses, or clock/pulse-count division. On LEN, ALT selects offset; on SCALE, ALT switches between scale and root.
- **Encoder presses** toggle Forward/Reverse on ORDER, Stage/Pulse on probability, Stage/Pulse/Ratchet on accumulation, and Step/Ramp on a MOD stage. Turning still edits the corresponding main value. These changes support Rack undo.
- **BPM:** the first click opens the tempo page. Subsequent BPM clicks tap an internal tempo. External clock shows measured tempo after two edges; the existing input convention remains one edge per sixteenth note.
- **Select TRK 1 or TRK 2** before editing note-track settings. Select MOD for modulation; MOD → EDIT → a stage button chooses that MOD lane. Right-click MOD also provides direct lane selection.
- **Stage editing:** press EDIT, choose the printed function with a stage button, then select a stage. Drag the encoder or hold and drag a stage button to edit its value. Slide/Skip stage clicks toggle that stage. Right-click a stage button for direct editing and related choices.
- **ALT** enables the secondary stage-button actions. LOAD/SAVE open the internal 8 × 8 preset banks ([guide](PRESETS.md)); A/B select output assignment on the display. EXIT returns to the overview.
- **Optional grids** remain available through menus. They share parameters with the panel and preserve undo/redo. The hardware panel is 34 HP; opening a grid expands the module to 66 HP. Rack moves it to available space if needed.

## Accumulator modes

Select **TRK 1** or **TRK 2**, click **ALT**, then **LIM** (stage button 7).
Click the encoder twice to select **Mode**, then drag upward for **Track** or
downward for **Stage**. The display itself is also draggable; right-click the
active value for named choices. Alternatively, right-click a track button and
choose **Accumulator mode**. Each track has its own mode and Rack saves it.

Encoder clicks cycle Positive Limit, Negative Limit, Mode, Order, Polarity,
and Reset. The display scrolls between two groups of three settings. These
edits support undo. EXIT returns to HOME. Accumulator *mode* is different from
the per-stage Stage/Pulse/Ratchet *trigger* edited through EDIT → ACCUM.

`examples/Accumulator-modes.vcv` compares the manual's four-note C-major
example: Track 1 uses Stage mode and Track 2 uses Track mode. Only stage 1 has
an increment (+1). With Stage mode, only that note changes each pass; Track
mode shifts the entire phrase. Limits are +3/-3 for a short repeating test.
The two Scopes show each track's pitch/gate. Select an AUDIO device to hear it.

Try skipping stage 1 before RESET: neither track should accumulate. Unskip it
and reset, let the phrase rise, then skip it during playback: Track mode holds
the transposition already reached. Try moving the +1 to a different stage,
changing pulse counts, or choosing a generative order. These are comparison
exercises, not claims of hardware-verified behavior. Please include the patch,
mode, order, skip positions, and a hardware recording when reporting a mismatch.

Mode edits take effect on the next track-clock pulse. The two modes retain
separate runtime counters until reset; returning to a mode resumes its stored
state. Loading a patch restarts both. Older patches default to Stage mode.
The manual does not fully specify live switching or multiple accumulator
sources; see [the documented interpretations](VALIDATION.md).

## Compatibility and remaining controls

All 415 previous parameter IDs and all ports retain their identities. Six CTRL
parameters append at IDs 415–420. Schema 9 saves the CTRL page and selected
knob. Older patches and presets clear CTRL assignments, preserving their
existing panel values; previous AUX/mode migrations are unchanged. Temporary
ALT state clears on load.

AUX X/Y/Z now accept cables. Their knobs attenuvert; their black buttons open destination/track settings, with a live CV or gate meter on the display. Right-click either for named routing choices; encoder clicks switch fields. See the [AUX guide](AUX-INPUTS.md). Both CTRL knobs now work; see the [CTRL guide](CTRL-KNOBS.md). Loopy, swing, stage pitch/gate overrides, MX, and ALT+AUX selective randomization remain unimplemented, with explanatory help.

All four HOME views and editing pages for current parameters are represented. The display shows filled/current stages, thick starting stages, skipped stages, inactive dots, muted hatching, the scale keyboard, ratchet stacks, probability/accumulator bars, and CV/MOD graphs. Selection corners pulse and the selected numeric stage button flashes. MOD footers show A/B assignment where present.

Some visible readouts describe fixed behavior: slide type is Tempo, gate scale is 100%, and external clock division is /1. CTRL dials in the panel-value HOME view show the live knob positions. MOD values remain expressed in volts, including internal destinations; destination-specific units are a later refinement. A/B selection directly chooses one of the existing track-CV/MOD sources. Hardware long-press shortcuts, preset chains, and the remaining secondary functions are still incomplete. Numeric editing and context menus are intentional computer conveniences.

## Verification

The Rack widget test exercises held upward/downward drags, discrete values, numeric entry, menus, hover text, undo/redo, track/lane rebinding, optional editor sizing, and save/load. The separate display test exercises HOME/LIM navigation and renders 28 screen states for inspection. The hardware and accumulator demos also run through the stereo audio recorder. These callback tests complement a physical mouse playtest; they do not certify every operating-system input/focus interaction.
