# Build script for Lion's Den project
# Usage: .\build.ps1 [Debug|Release]

param(
    [string]$Configuration = "Release",
    [string]$Platform = "x64"
)

# Find MSBuild
$msbuildPaths = @(
    "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files (x86)\Microsoft Visual Studio\2020\BuildTools\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
)

$msbuild = $null
foreach ($path in $msbuildPaths) {
    if (Test-Path $path) {
        $msbuild = $path
        break
    }
}

if (-not $msbuild) {
    Write-Host "ERROR: MSBuild not found. Please install Visual Studio 2022 or 2020 Build Tools." -ForegroundColor Red
    exit 1
}

Write-Host "Using MSBuild: $msbuild" -ForegroundColor Green
Write-Host "Configuration: $Configuration" -ForegroundColor Cyan
Write-Host "Platform: $Platform" -ForegroundColor Cyan
Write-Host ""

# Build the project
& $msbuild "Lion's Den\Lion's Den.sln" `
    /p:Configuration=$Configuration `
    /p:Platform=$Platform `
    /t:Rebuild `
    /nologo `
    /v:minimal

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "Build succeeded!" -ForegroundColor Green
    
    $exePath = "x64\$Configuration\Lion's Den.exe"
    if (Test-Path $exePath) {
        Write-Host "Executable: $exePath" -ForegroundColor Green
    }
} else {
    Write-Host ""
    Write-Host "Build failed with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

