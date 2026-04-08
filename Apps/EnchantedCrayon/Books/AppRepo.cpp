// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppRepo.h"
// STD
#include <ranges>
// Monad
#include "AppSingleton.h"
#include "Globals/__MonadGlobals.h"
#include "Files/SaveUTF8Encrypted.h"
#include "AppXMLProfile.h"
#include "D3D12Framework.h"
#include "Files/Registry.h"
#include "Files/XML.h"
#include "Tools/Convert.h"

using namespace std;

namespace Monad
{
	using namespace Files;
	using namespace Pages;
	using namespace Renderer;
	using namespace Repositories;
	using namespace Safety;

	namespace App
	{
		const std::string
			NAME_PALETTE(":p"),
			NAME_SECONDARY_PALETTE(":q"),
			NAME_PALETTE1(":r"),
			NAME_SECONDARY_PALETTE1(":s");

		const Texture& Repository::GetArtTexture(
			const Repository::ConstArtIterator art
		) const
		{
			return m_shaderResources.GetMember(
				art->first
			);
		}

#pragma region Repository
		Repository::Repository() :
			m_currentArtProfile{
			m_memArts.end()
		}
		{}

		void Repository::WriteXMLProfile()
		{
			const XMLManager xmlFile({}, ranges::to<XMLManager::XMLElement::VectorNestedElements>(
				m_memArts
				| views::transform
				(
					[](const auto& art) -> XMLManager::XMLElement
					{
						/// <summary>
						/// This lambda converts an array to a map with size_t ids.
						/// First color in the palette is the special one and is dropped.
						/// </summary>
						const auto FnArrayToMap = [](const auto& palette)
							{
								static_assert(palette.size() > 1);
								return ranges::to<map<size_t, const ColorIndexGeneric&>>(
									palette | views::enumerate | views::drop(1)
								);
							};

						return
						{
							ART,
							{ { XML_ATTR_ID, Tools::UTF8ToUnicode(art.first) } },
							ranges::to<XMLManager::XMLElement::VectorNestedElements>(FnArrayToMap(art.second.m_palette)
							| views::filter
							(
								[](const auto& color) noexcept
								{
									return !color.second.IsWhite();
								}
							)
							| views::transform
							(
								[](const auto& color) -> XMLManager::XMLElement
								{
									return
									{
										COL,
										{
											{
												XML_ATTR_NAME,
												to_wstring(color.first)
											},
											{
												XML_ATTR_R,
												to_wstring(color.second.m_newColor.x)
											},
											{
												XML_ATTR_G,
												to_wstring(color.second.m_newColor.y)
											},
											{
												XML_ATTR_B,
												to_wstring(color.second.m_newColor.z)
											}
										},
										XML_NO_ELEMENTS
									};
								}
							)
							)
						};
					}
				)
				| views::filter
				(
					[](const auto& element) noexcept
					{
						return !element.m_nestedElements.empty();
					}
				)
			)
			);
			if (xmlFile.HasNoNestedElements())
				DropProfile();
			else
				SaveStringAs(
					g_singleton->m_cipher,
					g_singleton->c_profilePath,
					xmlFile.BuildXML()
				);
		}

		void Repository::WriteCurrentArtNameIntoRegistry() noexcept
		{
			if (m_memArts.cend() != m_currentArtProfile.m_currentArt)
			{
				struct RegistryWriteDefaultSketchName : Registry
				{
					RegistryWriteDefaultSketchName(
						const string& profile
					) noexcept :
						Registry{ KEY_WRITE }
					{
						SetString(
							L"Art",
							profile
						);
					}
				} defaultSketchName(
					m_currentArtProfile.m_currentArt->first);
			}
		}

		void Repository::WriteCustomerData()
		{
			if (!m_memArts.empty())
			{
				WriteXMLProfile();
				WriteCurrentArtNameIntoRegistry();
			}
		}

		Repository::ArtIterator& Repository::GetNextArt() noexcept
		{
			assert(!m_memArts.empty());
			++m_currentArtProfile.m_currentArt;
			if (m_currentArtProfile.m_currentArt == m_memArts.cend())
				m_currentArtProfile.m_currentArt = m_memArts.begin();
			return m_currentArtProfile.m_currentArt;
		}

		Repository::ArtIterator& Repository::GetPreviousArt() noexcept
		{
			assert(!m_memArts.empty());
			if (m_currentArtProfile.m_currentArt == m_memArts.cbegin())
				m_currentArtProfile.m_currentArt = m_memArts.end();
			--m_currentArtProfile.m_currentArt;
			return m_currentArtProfile.m_currentArt;
		}

		void Repository::ImportArts()
		{
			try
			{
				m_currentArtProfile.ReadForCurrent();

				const ScopeCounter refCounter;
				for (auto& dds : m_memArts)
				{
					if (Exceptions::InAnyCloseReason())
						break;

					dds.second.Run();
					g_repositoryGeneric->m_shaderResources.Insert(
						dds.first,
						dds.second.GetCPUBuffer()
					);
					dds.second.ClearAndShrink();
				}
				g_fileManagerState = MERGE_ALL;
				return;
			}
			MONAD_CATCH;
			g_fileManagerState = FILE_MAN_STATE_EXCEPTION;
		}

		inline void Repository::FindAndSetArtByFilename(
			const string& filename
		)
		{
			assert(m_memArts.cbegin() != m_memArts.cend());
			const auto currentArt = m_memArts.find(filename);
			m_currentArtProfile.m_currentArt =
				currentArt == m_memArts.cend()
				? m_memArts.begin()
				: currentArt;
		}
#pragma endregion

#pragma region Repository::CurrentArtProfile
		Repository::CurrentArtProfile::CurrentArtProfile(ArtIterator end) noexcept :
			m_currentArt(end)
		{}

		void Repository::CurrentArtProfile::ReadForCurrent()
		{
			struct RegistryReadDefaultSketchName : Registry
			{
				RegistryReadDefaultSketchName() noexcept :
					Registry(KEY_READ)
				{
					g_repository->FindAndSetArtByFilename(
						GetString(L"Art")
					);
				}
			} defaultSketch;
		}
#pragma endregion
	}
}