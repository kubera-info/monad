// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "D2DPrintJobChecker.h"

// Platform
#pragma comment(lib, "Prntvpt.lib")

namespace Monad::Print
{
	D2DPrintJobChecker::D2DPrintJobChecker() noexcept :
		m_eventCookie{},
		m_completionEvent{},
		m_connectionPoint{},
		m_isInitialized{ false },
		m_documentPackageStatus{}
	{}

	D2DPrintJobChecker::~D2DPrintJobChecker()
	{
		ReleaseResources();
	}

	// Initialize this print job checker by registering a print document package target.
	// This is required before the application can use this print job checker to monitor a print job.
	HRESULT D2DPrintJobChecker::Initialize(
		_In_ IPrintDocumentPackageTarget* documentPackageTarget
	) noexcept
	{
		// Application can only initialize this job checker once.
		if (m_isInitialized)
		{
			return E_FAIL;
		}

		HRESULT hr = (documentPackageTarget != nullptr) ? S_OK : E_INVALIDARG;

		IConnectionPointContainer* connectionPointContainer = nullptr;
		if (SUCCEEDED(hr))
		{
			hr = documentPackageTarget->QueryInterface(IID_PPV_ARGS(&connectionPointContainer));
		}

		if (SUCCEEDED(hr))
		{
			hr = connectionPointContainer->FindConnectionPoint(
				__uuidof(IPrintDocumentPackageStatusEvent),
				&m_connectionPoint
			);
		}

		IUnknown* unknownEvent = nullptr;
		if (SUCCEEDED(hr))
		{
			hr = QueryInterface(IID_PPV_ARGS(&unknownEvent));
		}

		if (SUCCEEDED(hr))
		{
			hr = m_connectionPoint->Advise(unknownEvent, &m_eventCookie);
		}

		// Create an event handle for print job completion.
		if (SUCCEEDED(hr))
		{
			m_completionEvent = ::CreateEvent(
				nullptr, // The handle cannot be inherited by child processes.
				TRUE,    // The event object requires the use of the ResetEvent function to set the event state to nonsignaled.
				FALSE,   // The initial state of the event object is nonsignaled.
				nullptr  // Name of the event object.
			);
			hr = (!m_completionEvent) ? HRESULT_FROM_WIN32(::GetLastError()) : hr;
		}
		SAFE_RELEASE(unknownEvent);
		SAFE_RELEASE(connectionPointContainer);

		if (SUCCEEDED(hr))
		{
			m_isInitialized = true;
		}
		else
		{
			ReleaseResources();
		}

		return hr;
	}

	HRESULT D2DPrintJobChecker::GetTypeInfoCount(
		_Out_ uint32_t* /* pctinfo */
	) noexcept
	{
		return E_NOTIMPL;
	}

	HRESULT D2DPrintJobChecker::GetTypeInfo(
		uint32_t /* iTInfo */,
		LCID /* lcid */,
		_Outptr_result_maybenull_ ITypeInfo** /* ppTInfo */
	) noexcept
	{
		return E_NOTIMPL;
	}

	HRESULT D2DPrintJobChecker::GetIDsOfNames(
		_In_                        REFIID     /* riid */,
		_In_reads_(cNames)          LPOLESTR*  /* rgszNames */,
		[[maybe_unused]] _In_range_(0, 16384)        uint32_t       cNames,
		LCID       /* lcid */,
		[[maybe_unused]] __out_ecount_full(cNames)   DISPID*    /* rgDispId */
	) noexcept
	{
		return E_NOTIMPL;
	}

	HRESULT D2DPrintJobChecker::Invoke(
		DISPID          /* dispIdMember */,
		REFIID          /* riid */,
		LCID            /* lcid */,
		WORD            /* wFlags */,
		DISPPARAMS*     /* pDispParameters */,
		VARIANT*        /* pVarResult */,
		EXCEPINFO*      /* pExcepInfo */,
		uint32_t*           /* puArgErr */
	) noexcept
	{
		return E_NOTIMPL;
	}

	// Callback to indicate that the status of the package has changed.
	STDMETHODIMP D2DPrintJobChecker::PackageStatusUpdated(
		_In_ PrintDocumentPackageStatus* packageStatus
	)
	{
		HRESULT hr = (packageStatus == nullptr) ? E_INVALIDARG : S_OK;

		if (SUCCEEDED(hr))
		{
			// The package status update operation is guarded with a critical section,
			// since PackageStatusUpdated may be called in a very short time slot.
			try
			{
				m_criticalSection.lock();
				m_documentPackageStatus = *packageStatus;
				m_criticalSection.unlock();
			}
			catch (std::system_error&)
			{
				return E_FAIL;
			}
		}

		if (SUCCEEDED(hr))
		{
			// Signal the print job complete event.
			if (PrintDocumentPackageCompletion_InProgress != m_documentPackageStatus.Completion)
			{
				if (!SetEvent(m_completionEvent))
				{
					hr = HRESULT_FROM_WIN32(::GetLastError());
				}
			}
		}

#if defined _DEBUG
		OutputPackageStatus(m_documentPackageStatus);
#endif

		return hr;
	}

	// Return print document package status.
	/*PrintDocumentPackageStatus D2DPrintJobChecker::GetStatus() noexcept
	{
		return(m_documentPackageStatus);
	}*/

	// Wait for job completion event to be signaled; in the meanwhile keep pumping messages to avoid deadlocks.
	//
	// This is an example for deadlock: when user print to a " Print To File" printer, the winspool.dll will try to pop
	// up a modal dialog asking for file name. In order to show modal dialog, winspool will call EnableWindow(FALSE) to
	// disable this window. Therefore, the winspool thread is waiting for us to process the message of disabling the
	// window, while our thread is waiting for the winspool thread to complete.
	HRESULT D2DPrintJobChecker::WaitForCompletion() noexcept
	{
		// Return successfully if this job checker is not initialized.
		if (!m_isInitialized)
		{
			return S_OK;
		}

		bool isWaiting = true;
		while (isWaiting)
		{
			DWORD waitResult = ::MsgWaitForMultipleObjects(
				1,                  // Number of object handles in the array.
				&m_completionEvent, // Array of object handles.
				FALSE,              // Function returns when the state of any one of the objects is set to signaled.
				INFINITE,           // Function will return only when the specified objects are signaled.
				QS_ALLINPUT         // Any message is in the queue.
			);
			if (waitResult == WAIT_OBJECT_0 + 1)
			{
				// This is not the desired completion event.
				MSG msg;
				while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					::DispatchMessage(&msg);
				}
				if (msg.message == WM_QUIT)
				{
					isWaiting = false;
					break;
				}
			}
			else
			{
				isWaiting = false;
			}
		}

		return S_OK;
	}

	// Output debug message for the print document package status.
	void D2DPrintJobChecker::OutputPackageStatus(
		_In_ PrintDocumentPackageStatus
	) noexcept
	{
		/*switch (packageStatus.Completion)
		{
		case PrintDocumentPackageCompletion_Completed:
			System::MndOutputDebugString(L"PrintDocumentPackageCompletion_Completed; ");
			break;

		case PrintDocumentPackageCompletion_Canceled:
			System::MndOutputDebugString(L"PrintDocumentPackageCompletion_Canceled; ");
			break;

		case PrintDocumentPackageCompletion_Failed:
			System::MndOutputDebugString(L"PrintDocumentPackageCompletion_Failed; ");
			break;

		case PrintDocumentPackageCompletion_InProgress:
			System::MndOutputDebugString(L"PrintDocumentPackageCompletion_InProgress; ");
			break;

		default:
			System::MndOutputDebugString(L"PrintDocumentPackageStatus unknown! ");
			break;
		}
		WCHAR wchar_t[256] = {0};
		StringCchPrintf(
			messageBuffer,
			_countof(messageBuffer),
			L"\tjobID:%3d, currentDoc:%3d, currentPage:%3d, currentPageTotal:%3d, packageStatus:0x%08X\n",
			packageStatus.JobId,
			packageStatus.CurrentDocument,
			packageStatus.CurrentPage,
			packageStatus.CurrentPageTotal,
			packageStatus.PackageStatus
			);
		System::OutputDebugString(messageBuffer);*/
	}

	// Release resources.
	void D2DPrintJobChecker::ReleaseResources() noexcept
	{
		if (m_connectionPoint != nullptr)
		{
			if (m_eventCookie != 0)
			{
				m_connectionPoint->Unadvise(m_eventCookie);
			}
			m_connectionPoint->Release();
			m_connectionPoint = nullptr;
		}

		if (m_completionEvent != nullptr)
		{
			::CloseHandle(m_completionEvent);
			m_completionEvent = nullptr;
		}

		return;
	}
}