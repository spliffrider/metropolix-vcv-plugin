param([string]$RackPath = 'C:\Program Files\VCV\Rack2Free\Rack.exe', [switch]$Audio, [switch]$Modulation, [switch]$Hardware, [switch]$Display, [switch]$Aux, [switch]$Musical, [switch]$Variations, [switch]$Accumulator)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
if ($Hardware) { $Modulation = $true }
if ($Aux) { $Modulation = $true }
if ($Modulation) { $Audio = $true }
if ($Variations) { $Musical = $true }
if ($Musical) { $Audio = $true }
if ($Accumulator) { $Audio = $true }
if (Get-Process Rack -ErrorAction SilentlyContinue) { throw 'Close Rack before the UI test.' }
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
Push-Location -LiteralPath $projectRoot
try {
    & "$env:LOCALAPPDATA\MetropolixDev\msys64\usr\bin\bash.exe" -lc 'make ui-test-build'
    if ($LASTEXITCODE -ne 0) { throw 'UI test build failed.' }
    $profile = Join-Path $projectRoot 'tmp\rack-qa'
    $output = Join-Path $projectRoot $(if ($Display) { 'tmp\display-qa' } elseif ($Accumulator) { 'tmp\accumulator-qa' } elseif ($Variations) { 'tmp\variations-qa' } elseif ($Musical) { 'tmp\musical-qa' } elseif ($Aux) { 'tmp\aux-qa' } elseif ($Hardware) { 'tmp\hardware-qa' } elseif ($Modulation) { 'tmp\mod-qa' } elseif ($Audio) { 'tmp\audio-qa' } else { 'tmp\qa' })
    $plugin = Join-Path $profile 'plugins-win-x64\MetropolixLab'
    New-Item -ItemType Directory -Force -Path $plugin, $output | Out-Null
    Copy-Item -LiteralPath 'build\ui-test\plugin.dll', 'plugin.json', 'README.md' -Destination $plugin -Force
    Copy-Item -LiteralPath 'res' -Destination $plugin -Recurse -Force
    $manifest = Get-Content -LiteralPath 'plugin.json' -Raw | ConvertFrom-Json
    $manifest.modules += @{slug='AudioQA'; name='Internal audio test'}
    $manifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $plugin 'plugin.json') -Encoding utf8NoBOM
    '{"windowSize":[1600,950],"windowPos":[30,30],"sampleRate":48000,"showTipsOnLaunch":false,"autoCheckUpdates":false}' | Set-Content -LiteralPath (Join-Path $profile 'settings.json') -Encoding utf8NoBOM
    '' | Set-Content -LiteralPath (Join-Path $output 'report.txt') -Encoding utf8NoBOM
    $env:METROPOLIX_QA_DIR = $output.Replace('\', '/')
    $patch = Join-Path $projectRoot 'examples\Hybrid-demo.vcv'
    if ($Display) {
        $env:METROPOLIX_DISPLAY_QA = '1'
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
        $audioExample = if ($Accumulator) { 'examples\Accumulator-modes.vcv' } elseif ($Variations) { 'examples\Musical-variations.vcv' } elseif ($Musical) { 'examples\Musical-demo.vcv' } elseif ($Aux) { 'examples\Aux-inputs-demo.vcv' } elseif ($Hardware) { 'examples\Hardware-panel-demo.vcv' } elseif ($Modulation) { 'examples\Modulation-demo.vcv' } else { 'examples\Two-voice-demo.vcv' }
        $audioPatch = Get-Content -LiteralPath $audioExample -Raw | ConvertFrom-Json
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
    Write-Output $report
    $completion = if ($Display) { 'PASS DISPLAY QA COMPLETE' } elseif ($Audio) { 'PASS AUDIO QA COMPLETE' } else { 'PASS UI QA COMPLETE' }
    if ($process.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains($completion)) {
        throw 'UI test failed; inspect tmp/rack-qa/log.txt and tmp/qa/report.txt.'
    }
    if ($Aux -and !$report.Contains('PASS AUX WIRING COMPLETE')) { throw 'AUX wiring verification did not finish.' }
} finally {
    Remove-Item Env:METROPOLIX_QA_DIR -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_AUDIO_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_MUSICAL_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_VARIATIONS_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_MOD_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_DISPLAY_QA -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_AUX_QA -ErrorAction SilentlyContinue
    Pop-Location
}
