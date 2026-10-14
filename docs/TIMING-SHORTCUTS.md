# LEN and DIV shortcuts — 2.35.0

These panel gestures follow printed pages 76 and 78 of the
[firmware 1.6 manual](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).
Select TRK 1, TRK 2 or a MOD lane, then open LEN or DIV. Click the encoder to
choose a field. Pressing the same panel button again preserves that field.

| Screen and selected field | Hold the panel button, then press stage 1–8 |
| --- | --- |
| LEN / Stages | 1, 2, 3, 4, 5, 6, 7, 8 stages |
| LEN / Pulses | 8, 16, 24, 32, 40, 48, 56, 64 pulses |
| DIV / Clock | /1, /2, /3, /4, /6, /8, /16, /32 |
| DIV / Pulse Count, note tracks | /1, /2, /3, /4, /5, /6, /7, /8 |

MOD lanes have clock division only. DIV shortcuts differ from the BPM screen's
external input clock shortcuts. Individual values between the shortcuts remain
available through dragging, the encoder and Rack's numeric entry.

On LEN, ALT + encoder edits Stage Offset. The LEN button begins shortcuts for
the selected Stages/Pulses field and clears ALT. Right-click LEN, DIV, the
encoder or the display for field selection, named shortcuts and reset. A stage
button's menu on either screen offers that stage's shortcut directly. The
matching stage button lights while the panel button is held; its hover text
describes the value. The manual does not specify shortcut LED priority here,
so this indicator still needs physical-module comparison.

Hold the encoder for more than one second to reset the selected target's whole
screen. LEN resets Stages to 8, Offset to 0 and Pulses to Auto. DIV resets clock
division and, on note tracks, pulse-count division to /1. CTRL-owned fields
remain controlled; other fields can still reset.

Each held panel gesture records one Rack undo action, including shortcuts on
multiple targets before release. Stage clicks do not create duplicate actions
or edit the stage's underlying values. Long encoder reset and menu operations
also support Undo/Redo. Group mute and CTRL-owned timing fields remain outside
the gesture's undo receipt. EXIT cancels held timing and returns HOME.

Explicit shortcuts and resets cancel pending encoder edits and apply directly.
Encoder confirmation still works for ordinary encoder dragging. Up/down panel
dragging edits the selected field; Ctrl provides Rack's fine adjustment, and
right-click retains native numeric entry. These are desktop additions to the
hardware workflow.

## Compatibility and checks

This change adds no parameter IDs or musical fields: the existing 787
parameters, settings schema33, 522-value format11 musical bank and backup
formats remain unchanged from 2.34. Held UI state is transient and clears on
reset and reopen; selected screen/field and timing values persist in a patch.

The dedicated Windows/Linux Rack callback checks exercise all stage shortcuts
and all ten targets, whole-screen resets, CTRL ownership, grouped undo, menu
entry, confirmation cancellation and save/reopen. Existing production and
engine checks cover the timing parameters' signal behavior. This is automated
callback coverage; OS mouse/focus playtests and comparison with a physical
Metropolix remain useful.
