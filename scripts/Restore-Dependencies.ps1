[CmdletBinding()]
param(
    [string]$DependencyFile = "dependencies.txt"
)

$root = Split-Path -Parent $PSScriptRoot
$parent = Split-Path -Parent $root
$lib = Join-Path $root "lib"

foreach ($line in Get-Content (Join-Path $root $DependencyFile)) {
    if ([string]::IsNullOrWhiteSpace($line) -or $line.StartsWith("#")) {
        continue
    }

    $parts = $line -split "\s+", 4
    if ($parts.Count -ne 4) {
        throw "Invalid dependency line: $line"
    }

    $hash, $branch, $name, $url = $parts
    $checkout = Join-Path $parent $name
    if (-not (Test-Path $checkout)) {
        git clone --branch $branch $url $checkout
        if ($LASTEXITCODE -ne 0) { throw "git clone failed: $name" }
    }
    git -C $checkout fetch --all --prune
    git -C $checkout checkout --detach $hash
    if ($LASTEXITCODE -ne 0) { throw "git checkout failed: $name@$hash" }

    $link = Join-Path $lib $name
    if (Test-Path $link) {
        $item = Get-Item $link -Force
        if (-not ($item.Attributes -band [IO.FileAttributes]::ReparsePoint)) {
            throw "Dependency path exists and is not a junction: $link"
        }
        cmd /c rmdir $link
    }
    cmd /c mklink /J $link $checkout | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "junction creation failed: $link" }
}
