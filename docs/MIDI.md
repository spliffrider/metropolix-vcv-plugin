# MIDI — 2.24.0 alpha

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

## Example and verification

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
without documenting the device's payload format. Plugin backup/interchange
will use a documented software format; it must not be presented as hardware
SysEx compatibility.
