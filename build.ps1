$ErrorActionPreference = "Stop"

$projectRoot = "E:\04Project\Claude\Viewer"
$buildDir = "$projectRoot\build_vs"

# Set Qt path
$env:Path = "D:\QT\Qt5.14.2\5.14.2\msvc2017_64\bin;" + $env:Path

# Build
Write-Host "Building SchematicViewer..."
cd $buildDir

cmake --build . --config Release

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful!" -ForegroundColor Green

    # Run viewer if requested
    if ($args.Contains("--run")) {
        Write-Host "Starting Qt viewer..."
        & "$buildDir\Release\schematic_qt_viewer.exe" "$projectRoot\examples\demo_circuit.json"
    }
    cd $projectRoot
} else {
    Write-Host "Build failed!" -ForegroundColor Red
    cd $projectRoot
    exit 1
}
