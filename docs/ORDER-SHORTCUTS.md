# ORDER shortcuts — 2.32.0

The eight shortcuts remember **Play Order and Direction together**. They are
global settings shared by both note tracks and all eight MOD lanes. Recalling
a musical preset leaves these shortcut definitions unchanged.

Select TRK 1, TRK 2 or a MOD lane, then hold **ORDER**. All eight stage buttons
light white. A short stage-button press recalls that shortcut into the selected
track/lane. Hold a stage button for more than one second to save the current
pair; the display shows `SHORTCUT N SAVED`. Release ORDER to finish the gesture.
This follows the [firmware 1.6 manual, p.74 and p.169](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).

On the ORDER screen, press the encoder to switch Direction. Hold the encoder
for more than one second to reset **both** fields to Linear / Forward.
Dragging ORDER, the encoder or the value field up/down still edits the selected
value with Rack's ordinary adjustment readout.

## Desktop controls and undo

Right-click ORDER, the encoder or the display for Direction, paired reset and
the **Order shortcuts (global)** submenu. On the ORDER screen, right-click a
stage button to recall or save its shortcut directly. The labels show the saved
order and direction, including Random/Brownian variants.

Each desktop save, recall and paired reset produces one Rack undo action when
values change. Multiple recalls during one held-ORDER gesture share one undo
action, including when you switch tracks or MOD lanes before releasing ORDER.
Undo restores both fields; unrelated CTRL-driven parameter changes are excluded.

A pair recall/reset is blocked if either field belongs to CTRL 1/2. Its menu
item is disabled, and a held recall displays `ORDER OWNED BY CTRL`; Direction
is not partially applied. Saving the current pair remains available. Clear or
change the CTRL assignment to edit that target directly again.

## Storage and older files

The default shortcut pairs are software choices: Linear / Forward, Linear /
Reverse, Ping Pong, Pendulum, Odd / Even, Converge, Shuffle and Random (the last
six use Forward direction). The manual does not list all
hardware factory shortcut pairs, so these are not a hardware-default claim.

Rack files and All/Settings backups save all sixteen shortcut values. Older
settings schemas29–31 with 770 parameters still import and initialize the new
shortcuts to plugin defaults. Schema32 retained 786 parameters; schema33 added MOD group mute at ID786. Schema34 keeps these IDs and migrates SETUP navigation.
Current settings backups require 2.34 or later. Existing parameter IDs are unchanged.
As of 2.34, musical presets use format11 with 522 values; the global ORDER
shortcuts remain excluded. Older banks still import; new musical banks require
2.34. See [Preset storage](PRESET-STORAGE.md) and [Backups](BACKUPS.md).

Native tests cover all 3,360 target/slot/order/direction combinations, CTRL
ownership, preset exclusion and old/new backup validation. Actual Rack callback
tests cover held and desktop gestures, undo/redo, white button illumination and
save/reopen. Windows and Linux render four additional ORDER display states.
These checks do not establish hardware timing or native OS mouse/focus behavior.
