param([string]$RackPath = 'C:\Program Files\VCV\Rack2Free\Rack.exe', [switch]$Audio, [switch]$Modulation, [switch]$Hardware, [switch]$Display, [switch]$Aux, [switch]$Musical, [switch]$Variations, [switch]$Accumulator, [switch]$Controls, [switch]$Presets, [switch]$PresetAudio, [switch]$Polish, [switch]$Chains, [switch]$ChainAudio, [switch]$Swing, [switch]$SwingAudio, [switch]$Overrides, [switch]$OverrideAudio, [switch]$Loopy, [switch]$LoopyAudio, [switch]$Scales, [switch]$ScaleAudio, [switch]$Slides, [switch]$SlideAudio, [switch]$StageCv, [switch]$CvAudio, [switch]$Clock, [switch]$ClockAudio, [switch]$Outputs, [switch]$OutputAudio, [switch]$OutputArrangement, [switch]$Tracks, [switch]$TrackAudio, [switch]$AccumRouting, [switch]$AccumAudio, [switch]$ExtendedRouting, [switch]$RoutingAudio, [switch]$Config, [switch]$ConfigAudio, [switch]$Workflows, [switch]$WorkflowsAudio, [switch]$SetupOptions, [switch]$SetupAudio, [switch]$Pickup, [switch]$PickupAudio, [switch]$Midi, [switch]$MidiAudio, [switch]$Mx, [switch]$MxAudio, [switch]$Backup, [switch]$Fidelity, [switch]$Order, [switch]$Storage, [switch]$Group, [switch]$Timing, [switch]$StagePair, [switch]$ScaleCopy, [switch]$Pending, [switch]$CtrlEditor, [switch]$StagePattern)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
if ($Hardware) { $Modulation = $true }
if ($Aux) { $Modulation = $true }
if ($Modulation) { $Audio = $true }
if ($Variations) { $Musical = $true }
if ($Musical) { $Audio = $true }
if ($Accumulator) { $Audio = $true }
if ($Controls) { $Audio = $true }
if ($PresetAudio) { $Audio = $true }
if ($ChainAudio) { $Audio = $true }
if ($LoopyAudio) { $Audio = $true }
if ($ScaleAudio) { $Audio = $true }
if ($SlideAudio) { $Audio = $true; $ChainAudio = $true }
if ($RoutingAudio) { $OutputArrangement = $true; $env:METROPOLIX_EXTENDED_ROUTING_AUDIO_QA = '1' }
if ($AccumAudio) { $OutputArrangement = $true; $env:METROPOLIX_ACCUM_ROUTING_AUDIO_QA = '1' }
if ($TrackAudio) { $OutputArrangement = $true; $env:METROPOLIX_TRACK_AUDIO_QA = '1' }
if ($OutputArrangement) { $OutputAudio = $true; $ChainAudio = $true; $env:METROPOLIX_OUTPUT_ARRANGEMENT_QA = '1' }
if ($OutputAudio) { $Audio = $true }
if ($ConfigAudio) { $ClockAudio = $true }
if ($WorkflowsAudio) { $Audio = $true }
if ($SetupAudio) { $Audio = $true }
if ($PickupAudio) { $Audio = $true }
if ($MidiAudio) { $Audio = $true }
if ($MxAudio) { $Audio = $true; $env:METROPOLIX_MX_AUDIO_QA = '1' }
if ($ClockAudio) { $Audio = $true }
if ($CvAudio) { $Audio = $true; $ChainAudio = $true }
if ($OverrideAudio) { $Audio = $true; $ChainAudio = $true }
if ($SwingAudio) { $Audio = $true; $ChainAudio = $true }
if (Get-Process Rack -ErrorAction SilentlyContinue) { throw 'Close Rack before the UI test.' }
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
Push-Location -LiteralPath $projectRoot
try {
    & "$env:LOCALAPPDATA\MetropolixDev\msys64\usr\bin\bash.exe" -lc 'make ui-test-build'
    if ($LASTEXITCODE -ne 0) { throw 'UI test build failed.' }
    $profile = Join-Path $projectRoot 'tmp\rack-qa'
    $output = Join-Path $projectRoot $(if ($Presets) { 'tmp\preset-ui-qa' } elseif ($PresetAudio) { 'tmp\preset-audio-qa' } elseif ($Display) { 'tmp\display-qa' } elseif ($Controls) { 'tmp\ctrl-qa' } elseif ($Accumulator) { 'tmp\accumulator-qa' } elseif ($Variations) { 'tmp\variations-qa' } elseif ($Musical) { 'tmp\musical-qa' } elseif ($Aux) { 'tmp\aux-qa' } elseif ($Hardware) { 'tmp\hardware-qa' } elseif ($Modulation) { 'tmp\mod-qa' } elseif ($Audio) { 'tmp\audio-qa' } else { 'tmp\qa' })
    $plugin = Join-Path $profile 'plugins-win-x64\MetropolixLab'
    if ($Polish) { $output = Join-Path $projectRoot 'tmp\polish-qa'; $env:METROPOLIX_POLISH_QA = '1' }
    if ($Chains) { $output = Join-Path $projectRoot 'tmp\chain-ui-qa'; $env:METROPOLIX_CHAIN_UI_QA = '1' }
    if ($ChainAudio) { $output = Join-Path $projectRoot 'tmp\chain-audio-qa'; $env:METROPOLIX_CHAIN_AUDIO_QA = '1' }
    if ($CvAudio) { $output = Join-Path $projectRoot 'tmp/stage-cv-audio-qa'; $env:METROPOLIX_STAGE_CV_AUDIO_QA = '1' }
    if ($ClockAudio) { $output = Join-Path $projectRoot 'tmp/clock-audio-qa'; $env:METROPOLIX_CLOCK_AUDIO_QA = '1' }
    if ($OutputAudio) { $output = Join-Path $projectRoot 'tmp/output-audio-qa'; $env:METROPOLIX_OUTPUT_AUDIO_QA = '1' }
    if ($OutputArrangement) { $output = Join-Path $projectRoot 'tmp/output-arrangement-qa' }
    if ($RoutingAudio) { $output = Join-Path $projectRoot 'tmp/extended-routing-audio-qa' }
    if ($WorkflowsAudio) { $output = Join-Path $projectRoot 'tmp/workflow-audio-qa' }
    if ($ConfigAudio) { $output = Join-Path $projectRoot 'tmp/config-audio-qa' }
    if ($Pending) { $output = Join-Path $projectRoot 'tmp/pending-ui-qa'; $env:METROPOLIX_PENDING_UI_QA = '1' }
    if ($CtrlEditor) { $output = Join-Path $projectRoot 'tmp/ctrleditor-ui-qa'; $env:METROPOLIX_CTRLEDITOR_UI_QA = '1' }
    if ($StagePattern) { $output = Join-Path $projectRoot 'tmp/stagepattern-ui-qa'; $env:METROPOLIX_STAGEPATTERN_UI_QA = '1' }
    if ($ScaleCopy) { $output = Join-Path $projectRoot 'tmp/scalecopy-ui-qa'; $env:METROPOLIX_SCALECOPY_UI_QA = '1' }
    if ($StagePair) { $output = Join-Path $projectRoot 'tmp/stagepair-ui-qa'; $env:METROPOLIX_STAGEPAIR_UI_QA = '1' }
    if ($Timing) { $output = Join-Path $projectRoot 'tmp/timing-ui-qa'; $env:METROPOLIX_TIMING_UI_QA = '1' }
    if ($Group) { $output = Join-Path $projectRoot 'tmp/group-ui-qa'; $env:METROPOLIX_GROUP_UI_QA = '1' }
    if ($Storage) { $output = Join-Path $projectRoot 'tmp/storage-ui-qa'; $env:METROPOLIX_STORAGE_UI_QA = '1' }
    if ($Order) { $output = Join-Path $projectRoot 'tmp/order-ui-qa'; $env:METROPOLIX_ORDER_UI_QA = '1' }
    if ($Fidelity) { $output = Join-Path $projectRoot 'tmp/fidelity-ui-qa'; $env:METROPOLIX_FIDELITY_UI_QA = '1' }
    if ($Backup) { $output = Join-Path $projectRoot 'tmp/backup-ui-qa'; $env:METROPOLIX_BACKUP_UI_QA = '1' }
    if ($Mx) { $output = Join-Path $projectRoot 'tmp/mx-ui-qa'; $env:METROPOLIX_MX_UI_QA = '1' }
    if ($MxAudio) { $output = Join-Path $projectRoot 'tmp/mx-audio-qa' }
    if ($Midi) { $output = Join-Path $projectRoot 'tmp/midi-ui-qa'; $env:METROPOLIX_MIDI_UI_QA = '1' }
    if ($MidiAudio) { $output = Join-Path $projectRoot 'tmp/midi-audio-qa' }
    if ($Pickup) { $output = Join-Path $projectRoot 'tmp/pickup-ui-qa'; $env:METROPOLIX_PICKUP_UI_QA = '1' }
    if ($PickupAudio) { $output = Join-Path $projectRoot 'tmp/pickup-audio-qa' }
    if ($SetupOptions) { $output = Join-Path $projectRoot 'tmp/setup-ui-qa'; $env:METROPOLIX_SETUP_OPTIONS_QA = '1' }
    if ($SetupAudio) { $output = Join-Path $projectRoot 'tmp/setup-audio-qa' }
    if ($Workflows) { $output = Join-Path $projectRoot 'tmp/workflow-ui-qa'; $env:METROPOLIX_WORKFLOW_UI_QA = '1' }
    if ($Config) { $output = Join-Path $projectRoot 'tmp/config-ui-qa'; $env:METROPOLIX_CONFIG_UI_QA = '1' }
    if ($ExtendedRouting) { $output = Join-Path $projectRoot 'tmp/extended-routing-ui-qa'; $env:METROPOLIX_EXTENDED_ROUTING_UI_QA = '1' }
    if ($AccumAudio) { $output = Join-Path $projectRoot 'tmp/accum-routing-audio-qa' }
    if ($AccumRouting) { $output = Join-Path $projectRoot 'tmp/accum-routing-ui-qa'; $env:METROPOLIX_ACCUM_ROUTING_UI_QA = '1' }
    if ($TrackAudio) { $output = Join-Path $projectRoot 'tmp/track-audio-qa' }
    if ($Tracks) { $output = Join-Path $projectRoot 'tmp/track-ui-qa'; $env:METROPOLIX_TRACK_UI_QA = '1' }
    if ($Outputs) { $output = Join-Path $projectRoot 'tmp/output-ui-qa'; $env:METROPOLIX_OUTPUT_UI_QA = '1' }
    if ($Clock) { $output = Join-Path $projectRoot 'tmp/clock-ui-qa'; $env:METROPOLIX_CLOCK_UI_QA = '1' }
    if ($StageCv) { $output = Join-Path $projectRoot 'tmp/stage-cv-ui-qa'; $env:METROPOLIX_STAGE_CV_UI_QA = '1' }
    if ($SlideAudio) { $output = Join-Path $projectRoot 'tmp/slide-audio-qa'; $env:METROPOLIX_SLIDE_AUDIO_QA = '1' }
    if ($Slides) { $output = Join-Path $projectRoot 'tmp/slide-ui-qa'; $env:METROPOLIX_SLIDE_UI_QA = '1' }
    if ($ScaleAudio) { $output = Join-Path $projectRoot 'tmp/scale-audio-qa'; $env:METROPOLIX_SCALE_AUDIO_QA = '1' }
    if ($Scales) { $output = Join-Path $projectRoot 'tmp/scale-ui-qa'; $env:METROPOLIX_SCALE_UI_QA = '1' }
    if ($Loopy) { $output = Join-Path $projectRoot 'tmp/loopy-ui-qa'; $env:METROPOLIX_LOOPY_UI_QA = '1' }
    if ($LoopyAudio) { $output = Join-Path $projectRoot 'tmp/loopy-audio-qa'; $env:METROPOLIX_LOOPY_AUDIO_QA = '1' }
    if ($Overrides) { $output = Join-Path $projectRoot 'tmp/override-ui-qa'; $env:METROPOLIX_OVERRIDE_UI_QA = '1' }
    if ($Swing) { $output = Join-Path $projectRoot 'tmp\swing-ui-qa'; $env:METROPOLIX_SWING_UI_QA = '1' }
    if ($OverrideAudio) { $output = Join-Path $projectRoot 'tmp/override-audio-qa'; $env:METROPOLIX_OVERRIDE_AUDIO_QA = '1' }
    if ($SwingAudio) { $output = Join-Path $projectRoot 'tmp\swing-audio-qa'; $env:METROPOLIX_SWING_AUDIO_QA = '1' }
    New-Item -ItemType Directory -Force -Path $plugin, $output | Out-Null
    # A killed test leaves a truncated log, which Rack interprets as a crash on
    # the next launch and presents a modal before loading the explicit patch.
    # Keep the evidence, but give only this isolated QA profile a fresh log.
    $qaLog = Join-Path $profile 'log.txt'
    if (Test-Path -LiteralPath $qaLog) {
        $previousLog = Join-Path $output ('previous-rack-' + [guid]::NewGuid().ToString('N') + '.log')
        Move-Item -LiteralPath $qaLog -Destination $previousLog
    }
    Copy-Item -LiteralPath 'build\ui-test\plugin.dll', 'plugin.json', 'README.md' -Destination $plugin -Force
    Copy-Item -LiteralPath 'res' -Destination $plugin -Recurse -Force
    $manifest = Get-Content -LiteralPath 'plugin.json' -Raw | ConvertFrom-Json
    $manifest.modules += @{slug='AudioQA'; name='Internal audio test'}
    $manifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $plugin 'plugin.json') -Encoding utf8NoBOM
    '{"windowSize":[1600,950],"windowPos":[30,30],"sampleRate":48000,"showTipsOnLaunch":false,"autoCheckUpdates":false}' | Set-Content -LiteralPath (Join-Path $profile 'settings.json') -Encoding utf8NoBOM
    '' | Set-Content -LiteralPath (Join-Path $output 'report.txt') -Encoding utf8NoBOM
    $env:METROPOLIX_QA_DIR = $output.Replace('\', '/')
    $patch = Join-Path $projectRoot 'examples\Hybrid-demo.vcv'
    if ($Pending) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($CtrlEditor) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($StagePattern) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($ScaleCopy) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($StagePair) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($Timing) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($Group) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($Storage) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($Order) { $patch = Join-Path $projectRoot 'examples/Hardware-panel-demo.vcv' }
    if ($Fidelity) { $patch = Join-Path $projectRoot 'examples/MX-performance.vcv' }
    if ($Presets) { $env:METROPOLIX_PRESET_UI_QA = '1' }
    if ($Display -or $Polish) {
        if ($Display) { $env:METROPOLIX_DISPLAY_QA = '1' }
        $displayPatch = Get-Content -LiteralPath 'examples\Hardware-panel-demo.vcv' -Raw | ConvertFrom-Json
        $displayPatch.modules = @($displayPatch.modules | Where-Object plugin -eq 'MetropolixLab')
        $displayPatch.cables = @()
        $displayPatch.zoom = 2.0
        $displayPatch.gridOffset = @(0,0)
        $patch = Join-Path $output 'display-test.vcv'
        $displayPatch | ConvertTo-Json -Depth 30 | Set-Content -LiteralPath $patch -Encoding utf8NoBOM
    }
    if ($Audio) {
        $env:METROPOLIX_AUDIO_QA = '1'
        if ($Musical) { $env:METROPOLIX_MUSICAL_QA = '1' }
        if ($Variations) { $env:METROPOLIX_VARIATIONS_QA = '1' }
        if ($Aux) { $env:METROPOLIX_AUX_QA = '1' }
        if ($Controls) { $env:METROPOLIX_CTRL_QA = '1' }
        if ($PresetAudio) { $env:METROPOLIX_PRESET_AUDIO_QA = '1' }
        $audioExample = if ($MxAudio) { 'examples/MX-performance.vcv' } elseif ($MidiAudio) { 'examples/MIDI-performance.vcv' } elseif ($PickupAudio) { 'examples/Pickup-performance.vcv' } elseif ($SetupAudio) { 'examples/Setup-performance.vcv' } elseif ($WorkflowsAudio) { 'examples/Editing-workflows-performance.vcv' } elseif ($ConfigAudio) { 'examples/Clock-reset-performance.vcv' } elseif ($RoutingAudio) { 'examples/Complete-routing-performance.vcv' } elseif ($AccumAudio) { 'examples/Accumulator-routing-performance.vcv' } elseif ($TrackAudio) { 'examples/Track-options-performance.vcv' } elseif ($OutputAudio) { 'examples/Output-routing-performance.vcv' } elseif ($ClockAudio) { 'examples/Clock-output-demo.vcv' } elseif ($CvAudio) { 'examples/Stage-CV-performance.vcv' } elseif ($SlideAudio) { 'examples/Slide-performance.vcv' } elseif ($ScaleAudio) { 'examples/Scale-progression.vcv' } elseif ($LoopyAudio) { 'examples/Loopy-performance.vcv' } elseif ($OverrideAudio) { 'examples/Stage-overrides-performance.vcv' } elseif ($SwingAudio) { 'examples\Swing-performance.vcv' } elseif ($ChainAudio) { 'examples\Chain-performance.vcv' } elseif ($PresetAudio) { 'examples\Preset-performance.vcv' } elseif ($Controls) { 'examples\CTRL-performance.vcv' } elseif ($Accumulator) { 'examples\Accumulator-modes.vcv' } elseif ($Variations) { 'examples\Musical-variations.vcv' } elseif ($Musical) { 'examples\Musical-demo.vcv' } elseif ($Aux) { 'examples\Aux-inputs-demo.vcv' } elseif ($Hardware) { 'examples\Hardware-panel-demo.vcv' } elseif ($Modulation) { 'examples\Modulation-demo.vcv' } else { 'examples\Two-voice-demo.vcv' }
        $audioPatch = Get-Content -LiteralPath $audioExample -Raw | ConvertFrom-Json
        if ($OutputAudio -and !$OutputArrangement) {
            # Measure the first scene at a fixed 120 BPM without arrangement resets.
            # The distributed demo retains its complete chain and tempo.
            $routingModule = $audioPatch.modules | Where-Object plugin -eq 'MetropolixLab'
            $routingModule.data.chains.enabled = $false
            $routingModule.params | Where-Object id -eq 0 | ForEach-Object { $_.value = 120 }
            $routingModule.params | Where-Object id -eq 421 | ForEach-Object { $_.value = 0 }
        }
        if ($SwingAudio -or $OverrideAudio -or $LoopyAudio -or $ScaleAudio -or $SlideAudio -or $CvAudio -or $ClockAudio -or $OutputAudio -or $WorkflowsAudio -or $SetupAudio -or $PickupAudio -or $MidiAudio -or $MxAudio) { ($audioPatch.modules | Where-Object { $_.plugin -eq 'MetropolixLab' -and $_.model -eq 'Metropolix' }).params | Where-Object id -eq 1 | ForEach-Object { $_.value = 1 } }
        $audioPatch.modules += @{id=100; plugin='MetropolixLab'; model='AudioQA'; pos=@(100,1)}
        # Record exactly what reaches AUDIO, including the musical demo's stereo mix.
        for ($channel = 0; $channel -lt 2; $channel++) {
            $tap = @($audioPatch.cables | Where-Object { $_.inputModuleId -eq 9 -and $_.inputId -eq $channel })
            if ($tap.Count -ne 1) { throw "Expected one AUDIO cable for channel $channel." }
            $audioPatch.cables += @{id=(100+$channel); outputModuleId=$tap[0].outputModuleId; outputId=$tap[0].outputId; inputModuleId=100; inputId=$channel; color='#777777'}
        }
        if ($MxAudio) {
            for ($outputId=0; $outputId -lt 8; $outputId++) { $audioPatch.cables += @{id=(102+$outputId); outputModuleId=120; outputId=$outputId; inputModuleId=100; inputId=(2+$outputId); color='#e0ac36'} }
            $audioPatch.cables += @{id=110; outputModuleId=19; outputId=0; inputModuleId=100; inputId=10; color='#777777'}
            $audioPatch.cables += @{id=111; outputModuleId=131; outputId=0; inputModuleId=100; inputId=11; color='#777777'}
        }
        if ($ClockAudio) {
            $audioPatch.cables += @{id=102; outputModuleId=1; outputId=4; inputModuleId=100; inputId=2; color='#d76737'}
            $audioPatch.cables += @{id=103; outputModuleId=1; outputId=1; inputModuleId=100; inputId=3; color='#2d8c89'}
        }
        if ($CvAudio -or $OutputAudio) {
            $audioPatch.cables += @{id=102; outputModuleId=1; outputId=5; inputModuleId=100; inputId=2; color='#d76737'}
            $audioPatch.cables += @{id=103; outputModuleId=1; outputId=6; inputModuleId=100; inputId=3; color='#2d8c89'}
        }
        if ($Modulation) {
            $env:METROPOLIX_MOD_QA = '1'
            $audioPatch.cables += @{id=102; outputModuleId=1; outputId=5; inputModuleId=100; inputId=2; color='#d76737'}
            $audioPatch.cables += @{id=103; outputModuleId=1; outputId=6; inputModuleId=100; inputId=3; color='#2d8c89'}
        }
        $patch = Join-Path $output 'audio-test.vcv'
        $audioPatch | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $patch -Encoding utf8NoBOM
    }
    $system = Split-Path -Parent $RackPath
    $arguments = @('-u', ('"' + $profile + '"'), '-s', ('"' + $system + '"'), ('"' + $patch + '"'))
    $process = Start-Process -FilePath $RackPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
    $timeout = if ($Variations) { 180 } elseif ($Display) { 120 } elseif ($Musical) { 60 } else { 45 }
    $elapsed = [System.Diagnostics.Stopwatch]::StartNew()
    while (!$process.WaitForExit(1000)) {
        if ($elapsed.Elapsed.TotalSeconds -gt $timeout) {
            Stop-Process -Id $process.Id
            throw 'UI test timed out; test Rack process stopped.'
        }
    }
    $report = Get-Content -LiteralPath (Join-Path $output 'report.txt') -Raw
    Copy-Item -LiteralPath $qaLog -Destination (Join-Path $output 'rack.log') -Force
    Write-Output $report
    $completion = if ($Presets) { 'PASS PRESET UI QA COMPLETE' } elseif ($Display) { 'PASS DISPLAY QA COMPLETE' } elseif ($Audio) { 'PASS AUDIO QA COMPLETE' } else { 'PASS UI QA COMPLETE' }
    if ($Polish) { $completion = 'PASS UI POLISH QA COMPLETE' }
    if ($Chains) { $completion = 'PASS CHAIN UI QA COMPLETE' }
    if ($Pending) { $completion = 'PASS PENDING UI QA COMPLETE' }
    if ($CtrlEditor) { $completion = 'PASS CTRLEDITOR UI QA COMPLETE' }
    if ($StagePattern) { $completion = 'PASS STAGEPATTERN UI QA COMPLETE' }
    if ($ScaleCopy) { $completion = 'PASS SCALECOPY UI QA COMPLETE' }
    if ($StagePair) { $completion = 'PASS STAGEPAIR UI QA COMPLETE' }
    if ($Timing) { $completion = 'PASS TIMING UI QA COMPLETE' }
    if ($Group) { $completion = 'PASS GROUP UI QA COMPLETE' }
    if ($Storage) { $completion = 'PASS STORAGE UI QA COMPLETE' }
    if ($Order) { $completion = 'PASS ORDER UI QA COMPLETE' }
    if ($Fidelity) { $completion = 'PASS FIDELITY UI QA COMPLETE' }
    if ($Backup) { $completion = 'PASS BACKUP UI QA COMPLETE' }
    if ($Mx) { $completion = 'PASS MX UI QA COMPLETE' }
    if ($Midi) { $completion = 'PASS MIDI UI QA COMPLETE' }
    if ($Pickup) { $completion = 'PASS PICKUP UI QA COMPLETE' }
    if ($SetupOptions) { $completion = 'PASS SETUP UI QA COMPLETE' }
    if ($Workflows) { $completion = 'PASS WORKFLOW UI QA COMPLETE' }
    if ($Config) { $completion = 'PASS CONFIG UI QA COMPLETE' }
    if ($ExtendedRouting) { $completion = 'PASS EXTENDED ROUTING UI QA COMPLETE' }
    if ($AccumRouting) { $completion = 'PASS ACCUM ROUTING UI QA COMPLETE' }
    if ($Tracks) { $completion = 'PASS TRACK UI QA COMPLETE' }
    if ($Outputs) { $completion = 'PASS OUTPUT UI QA COMPLETE' }
    if ($Clock) { $completion = 'PASS CLOCK UI QA COMPLETE' }
    if ($StageCv) { $completion = 'PASS STAGE CV UI QA COMPLETE' }
    if ($Slides) { $completion = 'PASS SLIDE UI QA COMPLETE' }
    if ($Scales) { $completion = 'PASS SCALE UI QA COMPLETE' }
    if ($Loopy) { $completion = 'PASS LOOPY UI QA COMPLETE' }
    if ($Overrides) { $completion = 'PASS OVERRIDE UI QA COMPLETE' }
    if ($Swing) { $completion = 'PASS SWING UI QA COMPLETE' }
    if ($process.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains($completion)) {
        throw 'UI test failed; inspect tmp/rack-qa/log.txt and tmp/qa/report.txt.'
    }
    if ($Presets -or $Chains -or $Swing -or $Overrides -or $Loopy -or $Scales -or $Slides -or $StageCv -or $Clock -or $Outputs -or $Tracks -or $AccumRouting -or $ExtendedRouting -or $Config -or $Workflows -or $SetupOptions -or $Pickup -or $Midi -or $Mx -or $Backup -or $Fidelity -or $Order -or $Storage -or $Group -or $Timing -or $StagePair -or $ScaleCopy -or $Pending -or $CtrlEditor -or $StagePattern) {
        if ($Pending) { $env:METROPOLIX_PENDING_REOPEN_QA = '1' } elseif ($CtrlEditor) { $env:METROPOLIX_CTRLEDITOR_REOPEN_QA = '1' } elseif ($StagePattern) { $env:METROPOLIX_STAGEPATTERN_REOPEN_QA = '1' } elseif ($ScaleCopy) { $env:METROPOLIX_SCALECOPY_REOPEN_QA = '1' } elseif ($StagePair) { $env:METROPOLIX_STAGEPAIR_REOPEN_QA = '1' } elseif ($Timing) { $env:METROPOLIX_TIMING_REOPEN_QA = '1' } elseif ($Group) { $env:METROPOLIX_GROUP_REOPEN_QA = '1' } elseif ($Storage) { $env:METROPOLIX_STORAGE_REOPEN_QA = '1' } elseif ($Order) { $env:METROPOLIX_ORDER_REOPEN_QA = '1' } elseif ($Fidelity) { $env:METROPOLIX_FIDELITY_REOPEN_QA = '1' } elseif ($Backup) { $env:METROPOLIX_BACKUP_REOPEN_QA = '1' } elseif ($Mx) { $env:METROPOLIX_MX_REOPEN_QA = '1' } elseif ($Midi) { $env:METROPOLIX_MIDI_REOPEN_QA = '1' } elseif ($Pickup) { $env:METROPOLIX_PICKUP_REOPEN_QA = '1' } elseif ($SetupOptions) { $env:METROPOLIX_SETUP_REOPEN_QA = '1' } elseif ($Workflows) { $env:METROPOLIX_WORKFLOW_REOPEN_QA = '1' } elseif ($Config) { $env:METROPOLIX_CONFIG_REOPEN_QA = '1' } elseif ($ExtendedRouting) { $env:METROPOLIX_EXTENDED_ROUTING_REOPEN_QA = '1' } elseif ($AccumRouting) { $env:METROPOLIX_ACCUM_ROUTING_REOPEN_QA = '1' } elseif ($Tracks) { $env:METROPOLIX_TRACK_REOPEN_QA = '1' } elseif ($Outputs) { $env:METROPOLIX_OUTPUT_REOPEN_QA = '1' } elseif ($Clock) { $env:METROPOLIX_CLOCK_REOPEN_QA = '1' } elseif ($StageCv) { $env:METROPOLIX_STAGE_CV_REOPEN_QA = '1' } elseif ($Slides) { $env:METROPOLIX_SLIDE_REOPEN_QA = '1' } elseif ($Scales) { $env:METROPOLIX_SCALE_REOPEN_QA = '1' } elseif ($Loopy) { $env:METROPOLIX_LOOPY_REOPEN_QA = '1' } elseif ($Overrides) { $env:METROPOLIX_OVERRIDE_REOPEN_QA = '1' } elseif ($Swing) { $env:METROPOLIX_SWING_REOPEN_QA = '1' } elseif ($Chains) { $env:METROPOLIX_CHAIN_REOPEN_QA = '1' } else { $env:METROPOLIX_PRESET_REOPEN_QA = '1' }
        $arguments[-1] = '"' + (Join-Path $output $(if ($Pending) { 'pending-roundtrip.vcv' } elseif ($CtrlEditor) { 'ctrleditor-roundtrip.vcv' } elseif ($StagePattern) { 'stagepattern-roundtrip.vcv' } elseif ($ScaleCopy) { 'scalecopy-roundtrip.vcv' } elseif ($StagePair) { 'stagepair-roundtrip.vcv' } elseif ($Timing) { 'timing-roundtrip.vcv' } elseif ($Group) { 'group-roundtrip.vcv' } elseif ($Storage) { 'storage-roundtrip.vcv' } elseif ($Order) { 'order-roundtrip.vcv' } elseif ($Fidelity) { 'fidelity-roundtrip.vcv' } elseif ($Backup) { 'backup-roundtrip.vcv' } elseif ($Mx) { 'mx-roundtrip.vcv' } elseif ($Midi) { 'midi-roundtrip.vcv' } elseif ($Pickup) { 'pickup-roundtrip.vcv' } elseif ($SetupOptions) { 'setup-roundtrip.vcv' } elseif ($Workflows) { 'workflow-roundtrip.vcv' } elseif ($Config) { 'config-roundtrip.vcv' } elseif ($ExtendedRouting) { 'extended-routing-roundtrip.vcv' } elseif ($AccumRouting) { 'accum-routing-roundtrip.vcv' } elseif ($Tracks) { 'track-roundtrip.vcv' } elseif ($Outputs) { 'output-roundtrip.vcv' } elseif ($Clock) { 'clock-roundtrip.vcv' } elseif ($StageCv) { 'cv-roundtrip.vcv' } elseif ($Slides) { 'slide-roundtrip.vcv' } elseif ($Scales) { 'scale-roundtrip.vcv' } elseif ($Loopy) { 'loopy-roundtrip.vcv' } elseif ($Overrides) { 'override-roundtrip.vcv' } elseif ($Swing) { 'swing-roundtrip.vcv' } elseif ($Chains) { 'chain-roundtrip.vcv' } else { 'preset-roundtrip.vcv' })) + '"'
        $reopened = Start-Process -FilePath $RackPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
        $elapsed.Restart()
        while (!$reopened.WaitForExit(1000)) {
            if ($elapsed.Elapsed.TotalSeconds -gt 45) {
                Stop-Process -Id $reopened.Id
                throw 'Preset reopen test timed out.'
            }
        }
        $report = Get-Content -LiteralPath (Join-Path $output 'report.txt') -Raw
        $reopenCompletion = if ($Pending) { 'PASS PENDING REOPEN QA COMPLETE' } elseif ($CtrlEditor) { 'PASS CTRLEDITOR REOPEN QA COMPLETE' } elseif ($StagePattern) { 'PASS STAGEPATTERN REOPEN QA COMPLETE' } elseif ($ScaleCopy) { 'PASS SCALECOPY REOPEN QA COMPLETE' } elseif ($StagePair) { 'PASS STAGEPAIR REOPEN QA COMPLETE' } elseif ($Timing) { 'PASS TIMING REOPEN QA COMPLETE' } elseif ($Group) { 'PASS GROUP REOPEN QA COMPLETE' } elseif ($Storage) { 'PASS STORAGE REOPEN QA COMPLETE' } elseif ($Order) { 'PASS ORDER REOPEN QA COMPLETE' } elseif ($Fidelity) { 'PASS FIDELITY REOPEN QA COMPLETE' } elseif ($Backup) { 'PASS BACKUP REOPEN QA COMPLETE' } elseif ($Mx) { 'PASS MX REOPEN QA COMPLETE' } elseif ($Midi) { 'PASS MIDI REOPEN QA COMPLETE' } elseif ($Pickup) { 'PASS PICKUP REOPEN QA COMPLETE' } elseif ($SetupOptions) { 'PASS SETUP REOPEN QA COMPLETE' } elseif ($Workflows) { 'PASS WORKFLOW REOPEN QA COMPLETE' } elseif ($Config) { 'PASS CONFIG REOPEN QA COMPLETE' } elseif ($ExtendedRouting) { 'PASS EXTENDED ROUTING REOPEN QA COMPLETE' } elseif ($AccumRouting) { 'PASS ACCUM ROUTING REOPEN QA COMPLETE' } elseif ($Tracks) { 'PASS TRACK REOPEN QA COMPLETE' } elseif ($Outputs) { 'PASS OUTPUT REOPEN QA COMPLETE' } elseif ($Clock) { 'PASS CLOCK REOPEN QA COMPLETE' } elseif ($StageCv) { 'PASS STAGE CV REOPEN QA COMPLETE' } elseif ($Slides) { 'PASS SLIDE REOPEN QA COMPLETE' } elseif ($Scales) { 'PASS SCALE REOPEN QA COMPLETE' } elseif ($Loopy) { 'PASS LOOPY REOPEN QA COMPLETE' } elseif ($Overrides) { 'PASS OVERRIDE REOPEN QA COMPLETE' } elseif ($Swing) { 'PASS SWING REOPEN QA COMPLETE' } elseif ($Chains) { 'PASS CHAIN REOPEN QA COMPLETE' } else { 'PASS PRESET REOPEN QA COMPLETE' }
        if ($reopened.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains($reopenCompletion)) {
            throw 'Preset banks did not survive an actual Rack file save/reopen.'
        }
        Write-Output $reopenCompletion
    }
    if ($Backup) {
        $env:METROPOLIX_FACTORY_REOPEN_QA = '1'
        $arguments[-1] = '"' + (Join-Path $output 'maintenance-disarmed.vcv') + '"'
        $maintenance = Start-Process -FilePath $RackPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
        $elapsed.Restart()
        while (!$maintenance.WaitForExit(1000)) {
            if ($elapsed.Elapsed.TotalSeconds -gt 45) { Stop-Process -Id $maintenance.Id; throw 'Maintenance reopen test timed out.' }
        }
        $report = Get-Content -LiteralPath (Join-Path $output 'report.txt') -Raw
        if ($maintenance.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains('PASS FACTORY REOPEN QA COMPLETE')) { throw 'Saved maintenance confirmation did not reopen safely.' }
        Write-Output 'PASS FACTORY REOPEN QA COMPLETE'
    }
    if ($LoopyAudio -and !$report.Contains('PASS LOOPY PERFORMANCE COMPLETE')) { throw 'Loopy audio demonstration did not complete.' }
    if ($Aux -and !$report.Contains('PASS AUX WIRING COMPLETE')) { throw 'AUX wiring verification did not finish.' }
    if ($PresetAudio -and !$report.Contains('PASS PRESET PERFORMANCE COMPLETE')) { throw 'Preset transitions did not finish.' }
    if ($SlideAudio -and !$report.Contains('PASS SLIDE PERFORMANCE COMPLETE')) { throw 'Slide types were not observed during actual audio playback.' }
    if ($ScaleAudio -and !$report.Contains('PASS SCALE PERFORMANCE COMPLETE')) { throw 'Scale progression was not observed in actual playback.' }
    if ($ChainAudio -and !$report.Contains('PASS CHAIN PERFORMANCE COMPLETE')) { throw 'Chain transitions did not finish.' }
    if ($OverrideAudio -and !$report.Contains('PASS OVERRIDE PERFORMANCE COMPLETE')) { throw 'Override demo transitions did not finish.' }
    if ($SwingAudio -and !$report.Contains('PASS SWING PERFORMANCE COMPLETE')) { throw 'Swing values were not observed in real playback.' }
    if ($Controls -and !$report.Contains('PASS CTRL PERFORMANCE COMPLETE')) { throw 'CTRL performance verification did not finish.' }
    if ($MxAudio -and !$report.Contains('PASS MX AUDIO SIGNALS')) { throw 'MX stage gates were not observed through actual cables.' }
    if ($ClockAudio -and !$report.Contains('PASS CLOCK AUDIO TIMING')) { throw 'Clock output audio timing did not pass' }
    if ($RoutingAudio -and !$report.Contains('PASS EXTENDED ROUTING PERFORMANCE COMPLETE')) { throw 'Extended routes were not observed' }
    if ($AccumAudio -and !$report.Contains('PASS ACCUM ROUTING PERFORMANCE COMPLETE')) { throw 'Accumulator routes were not observed in audio playback' }
    if ($TrackAudio -and !$report.Contains('PASS TRACK PERFORMANCE COMPLETE')) { throw 'Track options were not observed' }
    if ($OutputArrangement -and !$report.Contains('PASS OUTPUT ARRANGEMENT COMPLETE')) { throw 'Output scenes were not observed' }
    if ($OutputAudio -and !$report.Contains('PASS OUTPUT AUDIO ROUTING')) { throw 'Output routing audio timing did not pass' }
    if ($CvAudio -and !$report.Contains('PASS STAGE CV PERFORMANCE COMPLETE')) { throw 'Stage CV performance did not finish' }
} finally {
    Remove-Item Env:METROPOLIX_PENDING_UI_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_PENDING_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_CTRLEDITOR_UI_QA, Env:METROPOLIX_CTRLEDITOR_REOPEN_QA, Env:METROPOLIX_STAGEPATTERN_UI_QA, Env:METROPOLIX_STAGEPATTERN_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SCALECOPY_UI_QA, Env:METROPOLIX_SCALECOPY_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_STAGEPAIR_UI_QA, Env:METROPOLIX_STAGEPAIR_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_TIMING_UI_QA, Env:METROPOLIX_TIMING_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_GROUP_UI_QA, Env:METROPOLIX_GROUP_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_STORAGE_UI_QA, Env:METROPOLIX_STORAGE_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_ORDER_UI_QA, Env:METROPOLIX_ORDER_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_FIDELITY_UI_QA, Env:METROPOLIX_FIDELITY_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_BACKUP_UI_QA, Env:METROPOLIX_BACKUP_REOPEN_QA, Env:METROPOLIX_MX_AUDIO_QA, Env:METROPOLIX_MX_UI_QA, Env:METROPOLIX_MX_REOPEN_QA, Env:METROPOLIX_MIDI_UI_QA, Env:METROPOLIX_MIDI_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_PICKUP_UI_QA, Env:METROPOLIX_PICKUP_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SETUP_OPTIONS_QA, Env:METROPOLIX_SETUP_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_WORKFLOW_UI_QA, Env:METROPOLIX_WORKFLOW_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_CONFIG_UI_QA, Env:METROPOLIX_CONFIG_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_EXTENDED_ROUTING_UI_QA, Env:METROPOLIX_EXTENDED_ROUTING_REOPEN_QA, Env:METROPOLIX_EXTENDED_ROUTING_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_ACCUM_ROUTING_UI_QA, Env:METROPOLIX_ACCUM_ROUTING_REOPEN_QA, Env:METROPOLIX_ACCUM_ROUTING_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_TRACK_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_TRACK_UI_QA, Env:METROPOLIX_TRACK_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_OUTPUT_ARRANGEMENT_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_OUTPUT_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_OUTPUT_UI_QA, Env:METROPOLIX_OUTPUT_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_CLOCK_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_CLOCK_UI_QA, Env:METROPOLIX_CLOCK_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_STAGE_CV_AUDIO_QA, Env:METROPOLIX_STAGE_CV_UI_QA, Env:METROPOLIX_STAGE_CV_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SLIDE_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SLIDE_UI_QA, Env:METROPOLIX_SLIDE_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SCALE_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SCALE_UI_QA, Env:METROPOLIX_SCALE_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_LOOPY_UI_QA, Env:METROPOLIX_LOOPY_REOPEN_QA, Env:METROPOLIX_LOOPY_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_QA_DIR -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_MUSICAL_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_FACTORY_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_VARIATIONS_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_MOD_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_DISPLAY_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_AUX_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_CTRL_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_PRESET_UI_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_PRESET_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_PRESET_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_POLISH_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_CHAIN_UI_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_CHAIN_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_CHAIN_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SWING_UI_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SWING_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_SWING_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_OVERRIDE_UI_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_OVERRIDE_REOPEN_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_OVERRIDE_AUDIO_QA -ErrorAction SilentlyContinue
    Pop-Location
}
