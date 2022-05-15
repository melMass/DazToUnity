$parent_dir = Split-Path $MyInvocation.MyCommand.Path

if (Get-Command "Remove-ItemSafely" -errorAction SilentlyContinue) {

  Remove-ItemSafely "$parent_dir\build"

}

else {
  Write-Output "You need the recycle plugin https://www.powershellgallery.com/packages/Recycle/1.0.2"
}
