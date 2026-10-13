# Accumulator performance routing — 2.18.0

The two CTRL knobs now offer all 29 destinations listed by the hardware manual.
**Accum Dir** completes that list: clockwise runs accumulation forward, exact
noon freezes each accumulator's last sounding offset, and counterclockwise
traverses the same pitch range backwards. Use **Centre knob (noon)** in the
right-click menu or enter 50% to freeze exactly. Track assignments remain
independent. This changes accumulation; the stage order and pulse rhythm keep
running.

All three AUX inputs and eight internal MOD lanes now share these six additional
destinations, bringing their supported destination count to 18 plus None:

| Destination | Effect |
| --- | --- |
| Accumulate | Assigned high permits accumulation; assigned low holds the last emitted offset. With no assignment, accumulation runs normally. |
| Accum Rev | High reverses accumulation inside its existing polarity range; low resumes forward movement. |
| Accum Invert | High reverses the signed increment and therefore its unipolar range; low restores the programmed increment. |
| Accum Mode | High toggles programmed Stage/Track mode; low restores it. Banks remain independent and changes apply on a track pulse. |
| Accum Polar | High toggles programmed Unipolar/Bipolar polarity; low restores it. |
| Accum Reset | A rising trigger resets only the assigned track's accumulator banks, on its next track pulse. It preserves stage traversal, clocks, MOD lanes and transport. |

A gate or trigger is high at **1.5 V or above**. AUX attenuverters use the
existing logical gate inversion: clockwise follows the gate, counterclockwise
inverts it, and noon supplies low. With no cable, clockwise supplies a manual
high. MOD gate destinations toggle 0/5 V on stage clicks and ignore ramps.
Hover help explains these operations; destination menus, vertical dragging,
numeric entry, undo, Rack saving and internal presets retain their normal use.

Accumulate is sampled at the scheduled Stage/Pulse/Ratchet accumulation event.
Rests, muted notes, failed probability decisions and skipped stages still obey
the existing rules. Pausing does not add a clock, truncate a multi-pulse stage,
or stop its gates. A muted lane assigned to Accumulate is an assigned low gate,
so it pauses; clear that destination to restore unconditional accumulation.

Accumulator Reset captures brief triggers between clocks. A divided or swung
track applies the queued reset on its next actual track pulse. Holding the
input high does not repeat it. Both normal and Loopy foreground banks reset,
including the shared Track-mode bank. A normal transport reset still resets
the whole sequencer.

## Try the arrangement

Open [Accumulator-routing-performance.vcv](../examples/Accumulator-routing-performance.vcv),
select your output device in AUDIO and press RUN. Four chained scenes vary the
lead through all six accumulator gates, while a velocity-controlled bass and
independent MOD-clock percussion continue. CTRL 1 targets the bass and CTRL 2
the lead. Try freezing CTRL 2 at noon, then reversing it; compare Accum Rev and
Accum Invert on an AUX input. The patch is saved stopped and uses Core/VCV Free
modules. Keep the previous track-options demo for simpler source comparisons.

## Reference and comparison limits

The reference is firmware 1.6 manual pp.149 and 157–159. Tests reproduce its
Wrap examples, including live Invert from +6 into the negative range at -2.
Reverse and Invert are separate operations. No physical module comparison has
been performed.

Pendulum reversal reflects back through the bounded trajectory. Hold reverses
toward the opposite limit. Random reversal reverses its directional bias and
draws new random steps; it does not replay a stored random history. Mixed signed
Track-mode sources use the current stage's increment. These cases, collisions
between CTRL and multiple gates, and exact fractional thresholds need owner
feedback: the manual has no complete diagrams for them.

CTRL negative or a high Accum Rev gate requests reverse; either low/positive
alone does not cancel the other. CTRL Invert and a high Accum Invert gate combine
as two inversions. Routed gate contributions add using the established routing
rule. Parameter IDs and internal preset format 7 remain unchanged; older patches
keep their saved routing selections.
