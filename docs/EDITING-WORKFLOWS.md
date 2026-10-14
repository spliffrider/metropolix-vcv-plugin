# Editing workflows — 2.21.0

These operations follow the firmware 1.6 manual, pp. 51–57, 156, 164–165 and
179. The clipboard and randomization baseline belong to this module instance
and clear when the Rack patch is reopened. Normal Rack undo/redo records each
operation. Parameters currently controlled by CTRL remain under that knob.

## Randomize, morph and restore

Open a track setting such as ORDER, LEN, DIV, SWING, SLIDE, GATE or LIM, or an
EDIT stage-function/MOD screen. Click ALT, then:

- AUX X randomizes the parameters on that screen within their allowed ranges.
- AUX Y morphs them by a smaller amount. Toggles and small option lists have a
  20% change probability; larger ranges move by one unit through 10% of range.
- AUX Z restores those parameters to their values when ALT was entered.

ALT stays active so you can compare several variations with the original. Click
ALT again to leave. Entering ALT again captures a new baseline. Right-click
the display, encoder or stage button for the same operations.

LEN randomization changes Stages and Offset, **never Pulses**. Copy/paste does
include Pulses. A stage screen affects all eight values of the selected track
or lane, including the applicable probability target, accumulator trigger or
CV transition. Other tracks and lanes retain their values. Setup and HOME
screens are not randomization targets.

Full discrete randomization gives each option the same probability. Gate CV
uses Off/On rather than intermediate voltages. Continuous CV morph uses a
1 mV minimum step. The manual distinguishes lists below and above ten options;
this implementation treats exactly ten as a small list. Changes clamp at range
boundaries and may therefore remain unchanged there. Random sequences need
not match the hardware's generator.

## Copy and paste

- On a stage-function screen, hold its source stage button and click one or
  more destination stages. This copies that function's value and its associated
  option, such as probability Stage/Pulse or CV transition. Releasing a Slide,
  Skip or Gate CV source after copying does not toggle it.
- On a track setting screen, hold the source TRK button and click the other
  TRK button. This copies every field of the current screen.
- On ALT → TRK, the same gesture copies the complete hardware track menu,
  including Global Reset and Velocity. It does not initialize the destination.
- In MOD editing, hold EDIT and the source lane's stage button, then click
  another lane's stage button. This copies the entire lane: values, transitions,
  traversal, timing, voltage type, destination, assignment and mute.

Right-click controls also provide explicit Copy and Paste actions. Stage
clipboards require the same function; track-screen clipboards require the same
screen. Whole-lane and whole-menu clipboards are separate types. Each paste is
one Rack undo action, and the source remains unchanged.

## Init preset and AUX clear

SETUP → Init Preset recalls initialized musical settings. SETUP → Preset
Settings → Set Init Preset chooses Off or one of the 64 bank slots. Off uses
the initialized pattern; an empty chosen slot falls back to it. A saved slot
lets you supply your own starting pattern. The global preference is also
available in the preset context menu.

Init follows the existing Pitch/Switch/CTRL/AUX recall policies and retains
global clock, tuner and setup preferences. As of 2.33, Scale Lock and both Loopy
settings follow the initialized or custom musical preset. It disables a running preset
chain before initializing, so the chain cannot immediately overwrite the
result. Pattern initialization is undoable; chain transport remains disabled.

On an AUX assignment screen, hold the encoder for more than one second to
clear destination and assignment together. The attenuverter remains at its
current position. Undo restores both fields in one action.

## Try it

Open [Editing-workflows-performance.vcv](../examples/Editing-workflows-performance.vcv),
choose your AUDIO device and press RUN. EDIT shows TRK 1 probability; lower
probabilities are useful for audible comparisons. Try ALT → AUX Y several
times, compare with AUX Z, then copy a stage while holding its source. Test
undo and save/reopen. Native desktop mouse/focus behavior and physical-module
comparisons remain tester checks.
