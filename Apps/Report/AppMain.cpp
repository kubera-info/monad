// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppMain.h"
// Monad
#include "Globals/__MonadGlobals.h"
#include "Win32Application.h"
#include "AppParameter.h"
#include "BugXML.h"
#include "Exceptions/EventView.h"
#include "Exceptions/Exceptions.h"
#include "Kernel/BytesVector.h"
#include "HttpsAPI.h"
#include "System/Parameter.h"
#include "System/Exit.h"
#include "AppSingleton.h"
#include "Input/Cursor.h"
#include "Renderer/__MonadRenderer.h"

using namespace std;
using namespace Monad::I18N;
using namespace Monad::System;
using namespace Monad::Renderer;

namespace
{
	const Monad::GUI::IconicStyle
		STYLE_CANCEL_BUTTON{
				Monad::GUI::GUI_RED,
				{ "no"_cue },
				{ "no"_shaderResource }
	},
		STYLE_OK_BUTTON{
					Monad::GUI::GUI_GREEN,
					{},
					{ "ok"_shaderResource }
	};

	const wchar_t nl0 = L'\n';

	namespace Report::GUI
	{
		const I18NString
			QUESTION_EXCEPTION
		{
			L"Program wymaga zgody na wysłanie\nanonimowego raportu błędu do twórców.\nCzy {} może przekazać raport ? "_pl,
			L"Программа требует разрешения на отправку\nанонимного отчета об ошибке разработчикам.\nМожет ли {} отправить отчет?"_ru,
			L"This program needs a permission to send\nan anonymous bug report to the developers.\nCould {} post the report?"_us
		},
			STAGE
		{
			L"Miejsce: "_pl,
			L"Стадия: "_ru,
			L"Stage: "_us
		},
			THREAD
		{
			L"Wątek: "_pl,
			L"Нить: "_ru,
			L"Thread: "_us
		},
			ERROR_DESC
		{
			L"Błąd: "_pl,
			L"Ошибка: "_ru,
			L"Error: "_us
		},
			WINE
		{
			L"Windows czy Wine: "_pl,
			L"Windows или Wine: "_ru,
			L"Windows or Wine: "_us
		},
			PAGE
		{
			L"Strona: "_pl,
			L"Страница: "_ru,
			L"Page: "_us
		};
	}
}

#pragma region DialogReportGeneric
DialogReportGeneric::DialogReportGeneric()
	: DialogFullScreenGeneric(
		"quick"_pageNo,
		L"#logo#" + g_parameters->GetParameter(PAR_NAME_APP_TITLE)
	)
{
}

std::wstring DialogReportGeneric::GetReportString()
{
	return Monad::I18N::Errors::EXCEPTION_TITLE.GetVal()
		+ nl0 + Monad::I18N::APP_VER_LABEL.VFormat(g_parameters->GetParameter(PAR_NAME_VERSION))
		+ nl0 + g_parameters->GetParameter(PAR_NAME_EXCEPTION)
		+ nl0 + Report::GUI::ERROR_DESC.GetVal() + g_parameters->GetErrorMessageFromHRESULT()
		+ nl0 + Report::GUI::STAGE.GetVal() + g_parameters->GetParameter(PAR_NAME_STAGE)
		+ nl0 + g_parameters->GetParameter(PAR_NAME_GPU);
}

void DialogReportGeneric::AddRenderedText(
	const std::wstring& text
)
{
	GetFont(Monad::Fonts::FONT_CENTURY_GOTHIC).AddRenderedText(
		Monad::Fonts::REPORT_FONT_STYLE,
		std::make_optional<DirectX::XMFLOAT2>(
			-.9f,
			.7125f
		),
		text);
}

void DialogReportGeneric::OnInitDialogGeneric()
{
	m_label = GetReportString();
	Monad::Exceptions::EventLog(g_parameters->GetParameter(PAR_NAME_APP_TITLE)).ReportErrorEvent(m_label);
}
DialogReportGeneric::ButtonOneToolbar::ButtonOneToolbar(
	const float horizontalOffset,
	const Monad::GUI::IconicStyle& style
) :
		ButtonOneToolbarGeneric(
		"quick"_pageNo,
			{ horizontalOffset, Monad::GUI::POS_Y },
			style
	)
{
}

void DialogReportGeneric::ButtonAbort::OnClick()
{
	g_keyPressed = true;
	Monad::Pages::SelectPage("exit"_pageNo);
}

#pragma endregion

#pragma region DialogReport
DialogReport::DialogReport() :
	m_abortButton(
		-Monad::GUI::DISTANCE / 2.0f,
		STYLE_CANCEL_BUTTON),
	m_sendReportButton(
		Monad::GUI::DISTANCE / 2.0f,
		STYLE_OK_BUTTON
)
{
	m_label = GetReportString() + nl0 + Report::GUI::THREAD.GetVal() + g_parameters->GetParameter(PAR_NAME_THREAD_NAME)
		+ nl0 + Report::GUI::WINE.GetVal() + g_parameters->GetParameter(PAR_NAME_NATIVE)
		+ nl0 + Report::GUI::PAGE.GetVal() + g_parameters->GetParameter(PAR_NAME_PAGE);

	AddRenderedText(
		GetReportStringUI() + nl0 + nl0 +
		Report::GUI::QUESTION_EXCEPTION.
		VFormat(g_parameters->GetParameter(PAR_NAME_APP_TITLE)));
	AddRenderedObjects({
		&m_abortButton,
		&m_sendReportButton }		
	);
}

void DialogReport::ButtonSendReport::OnClick()
{
	if (Monad::App::g_sent.test())
		Monad::Pages::SelectPage("exit"_pageNo);
	else
	{
		g_keyPressed = true;
		Monad::App::g_singleton->Bulk();
	}
}

std::wstring DialogReport::GetReportStringUI()
{
	return Monad::I18N::Errors::EXCEPTION_TITLE.GetVal() + nl0 + Report::GUI::ERROR_DESC.GetVal()
		+ g_parameters->GetParameter(PAR_NAME_EXCEPTION)
		+ nl0 + Monad::I18N::APP_VER_LABEL.VFormat(g_parameters->GetParameter(PAR_NAME_VERSION))
		+ nl0 + Report::GUI::STAGE.GetVal() + g_parameters->GetParameter(PAR_NAME_STAGE)
		+ nl0 + L"GPU: " + g_parameters->GetParameter(PAR_NAME_GPU);
}
#pragma endregion

#pragma region DialogInfo
DialogInfo::DialogInfo() :
	m_abortButton(
		-0.0f,
		STYLE_CANCEL_BUTTON)
{
	m_label = GetReportString();
	AddRenderedText(m_label);
	AddRenderedObject(&m_abortButton);
}
#pragma endregion