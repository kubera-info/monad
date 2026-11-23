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

	D3D12_INPUT_LAYOUT_DESC VectorInputElementsDesc::GetInputLayoutDesc() const
	{
		THROW_EXC_IFFALSE(Exceptions::NoDataFound, !empty(), L"3D Input Layout Description (vector)");
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
		THROW_EXC_ONEND(*this, foundIL, L"3D Input Layout Descriptions (map)");
		return foundIL->second.GetInputLayoutDesc();
	}

	MapInputLayouts
		FontEndEmoji
	{
		{ "emoji",
			{
				{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT },
				{ "TEXCOORD", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT }
			}
		},
		{ "font",
			{
				{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT },
				{ "TEXCOORD", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT },
				{ "COLOR", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT }
			}
		}
	},
		Basics
	{
		{ "plain",
			{
				{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT },
				{ "NORMAL", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT }
			}
		},
		{ "tex",
			{
				{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT },
				{ "NORMAL", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT },
				{ "TEXCOORD", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT }
			}
		}
	},
		Image
	{
		{ "image",
			{
				{ "POSITION", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32B32_FLOAT },
				{ "TEXCOORD", SEMANTIC_INDEX::SI0, DXGI_FORMAT_R32G32_FLOAT }
			}
		}
	};

	MapInputLayouts& MapInputLayouts::operator+=(
		const MapInputLayouts& source
		)
	{
		const size_t oldSize = size();
		insert(source.cbegin(), source.cend());
		THROW_EXC_IFFALSE(Exceptions::ClassAlreadyExists, oldSize + source.size() == size(), L"3D Failed to merge input layout maps");
		return *this;
	}

	MapInputLayouts MapInputLayouts::operator+(
		const MapInputLayouts& source
		)
	{
		return MapInputLayouts{ *this } += source;
	}
}