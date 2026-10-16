# Production runtime checks — 2.34.0

The runtime host loads the stripped binary included in the download. It uses
Rack's public module API; the instrument implementation is not compiled into
the host. Windows uses the installed Rack Free 2.6.6 library. Linux and both Mac
jobs use their matching official Rack 2.6.6 SDK libraries.

Each run exercises the main sequencer and MX for six virtual minutes, plus a
one-second default-signal check and short control-settling periods. It covers
44.1, 48 and 96 kHz. The parameter edits use seed `0x4d455452`; this fixes the
edit sequence, not the sequencer's separate random musical choices.

The host checks:

- Both public factories and the789 parameters (the established770 plus sixteen ORDER shortcut values, independent MOD group mute and two MIDI MX settings), seven main outputs
  and eight MX outputs.
- Default main gates and linked MX actually reaching 5 V.
- 56,664 parameter writes: repeated minimum, maximum and seeded interior
  values from the parameter metadata. Transport and clock are then configured
  deliberately, and direct CTRL assignments settle before state is captured.
- Finite voltages on every output after every frame; main gate/clock and MX
  outputs remain exactly 0 or 5 V.
- 145 in-place state restores and 144 independently constructed instances,
  with exact parameter and bank/settings comparisons against loaded receipts.
- Twelve Rack Initialize calls preserving the saved preset bank.
- Reciprocal MX links, disconnected links and stale one-sided links, including
  reconnects with delayed output pending. Disconnected/stale outputs drop
  immediately to zero.
- External clock/reset edges, internal playback, stopped transport and AUX
  voltages across the three sample rates.

The public backup fixture is first loaded to canonicalize decimal numbers to
Rack's float precision. Subsequent comparisons are exact. History-style
receipts explicitly preserve RUN rather than applying Run on boot. A failure
prints the differing parameter/field and saves before/after JSON for diagnosis.

The completion report records 22,624,609 processed frames and 339,369,135 output
checks, plus elapsed wall time. These are simulated engine samples processed
as fast as the computer allows, not six minutes spent recording audio. Logs
must contain the final `PASS: stripped production binary` marker to count.

## Run locally

On Windows, package the production binary, then run the separate host:

```powershell
.\scripts\build.ps1 -Package
.\scripts\test-production.ps1
```

This does not open Rack's GUI or change a user patch/profile. It does not connect
to physical audio or MIDI devices. On Linux/Mac, build `production-test-build`
and run `build/tests/plugin-load` with the absolute production binary path and
`examples/Backup-example.json` path; use the library environment documented by
the package workflow.

The test is a single-threaded host. Actual Rack renderer, widget-callback,
save/reopen and MX/MIDI audio-path checks remain separate. This does not prove
absence of concurrency problems, behavior on every computer, subjective
musical quality or hardware equivalence. Long human sessions and physical
Metropolix comparisons remain useful. See [Validation](VALIDATION.md) and
[Platform coverage](PLATFORMS.md).
