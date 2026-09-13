param (
  [Parameter(Mandatory = $true)]
  [ValidateSet("debug", "release")]
  $config
)

# $common_flags = @("/std:c++latest", "/utf-8", "/I..\mttlib\include", "/Iinclude", "/W4")
$common_flags = @("/std:c++latest", "/utf-8", "/EHsc", "/Iinclude", "/W4")
$source_files = Get-ChildItem "src\*" -Name | ForEach { "src\$_" }
$executable_name = "mttPAK"

switch ($config) {
  "debug" {
    $binary_directory = "bin\debug\"
    $flags = @("/Fo:$binary_directory", "/Fe:$binary_directory$executable_name", "/Fd:$binary_directory", "/Zi", "/Od", "/MDd")
  }
  "release" {
    $binary_directory = "bin\release\"
    $flags = @("/Fo:$binary_directory", "/Fe:$binary_directory$executable_name", "/Fd:$binary_directory", "/O2", "/MD")
  }
}

# $source_files = $source_files + "..\mttlib\bin\$config\mttlib.lib"

if (-not (Test-Path $binary_directory)) { New-Item -ItemType Directory -Path $binary_directory -Force | Out-Null }

& cl @common_flags @flags @source_files