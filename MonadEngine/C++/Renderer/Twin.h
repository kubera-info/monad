// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <memory>
// Monad
#include "Combine.h"
#include "First.h"

namespace Monad::Renderer
{
	template<typename R, const uint32_t F>
	struct Frame final : R
	{
		using R::R;
		bool IsVisible() const noexcept final
		{
			return __super::IsVisible() && F == g_frameIndex;
		}
	};
	template<typename R>
	struct CombineFrames : Combine
	{
		using MonadType = R;
		using FrameWithID = Frame<R, 0>;
		using FrameWithID1 = Frame<R, 1>;
		using PtrFrame = std::unique_ptr<FrameWithID>;
		using PtrFrame1 = std::unique_ptr<FrameWithID1>;
		inline CombineFrames(
			PtrFrame monad,
			PtrFrame1 monad1
		) :
			m_frame(std::move(monad)),
			m_frame1(std::move(monad1))
		{
			AddRenderedObjects({
				m_frame.get(),
				m_frame1.get() }
				);
		}

	protected:
		PtrFrame m_frame;
		PtrFrame1 m_frame1;
	};
}