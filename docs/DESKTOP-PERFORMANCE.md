# Desktop performance — 2.71.0

The optional Performance view brings stage playing, loop ranges and mutes
together beside the hardware panel. It uses the existing Loopy/Stage Player
engine. No MIDI controller is required.

Hover over this module and press **Shift+P**, or right-click the module and
choose **Desktop performance view**. The hardware panel stays at 34 HP; the
view uses the same 32 HP extension as the stage and MOD editors. An adjacent
MX moves to the new right edge. **EDIT** and **MOD** open the value editors;
**CLOSE** collapses the extension. Shift+P returns to Performance.

## Play with the mouse

- Select **TRACK 1** or **TRACK 2**. **TARGET** chooses that track or both tracks.
- With RUN on, **hold a stage pad** for a temporary loop. Release a single pad
  to resume the original sequence at its underlying position.
- **Drag from one stage to another, then release** to latch that range. You can
  drag either direction. The highlighted range and **LATCHED** status show what
  is active. Direction still comes from each track's settings.
- **RELEASE** returns to the full sequence. **LATCH/UNLATCH** also controls an
  active loop. Drag the **PULSES** value up/down, or right-click it for numeric
  entry, to choose Auto or a fixed loop length.
- With RUN off, **hold a pad to audition** its frozen pitch, gate and stage CV.
  Release ends the audition. Stopped pad drags remain momentary.

Auto and fixed pulse lengths, background continuation, gate types and
accumulator behavior follow the [Loopy guide](LOOPY.md).

## Play with the computer keyboard

First click **CLICK TO ARM KEYBOARD**. Its lit state makes ownership explicit.
The keys work while this Performance view retains keyboard focus.

| Key | Action |
| --- | --- |
| Number row **1–8**, or numpad **1–8** | Hold the corresponding stage |
| Two stage keys together | Hold the two boundaries of a loop range |
| **L** | Latch the held running loop; press again to release |
| **Space** | Start/stop RUN |
| **T** | Select the other note track |
| **M** | Mute/unmute the selected note track |
| **G** | Mute/unmute the MOD group |
| **Escape** | Release the loop/audition and disarm the keyboard |

Keyboard auto-repeat does not retrigger audition. Number-row and numpad keys
can be mixed. Use the keys without Ctrl/Alt/Shift/Cmd; Rack's modified shortcuts
remain available. Starting a modified shortcut disarms first, so native file
dialogs cannot retain an audition. Releases also work if modifiers change mid-hold.

Clicking another control, opening a numeric entry, switching editors or losing
window focus disarms and releases temporary stages. Re-arm explicitly to play
again. Intentional running latches remain until released or stopped. Keyboard
arming and held stages are never saved; opening a patch cannot hold an audition
gate. The selected desktop view itself is saved.

## Mutes and playback feedback

Each note track has a visible mute button; the eight MOD lane buttons and
**ALL MOD** provide individual and group mutes. The group switch preserves
individual choices. While running, changes wait for a base-clock pulse; a `*`
and **MUTE CHANGE QUEUED** show a pending request. While stopped they apply
immediately. Performance mutes retain the original gesture's non-undoable
behavior; normal parameter menus still provide undoable edits.

Each track card shows its current stage, pulse, pitch voltage, gate/mute state
and active accumulator offset in scale degrees. The pad row shows the requested
range and live track position; the status distinguishes held, latched and
stopped audition. Clock and accumulator processing keep using the existing
sequencer engine. These readouts describe logical tracks before output swap.

## Validation

The targeted Rack test exercises key chords, repeats, modified releases, text
focus, lost release recovery, mouse ranges in both directions, latch/release,
clock-queued mutes, group preservation, editor switching, reset and actual patch
save/reopen. It checks real pitch/gate outputs during stopped keyboard audition.
Run `scripts/test-performance.ps1` on Windows or the Linux UI runner's
`--mode Performance`. Platform/OS input differences and long performance
sessions still need user testing.

[Play the desktop opening patch](../examples/Pulse-Pressure-Desktop.vcv).
It uses the same dependencies and opening voices as the
[Pulse Pressure sidechain patch](SIDECHAIN-DEMO.md).
