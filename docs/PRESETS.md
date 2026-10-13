# Internal presets — 2.7.0

Metropolix Lab now holds **64 patterns in eight banks of eight**. Each module
instance owns its banks. They are saved inside your Rack patch and native Rack
module presets; **save your Rack patch to keep bank edits**. There is no separate
global bank file and no hardware SysEx compatibility.

## Panel operation

1. Press **ALT**, then **SAVE** (stage button 2).
2. Drag the encoder or OLED upward/downward to select a bank. The banks are
   Red, Orange, Yellow, Green, Cyan, Blue, Magenta, and Pink.
3. Click the encoder to cycle **Save + Exit**, **Save**, and **Clear**.
4. Click a stage button to choose its slot. Filled slots light up in the bank
   colour. Overwriting or clearing a filled slot asks for confirmation.
5. Press **ALT → LOAD** (stage button 1). Choose a bank with the encoder and a
   slot with a stage button. Click the encoder to change the load delay.

The last loaded slot flashes slowly; a queued slot flashes faster. The LOAD
screen identifies a queued bank/slot. Click the queued slot again, click an
empty slot, or use **Cancel queued load** to cancel. Selecting another filled
slot replaces the pending choice. Save and Clear are blocked while a load is
queued. EXIT leaves the screen without cancelling the queue.

Right-click the encoder/OLED on these pages, a preset slot, SETUP, or the module
background for preset commands and settings. Rack's native preset file menu
remains available in the module background menu. Saving, clearing, and completed
loads support Rack undo/redo. Undoing a load restores the parameters it changed
and restarts the sequence; it does not rewind playback time.

## What loads

Track settings, stage edits, MOD lanes, root/scale and **Total Pulses** always
load. The following choices are global to this module instance:

| Setting | Default | Choices |
| --- | --- | --- |
| Pitch Load | Off | Keep live pitch sliders, or Jump to saved positions |
| Switch Load | Off | Keep pulse/gate switches, or Jump to saved positions |
| Ctrl Load | Assigns | Off; saved destinations/track assignments with current knob positions; or Jump including saved positions |
| Aux Load | Assigns | Keep current routes, or recall destinations/track assignments |

Choose **Recall complete pattern** in the preset menu to enable Pitch Jump,
Switch Jump, CTRL Jump and AUX Assigns together. The demonstration patch uses
these settings. This option has one undo step.

AUX attenuverter positions, tempo, RUN state, shared legacy slider range, A/B output routing,
and preset loading preferences stay live. Direct CTRL ownership still applies:
for example, recalling a CTRL assignment to Tempo lets that knob set the tempo,
even though the BPM screen itself is not recalled. Assigns can similarly change
a direct track setting according to the current knob position.

Hardware **Thru** pickup is not offered here: recalled virtual knobs/sliders
move to their saved positions, and mouse drags continue from those positions.
External MIDI-controller pickup is not implemented.

As of 2.9.0, both track Swing values are stored and recalled with each pattern.
Format-1 banks from earlier releases migrate with 50% Swing, preserving their
straight timing. Format 2 appends two values; other recall policies are
unchanged. See [Swing timing and controls](SWING.md).

## When loads happen

**Instant** loads immediately and resets note tracks, MOD lanes and accumulation.

**Queue Pulses** loads at the next multiple of the configured base-clock count
since start/reset/recall. For example, with 32 pulses, requesting during pulse
20 waits until the next 32-pulse boundary. It does not wait another 32 pulses
from the request. One pulse is one sixteenth note before track division.

**Total Pulses** waits for the currently playing pattern's overall length. If
that length is Off, Queue Pulses is used instead. A value of **0 / “–”** for Queue
Pulses waits for an explicit RESET button/input or for playback to stop.
Stopping commits a queued load silently so the new pattern is ready to start.

Right-click **SETUP → Preset settings** to edit the counts and policies. Click
the encoder to select a field, then drag to change its value. Right-click a
selected count for numeric entry. Queue Pulses defaults to 32 in this plugin;
Total Pulses defaults to Off. Both support 1–4096.

Total Pulses also restarts both note tracks, all MOD lanes and accumulation at
that length when no load is pending. Unlike a manual reset, an automatic Total
wrap keeps the independent Queue Pulses grid running. Thus a 16-pulse Total
length and a 32-pulse Queue interval allow two repetitions before loading.
The manual does not explicitly resolve that combined case; this is a documented
implementation choice awaiting hardware comparison.

Track-specific LEN/Pulses, clock division, skips and accumulator events do not
trigger queued loads. The new first note and MOD values take effect on the due
clock edge. Queued transitions preserve fractional clock phase and measured
external tempo. A pending request is transient and is cancelled when a Rack
patch is reopened; the currently sounding pattern and saved banks persist.

## Try it

Open **[Preset-performance.vcv](../examples/Preset-performance.vcv)**, choose your
audio device, and use the four filled slots in the red bank:

1. Night Current baseline.
2. Higher melody, odd/even traversal, skip and ratchet variation.
3. Sparse breakdown with the bass muted.
4. Bass return, held answer, slide and a small ratchet fill.

The patch uses Metropolix Lab, VCV Free and Core. It starts running, uses complete
pattern recall and Total Pulses = 128 (eight bars). Select another filled slot
and let the phrase finish. For faster comparison, choose Queue Pulses with 16
or 32 pulses. These are original demo patterns, not the hardware factory bank.

## Reference and remaining work

Based on the firmware 1.6 manual, printed pp.51, 53–56, 107–113, 169, 198–199.
The later changelog's explicit Load Delay rules take precedence over older
paragraphs that still refer to Track 1 LEN/Pulses. The 4096 maximum follows the
later firmware notes. Physical hardware comparison remains outstanding.

Version 2.8.0 adds [preset chains and repeat counts](PRESET-CHAINS.md), using these
banks. MIDI program changes, hardware bank interchange, custom Init presets and
panel pickup modes remain future work.

Since 2.16.0, velocity and per-track slider octave/direction settings are
stored in format6 presets. Formats1–5 migrate to 100% velocity and the shared
legacy range. Global cable functions remain unchanged by recall.

Since 2.16.0, velocity and per-track slider octave/direction settings are
stored in format6 presets. Formats1–5 migrate to 100% velocity and the shared
legacy range. Global cable functions remain unchanged by recall.
