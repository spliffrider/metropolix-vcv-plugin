# MIDI — 2.70.0 alpha

Right-click the module for **MIDI input device** and **MIDI output device**.
These use [Rack's MIDI ports](https://github.com/VCVRack/Rack/blob/v2/include/midi.hpp).
Select a hardware device, virtual MIDI port or Rack Loopback bus. Channel
selection belongs to each track/lane; the device ports use All channels.
Device names and settings are saved in Rack patches, outside internal presets.

SETUP → MIDI Setup provides Clock Out, Transport, Root Channel and PC Channel.
SETUP → Global Setup → Clock Src includes MIDI. Its BPM screen shows received
tempo and 24 PPQN; it does not edit the analog clock input divider.
Encoder/display drags, native value menus, undo and long-press defaults are
available for the MIDI parameters, as on other settings screens.

## MIDI MX output — firmware 1.6.5

This independent eight output MIDI bus works **without the physical MX module**.
Choose a Rack MIDI output device, then SETUP → MIDI MX Ch (Off or1–16) and
MIDI MX Output (CC or Note). The same channel is available on the MX screen:
ALT → stage 6, ALT again, drag up/down, then click the encoder to confirm.
Right-click the encoder/display for direct channel and CC/Note choices.
The MIDI Setup menu also exposes both settings. Both are global and remain
unchanged when a musical preset loads. New modules and older imported patches
default this bus to Off and CC.

The nine MX modes drive controllers 102–109 or notes 35–42. Transport/stage/clock
modes use 127 while high and0 while low. Mod Gate/Mod Trig use the lane's
programmed current stage value for the MIDI value or note velocity, including
while its audible voltage ramps. The existing voltage conversion applies:
−5…+5V →0…127 for bipolar,0…5V →0…127 for unipolar/gate. Mode, channel or
message-kind changes release the old bus before opening the new one. CCs send
only changed values; held notes do not repeatedly retrigger. Zero velocity
stays silent. Release MIDI notes suppresses held gates until a fresh edge.

Manual 1.6.5 pp125/182–183 specify the bus, channels, controller/note numbers and
stage-value source. Voltage conversion, threshold and held-note policy remain
explicit software interpretations pending a hardware comparison. See [MX](MX.md).

## Notes, velocity and CC

ALT → TRK 1/2 opens the track menu. Choose MIDI Channel (Off or 1–16), CV to CC,
Slide Toggle CC, Slide Amount CC and Slide Amount (Toggle/Always). CC assignments
include Off and every CC from 0–127. Velocity uses the existing track setting:
50%, 75%, 100% or Track CV, including velocity modulation.

New modules follow the manual's later defaults: track channels 1/2, CV CC1,
Slide Toggle CC65 and Slide Amount CC5; MOD channels 1 with CC20–27. Older Rack
patches and format 1–8 banks migrate with channels Off, preserving their sound
and avoiding newly enabled MIDI output. Track/lane MIDI settings are stored
with musical presets in format 9, after the unchanged format 8 prefix.

Notes follow stage pitch, accumulators, probability, skips, gates and ratchets.
Pitch is the destination note, rather than intermediate analog glide voltages.
Analog and Tempo slides send the same MIDI slide controls. Acid holds the
current note and enables slide when entering the following stage. Slide CCs
precede the new note; legato Note On precedes the previous Note Off. Toggle
amount returns to zero without slide. Always sends amount at sequence start
and when it changes. Track swap exchanges MIDI channels while each source
track retains its own pitches, velocity and CC assignments.

Rack uses **0 V = MIDI note 60** by default, matching its
[MIDI-CV module](https://github.com/VCVRack/Rack/blob/v2/src/core/MIDI_CV.cpp).
The additional Note at 0V setting changes that mapping, e.g. 12 for C0 in MIDI
systems naming note 12 C0. Notes and velocity clamp to 0–127; zero velocity
silences/releases the note, following MIDI's Note On zero semantics. Track CV velocity
uses Unipolar or Gate type. CC voltage maps −5…+5 V to 0…127 for bipolar lanes,
and 0…+5 V to 0…127 for unipolar/gate lanes. Those mappings and rounding are
software interpretations where the manual does not specify conversion details.

ALT → MOD opens each lane's MIDI Channel and MOD to CC. Continuous CC output is
quantized to 7 bits and limited to one changed value per millisecond per route;
this avoids flooding a MIDI device. Note and transport timing remain sample
based. While stopped, moving an assigned stage CV, Slide toggle/amount or MOD
voltage sends that isolated CC for DAW learn. The CV stage display also shows
the corresponding CC value.

Stopping, disabling a track channel, changing channels, removing a module or
disconnecting an output releases notes. Release MIDI notes silences the held
notes until the next gate/stage event. Device changes send All Notes Off on
previously used channels before Rack disconnects the old device.

## Clock, transport and incoming control

MIDI input consumes six 24-PPQN clocks per master sixteenth; analog Ext Clock
Div does not apply. Start restarts; Continue preserves position and aligns the
six-pulse phase; Stop stops. These messages are enabled by MIDI Transport when
Clock Src is MIDI. A zero Song Position Pointer resets sequence position while
retaining MIDI clock phase. Incoming clocks continue counting while stopped.
The connected Reset jack in Run/Run-C mode retains its normal transport priority.

For Internal/External clock sources, Transport sends Start/Continue/Stop and
Song Position Pointer zero for global reset. The first Run/Pause start sends
Start; a later resume sends Continue. Internal MIDI clock runs at 24 PPQN.
External clock output interpolates six pulses using the measured sixteenth
period; abrupt tempo changes can affect that prediction. Clock is unswung.
The 0–21 ms analog output delay does not delay MIDI.

**Manual ambiguity:** p.49 says Clk/Rset Out does not affect MIDI; the later
firmware correction on p.193 says Running suppresses stopped MIDI clocks.
This implementation follows that correction: Running suppresses stopped
MIDI clock; Always and the existing Rack legacy setting allow it. Analog
reset/sequence reset does not restart outgoing MIDI clock phase. A MIDI input
and output on the same Rack Loopback bus do not echo received clocks back into
that bus, avoiding clock feedback.

Root Channel accepts positive-velocity Note On and selects its pitch class for
factory scales. It excludes user scales. PC Channel accepts programs 0–63
(some devices display 1–64), selecting banks/slots 1-1…8-8. It uses the existing
Instant/Queue/Total load delay, rejects empty slots and enabled chains, and uses
an audio-owned bank cache. A burst uses its most recent program request. These
external automation recalls do not create Rack undo entries.

Gate destinations send CC 0/127 regardless of a saved Bipolar lane type; stopped
learning uses the effective gate value. Voltage destinations retain their normal
bipolar/unipolar mapping.

## Example and verification

For hands-on control of the Pulse Pressure opening, see the
[Axiom Pro starting layout](AXIOM-PRO.md). It adds Core MIDI-Map assignments
for the factory GM faders and encoders; physical-controller setup and response
tuning are still pending.

Open `examples/MIDI-performance.vcv`, select an AUDIO device and press RUN.
Both voices receive pitch/gate exclusively through Rack Loopback 16 and two
Core MIDI-CV modules on channels 1/2. The patch starts stopped and has no
preselected hardware MIDI/audio device. Avoid sharing Loopback 16 with an
unrelated sender in the same patch. Standard MIDI-CV does not implement the
external synthesizer's CC-controlled portamento.

Portable tests cover every channel, note and velocity, note-off/legato order,
CC ranges and clock rates. Native tests record actual Rack MIDI bytes and frame
timestamps, including clock/transport reception, program delay, root, slides,
ratchets, swap, CC learning/reassignment, disconnect and migration. Widget tests
cover menu navigation, dragging, undo and actual save/reopen. A real MIDI-CV
patch is recorded for both audio channels. Physical MIDI hardware/DAW behavior
and comparisons with a Metropolix remain unverified.

Hardware SysEx interchange is not included: the manual describes transfers
without documenting the device's payload format. [Portable backups](BACKUPS.md) use a documented software JSON format. These files
do not provide hardware SysEx compatibility.
