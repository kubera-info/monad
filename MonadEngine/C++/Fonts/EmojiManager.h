// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "EmojiBuilder.h"
#include "Renderer/Combine.h"

namespace Monad::Fonts
{
	/// <summary>
	/// Lightweight repository managing emojis associated with a dialog object.
	/// </summary>
	struct EmojiManager final : Renderer::Combine, private Kernel::EventLanguageChangeEmojiStub
	{
		EmojiManager();
		void AddEmoji(
			const std::string& name,
			const DirectX::XMFLOAT4& rect
		);
		///<summary>
		/// Generates a single mesh composed of multiple emoji elements.
		/// </summary>
		void Commit();

	private:
		bool OnLanguageChangeEmojiStub() final;

		using PtrEmoji = std::unique_ptr<ShaderEmoji>;
		std::list<PtrEmoji> m_guiEmoji;
	};
}
