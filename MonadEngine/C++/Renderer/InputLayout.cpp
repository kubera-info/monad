// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "InputLayout.h"
// Monad
#include "Exceptions/Exceptions.h"

namespace Monad::Renderer::InputLayout
{
	INPUT_ELEMENT_DESC::INPUT_ELEMENT_DESC(
		const char* semanticName,
		SEMANTIC_INDEX semanticIndex,
		DXGI_FORMAT format
	) noexcept :
		D3D12_INPUT_ELEMENT_DESC
	{
		semanticName,
		static_cast<uint32_t>(semanticIndex),
		format,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	}
	{}

	D3D12_INPUT_LAYOUT_DESC VectorInputElementsDesc::GetInputLayoutDesc() const noexcept
	{
		return
		{
			.pInputElementDescs = data(),
			.NumElements = static_cast<uint32_t>(size())
		};
	}

	D3D12_INPUT_LAYOUT_DESC MapInputLayouts::GetInputLayoutDesc(
		const std::string& id
	) const
	{
		const auto foundIL = find(id);
		THROW_EXC_ONEND(*this, foundIL, L"Input Layout Desc");
		return foundIL->second.GetInputLayoutDesc();
	}

	MapInputLayouts FontEndEmoji()
	{
		return
		{
			{ "emoji",
				{
					{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT },
					{ "TEXCOORD", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT } }
				},
			{ "font",
				{
					{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT },
					{ "TEXCOORD", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT },
					{ "COLOR", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT }

				}
			}
		};
	}

	MapInputLayouts Basics()
	{
		return
		{
			{ "plain",
				{
					{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT },
					{ "NORMAL", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT}
				}
			},
			{ "tex",
				{
					{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT },
					{ "NORMAL", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT },
					{ "TEXCOORD", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT }
				}
			}
		};
	}

	MapInputLayouts Image()
	{
		return
		{
			{ "image",
				{
					{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT },
					{ "TEXCOORD", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT }
				}
			}
		};
	}

	MapInputLayouts& MapInputLayouts::operator+=(
		const MapInputLayouts& source
		)
	{
		merge(MapInputLayouts(source));
		return *this;
	}

	MapInputLayouts MapInputLayouts::operator+(
		const MapInputLayouts& source
		)
	{
		MapInputLayouts newILs{ *this };
		return newILs += source;
	}
}