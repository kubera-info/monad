// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppMain.h"
// Monad
#include "AppDict.h"
#include "AppSingleton.h"
#include "Kernel/Format.h"

using namespace DirectX;
using namespace std;

namespace Monad
{
	using namespace GUI;
	using namespace I18N;
	using namespace Kernel;

	namespace App
	{
		constexpr float
			PANE_Y1 = -LANDSCAPE_RATIO + SCALE,
			PANE_Y2 = (1.0f - MOVE_HR) * LANDSCAPE_RATIO,
			MARGIN = 0.0125f,
			ASPECT = 1.0f - A4_LANDSCAPE_RATIO * (PANE_Y2 - PANE_Y1) - SCALE / LANDSCAPE_RATIO,
			ASPECT_MARGIN = ASPECT - MARGIN;

		CombineMain::DialogPicture::DialogPicture() :
			DialogGeneric{
			"game"_pageNo, {
					ASPECT,
					-1.0f,
					1.0f,
					1.0f
				},
				COLOURING_BOOK_SHORT
		},
			m_panesDialog{
				make_unique<CombineArtTex2Pane::FrameWithID>(
					g_combineFullRequirements->m_paletteMap.m_paletteContainer,
					NAME_PALETTE,
					GetInternalRect()),
				make_unique<CombineArtTex2Pane::FrameWithID1>(
					g_combineFullRequirements->m_paletteMap1.m_paletteContainer,
					NAME_PALETTE1,
					GetInternalRect())
		}
		{
			AddRenderedObject(&m_panesDialog);
		}

		void CombineMain::DialogPicture::SetArt(
			const Repository::ConstArtIterator art
		) noexcept
		{
			m_panesDialog.SetArt(art);
		}

		CombineMain::CombineMain() :
			m_paletteDialog{ {
					-1.0f,
					-.52f,
					ASPECT_MARGIN,
					1.0f
				},
					COLORS
		},
			m_toolboxDialog{ {
					-1.0f,
					-1.0f,
					ASPECT_MARGIN,
					-.545f
				},
					OTHERS
		}
		{
			AddRenderedObjects({
				&m_paletteDialog,
				&m_toolboxDialog,
				&m_pictureDialog
				}
			);
		}
	}
}