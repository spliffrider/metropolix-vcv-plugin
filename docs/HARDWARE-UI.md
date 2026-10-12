# Hardware panel and desktop interaction — 2.2.0

Open `examples/Hardware-panel-demo.vcv`. Its musical settings and signal connections come from the accepted MOD demo, with a new 34 HP front panel. Select an output device in the AUDIO module to hear its two voices.

This is the first working hardware-layout UI pass. The silver panel, jack arrangement, pitch faders, pulse-count and gate switches, display, encoder, and button positions follow the user-supplied reference. Component artwork and display/navigation fidelity can be refined further.

## Using the controls

- **Drag upward to increase; downward to decrease.** This applies to pitch faders, pulse-count/gate switches, the display's active value, the encoder, numeric buttons such as BPM/LEN/DIV, and values in the optional editors. Integer and toggle settings snap to valid values. Ctrl-drag provides fine adjustment. Release ends one undoable edit.
- **Hover for help.** Controls identify their value and purpose. Ports retain Rack cable behavior and tooltips. Controls for unfinished features explain their status.
- **Right-click for direct access.** Numeric controls provide Rack value entry and reset; named settings such as ORDER offer their choices. TRK 1/2 expose track settings. MOD offers lane selection and its expanded editor. EDIT opens the stage editor; EXIT offers either editor and a close action.
- **Click a settings button** to put its setting on the display. Drag the encoder or the displayed value to change it. Click the encoder/display to switch fields where available: root/scale, stages/offset, track division/pulse-count division, or MOD voltage/transition.
- **Select TRK 1 or TRK 2** before editing note-track settings. Select MOD for modulation and right-click MOD to choose one of its eight lanes.
- **Stage editing:** press EDIT, choose the printed function with a stage button, then select a stage. Drag the encoder or hold and drag a stage button to edit its value. Slide/Skip stage clicks toggle that stage. Right-click a stage button for direct editing and related choices.
- **ALT** enables the secondary stage-button actions. LOAD/SAVE use Rack module presets; A/B select output assignment on the display. EXIT returns to the overview.
- **Optional grids** remain available through menus. They share parameters with the panel and preserve undo/redo. The hardware panel is 34 HP; opening a grid expands the module to 66 HP. Rack moves it to available space if needed.

## Scope of this pass

The tested 2.1.0 musical engine, all 404 parameter IDs, and nine existing patchable ports are retained. Older patches keep their cables and settings even though the physical jack positions have changed. Screen selection is saved with the patch; temporary ALT state clears on load.

AUX X/Y/Z routing, CTRL assignments, Loopy, swing, stage pitch/gate overrides, and MX remain unimplemented. Their hardware controls are represented visually, with explanatory hover text and menus. The AUX sockets are not patchable until their routing is implemented. The new panel does not imply that these engine features have been added.

The display currently provides a position overview and editing pages for implemented parameters. Additional hardware home screens, long-press shortcuts, preset banks, and some secondary functions remain for later passes. Numeric editing and context menus are intentional computer conveniences.

## Verification

The Rack widget test exercises held upward/downward drags, discrete values, numeric entry, menus, hover text, undo/redo, track/lane rebinding, optional editor sizing, and save/load. The hardware demo also runs through the stereo audio and A/B CV recorder. These callback tests complement a physical mouse playtest; they do not certify every operating-system input/focus interaction.
