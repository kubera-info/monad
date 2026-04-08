##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

if ($args.length -ne 2)
{
	Write-Error "${MyInvocation.MyCommand.Name}: Incorrect number of parameters, should be: <Platform> <Configuration>"
	[System.Console]::Beep()
	exit 1
}

. "..\..\MonadEngine\bin\Tools.ps1"
	
function Copy-MonadReport([string]$pPlatform, [string]$pConfiguration)
{
	if ($pPlatform -eq $global:WindowsArm64Platform)
	{
		Set-Variable target -Option ReadOnly -Value ($global:WindowsArm64Platform + $pConfiguration)
	}
	else
	{
		Set-Variable target -Option ReadOnly -Value $pConfiguration
	}
	
	# Populate to projects
	function Copy-ReportFiles([string]$pSrcFiles, [string]$pDstFiles)
	{
		@('.pdb', '.exe')|
			ForEach-Object{ Copy-MonadFile "$pSrcFiles\Report$_" "$pDstFiles\Report$_" }
	}
	@('OldPolishClock', 'EnchantedCrayon')|
		ForEach-Object{ Copy-ReportFiles $Target ((Split-Path -Path $PSScriptRoot -Parent) + "\$_\$Target") }
}

# args[0] = Platform
# args[1] = Configuration
Copy-MonadReport $args[0] $args[1]