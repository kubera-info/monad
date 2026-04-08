// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

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

	void FireEvent(EVENT_IDS event);
	void FireEventOnCopy(EVENT_IDS event);
	void FireEventFont();

	void RegisterEvent(EVENT_IDS event, void* managerStub);
	void UnregisterEvent(EVENT_IDS event, void* managerStub);

	bool IsRegisteredEvent(EVENT_IDS event, void* managerStub);

	// ---------------------------------------------------------------------
	// Base manager stub
	// ---------------------------------------------------------------------

	template<auto ID>
	interface ManagerStub
	{
		ManagerStub() = default;

		ManagerStub(const ManagerStub& source)
		{
			if (IsRegisteredEvent(ID, &source))
				Register();
		}

		ManagerStub(ManagerStub&& source)
		{
			if (IsRegisteredEvent(ID, &source))
			{
				source.Unregister();
				Register();
			}
			else
			{
				Unregister();
			}
		}

		ManagerStub& operator=(const ManagerStub& source)
		{
			if (IsRegisteredEvent(ID, &source))
				Register();
			return *this;
		}

		ManagerStub& operator=(ManagerStub&& source)
		{
			if (IsRegisteredEvent(ID, &source))
			{
				source.Unregister();
				Register();
			}
			else
				Unregister();
			return *this;
		}

		virtual ~ManagerStub()
		{
			Unregister();
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
		virtual bool OnFrameMoveStub() = 0;
	};

	interface EventPointerDownStub abstract
		: ManagerStub<EVENT_IDS::POINTER_DOWN>
	{
		virtual bool OnPointerDownStub() = 0;
	};

	interface EventPointerUpStub abstract
		: ManagerStub<EVENT_IDS::POINTER_UP>
	{
		virtual bool OnPointerUpStub() = 0;
	};

	interface EventD3DCreateStub abstract
		: ManagerStub<EVENT_IDS::D3D_CREATE>
	{
		virtual bool OnD3DCreateStub() = 0;
	};

	interface EventD3DRenderStub abstract
		: ManagerStub<EVENT_IDS::D3D_RENDER>
	{
		virtual bool OnD3DRenderStub() = 0;
	};

	interface EventClearD3DAssetsStub abstract
		: ManagerStub<EVENT_IDS::D3D_CLEAR_ASSETS>
	{
		virtual bool OnClearD3DAssetsStub() = 0;
	};

	interface EventLanguageChangeEmojiStub abstract
		: ManagerStub<EVENT_IDS::LANGUAGE_CHANGE_EMOJI>
	{
		virtual bool OnLanguageChangeEmojiStub() = 0;
	};

	interface EventLanguageChangeFontStub abstract
		: ManagerStub<EVENT_IDS::LANGUAGE_CHANGE_FONT>
	{
		virtual bool OnLanguageChangeFontStub() = 0;
	};
}
