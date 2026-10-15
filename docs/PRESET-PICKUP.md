# Preset Thru pickup — 2.68.0

SETUP → Presets Setup offers **Pitch Load: Off / Thru / Jump** and
**Ctrl Load: Off / Assigns / Thru / Jump**. The same named choices are in the
preset context menu. Defaults remain Pitch Off and CTRL Assigns.

Thru loads the saved sounding value while leaving its virtual physical control
where it was. Movement toward the remembered position does not change that
value. Reaching or crossing it gives the control back to the slider/knob.
Each of the eight pitch sliders and two CTRL knobs picks up independently.
Another preset, Init or chain link replaces its remembered targets. Jump moves
the virtual control to the recalled value and gives immediate control.

Hover tells you the remembered value and the direction to move. The pitch slot
shows a small target marker; HOME's control overview shows override dots.
Right-click a waiting control and choose **Take over at current physical
position** to apply its current position immediately. Undo/redo preserves both
physical position and pickup state, including the takeover action.

Native numeric entry and reset move the physical control and follow its pickup
policy. Undo/Redo preserves both the physical position and recalled target for
numeric entry, Initialize, double-click reset, wheel edits and CTRL's Centre
knob action. A crossing-and-return wheel gesture retains its pickup-state
change as one history action. Zero/clamped scrolling adds none, outside edits
survive leaving the control, and patch reset/restore discards pending scrolling.

The temporary pitch popup shows the physical slider selection while HOME2
retains the remembered base-note overview, including fixed Bottom Pitch and
per-stage override dots. Neither includes temporary
modulation or accumulation. See [display behavior](UI-FIDELITY.md).

Use explicit takeover when you want to apply a value short of the saved
position. Setting Off releases the saved override; changing to Jump lets the
next movement take over without requiring a crossing.

Saving an internal pattern stores the effective sounding pitches and CTRL
positions. Saving the Rack patch retains both physical positions and remembered
targets. Older patches retain their existing Jump behavior and all parameter IDs
remain stable. See [Preset storage](PRESET-STORAGE.md) for current bank formats
and legacy import.

Open [Pickup-performance.vcv](../examples/Pickup-performance.vcv), select AUDIO
and press RUN. The sliders begin low while the memorized melody sounds. Move one
slider up toward its marker; then try CTRL pickup, undo, saving and reopening.
It uses free VCV/Core dependencies and starts with no audio device selected.

Based on manual pp. 54–55. This is pickup for the virtual panel controls;
external MIDI controller takeover and physical hardware comparison are separate.
