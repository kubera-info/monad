// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Platform
#include <DocumentTarget.h>
#include "Modules/Xbox-ATG-Samples/Kits/CPUTK/OSLockable.h"

// Monad
#include "Kernel/ComManager.h"
#include "Kernel/Del.h"

namespace Monad::Print
{
	/// <summary>
	/// Handles print job status notifications for Direct2D printing.
	/// Implements the IPrintDocumentPackageStatusEvent interface.
	/// </summary>
	struct D2DPrintJobChecker final : IPrintDocumentPackageStatusEvent
	{
		D2DPrintJobChecker() noexcept;
		~D2DPrintJobChecker();

		OPER_DEL_NO_DEF_CTOR(D2DPrintJobChecker);

		// ---------------------------------------------------------------------
		// IUnknown
		// ---------------------------------------------------------------------

		/// <summary>
		/// Implements reference counting for COM.
		/// </summary>
		MONAD_COM_REF_CNT(IPrintDocumentPackageStatusEvent);

		// ---------------------------------------------------------------------
		// IDispatch
		// ---------------------------------------------------------------------

		STDMETHODIMP GetTypeInfoCount(
			_Out_ uint32_t* pctinfo
		) noexcept final;

		STDMETHODIMP GetTypeInfo(
			uint32_t iTInfo,
			LCID lcid,
			_Outptr_result_maybenull_ ITypeInfo** ppTInfo
		) noexcept final;

		STDMETHODIMP GetIDsOfNames(
			_In_ REFIID riid,
			_In_reads_(cNames) LPOLESTR* rgszNames,
			_In_range_(0, 16384) uint32_t cNames,
			LCID lcid,
			_Out_writes_(cNames) DISPID* rgDispId
		) noexcept final;

		STDMETHODIMP Invoke(
			DISPID dispIdMember,
			REFIID riid,
			LCID lcid,
			WORD wFlags,
			DISPPARAMS* pDispParameters,
			VARIANT* pVarResult,
			EXCEPINFO* pExcepInfo,
			uint32_t* puArgErr
		) noexcept final;

		// ---------------------------------------------------------------------
		// IPrintDocumentPackageStatusEvent
		// ---------------------------------------------------------------------

		/// <summary>
		/// Called when the print package status changes.
		/// </summary>
		STDMETHODIMP PackageStatusUpdated(
			_In_ PrintDocumentPackageStatus* packageStatus
		) final;

		// ---------------------------------------------------------------------
		// Public API
		// ---------------------------------------------------------------------

		/// <summary>
		/// Initializes the print job checker and registers for notifications.
		/// </summary>
		HRESULT Initialize(
			_In_ IPrintDocumentPackageTarget* documentPackageTarget
		) noexcept;

		/// <summary>
		/// Blocks until the print job finishes or fails.
		/// </summary>
		HRESULT WaitForCompletion() noexcept;

		/// <summary>
		/// Outputs human-readable information about a print job status.
		/// </summary>
		static void OutputPackageStatus(
			_In_ PrintDocumentPackageStatus packageStatus
		) noexcept;

	private:
		/// <summary>
		/// Releases all COM and synchronization resources.
		/// </summary>
		void ReleaseResources() noexcept;

	private:
		PrintDocumentPackageStatus m_documentPackageStatus{};
		DWORD m_eventCookie = 0;
		HANDLE m_completionEvent = nullptr;

		ATG::CriticalSectionLockable m_criticalSection;
		IConnectionPoint* m_connectionPoint = nullptr;

		bool m_isInitialized = false;
	};
}
