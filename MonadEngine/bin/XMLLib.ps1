############################################################################
### Copyright © 2016-2025 by W³odzimierz O. Kubera. All rights reserved. ###
############################################################################

# Functions for compacting XML files.

# This subroutine removes selected attributes from all XML tags
# across the entire source document.
# WARNING: This method is not fully safe — it may also remove matching
# attribute patterns outside of XML tags.
# It is intended for controlled input only (Monad(a) files and BMFont),
# where this behavior is acceptable.
function Remove-MonadAttributes(
	[string]$SourceXML, 
	[string[]]$AttributesToRemove
)
{
	ForEach ($Attribute in $AttributesToRemove)
	{
		$SourceXML = $SourceXML -replace $Attribute+'="[^"]*"\s*'
	}
	$SourceXML
}

# Removes all XML elements with the given tag name.
# Note: self-closing tags (e.g. <tag />) are NOT removed.
# Only full elements in the form <tag>...</tag> are affected.
function Remove-MonadTag(
	[string]$SourceXML, 
	[string]$TagToRemove
)
{
	#$SourceXML = $SourceXML -replace "<$TagToRemove[^\/]*\/>"
	$SourceXML -replace "<$TagToRemove[^>]*>.*<\/$TagToRemove>"
}

# Removes insignificant whitespace that does not affect XML semantics.
# This helps reduce file size and improve compactness.
function Remove-MonadSpace(
	[string]$SourceXML
)
{
	$SourceXML.Replace(' />', '/>') -replace '>\s+', '>' -replace '\s+<', '<'
}

# Removes the XML declaration header.
# This is optional and used to minimize file size.
function Remove-MonadHeader(
	[string]$SourceXML
)
{
	$SourceXML.Replace('<?xml version="1.0"?>', '').Replace('<?xml version="1.0" encoding="utf-8"?>', '')	
}