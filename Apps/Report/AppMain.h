// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <shobjidl.h>
// Monad
#include "System/System.h"
#include "Threads/ThreadPool.h"
#include "GUI/Dialog.h"
#include "GUI/LookAndFeel.h"

/// <summary>
/// This is an abstract, base dialog class for all report windows
/// </summary>
struct DialogReportGeneric : Monad::GUI::DialogFullScreenGeneric
{
	DialogReportGeneric();

protected:
	std::wstring GetReportString();
	void AddRenderedText(
		const std::wstring& text
	);
	void OnInitDialogGeneric();
	
	/// <summary>
	/// Base class for toolbar buttons.
	/// </summary>
	interface ButtonOneToolbar abstract : Monad::GUI::ButtonOneToolbarGeneric
	{
		ButtonOneToolbar(
			const float horizontalOffset,
			const Monad::GUI::IconicStyle & style
		);
	};
	struct ButtonAbort : ButtonOneToolbar
	{
		using ButtonOneToolbar::ButtonOneToolbar;

	private:
		void OnClick() final;
	};

	std::wstring m_label;
};

/// <summary>
/// This dialog is used to report errors through the HTTPS protocol
/// </summary>
struct DialogReport final : DialogReportGeneric
{
	// Construction
	DialogReport();
	
private:
	ButtonAbort m_abortButton;
	struct ButtonSendReport : ButtonOneToolbar
	{
		using ButtonOneToolbar::ButtonOneToolbar;

	private:
		void OnClick() final;
	} m_sendReportButton;
	
	std::wstring GetReportStringUI();
};

/// <summary>
/// This dialog is used to show information about the error
/// </summary>
struct DialogInfo final : DialogReportGeneric
{
	// Construction
	DialogInfo();

private:
	ButtonAbort m_abortButton;
};