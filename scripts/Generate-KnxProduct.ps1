[CmdletBinding()]
param(
    [string]$Producer = "OpenKNXproducer",
    [ValidateSet("Development", "Release")]
    [string]$Configuration = "Development"
)

$root = Split-Path -Parent $PSScriptRoot
$xmlName = if ($Configuration -eq "Release") { "DigitalLEDController-Release.xml" } else { "DigitalLEDController.xml" }
$xmlPath = Join-Path $root "src\$xmlName"
$headerPath = Join-Path $root "include\knxprod.h"
$outputPath = Join-Path $root "outputs\$($xmlName -replace '\.xml$', '.knxprod')"

if (-not (Test-Path $xmlPath)) {
    throw "XML product definition not found: $xmlPath"
}

New-Item -ItemType Directory -Path (Split-Path $outputPath) -Force | Out-Null
Push-Location $root
try {
    & $Producer create $xmlPath -h $headerPath -o $outputPath
    if ($LASTEXITCODE -ne 0) {
        throw "OpenKNXproducer failed with exit code $LASTEXITCODE"
    }
}
finally {
    Pop-Location
}
