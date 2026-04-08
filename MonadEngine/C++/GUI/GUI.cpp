// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "GUI.h"

// Monad
#include "Kernel/Format.h"

using namespace DirectX;
using std::wstring;

namespace Monad
{
	using namespace Kernel;
	using namespace Renderer;

	namespace GUI
	{
		Clickable::Clickable(
			const std::string& pageNo,
			const Audio::RandomJingle& clicksSounds
		) :
			c_clicksSounds{
				clicksSounds
		},
			c_pageNo{ pageNo }
		{
			Register();
		}

		bool Clickable::OnPointerDownStub()
		{
			if (IsInRegion())
			{
				PlayClickJingle();
				OnClick();
				return false; // Found
			}
			return true;
		}

		inline XMFLOAT4X4 InitializePane(
			const XMFLOAT4& positionOfCenter
		)
		{
			XMFLOAT4X4 transformRet;

			XMStoreFloat4x4(&transformRet,
				XMMatrixMultiply(
					XMMatrixScaling(positionOfCenter.z, positionOfCenter.w, 0.065f),
					XMMatrixTranslation(positionOfCenter.x, positionOfCenter.y, 0.0f)
				));

			return transformRet;
		}

		Pane::Pane(
			const XMFLOAT4& positionOfCenter,
			const std::string& technique,
			const VectorString& textureFilenames,
			const std::string& meshFilename
		) :
			CalculatePosition{
				InitializePane(positionOfCenter),
				technique,
				textureFilenames,
				Renderer::Registers::INPUT_RESOURCE::T2,
				meshFilename },
				m_positionOfCenter{ positionOfCenter }
		{}

		const DirectX::XMFLOAT2 Pane::GetPos() const noexcept
		{
			return{ -m_positionOfCenter.x + 0.0675f, m_positionOfCenter.y * INV_LANDSCAPE_RATIO + 0.05f };
		}

		void Pane::OnComputePerWorldFrame(
			const XMFLOAT4X4& modelSpaceMatrix
		)
		{
			__super::OnComputePerWorldFrame(modelSpaceMatrix);

			Light();
		}
	}
}