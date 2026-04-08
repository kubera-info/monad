// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "EmojiManager.h"

namespace Monad::Fonts
{
	EmojiManager::EmojiManager()
	{
		Register();
	}

	void EmojiManager::AddEmoji(
		const std::string& name,
		const DirectX::XMFLOAT4& rect
	)
	{
		ShaderEmoji* resEmo = nullptr;
		if (const auto itShaderEmoji =
			std::ranges::find_if(
				m_guiEmoji, [&](auto& it)
				{ return (it->m_emojiName == name); });
			m_guiEmoji.cend() == itShaderEmoji)
		{
			resEmo = m_guiEmoji.emplace_back(
				std::make_unique<ShaderEmoji>(name)).get();
			if (!contains(resEmo))
				AddRenderedObject(resEmo);
		}
		else
			resEmo = itShaderEmoji->get();
		resEmo->AddEmoji(rect);
	}

	void EmojiManager::Commit()
	{
		for (const auto& emoji : m_guiEmoji)
			emoji->Commit();
	}

	bool EmojiManager::OnLanguageChangeEmojiStub()
	{
		Commit();
		return true;
	}
}