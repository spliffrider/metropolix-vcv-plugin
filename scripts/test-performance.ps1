param([string]$RackPath = 'C:\Program Files\VCV\Rack2Free\Rack.exe')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
if (Get-Process Rack -ErrorAction SilentlyContinue) { throw 'Close Rack before the isolated performance UI test.' }
$env:CHERE_INVOKING = 'yes'
$env:MSYSTEM = 'MINGW64'
Push-Location -LiteralPath $projectRoot
try {
    & "$env:LOCALAPPDATA\MetropolixDev\msys64\usr\bin\bash.exe" -lc 'make ui-test-build'
    if ($LASTEXITCODE -ne 0) { throw 'Performance UI test build failed.' }
    $output = Join-Path $projectRoot ('tmp/performance-qa-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
    $profile = Join-Path $output 'profile'
    $plugin = Join-Path $profile 'plugins-win-x64/MetropolixLab'
    New-Item -ItemType Directory -Force -Path $plugin | Out-Null
    Copy-Item -LiteralPath 'build/ui-test/plugin.dll','plugin.json' -Destination $plugin
    $manifest=Get-Content 'plugin.json' -Raw | ConvertFrom-Json
    $manifest.modules+=@{slug='AudioQA';name='Internal audio test'}
    $manifest | ConvertTo-Json -Depth 12 | Set-Content (Join-Path $plugin 'plugin.json') -Encoding utf8NoBOM
    Copy-Item -LiteralPath 'res' -Destination $plugin -Recurse
    @{windowSize=@(1600,950);windowPos=@(20,20);sampleRate=48000;showTipsOnLaunch=$false;autoCheckUpdates=$false} |
        ConvertTo-Json | Set-Content (Join-Path $profile 'settings.json') -Encoding utf8NoBOM
    $patch = Get-Content 'examples/Hardware-panel-demo.vcv' -Raw | ConvertFrom-Json
    $patch.modules = @($patch.modules | Where-Object plugin -eq 'MetropolixLab')
    $patch.cables = @();$patch.zoom=1.3;$patch.gridOffset=@(-.2,-.1)
    $fixture = Join-Path $output 'fixture.vcv'
    $patch | ConvertTo-Json -Depth 40 | Set-Content $fixture -Encoding utf8NoBOM
    $reportPath = Join-Path $output 'report.txt'
    '' | Set-Content $reportPath -Encoding utf8NoBOM
    $env:METROPOLIX_QA_DIR = $output.Replace('\','/')
    $env:METROPOLIX_PERFORMANCE_UI_QA = '1'
    foreach ($phase in @('initial','reopen')) {
        if ($phase -eq 'reopen') { $env:METROPOLIX_PERFORMANCE_REOPEN_QA='1';$fixture=Join-Path $output 'performance-roundtrip.vcv' }
        $arguments=@('-u',('"'+$profile+'"'),'-s',('"'+(Split-Path -Parent $RackPath)+'"'),('"'+$fixture+'"'))
        $process=Start-Process -FilePath $RackPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
        $elapsed=[Diagnostics.Stopwatch]::StartNew()
        while (!$process.WaitForExit(1000)) {
            if ($elapsed.Elapsed.TotalSeconds -gt 45) { Stop-Process -Id $process.Id;throw "Performance $phase timed out: $output" }
        }
        $report=Get-Content $reportPath -Raw
        $marker=if ($phase -eq 'initial') {'PASS PERFORMANCE UI QA COMPLETE'} else {'PASS PERFORMANCE REOPEN QA COMPLETE'}
        if ($process.ExitCode -ne 0 -or $report.Contains('FAIL') -or !$report.Contains($marker)) { Write-Output $report;throw "Performance $phase failed: $output" }
    }
    Write-Output $report
    Write-Output "Performance evidence: $output"
} finally {
    Remove-Item Env:METROPOLIX_QA_DIR,Env:METROPOLIX_PERFORMANCE_UI_QA,Env:METROPOLIX_PERFORMANCE_REOPEN_QA -ErrorAction SilentlyContinue
    Pop-Location
}
