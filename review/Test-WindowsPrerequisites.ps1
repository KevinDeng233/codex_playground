# Read-only host checks for Windows 11 AirPods evaluation.
# Does not install, pair, connect, change services, or launch the application.
# Only the JSON output file is written. No tokens, addresses, or device IDs are collected.
[CmdletBinding()]
param(
    [ValidateNotNullOrEmpty()][string]$DeviceName = 'AirPods',
    [string]$AppDirectory,
    [string]$OutputPath = (Join-Path (Get-Location) 'airpods-preflight.json')
)
$ErrorActionPreference = 'Stop'
if ([Environment]::OSVersion.Platform -ne [PlatformID]::Win32NT) {
    throw 'Run this script on the Windows 11 computer with the Bluetooth adapter.'
}
$osKey = Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion'
$report = [ordered]@{
    schemaVersion = 1
    collectedAtUtc = [DateTime]::UtcNow.ToString('o')
    windowsBuild = [string]$osKey.CurrentBuildNumber
    windowsDisplayVersion = [string]$osKey.DisplayVersion
    windows11 = ([int]$osKey.CurrentBuildNumber -ge 22000)
    os64Bit = [Environment]::Is64BitOperatingSystem
    process64Bit = [Environment]::Is64BitProcess
    nativeArchitecture = 'unknown'
    bluetoothService = 'not-queried'
    bluetoothEnumeration = 'not-queried'
    bluetoothRadioCount = 0
    matchingDevices = @()
    airPodsApplicationRunning = [bool](Get-Process -Name AirPodsDesktop -ErrorAction SilentlyContinue)
    buildTools = [ordered]@{}
    package = $null
    note = 'PnP status is not a live Bluetooth connection or BLE scanning test.'
}
try {
    $processor = Get-CimInstance Win32_Processor | Select-Object -First 1
    $report.nativeArchitecture = switch ([int]$processor.Architecture) {
        9 { 'x64' }
        12 { 'ARM64' }
        0 { 'x86' }
        default { 'other' }
    }
} catch { $report.nativeArchitecture = 'query-unavailable' }
try { $report.bluetoothService = [string](Get-Service bthserv).Status }
catch { $report.bluetoothService = 'query-unavailable' }
try {
    $devices = @(Get-PnpDevice -Class Bluetooth -PresentOnly)
    $pattern = '*' + [WildcardPattern]::Escape($DeviceName) + '*'
    $report.matchingDevices = @($devices | Where-Object { $_.FriendlyName -like $pattern } |
        ForEach-Object {
            [ordered]@{ name = $_.FriendlyName; pnpStatus = [string]$_.Status }
        })
    foreach ($device in $devices) {
        $service = Get-PnpDeviceProperty -InstanceId $device.InstanceId `
            -KeyName DEVPKEY_Device_Service -ErrorAction SilentlyContinue
        if ($service.Data -eq 'BTHUSB') { $report.bluetoothRadioCount++ }
    }
    $report.bluetoothEnumeration = 'ok'
} catch { $report.bluetoothEnumeration = 'query-unavailable' }
foreach ($tool in @('cmake', 'ctest', 'cl', 'git')) {
    $report.buildTools[$tool] = [bool](Get-Command $tool -ErrorAction SilentlyContinue)
}
# cl.exe may be absent from PATH even when Visual Studio is installed.
$report.buildTools['visualStudioInstallerPresent'] = Test-Path `
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if ($AppDirectory) {
    $root = (Resolve-Path -LiteralPath $AppDirectory).Path
    $required = @('AirPodsDesktop.exe', 'Qt6Core.dll', 'Qt6Gui.dll', 'Qt6Widgets.dll',
        'platforms/qwindows.dll', 'multimedia/ffmpegmediaplugin.dll')
    $missing = @($required | Where-Object { -not (Test-Path -LiteralPath (Join-Path $root $_)) })
    $app = Join-Path $root 'AirPodsDesktop.exe'
    $report.package = [ordered]@{
        missingRequiredFiles = $missing
        executableSha256 = if (Test-Path -LiteralPath $app) {
            (Get-FileHash -LiteralPath $app -Algorithm SHA256).Hash
        } else { $null }
    }
}
$json = $report | ConvertTo-Json -Depth 8
Set-Content -LiteralPath $OutputPath -Value $json -Encoding UTF8
Write-Output $json
Write-Host 'Preflight saved. This does not verify live AirPods data or change Bluetooth settings.'
