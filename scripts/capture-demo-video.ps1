param([ValidateRange(1,180)][int]$Seconds = 80,
      [ValidateSet('Musical-variations','Neon-Transit','Broken-Orbit','Pulse-Pressure','Pulse-Pressure-Sidechain')][string]$Example = 'Musical-variations',
      [string]$RackPath = 'C:\Program Files\VCV\Rack2Free\Rack.exe')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
if (Get-Process Rack -ErrorAction SilentlyContinue) { throw 'Close Rack before recording the isolated video profile.' }
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
Push-Location -LiteralPath $projectRoot
try {
    & "$env:LOCALAPPDATA\MetropolixDev\msys64\usr\bin\bash.exe" -lc 'make ui-test-build'
    if ($LASTEXITCODE -ne 0) { throw 'Capture build failed.' }
    $take = 'video-' + (Get-Date -Format 'yyyyMMdd-HHmmss')
    $output = Join-Path $projectRoot ('tmp\' + $take)
    $profile = Join-Path $output 'profile'
    $plugin = Join-Path $profile 'plugins-win-x64\MetropolixLab'
    New-Item -ItemType Directory -Force -Path $plugin | Out-Null
    Copy-Item -LiteralPath 'build\ui-test\plugin.dll', 'plugin.json' -Destination $plugin
    Copy-Item -LiteralPath 'res' -Destination $plugin -Recurse
    $fundamental = Join-Path $projectRoot 'tmp\rack-user\plugins-win-x64\Fundamental'
    if (!(Test-Path -LiteralPath $fundamental)) { throw 'Install VCV Free in the development profile first.' }
    Copy-Item -LiteralPath $fundamental -Destination (Join-Path $profile 'plugins-win-x64') -Recurse
    $manifest = Get-Content plugin.json -Raw | ConvertFrom-Json
    $manifest.modules += @{slug='AudioQA'; name='Internal audio test'}
    $manifest | ConvertTo-Json -Depth 10 | Set-Content (Join-Path $plugin 'plugin.json') -Encoding utf8NoBOM
    @{windowSize=@(1800,1050); windowPos=@(10,10); sampleRate=48000; frameRateLimit=60;
      cableOpacity=0.12; tooltips=$false; showTipsOnLaunch=$false; autoCheckUpdates=$false} |
        ConvertTo-Json | Set-Content (Join-Path $profile 'settings.json') -Encoding utf8NoBOM
    $performance = $Example -ne 'Musical-variations'
    $patch = Get-Content (Join-Path 'examples' ($Example + '.vcv')) -Raw | ConvertFrom-Json
    foreach ($slug in @($patch.modules.plugin | Sort-Object -Unique | Where-Object { $_ -notin @('Core','Fundamental','MetropolixLab') })) {
        $installed = Join-Path $env:LOCALAPPDATA ('Rack2/plugins-win-x64/' + $slug)
        if (!(Test-Path -LiteralPath (Join-Path $installed 'plugin.json'))) { throw "Required installed plugin missing: $slug" }
        Copy-Item -LiteralPath $installed -Destination (Join-Path $profile 'plugins-win-x64') -Recurse
    }
    $patch.zoom = $(if ($performance) { 1.0 } else { 2.25 })
    $patch.gridOffset = @(-1,-.05)
    foreach ($module in $patch.modules) {
        if ($module.id -eq 1) {
            ($module.params | Where-Object id -eq 1).value = 0
            $module.data.uiHome = 0
        } elseif (!$performance) {
            # Keep sound generators patched but outside the close-up camera.
            $module.pos[1] += 10
        }
    }
    $patch.modules += @{id=100; plugin='MetropolixLab'; model='AudioQA'; pos=@(100,10)}
    foreach ($channel in 0,1) {
        $tap = @($patch.cables | Where-Object { $_.inputModuleId -eq 9 -and $_.inputId -eq $channel })
        if ($tap.Count -ne 1) { throw 'Unexpected AUDIO routing.' }
        $patch.cables += @{id=(100+$channel); outputModuleId=$tap[0].outputModuleId; outputId=$tap[0].outputId; inputModuleId=100; inputId=$channel; color='#777777'}
    }
    if ($performance) {
        # Record actual pitch/gate voltages and voice peaks for explanatory overlays.
        $taps = @(@(1,0),@(1,1),@(1,2),@(1,3),@(1,5),@(1,6),@(1,4),@(3,0),@(6,0),@(23,0))
        if ($Example -eq 'Pulse-Pressure') { $taps = @(@(1,0),@(1,1),@(1,2),@(1,3),@(2,0),@(2,1),@(2,2),@(4,2),@(7,2),@(23,0)) }
        if ($Example -eq 'Pulse-Pressure-Sidechain') { $taps = @(@(1,0),@(1,1),@(1,2),@(1,3),@(2,0),@(2,1),@(2,2),@(15,0),@(31,1),@(1,6)) }
        for ($t = 0; $t -lt $taps.Count; $t++) {
            $patch.cables += @{id=(102+$t);outputModuleId=$taps[$t][0];outputId=$taps[$t][1];inputModuleId=100;inputId=(2+$t);color='#777777'}
        }
        $env:METROPOLIX_PERFORMANCE_VIDEO = '1'
        $settings = Get-Content (Join-Path $profile 'settings.json') -Raw | ConvertFrom-Json
        $settings.cableOpacity = 0.3
        $settings | ConvertTo-Json | Set-Content (Join-Path $profile 'settings.json') -Encoding utf8NoBOM
    }
    if ($Example -like 'Pulse-Pressure*') { $env:METROPOLIX_CLUB_VIDEO = '1' }
    $patchFile = Join-Path $output 'capture.vcv'
    $patch | ConvertTo-Json -Depth 30 | Set-Content $patchFile -Encoding utf8NoBOM
    @{example=$Example; seconds=$Seconds; performance=$performance} | ConvertTo-Json | Set-Content (Join-Path $output 'take.json') -Encoding utf8NoBOM
    '' | Set-Content (Join-Path $output 'report.txt') -Encoding utf8NoBOM
    $env:METROPOLIX_QA_DIR = $output.Replace('\','/')
    $env:METROPOLIX_VIDEO_QA = '1'
    $env:METROPOLIX_VIDEO_SECONDS = $Seconds.ToString()
    $system = Split-Path -Parent $RackPath
    $arguments = @('-u', ('"'+$profile+'"'), '-s', ('"'+$system+'"'), ('"'+$patchFile+'"'))
    $process = Start-Process -FilePath $RackPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
    Write-Output "Recording $Seconds seconds to $output (PID $($process.Id))"
    $elapsed = [System.Diagnostics.Stopwatch]::StartNew()
    while (!$process.WaitForExit(1000)) {
        if ($elapsed.Elapsed.TotalSeconds -gt [Math]::Max(120, $Seconds*10)) {
            Stop-Process -Id $process.Id
            throw "Capture timed out. Inspect $output."
        }
    }
    $report = Get-Content (Join-Path $output 'report.txt') -Raw
    Write-Output $report
    if ($process.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains('PASS AUDIO QA COMPLETE')) { throw 'Capture audio check failed.' }
    $output | Set-Content 'tmp\latest-video-capture.txt' -Encoding utf8NoBOM
    Write-Output "Completed capture: $output"
} finally {
    Remove-Item Env:METROPOLIX_QA_DIR, Env:METROPOLIX_VIDEO_QA, Env:METROPOLIX_VIDEO_SECONDS, Env:METROPOLIX_PERFORMANCE_VIDEO, Env:METROPOLIX_CLUB_VIDEO -ErrorAction SilentlyContinue
    Pop-Location
}
