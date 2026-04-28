##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

# This script downloads, unpacks and prepares the libCurl for the further usage.
# It also removes older versions of the libCurl.
# After preparation it copies the libCurl dynamic link library and headers to the target projects.
# The algorithm supports only Windows x64 and Windows ARM64 as a target.

if ($args.length -ne 2)
{
	Write-Error "${MyInvocation.MyCommand.Name}: Incorrect number of parameters, should be: <Platform> <Configuration>"
	[System.Console]::Beep()
	exit 1
}

. "..\..\MonadEngine\bin\Tools.ps1"

function Update-MonadCURL(
	[string]$Platform, 
	[string]$Configuration
)
{
	Set-Variable WindowsX64Zip -Option ReadOnly -Value 'win64-mingw.zip'
	Set-Variable WindowsArm64Zip -Option ReadOnly -Value 'win64a-mingw.zip'
	Set-Variable CookiePath -Option ReadOnly -Value "Current Name ${Platform}.txt"
	if (Test-Path $CookiePath) 
	{
		Set-Variable CookieRead -Option ReadOnly -Value (Get-Content $CookiePath)
	}
	else
	{
		Set-Variable CookieRead -Option ReadOnly -Value 'No folder found'
	}

	if ($Platform -eq $global:WindowsArm64Platform)
	{
		$File = $WindowsArm64Zip
		$Output = $global:WindowsArm64Platform
		$DynamicLinkLibrary = 'libcurl-arm64.dll'
	}
	else
	{
		$File = $WindowsX64Zip
		$Output = $global:WindowsX64Platform
		$DynamicLinkLibrary = 'libcurl-x64.dll'
	}
	$Release = "${Output}Release"

	# Get new curl
	Invoke-WebRequest -Uri "https://curl.se/windows/latest.cgi?p=$File" -OutFile $File
	Expand-Archive -Path $File -DestinationPath $Release -Force
	Remove-Item -Path $File

	# Found all available versions in the folder
	$Folders = Get-ChildItem -Path (Join-Path $Release 'curl*') -Directory
	$CurrentName = $Folders| 
	    Sort-Object -Property Name -Descending| 
	    Select-Object -First 1 -ExpandProperty Name
	$Folders|
		Where-Object Name -ne $CurrentName|
		Remove-Item -Recurse -Force

	# Prepare for the new libCurl content
	$Target = Join-Path $Release 'libCurl'
	Remove-Item -Path $Target -Recurse -Force -ErrorAction Ignore
	Move-Item -Path (Join-Path $Release $CurrentName) -Destination $Target
	$SourceProject = Join-Path (Join-Path $Target bin) $DynamicLinkLibrary

	# Status of the process
	if ($CookieRead -ne $CurrentName)
	{
		Write-MonadHr
		Write-MonadOutput "New version - $CookieRead => $CurrentName"
		$CurrentName|Out-File -NoNewline -FilePath $CookiePath
		& ..\..\MonadEngine\bin\Say.ps1 'New Curl library downloaded'
	}

	# Populate to projects
	@('Report', 'OldPolishClock', 'EnchantedCrayon')|
		ForEach-Object{ Copy-MonadFile $SourceProject (Join-Path (Join-Path (Join-Path '..\..\Apps' $_) ($Output + $Configuration)) $DynamicLinkLibrary) }
}

# args[0] = Platform - x64 | ARM64
# args[1] = Configuration - Debug | Release
Update-MonadCURL $args[0] $args[1]