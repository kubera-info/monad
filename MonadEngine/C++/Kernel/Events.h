// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// STD
#include <xutility>
// Monad
#include "Del.h"

//
// Monad Events – event interfaces that can be implemented by derived classes
//

namespace Monad::Kernel
{
	/// <summary>
	/// Identifiers for supported engine events.
	/// The order is significant and corresponds to internal storage.
	/// </summary>
	enum class EVENT_IDS : size_t
	{
		FRAME_MOVE_STUB,
		POINTER_DOWN,
		POINTER_UP,
		D3D_CREATE,
		D3D_RENDER,
		D3D_CLEAR_ASSETS,
		LANGUAGE_CHANGE_EMOJI,
		LANGUAGE_CHANGE_FONT,
		EVENT_ID_SIZE
	};

	// ---------------------------------------------------------------------
	// Event dispatch API
	// ---------------------------------------------------------------------

	void FireEvent(
		EVENT_IDS event
	);
	void FireEventOnCopy(
		EVENT_IDS event
	);
	void FireEventFont();

	void RegisterEventGeneric(
		const EVENT_IDS event,
		void* const object
	);
	void RegisterEvent(
		EVENT_IDS event,
		void* managerStub
	);
	void UnregisterEvent(
		EVENT_IDS event,
		void* managerStub
	);
	void ReplaceEvent(
		EVENT_IDS event,
		void* soourceManagerStub,
		void* destinateManagerStub
	);
	bool IsRegisteredEvent(
		EVENT_IDS event,
		const void* managerStub
	);
	void ReplaceEvent(
		EVENT_IDS event,
		void* const soourceManagerStub,
		void* const destinateManagerStub
	);
	void* RegisteredEventIterator(
		const EVENT_IDS event,
		const void* managerStub
	);

	// ---------------------------------------------------------------------
	// Base manager stub
	// ---------------------------------------------------------------------

	template<auto ID>
	interface ManagerStub abstract
	{
		ManagerStub() = default;

		ManagerStub(const ManagerStub& source)
		{
			CopyEvent(source);
		}

		ManagerStub(ManagerStub&& source) noexcept
		{
			MoveEvent(std::move(source));
		}

		ManagerStub& operator=(const ManagerStub& source)
		{
			CopyEvent(source);

			return *this;
		}

		ManagerStub& operator=(ManagerStub&& source) noexcept
		{
			MoveEvent(source);

			return *this;
		}

		virtual ~ManagerStub()
		{
			Unregister();
		}

		void CopyEvent(const ManagerStub& source) noexcept
		{
			if (IsRegisteredEvent(ID, &source))
				RegisterEventGeneric(ID, this);
		}

		void MoveEvent(ManagerStub&& source) noexcept
		{
			if (auto src = RegisteredEventIterator(ID, &source); IsRegisteredEvent(ID, this))
				if (src)
					source.Unregister();
				else
					Unregister();
			else
				if (src)
					ReplaceEvent(ID, &source, this);
		}

		/// <summary>
		/// Registers this object for its associated event.
		/// </summary>
		void Register()
		{
			RegisterEvent(ID, this);
		}

		/// <summary>
		/// Unregisters this object from its associated event.
		/// </summary>
		void Unregister()
		{
			UnregisterEvent(ID, this);
		}
	};

	// ---------------------------------------------------------------------
	// Event interfaces
	// ---------------------------------------------------------------------

	interface EventFrameMoveStub abstract
		: ManagerStub<EVENT_IDS::FRAME_MOVE_STUB>
	{
		OPER_DEF(EventFrameMoveStub);
		virtual bool OnFrameMoveStub() = 0;
	};

	interface EventPointerDownStub abstract
		: ManagerStub<EVENT_IDS::POINTER_DOWN>
	{
		OPER_DEF(EventPointerDownStub);
		virtual bool OnPointerDownStub() = 0;
	};

	interface EventPointerUpStub abstract
		: ManagerStub<EVENT_IDS::POINTER_UP>
	{
		OPER_DEF(EventPointerUpStub);
		virtual bool OnPointerUpStub() = 0;
	};

	interface EventD3DCreateStub abstract
		: ManagerStub<EVENT_IDS::D3D_CREATE>
	{
		OPER_DEF(EventD3DCreateStub);
		virtual bool OnD3DCreateStub() = 0;
	};

	interface EventD3DRenderStub abstract
		: ManagerStub<EVENT_IDS::D3D_RENDER>
	{
		OPER_DEF(EventD3DRenderStub);
		virtual bool OnD3DRenderStub() = 0;
	};

	interface EventClearD3DAssetsStub abstract
		: ManagerStub<EVENT_IDS::D3D_CLEAR_ASSETS>
	{
		OPER_DEF(EventClearD3DAssetsStub);
		virtual bool OnClearD3DAssetsStub() = 0;
	};

	interface EventLanguageChangeEmojiStub abstract
		: ManagerStub<EVENT_IDS::LANGUAGE_CHANGE_EMOJI>
	{
		OPER_DEF(EventLanguageChangeEmojiStub);
		virtual bool OnLanguageChangeEmojiStub() = 0;
	};

	interface EventLanguageChangeFontStub abstract
		: ManagerStub<EVENT_IDS::LANGUAGE_CHANGE_FONT>
	{
		OPER_DEF(EventLanguageChangeFontStub);
		virtual bool OnLanguageChangeFontStub() = 0;
	};
}
