# Modified evaluation version, 2026-11-01. GPL-3.0-or-later.
# Builds and tests only. Does not install, launch, or register startup tasks.
param(
    [Parameter(Mandatory = $true)][string]$QtRoot,
    [Parameter(Mandatory = $true)][string]$VcpkgRoot
)
$ErrorActionPreference = 'Stop'
$source = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$toolchain = Join-Path $VcpkgRoot 'scripts/buildsystems/vcpkg.cmake'
if (-not (Test-Path -LiteralPath $toolchain)) { throw 'vcpkg toolchain not found.' }
if (-not (Test-Path -LiteralPath (Join-Path $QtRoot 'lib/cmake/Qt6/Qt6Config.cmake'))) {
    throw 'QtRoot must point to the Qt 6.8.3 MSVC 2022 x64 installation.'
}
$build = Join-Path $source 'Build/Evaluation'
& cmake -S $source -B $build -G 'Visual Studio 17 2022' -A x64 `
    "-DCMAKE_TOOLCHAIN_FILE=$toolchain" "-DCMAKE_PREFIX_PATH=$QtRoot" `
    -DAPD_BUILD_TESTS=ON -DAPD_GENERATE_INSTALLER=OFF
if ($LASTEXITCODE -ne 0) { throw "Configure failed: $LASTEXITCODE" }
& cmake --build $build --config RelWithDebInfo --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Build failed: $LASTEXITCODE" }
& ctest --test-dir $build -C RelWithDebInfo --output-on-failure
if ($LASTEXITCODE -ne 0) { throw "Tests failed: $LASTEXITCODE" }
Write-Host "Build and tests passed. Output: $build/Binary. Application was not launched or installed."
