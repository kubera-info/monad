// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "LookAndFeel.h"

// Monad
#include "__MonadGUI.h"
#include "Renderer/__MonadRenderer.h"
#include "Input/Cursor.h"
#include "Kernel/Templates.h" 

using namespace DirectX;

namespace Monad::GUI
{
	using namespace Input;
	using namespace Kernel;
	using namespace Renderer;

	IconicStyleGeneric::IconicStyleGeneric(
		const MeshStyle& style,
		const XMFLOAT3& rgb,
		const Audio::RandomJingle& clicksSounds,
		const Kernel::VectorString& icons
	) :
		c_style{ style },
		c_rgb{ rgb },
		c_clicksSounds{ clicksSounds },
		c_icons{ icons }
	{}

	// ---------------------------------------------------------------------
	// Predefined mesh styles used by GUI elements
	// ---------------------------------------------------------------------

	const MeshStyle
		CIRCULAR_STYLE{
			[](const XMFLOAT4& positionOfCenter) noexcept
			{
				// Circular hit-test based on cursor distance
				const XMFLOAT2 v1(g_cursorPos.x, g_cursorPos.y);
				const XMFLOAT2 v2(positionOfCenter.x, positionOfCenter.y);

				float distance;
				XMStoreFloat(
					&distance,
					XMVector2Length(
						XMVectorSubtract(
							XMLoadFloat2(&v1),
							XMLoadFloat2(&v2)
						)
					)
				);

				return distance <= positionOfCenter.z;
			},
			"circullar_button"_mesh,
			"iconic"_technique
	},

		ONE_COLOR_STYLE{
			[](const XMFLOAT4& positionOfCenter) noexcept
			{
				// Rectangular hit-test
				return between(
					positionOfCenter.x - positionOfCenter.z,
					positionOfCenter.x + positionOfCenter.z,
					g_cursorPos.x
				) &&
				between(
					positionOfCenter.y - positionOfCenter.w,
					positionOfCenter.y + positionOfCenter.w,
					g_cursorPos.y
				);
			},
			"push_button"_mesh,
			"color_button"_technique
	},

		FLAG_STYLE{
			[](const XMFLOAT4& positionOfCenter) noexcept
			{
				// Wider horizontal hit area for flag buttons
				return between(
					positionOfCenter.x - positionOfCenter.z * 2.0f,
					positionOfCenter.x + positionOfCenter.z * 2.0f,
					g_cursorPos.x
				) &&
				between(
					positionOfCenter.y - positionOfCenter.w,
					positionOfCenter.y + positionOfCenter.w,
					g_cursorPos.y
				);
			},
			"flag_button"_mesh,
			"flag"_technique
	};

	// ---------------------------------------------------------------------
	// IconicStyle – base style for icon-based GUI buttons
	// ---------------------------------------------------------------------

	IconicStyle::IconicStyle(
		const MeshStyle& style,
		const Audio::RandomJingle& clicksSounds,
		const Kernel::VectorString& icons
	) :
		IconicStyleGeneric{
			style,
			GUI_WHITE,
			clicksSounds,
			icons
	}
	{}

	IconicStyle::IconicStyle(
		const XMFLOAT3& rgb,
		const Audio::RandomJingle& clicksSounds,
		const Kernel::VectorString& icons
	) :
		IconicStyleGeneric{
			CIRCULAR_STYLE,
			rgb,
			clicksSounds,
			icons
	}
	{}

	IconicStyle::IconicStyle(
		const XMFLOAT3& rgb,
		const Kernel::VectorString& icons
	) :
		IconicStyle{
			rgb,
			{ JINGLE_FOCUS },
			icons
	}
	{}

	IconicStyle::IconicStyle(
		const Kernel::VectorString& icons
	) :
		IconicStyle{
			GUI_CHECKBOX_COLOR,
			icons
	}
	{}

	IconicStyle::IconicStyle(
		const std::string& flag
	) :
		IconicStyle{
			FLAG_STYLE,
			{ JINGLE_FOCUS },
			{ flag }
	}
	{}
}