// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// STD
#include <queue>
#include <string>

// Monad
#include "__MonadPage.h"
#include "GUI/__MonadGUI.h"
#include "Kernel/Templates.h"
#include "Time/TimeLerp.h"

namespace Monad::Pages
{
	/// <summary>
	/// Language and accessibility state flags.
	/// </summary>
	extern bool g_langChanged2;
	extern bool g_langChanged;
	extern bool g_aAChanged;

	/// <summary>
	/// Restores the previous page state.
	/// </summary>
	const std::string& ResetPageBackup();

	/// <summary>
	/// Reference counter for hourglass cursor usage.
	/// Displays busy cursor while references exist.
	/// </summary>
	struct HourGlassCursorRefs final
	{
		void operator++() noexcept;
		void operator--() noexcept;
		operator bool() const noexcept;
		void Wait() const noexcept;

	private:
		std::atomic_size_t m_liveRefs = 0;
	};

	extern HourGlassCursorRefs g_hourGlassBlockingRefs;

	/// <summary>
	/// RAII helper for scoped page processing.
	/// </summary>
	struct ScopeCounter final
	{
		ScopeCounter() noexcept;
		~ScopeCounter();

		OPER_DEL_NO_DEF_CTOR(ScopeCounter);
	};

	/// <summary>
	/// Indicates whether page processing is active.
	/// </summary>
	extern std::atomic_bool g_processingPages;

	/// <summary>
	/// Resets the current page to a splash/loading state.
	/// </summary>
	void ResetCurrentPage() noexcept;

	/// <summary>
	/// Applies the current cursor state.
	/// </summary>
	void SetMyCursor() noexcept;

	/// <summary>
	/// Selects a new page (dialog set).
	/// </summary>
	void SelectPage(const std::string& pageNo);

	/// <summary>
	/// Removes the current page from the queue.
	/// </summary>
	void DequeuePage();

	// ---------------------------------------------------------------------

	/// <summary>
	/// Base class for animated page transitions.
	/// </summary>
	interface TimeLerpMorphing abstract : Time::TimeLerp
	{
		enum class GET_LERPS : size_t
		{
			DECAY,
			RAISE
		};

	protected:
		void SetMorphing(float lerp) noexcept;

		const GET_LERPS c_isMe;

		TimeLerpMorphing(
			double startValue,
			double stopValue,
			GET_LERPS isMe
		) noexcept;

		void SetFinish() noexcept;

	private:
		void OnFrameMove() noexcept override;
	};

	/// <summary>
	/// Page transition handler (fade-out / decay phase).
	/// </summary>
	struct TimeLerpMorphingDecay final : TimeLerpMorphing
	{
		TimeLerpMorphingDecay();

		/// <summary>
		/// Selects a new page to transition to.
		/// </summary>
		void SelectPage(const std::string& pageNo);
		/// <summary>
		/// Removes the current page from the queue.
		/// </summary>
		void DequeuePage();
		bool IsInExit() const noexcept;
		/// <summary>
		/// Indicates whether a transition is in progress.
		/// </summary>
		bool IsWorking() const noexcept;

	private:
		void SetMorphedPage(const std::string& pageNo);
		void OnStart() noexcept final;
		void OnFlush() noexcept final;

		const Kernel::Me<TimeLerpMorphingDecay> c_me;
		std::string m_pageNo = "boot"_pageNo;

		/// <summary>
		/// Page fade-in phase.
		/// </summary>
		struct TimeLerpRaise final : TimeLerpMorphing
		{
			using TimeLerpMorphing::TimeLerpMorphing;
			void OnFlush() noexcept final;
		} m_raise;

		std::queue<std::string> m_pageQueue;
	};

	extern TimeLerpMorphingDecay* g_MorphingDecay;
}
