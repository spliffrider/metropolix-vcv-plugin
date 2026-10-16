# Axiom Pro: Pulse Pressure starting layout

[Open the prepared Rack patch](../examples/Pulse-Pressure-Axiom-Pro.vcv).
It uses the bass-and-lead opening loop with two built-in VCV MIDI-Map modules.
All audio wiring and opening voice settings are retained. CTRL 1 and CTRL 2
now provide lead pulse-count and ratchet offsets; both start at noon, with no
offset. The chain stays disabled so the groove keeps playing while you edit.

**Prepared from the manual; not yet tested on the user's keyboard.** The
controller is currently disconnected. The mappings assume the Axiom Pro's
unchanged factory **GM Patch (1)**, absolute MIDI CC and channel 1. A modified
controller preset may send different messages. This file does not select a
hardware device or send any preset data to the keyboard.

## When the controller is available

1. Connect and power on the Axiom Pro. Choose the GM Patch if its factory
   assignments are intact; alternatively we can learn your existing controls.
2. In both MIDI-Map modules, choose **Windows MIDI**, the regular Axiom keyboard
   input, and channel 1. The manual calls the regular port USB A In; Windows
   may display another name. Verify it with one fader before mapping the rest.
3. Select your output device in AUDIO. The patch already has RUN enabled.
4. Move one control at a time to verify the table below. Save the Rack patch
   under your own name after selecting the devices and adjusting the mappings.

For a different CC assignment, click the corresponding MIDI-Map row, click its
target parameter, then move the physical control. See Rack's
[MIDI-Map instructions](https://vcvrack.com/manual/Core#MIDI-Map).

## Starting assignments

| Axiom fader | CC | Metropolix target |
| --- | --- | --- |
| S1 | 74 | Stage 1 pitch |
| S2 | 71 | Stage 2 pitch |
| S3 | 91 | Stage 3 pitch |
| S4 | 93 | Stage 4 pitch |
| S5 | 73 | Stage 5 pitch |
| S6 | 72 | Stage 6 pitch |
| S7 | 5 | Stage 7 pitch |
| S8 | 84 | Stage 8 pitch |

Pitch sliders are shared by both tracks and follow their scale/range settings.
S9 (factory CC7) remains available for a later master-volume mapping.

| Axiom encoder | CC | Target |
| --- | --- | --- |
| E1 | 10 | CTRL 1: lead pulse-count offset |
| E2 | 2 | CTRL 2: lead ratchet offset |
| E3 | 12 | Bass filter cutoff |
| E4 | 13 | Lead filter cutoff |
| E5 | 75 | Bass filter resonance |
| E6 | 76 | Lead filter resonance |
| E7 | 92 | Delay tone |
| E8 | 95 | Reverb wet level |

These CC numbers come from Appendix A, GM Patch, of the manufacturer's
[Axiom Pro User Guide](https://www.bhphotovideo.com/lit_files/75902.pdf).
Use encoder Type **MIDI CC** for this template. Rack's Core MIDI-Map expects
absolute values; the Axiom's relative encoder formats require another mapping
approach. If HyperControl is active in another application, the manual's
**Transport** setting leaves the other control groups available for custom MIDI.

## Tune together later

Smooth CC is enabled on both MIDI-Maps. It smooths changes but does not provide
pickup, parameter feedback or restricted mapping ranges. The first received
value can still move a parameter away from its saved position. We will check
encoder acceleration, useful filter ranges and fader takeover on the physical
controller before treating this as a finished performance preset.

Pads, transport and keyboard playing are left for the connected session. The
GM factory transport assignments send computer keystrokes, so mapping its Play
button to Rack RUN needs separate setup. For keyboard scale-root control, the
plugin also provides MIDI Root Ch; that changes the sequence's root rather than
playing a separate voice. See the [MIDI guide](MIDI.md).

Dependencies match the [sidechain opening](SIDECHAIN-DEMO.md): Metropolix Lab,
VCV Free, Audible Instruments, Befaco, Valley and Bogaudio. MIDI-Map and Notes
are included in Rack. Regenerate with `python scripts/create-axiom-pro-patch.py`.
