param([string]$RackPath = 'C:\Program Files\VCV\Rack2Free\Rack.exe',
      [ValidateSet('Hybrid-demo', 'Two-voice-demo', 'Modulation-demo', 'Hardware-panel-demo', 'Aux-inputs-demo', 'Musical-demo', 'Musical-variations', 'Accumulator-modes', 'CTRL-performance', 'Preset-performance')][string]$Example = 'Hardware-panel-demo')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$profile = Join-Path $projectRoot 'tmp\rack-user'
$plugin = Join-Path $profile 'plugins-win-x64\MetropolixLab'
if (!(Test-Path -LiteralPath $RackPath)) { throw "Rack executable not found: $RackPath" }
if (Get-Process Rack -ErrorAction SilentlyContinue) {
    throw 'Close Rack first; Rack on Windows permits only one instance.'
}
New-Item -ItemType Directory -Force -Path $plugin | Out-Null
Copy-Item -LiteralPath (Join-Path $projectRoot 'plugin.dll'), (Join-Path $projectRoot 'plugin.json'), (Join-Path $projectRoot 'README.md') -Destination $plugin -Force
Copy-Item -LiteralPath (Join-Path $projectRoot 'res') -Destination $plugin -Recurse -Force
$settings = Join-Path $profile 'settings.json'
if (!(Test-Path -LiteralPath $settings)) {
    '{"windowSize":[1500,780],"windowPos":[60,60],"showTipsOnLaunch":false,"autoCheckUpdates":false}' | Set-Content -LiteralPath $settings -Encoding utf8NoBOM
}
$patch = Join-Path $projectRoot ("examples\$Example.vcv")
$system = Split-Path -Parent $RackPath
$arguments = @('-u', ('"' + $profile + '"'), '-s', ('"' + $system + '"'), ('"' + $patch + '"'))
$process = Start-Process -FilePath $RackPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
Write-Output "Development Rack process: $($process.Id)"
Write-Output "Development profile: $profile"
