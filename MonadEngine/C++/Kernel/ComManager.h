// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <assert.h>
// Monad
#include "Kernel/Del.h"

#define MONAD_COM_REF_CNT_GENERIC(CALL,VIRTUAL)\
	DWORD CALL AddRef() VIRTUAL\
	{\
		return InterlockedIncrement(&m_referenceCount);\
	}\
	DWORD CALL Release() VIRTUAL\
	{\
		assert(m_referenceCount > 0 && L"Nonzero ref count");\
		const auto remainingCount(InterlockedDecrement(&m_referenceCount));\
		if (0UL == remainingCount)\
			delete this;\
		return remainingCount;\
	}\
	private:\
	volatile DWORD m_referenceCount = 1;\
	public:

#define MONAD_CPP_REF_CNT MONAD_COM_REF_CNT_GENERIC(,noexcept)

#define MONAD_COM_REF_CNT(SUPER) MONAD_COM_REF_CNT_GENERIC(STDMETHODCALLTYPE,final)\
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface) final\
	{\
		if (IID_IUnknown == riid || __uuidof(SUPER) == riid)\
		{\
			AddRef();\
			*ppvInterface = this;\
			return S_OK;\
		}\
		else\
		{\
			*ppvInterface = nullptr;\
			return E_NOINTERFACE;\
		}\
	}

namespace Monad
{
	namespace Kernel
	{
		interface RefCount abstract
		{
			MONAD_CPP_REF_CNT;

		protected:
			RefCount() = default;
			virtual ~RefCount() = default;
			OPER_DEL_NO_DEF_CTOR(RefCount);

		public:
			inline DWORD GetRefCount() const noexcept { return m_referenceCount; }
		};
	}
}