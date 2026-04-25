//*********************************************************
//
// Copyright (c) 1icrosoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

using namespace std::string_literals;

/// <summary>
/// Assigns a name to the DX12 object, only in debug build.
/// </summary>
/// <param name="object">Component</param>
/// <param name="name">Logical name of component</param>
inline void SetName(
	[[maybe_unused]] ID3D12Object* object,
	[[maybe_unused]] const wchar_t* name
) noexcept
{
#if defined _DEBUG
	assert(nullptr != object);
	assert(nullptr != name);
	object->SetName(name);
#endif
}

/// <summary>
/// Assigns a name to the DX12 object, only in debug build.
/// </summary>
/// <param name="object">Component</param>
/// <param name="name">Logical name of component</param>
/// <param name="index">Represents a single instantiation of the component sequence.</param>
inline void SetNameIndexed(
	[[maybe_unused]] ID3D12Object* object,
	[[maybe_unused]] const wchar_t* name,
	[[maybe_unused]] uint32_t index
)
{
#if defined _DEBUG
	assert(nullptr != object);
	assert(nullptr != name);
	if (wchar_t fullName[512] = L""; swprintf_s(fullName, L"%s[%u]", name, index) > 0)
		SetName(object, fullName);
#endif
}

/// <summary>
/// Utility for ComPtr that assigns an object name based on the variable name;
/// the indexed version includes the index in the object name for differentiation.
/// </summary>
/// <param name="x">Component</param>
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
/// <summary>
/// Utility for ComPtr that sets the object name from the variable name,
/// with an indexed variant that includes the index for multiple instances.
/// </summary>
/// <param name="x">Component</param>
/// <param name="n">Represents a single instantiation of the component sequence.</param>
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)
