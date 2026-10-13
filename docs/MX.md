# MX gate expander

Place **MX immediately to the right of Metropolix Lab**, on the same rack row.
The green LINK light shows a connection. Configure it with **ALT → stage 6 (MX)**,
or right-click the sequencer/expander and open MX assignments. It adds eight 0/5 V
outputs; all original sequencer jacks and saved cable IDs remain unchanged.

Drag the encoder or display up/down to choose a mode. A `?` marks a preview:
press the encoder to apply it. Press again to select the track/lane field when
available. Both fields always require confirmation, including with Enc. Confirm
Off. EXIT cancels a preview. Hold the encoder for more than one second to reset
the mode and both assignments. Desktop context-menu choices apply directly and
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
