# Package a previously built and tested deployment. Never installs or runs the application.
[CmdletBinding()]
param(
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '..\Build\Evaluation'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\Build\Artifacts')
)
$ErrorActionPreference = 'Stop'
$sourceRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$binary = (Resolve-Path (Join-Path $BuildDirectory 'Binary')).Path
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$output = (Resolve-Path -LiteralPath $OutputDirectory).Path
if ($output -eq $binary -or $output.StartsWith($binary + [IO.Path]::DirectorySeparatorChar,
        [StringComparison]::OrdinalIgnoreCase)) {
    throw 'OutputDirectory must be outside the Binary directory.'
}
$required = @('AirPodsDesktop.exe', 'Qt6Core.dll', 'Qt6Gui.dll', 'Qt6Widgets.dll',
    'platforms/qwindows.dll', 'multimedia/ffmpegmediaplugin.dll')
foreach ($file in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $binary $file) -PathType Leaf)) {
        throw "Deployment is incomplete: $file"
    }
}
# Unique staging directory prevents overwriting an unrelated folder.
$staging = Join-Path $output ('stage-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $staging | Out-Null
try {
    Get-ChildItem -LiteralPath $binary -Recurse -File | ForEach-Object {
        $relative = $_.FullName.Substring($binary.Length).TrimStart('\', '/')
        if ($relative -match '(?i)(test|mock|simulator|simulation|fixture|harness|validation)' -or
            $_.Extension -match '^\.(pdb|ilk|lib|exp)$') { return }
        $destination = Join-Path $staging $relative
        New-Item -ItemType Directory -Path (Split-Path $destination) -Force | Out-Null
        Copy-Item -LiteralPath $_.FullName -Destination $destination
    }
    Copy-Item -LiteralPath (Join-Path $sourceRoot 'LICENSE') -Destination (Join-Path $staging 'LICENSE')
    Copy-Item -LiteralPath (Join-Path $sourceRoot 'UPSTREAM.md') -Destination (Join-Path $staging 'UPSTREAM.md')
    # Keep the preflight with the test package; it has no external dependencies.
    $review = Join-Path $sourceRoot '..\review'
    Copy-Item -LiteralPath (Join-Path $review 'Test-WindowsPrerequisites.ps1') -Destination $staging
    Copy-Item -LiteralPath (Join-Path $review 'HARDWARE-TEST.md') -Destination $staging
    $name = 'AirPodsDesktop-evaluation-win11-x64-' + [DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss')
    $zip = Join-Path $output ($name + '.zip')
    if (Test-Path -LiteralPath $zip) { throw 'Archive already exists; refusing to overwrite.' }
    Compress-Archive -Path (Join-Path $staging '*') -DestinationPath $zip
    $hash = (Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash
    Set-Content -LiteralPath ($zip + '.sha256') -Value ($hash + '  ' + [IO.Path]::GetFileName($zip)) -Encoding ASCII
    Write-Host "Portable archive: $zip"
    Write-Host "SHA256: $hash"
    Write-Host 'Packaging alone is not proof that builds, automated tests, or hardware checks passed.'
} finally {
    # Remove only the unique directory this invocation created.
    Remove-Item -LiteralPath $staging -Recurse -Force
}
