param([string]$RackPath = 'C:\Program Files\VCV\Rack2Free\Rack.exe', [switch]$Audio, [switch]$Modulation, [switch]$Hardware, [switch]$Display, [switch]$Aux, [switch]$Musical, [switch]$Variations, [switch]$Accumulator, [switch]$Controls, [switch]$Presets, [switch]$PresetAudio, [switch]$Polish, [switch]$Chains, [switch]$ChainAudio, [switch]$Swing, [switch]$SwingAudio)
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
    if ($Swing) { $output = Join-Path $projectRoot 'tmp\swing-ui-qa'; $env:METROPOLIX_SWING_UI_QA = '1' }
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
        $audioExample = if ($SwingAudio) { 'examples\Swing-performance.vcv' } elseif ($ChainAudio) { 'examples\Chain-performance.vcv' } elseif ($PresetAudio) { 'examples\Preset-performance.vcv' } elseif ($Controls) { 'examples\CTRL-performance.vcv' } elseif ($Accumulator) { 'examples\Accumulator-modes.vcv' } elseif ($Variations) { 'examples\Musical-variations.vcv' } elseif ($Musical) { 'examples\Musical-demo.vcv' } elseif ($Aux) { 'examples\Aux-inputs-demo.vcv' } elseif ($Hardware) { 'examples\Hardware-panel-demo.vcv' } elseif ($Modulation) { 'examples\Modulation-demo.vcv' } else { 'examples\Two-voice-demo.vcv' }
        $audioPatch = Get-Content -LiteralPath $audioExample -Raw | ConvertFrom-Json
        if ($SwingAudio) { ($audioPatch.modules | Where-Object plugin -eq 'MetropolixLab').params | Where-Object id -eq 1 | ForEach-Object { $_.value = 1 } }
        $audioPatch.modules += @{id=100; plugin='MetropolixLab'; model='AudioQA'; pos=@(100,1)}
        # Record exactly what reaches AUDIO, including the musical demo's stereo mix.
        for ($channel = 0; $channel -lt 2; $channel++) {
            $tap = @($audioPatch.cables | Where-Object { $_.inputModuleId -eq 9 -and $_.inputId -eq $channel })
            if ($tap.Count -ne 1) { throw "Expected one AUDIO cable for channel $channel." }
            $audioPatch.cables += @{id=(100+$channel); outputModuleId=$tap[0].outputModuleId; outputId=$tap[0].outputId; inputModuleId=100; inputId=$channel; color='#777777'}
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
    $timeout = if ($Variations) { 180 } elseif ($Musical) { 60 } else { 45 }
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
    if ($Swing) { $completion = 'PASS SWING UI QA COMPLETE' }
    if ($process.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains($completion)) {
        throw 'UI test failed; inspect tmp/rack-qa/log.txt and tmp/qa/report.txt.'
    }
    if ($Presets -or $Chains -or $Swing) {
        if ($Swing) { $env:METROPOLIX_SWING_REOPEN_QA = '1' } elseif ($Chains) { $env:METROPOLIX_CHAIN_REOPEN_QA = '1' } else { $env:METROPOLIX_PRESET_REOPEN_QA = '1' }
        $arguments[-1] = '"' + (Join-Path $output $(if ($Swing) { 'swing-roundtrip.vcv' } elseif ($Chains) { 'chain-roundtrip.vcv' } else { 'preset-roundtrip.vcv' })) + '"'
        $reopened = Start-Process -FilePath $RackPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
        $elapsed.Restart()
        while (!$reopened.WaitForExit(1000)) {
            if ($elapsed.Elapsed.TotalSeconds -gt 45) {
                Stop-Process -Id $reopened.Id
                throw 'Preset reopen test timed out.'
            }
        }
        $report = Get-Content -LiteralPath (Join-Path $output 'report.txt') -Raw
        $reopenCompletion = if ($Swing) { 'PASS SWING REOPEN QA COMPLETE' } elseif ($Chains) { 'PASS CHAIN REOPEN QA COMPLETE' } else { 'PASS PRESET REOPEN QA COMPLETE' }
        if ($reopened.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains($reopenCompletion)) {
            throw 'Preset banks did not survive an actual Rack file save/reopen.'
        }
        Write-Output $reopenCompletion
    }
    if ($Aux -and !$report.Contains('PASS AUX WIRING COMPLETE')) { throw 'AUX wiring verification did not finish.' }
    if ($PresetAudio -and !$report.Contains('PASS PRESET PERFORMANCE COMPLETE')) { throw 'Preset transitions did not finish.' }
    if ($ChainAudio -and !$report.Contains('PASS CHAIN PERFORMANCE COMPLETE')) { throw 'Chain transitions did not finish.' }
    if ($SwingAudio -and !$report.Contains('PASS SWING PERFORMANCE COMPLETE')) { throw 'Swing values were not observed in real playback.' }
    if ($Controls -and !$report.Contains('PASS CTRL PERFORMANCE COMPLETE')) { throw 'CTRL performance verification did not finish.' }
} finally {
    Remove-Item Env:METROPOLIX_QA_DIR -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_MUSICAL_QA -ErrorAction SilentlyContinue
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
    Pop-Location
}
