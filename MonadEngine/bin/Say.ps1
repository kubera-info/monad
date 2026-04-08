############################################################################
### Copyright © 2016–2025 by Włodzimierz O. Kubera. All rights reserved. ###
############################################################################

# Simple Text-To-Speech helper script.
# Uses Microsoft SAPI (Speech API) to speak text passed as arguments.

if ($args.length -eq 0)
{
    Write-Error "($MyInvocation.MyCommand.Name): Incorrect number of parameters, should be: <TTS>"
    [System.Console]::Beep()
    exit 1
}

# Create SAPI voice COM object
$SpeakingVoiceCom = New-Object -ComObject SAPI.SpVoice

try
{
    # Select the default voice
    $SpeakingVoiceCom.Voice = $SpeakingVoiceCom.GetVoices().Item(0)

    # Speak each argument passed to the script
    $args | ForEach-Object {
        $SpeakingVoiceCom.Speak($_) | Out-Null
    }
}
catch
{
    # Exceptions are intentionally ignored
}
finally
{
    # Properly release COM object
    [System.Runtime.InteropServices.Marshal]::ReleaseComObject($SpeakingVoiceCom) | Out-Null
}