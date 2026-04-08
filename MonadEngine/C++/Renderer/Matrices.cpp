// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Matrices.h"
// STD
#include <string>

using namespace DirectX;

namespace Monad::Renderer
{
#if defined _XM_NO_INTRINSICS_
	void Dummatrixatrix(
		const DirectX::XMMATRIX& matrix
	) noexcept
	{
		OutputDebugString(L"\n");
		for (size_t row = 0; row < 4; ++row)
		{
			for (size_t col = 0; col < 4; ++col)
			{
				OutputDebugString(L" ");
				OutputDebugString(std::to_wstring(matrix(row, col)).c_str());
				OutputDebugString(L"f");
				if (row != 3 || col != 3)
					OutputDebugString(L",");
			}
			OutputDebugString(L"\n");
		}
	}

	void DumpFloat4(
		const DirectX::XMFLOAT4& vector
	) noexcept
	{
		OutputDebugString(L"\n ");
		OutputDebugString(std::to_wstring(vector.x).c_str());
		OutputDebugString(L"f, ");
		OutputDebugString(std::to_wstring(vector.y).c_str());
		OutputDebugString(L"f, ");
		OutputDebugString(std::to_wstring(vector.z).c_str());
		OutputDebugString(L"f, ");
		OutputDebugString(std::to_wstring(vector.w).c_str());
		OutputDebugString(L"f\n");
	}
#endif
}