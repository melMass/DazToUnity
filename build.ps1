[CmdletBinding()]
	param(
    [Parameter(
    HelpMessage="Builds a release build")]
    [switch]
    [Alias("r")]
    $Release,
    [Parameter(
      HelpMessage="The path to your DAZ Studio's installation directory")]
    [Alias("dz")]
    $DazStudioPath = "C:\Daz 3D\Applications\64-bit\DAZ 3D\DAZStudio4\",
    [Parameter(
      HelpMessage="Disable installation of the plugin"
    )]
    [switch]
    $NoInstall
	)


# - CHECKS

# - daz studio path exists
if (-Not (Test-Path $DazStudioPath) -And -Not $NoInstall){
  Write-Host "The path to your DAZ Studio's installation directory is invalid" -ForegroundColor Red
  Write-Host "You can bypass installation by using -NoInstall" -ForegroundColor Yellow
  exit
}

# - cmake is available in PATH
if ( -Not (get-command cmake -ErrorAction Ignore)) {
  Write-Host "CMake is not installed. Please install CMake before running this script." -ForegroundColor Red
  exit 1
}

# - gather info
$current_pwd = $PWD
$parent_dir = Split-Path $MyInvocation.MyCommand.Path
Set-Location $parent_dir

# - build config type
$config = "Debug"
if ($Release){
  $config = "Release"
  Write-Host "Building a release build" -ForegroundColor Green
}
else{
  Write-Host "Building a debug build" -ForegroundColor Yellow
}

# - build in 'build' directory
New-Item -ErrorAction Ignore -ItemType Directory build
Set-Location build
cmake .. -DDAZ_STUDIO_EXE_DIR="$DazStudioPath"
cmake --build . --config $config

if (-Not $NoInstall){
  cmake --install .
}



Set-Location $current_pwd
