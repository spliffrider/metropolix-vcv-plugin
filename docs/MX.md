# MX gate expander

Place **MX immediately to the right of Metropolix Lab**, on the same rack row.
The green LINK light shows a connection. Configure it with **ALT → stage 6 (MX)**,
or right-click the sequencer/expander and open MX assignments. It adds eight 0/5 V
outputs; all original sequencer jacks and saved cable IDs remain unchanged.

Drag the encoder or display up/down to choose a mode. A `?` marks a preview:
press the encoder to apply it. Press again to select the track/lane field when
available. Both fields always require confirmation, including with Enc. Confirm
Off. EXIT cancels a preview. Hold the encoder for more than one second to reset
the mode, both assignments and MIDI MX channel. Desktop context-menu choices apply directly and
support Rack undo. Mode/assignments are global patch settings, outside presets.

| Mode | Outputs 1–8 |
|---|---|
| Transport A | Run, Reset, /1, /2, /4, /4 with +2 offset, /8, /16 |
| Transport B | Run, Reset, /1, /3, /4, /5, /8, /16 |
| Transport C | Run, Reset, ×4, ×2, ×1, /2, /4, /8 |
| Prob Clocks | ×1 at 100/75/50/25%, then /2 at 100/75/50/25% |
| Mod Clocks | Each of the eight MOD lanes' clock outputs |
| Stage Num | Stage 1–8 gates for the selected track |
| Mod Stage Num | Stage 1–8 gates for the selected MOD lane |
| Mod Gate | Each MOD lane's audible voltage converted to a gate |
| Mod Trig | A 5 ms trigger on each MOD lane's rising CV gate |

Transport/probability clocks follow the unswung master clock, independently of
the main CLK OUT division/offset. A global reset restarts their divider phase;
a targeted track/lane reset preserves it. Clk/Rset Out Running suppresses
stopped transport clocks/reset, while Always permits them. MOD clocks follow
the actual lane clocks, including lane divisions and mute.

Stage gates stay high for the whole stage, independent of note gate length,
ratchets, Rest and note probability. Skip/traversal determines which stage is
playing. Muting the selected track/lane silences its stage outputs. Track
assignments follow Trk Out Swap. With the transport stopped, Stage Num follows
Loopy Stage Player button holds and releases. MOD stage gates stop with transport.

Try `MX-performance.vcv`: the existing bass/lead arrangement varies through its
preset chain, while MX Stage Num triggers a sine kick on stage 1, noise snare on
stage 5, and hats on stages 2/4/6/8. It uses VCV Free/Core modules, starts stopped,
and does not select an audio device. Choose your AUDIO output and press RUN.
Change stage pulse counts or skips to hear the percussion follow actual stage
visits. Keep the MX module adjacent to the sequencer when moving the patch.

Rack's double-buffer expander API adds one sample of latency. MX also follows
UI Setup's CV Out Delay (0–21 ms); its gate LEDs use native Rack lights/Bloom and
follow Button LED Level. Disconnecting or moving it immediately silences outputs
and clears delayed gates. It connects only to this plugin's Metropolix model;
physical I2C devices and other plugins' expanders are not supported.

The manual specifies rates, assignments and stage behavior, but does not specify
trigger width, probability correlation, or external-clock prediction during a
sudden tempo change. This implementation uses independent probability draws,
1 ms clock/reset triggers (shorter than half an interval at very high rates), and
the measured master period for intervening multiplied clocks. Multipliers emit
only the expected intervening pulses and stop if the next master pulse is missing.
Defaults are Transport A, track 1 and MOD lane 1. Hardware comparison remains
unverified. Reference: [Metropolix manual, pp.122–125](https://intellijel.com/downloads/manuals/metropolix_manual_v1.6_2025.09.24.pdf).

## MIDI MX and the new screen — 2.70.0

The1.6.5 display has MX ASSIGN on the lower left and MIDI CH on the lower right.
ALT selects the MIDI channel; drag the encoder or OLED and press the encoder
to confirm. ALT returns to Mode. EXIT cancels a preview. Hold the encoder to
reset mode, track, lane and channel in one Undo action. The global CC/Note
preference remains unchanged. Right-click offers named modes, channels and
CC/Note. SETUP and MIDI Setup expose the same channel and message kind.

MIDI MX operates independently of the physical MX. Configure a MIDI output
device from the MIDI menu, select channel 1–16, and choose CC102–109 or
notes 35–42. Mod Gate/Mod Trig derive their MIDI levels from programmed stage
values; other modes use 127. See [MIDI](MIDI.md) for conversion and note cleanup.

Mod Gate/Mod Trig use the eight current audible MOD lane voltages. Muted lanes
are low. The existing 1.5V gate threshold applies. Mod Trig is rising-edge only:
a short input still creates a full 5 ms pulse, a held high does not retrigger,
and stopped transport clears its pulse. Threshold/edge policy is an
interpretation where the manual only specifies a trigger derived from a lane.
Older seven modes retain their 1 ms clock/reset pulse behavior.

The actual default MX OLED matches all 8192 native cells from manual 1.6.5
p122/p125 (the two images are duplicates). New channel numerals and pending
states use the established companion font. This is image evidence, not
hardware timing or MIDI equivalence. See [validation](VALIDATION.md).
