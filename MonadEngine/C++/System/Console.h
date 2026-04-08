// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

namespace Monad::System
{
	/// <summary>
	/// Outputs a debug string to the debugger console.
	/// Works similarly to OutputDebugString().
	/// </summary>
	/// <param name="text">
	/// Pointer to a null-terminated wide string to be printed.
	/// </param>
	/// <returns>
	/// void
	/// </returns>
	void MndOutputDebugString([[maybe_unused]] const wchar_t* text) noexcept;

	/// <summary>
	/// Outputs a debug string using a container type.
	/// This overload allows passing std::wstring, std::wstring_view, etc.
	/// </summary>
	/// <typeparam name="S">
	/// Any type exposing data() returning const wchar_t*.
	/// </typeparam>
	/// <param name="text">
	/// Text container holding a wide string.
	/// </param>
	template<typename S>
	void MndOutputDebugString([[maybe_unused]] const S& text) noexcept
	{
#if defined _DEBUG
		MndOutputDebugString(text.data());
#endif
	}
}