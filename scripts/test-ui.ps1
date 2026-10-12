param([string]$RackPath = 'C:\Program Files\VCV\Rack2Free\Rack.exe', [switch]$Audio)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
if (Get-Process Rack -ErrorAction SilentlyContinue) { throw 'Close Rack before the UI test.' }
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
Push-Location -LiteralPath $projectRoot
try {
    & "$env:LOCALAPPDATA\MetropolixDev\msys64\usr\bin\bash.exe" -lc 'make ui-test-build'
    if ($LASTEXITCODE -ne 0) { throw 'UI test build failed.' }
    $profile = Join-Path $projectRoot 'tmp\rack-qa'
    $output = Join-Path $projectRoot $(if ($Audio) { 'tmp\audio-qa' } else { 'tmp\qa' })
    $plugin = Join-Path $profile 'plugins-win-x64\MetropolixLab'
    New-Item -ItemType Directory -Force -Path $plugin, $output | Out-Null
    Copy-Item -LiteralPath 'build\ui-test\plugin.dll', 'plugin.json', 'README.md' -Destination $plugin -Force
    $manifest = Get-Content -LiteralPath 'plugin.json' -Raw | ConvertFrom-Json
    $manifest.modules += @{slug='AudioQA'; name='Internal audio test'}
    $manifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $plugin 'plugin.json') -Encoding utf8NoBOM
    '{"windowSize":[1600,950],"windowPos":[30,30],"sampleRate":48000,"showTipsOnLaunch":false,"autoCheckUpdates":false}' | Set-Content -LiteralPath (Join-Path $profile 'settings.json') -Encoding utf8NoBOM
    '' | Set-Content -LiteralPath (Join-Path $output 'report.txt') -Encoding utf8NoBOM
    $env:METROPOLIX_QA_DIR = $output.Replace('\', '/')
    $patch = Join-Path $projectRoot 'examples\Hybrid-demo.vcv'
    if ($Audio) {
        $env:METROPOLIX_AUDIO_QA = '1'
        $audioPatch = Get-Content -LiteralPath 'examples\Two-voice-demo.vcv' -Raw | ConvertFrom-Json
        $audioPatch.modules += @{id=100; plugin='MetropolixLab'; model='AudioQA'; pos=@(100,1)}
        $audioPatch.cables += @{id=100; outputModuleId=5; outputId=0; inputModuleId=100; inputId=0; color='#777777'}
        $audioPatch.cables += @{id=101; outputModuleId=8; outputId=0; inputModuleId=100; inputId=1; color='#777777'}
        $patch = Join-Path $output 'audio-test.vcv'
        $audioPatch | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $patch -Encoding utf8NoBOM
    }
    $system = Split-Path -Parent $RackPath
    $arguments = @('-u', ('"' + $profile + '"'), '-s', ('"' + $system + '"'), ('"' + $patch + '"'))
    $process = Start-Process -FilePath $RackPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
    if (!$process.WaitForExit(45000)) {
        Stop-Process -Id $process.Id
        throw 'UI test timed out; test Rack process stopped.'
    }
    $report = Get-Content -LiteralPath (Join-Path $output 'report.txt') -Raw
    Write-Output $report
    $completion = if ($Audio) { 'PASS AUDIO QA COMPLETE' } else { 'PASS UI QA COMPLETE' }
    if ($process.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains($completion)) {
        throw 'UI test failed; inspect tmp/rack-qa/log.txt and tmp/qa/report.txt.'
    }
} finally {
    Remove-Item Env:METROPOLIX_QA_DIR -ErrorAction SilentlyContinue
    Remove-Item Env:METROPOLIX_AUDIO_QA -ErrorAction SilentlyContinue
    Pop-Location
}
