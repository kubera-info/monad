// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Events.h"
// STD
#include <deque>
#include <vector>
#include <ranges>
// Monad
#include "Exceptions/Exceptions.h"

namespace Monad::Kernel
{
	namespace
	{
		/// <summary>
		/// Function signature used to execute an event callback.
		/// </summary>
		using FnExecuteEvent = bool(*)(void* const object);

		/// <summary>
		/// Container storing objects subscribed to a specific event.
		/// </summary>
		struct SkilledObjects final : std::deque<void*>
		{
			explicit SkilledObjects(FnExecuteEvent functor) noexcept;

			/// <summary>
			/// Executes callbacks for all registered objects.
			/// </summary>
			void ForEachFunctor();

			/// <summary>
			/// Executes callbacks on a copy of the container.
			/// Safe when callbacks may unregister themselves.
			/// </summary>
			void ForEachFunctorOnCopy();

		private:
			FnExecuteEvent m_functor;
		};

		/// <summary>
		/// Registry holding all event handlers grouped by event type.
		/// </summary>
		struct SkillCollector
			: std::array<SkilledObjects,
			static_cast<size_t>(EVENT_IDS::EVENT_ID_SIZE)>
		{
			SkillCollector() noexcept;
		};

		void SkilledObjects::ForEachFunctorOnCopy()
		{
			// Copy is required because callbacks may unregister themselves
			for (const auto& object : std::vector<void*>{ cbegin(), cend() })
				m_functor(object);
		}

		SkillCollector g_registry;
	}

	// ---------------------------------------------------------------------

	void RegisterEventGeneric(
		const EVENT_IDS event,
		void* const object)
	{
		g_registry
			[static_cast<size_t>(event)]
			.emplace_back(object);
	}

	void UnregisterEventGeneric(
		SkilledObjects& objects,
		void* const object)
	{
		if (const auto it = std::ranges::find(objects, object);
			it != objects.cend())
			objects.erase(it);
	}

	bool IsRegisteredEvent(
		const EVENT_IDS event,
		const void* managerStub)
	{
		const auto& container =
			g_registry[static_cast<size_t>(event)];

		return std::ranges::find(container, managerStub) != container.cend();
	}

	void* RegisteredEventIterator(
		const EVENT_IDS event,
		const void* managerStub)
	{
		auto& container =
			g_registry[static_cast<size_t>(event)];

		for (size_t i = 0; i < container.size(); ++i)
			if (managerStub == container[i])
				return &container[i];

		return nullptr;
	}

	void FireEvent(const EVENT_IDS event)
	{
		g_registry
			[static_cast<size_t>(event)]
			.ForEachFunctor();
	}

	void FireEventOnCopy(const EVENT_IDS event)
	{
		g_registry
			[static_cast<size_t>(event)]
			.ForEachFunctorOnCopy();
	}

	void FireEventFont()
	{
		FireEvent(EVENT_IDS::LANGUAGE_CHANGE_FONT);
		FireEvent(EVENT_IDS::LANGUAGE_CHANGE_EMOJI);
	}

	void RegisterEvent(
		const EVENT_IDS event,
		void* managerStub)
	{
		if (!IsRegisteredEvent(event, managerStub))
			RegisterEventGeneric(event, managerStub);
		else
			THROW_EXC(Monad::Exceptions::ClassAlreadyExists, E_FAIL, L"Already registered for event");
	}

	void UnregisterEvent(
		EVENT_IDS event,
		void* managerStub)
	{
		UnregisterEventGeneric(
			g_registry[static_cast<size_t>(event)],
			managerStub
		);
	}

	void ReplaceEvent(EVENT_IDS event, void* soourcePtrManagerStub, void* destinateManagerStub)
	{
		if (auto ptr = RegisteredEventIterator(event, soourcePtrManagerStub); ptr)
			*static_cast<void**> (ptr) = destinateManagerStub;
	}
	// ---------------------------------------------------------------------

	SkillCollector::SkillCollector() noexcept
		: array{
			SkilledObjects{ [](void* o) noexcept { return static_cast<EventFrameMoveStub*>(o)->OnFrameMoveStub(); } },
			SkilledObjects{ [](void* o) noexcept { return static_cast<EventPointerDownStub*>(o)->OnPointerDownStub(); } },
			SkilledObjects{ [](void* o) noexcept { return static_cast<EventPointerUpStub*>(o)->OnPointerUpStub(); } },
			SkilledObjects{ [](void* o) noexcept { return static_cast<EventD3DCreateStub*>(o)->OnD3DCreateStub(); } },
			SkilledObjects{ [](void* o) noexcept { return static_cast<EventD3DRenderStub*>(o)->OnD3DRenderStub(); } },
			SkilledObjects{ [](void* o) noexcept { return static_cast<EventClearD3DAssetsStub*>(o)->OnClearD3DAssetsStub(); } },
			SkilledObjects{ [](void* o) noexcept { return static_cast<EventLanguageChangeEmojiStub*>(o)->OnLanguageChangeEmojiStub(); } },
			SkilledObjects{ [](void* o) noexcept { return static_cast<EventLanguageChangeFontStub*>(o)->OnLanguageChangeFontStub(); } }
		}
	{}

	SkilledObjects::SkilledObjects(FnExecuteEvent functor) noexcept
		: m_functor(functor)
	{
		assert(m_functor != nullptr);
	}

	void SkilledObjects::ForEachFunctor()
	{
		for (const auto& object : *this)
			if (!m_functor(object))
				break;
	}
}
