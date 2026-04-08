// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Repo.h"
// Monad
#include "Renderer/__MonadRenderer.h"
#include "Files/FileManager.h"

using namespace std;
namespace fs = filesystem;

namespace Monad::Repositories
{
	RepositoryGeneric::RepositoryGeneric() :
		c_me(
			this,
			g_repositoryGeneric
		)
	{}

	void RepositoryGeneric::AddGhostTexture()
	{
		m_shaderResources.emplace(
			Kernel::GHOST,
			Renderer::RESOURCE_DESC
		);
	}

	void RepositoryGeneric::OnClearD3DAssets() noexcept
	{
		Kernel::FireEvent(Kernel::EVENT_IDS::D3D_CLEAR_ASSETS);
	}

	Renderer::Texture& RepositoryGeneric::MapMembersTextures::LoadEmoji(const std::string& filename)
	{
		auto texIt(this->find(filename));
		if (cend() != texIt)
			return texIt->second;
		else
		{
			Files::Path emojiPath{
				Kernel::DummyParameter{},
				L"Emoji" };
			emojiPath /= Tools::UTF8ToUnicode(filename) + Files::EXT_TEX;
			Files::TaskFileHolder loader(
				Files::g_fileSearcherGeneric,
				static_cast<fs::path&&> (emojiPath));
			loader.Run();
			return GetMember(filename);
		}
	}
	RepositoryGeneric* g_repositoryGeneric = nullptr;
}