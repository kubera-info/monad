// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppSettings.h"

// STD
#include <numeric>

// Monad
#include "Renderer/__MonadRenderer.h"
#include "AppDict.h"
#include "AppGUILookAndFeel.h"
#include "AppSingleton.h"
#include "Globals/__MonadGlobals.h"
#include "Win32Application.h"
#include "Time/__MonadTime.h"
#include "System/System.h"

using namespace DirectX;
using namespace std;

namespace Monad
{
	using namespace Fonts;
	using namespace Globals;
	using namespace GUI;
	using namespace I18N;
	using namespace Kernel;
	using namespace Pages;
	using namespace Renderer;
	using namespace System;
	using namespace Time;

	namespace App
	{
		namespace
		{
			/*const I18NFloat
				LEFT_COLUMN{
				make_pair("pl-PL"s, 0.82f),
				make_pair("ru-RU"s, -0.85f),
				make_pair(DEFAULT_LANGUAGE, -0.81f) },
				RIGHT_COLUMN{
				make_pair("pl-PL"s, 0.12f),
				make_pair("ru-RU"s, 0.04f),
				make_pair(DEFAULT_LANGUAGE, -0.15f) };*/
			constexpr float g_pos = -0.15f, g_vertical = 0.22f, g_size = 0.25f;

			consteval XMFLOAT2 GetEfficiencyPos(
				const float posInCol
			) noexcept
			{
				return { -0.175f, posInCol };
			}

			XMFLOAT2 GetLangPos(
				const uint32_t posInRow,
				const size_t numberOfLanguageButtos
			) noexcept
			{
				const float
					ONE_BUTTON = ICON_RADIUS * 2.0f,
					WIDTH =
					ONE_BUTTON * numberOfLanguageButtos
					+ DISTANCE * (numberOfLanguageButtos - 1.0f);
				return { WIDTH * .5f - (ONE_BUTTON + DISTANCE) * posInRow, g_vertical + g_pos * 3 };
			}

			constexpr XMFLOAT2 GetCheckboxPos(const float posInCol) noexcept
			{
				return { SIDE - DISTANCE, posInCol };
			}
		}

#pragma region DialogSettings
		DialogSettings::DialogSettings(
			uint32_t& langID
		) :
			DialogFullScreenGeneric{
			"settings"_pageNo,
			CONFIG_TITLE
		},
			m_graphicsRadio
		{
			*this
		},
			m_langRadio
		{
			*this,
			langID
		},
			m_ifVSyncButton
		{
			g_vertical,
			g_applied.m_vSync
		},
			m_ifPrivPrintButton
		{
			g_vertical + g_pos,
			g_applied.m_printer
		},
			m_ifPrivExitButton
		{
			g_vertical + g_pos * 2,
			g_applied.m_exit
		}
		{
			AddRenderedObjects({
				&m_exitButton,
				&m_okButton,
				&m_aboutButton,
				&m_descButton,
				&m_openSourceButton });

			LabelCheckbox(m_ifVSyncButton, { L"V-sync"_us });
			LabelCheckbox(m_ifPrivPrintButton, IF_PRINT_BUTTON);
			LabelCheckbox(m_ifPrivExitButton, IF_EXIT_BUTTON);
		}
#pragma endregion

#pragma region DialogSettings::*
		DialogSettings::ButtonOneToolbar::ButtonOneToolbar(
			const XMFLOAT2& positionOfCenter,
			const IconicStyle& style
		) :
			ButtonOneToolbarGeneric{
				"settings"_pageNo,
				positionOfCenter,
				style
		}
		{}

		DialogSettings::ButtonExit::ButtonExit() :
			ButtonOneToolbar{ {
					-SIDE,
					POS_Y
				},
					STYLE_CANCEL_BUTTON
		}
		{}

		void DialogSettings::ButtonExit::OnClick()
		{
			g_singleton->m_config.RollbackChanges();
			Pages::SelectPage("main_menu"_pageNo);
		}

		DialogSettings::ButtonOK::ButtonOK() :
			ButtonOneToolbar{ {
					-SIDE + DISTANCE,
					POS_Y },
					STYLE_OK_BUTTON }
		{}

		DialogSettings::ButtonAbout::ButtonAbout() :
			ButtonOneToolbar{ {
					SIDE - DISTANCE,
					POS_Y },
					STYLE_DESC_BUTTON
		}
		{}

		void DialogSettings::ButtonAbout::OnClick()
		{
			SelectPage("about"_pageNo);
		}

		DialogSettings::ButtonDesc::ButtonDesc() :
			ButtonOneToolbar{ { SIDE, POS_Y }, STYLE_ABOUT_BUTTON }
		{}

		void DialogSettings::ButtonDesc::OnClick()
		{
			SelectPage("desc"_pageNo);
		}

		DialogSettings::ButtonOpenSource::ButtonOpenSource() :
			ButtonOneToolbar{ { SIDE - DISTANCE * 2, POS_Y }, STYLE_OPEN_SOURCE_BUTTON }
		{}

		void DialogSettings::ButtonOpenSource::OnClick()
		{
			SelectPage("opensource"_pageNo);
		}

		bool DialogSettings::ButtonOK::IsVisible() const noexcept
		{
			return g_singleton->m_config.IsChanged();
		}

		void DialogSettings::ButtonOK::OnClick() noexcept
		{
			g_singleton->m_config.ApplyChanges();
		}

		DialogSettings::CheckboxIfSome::CheckboxIfSome(
			const float posInCol,
			bool& isSelected
		) :
			Checkbox{
				"settings"_pageNo,
				GetCheckboxPos(posInCol),
				STYLE_CHECKBOX,
				isSelected }
		{}

		DialogSettings::RadioButtonManager3DMode::RadioButtonManager3DMode(
			DialogGeneric& parent
		) :
			ManagerRadioButton(
				parent,
				g_applied.m_quality),
			m_efficiencyRadio(
				"settings"_pageNo,
				0,
				GetEfficiencyPos(g_vertical),
				*this,
				STYLE_RADIO_BUTTON,
				EFFICIENCY),
			m_mediumRadio(
				"settings"_pageNo,
				1,
				GetEfficiencyPos(g_vertical + g_pos),
				*this,
				STYLE_RADIO_BUTTON,
				MEDIUM),
			m_qualityRadio(
				"settings"_pageNo,
				2,
				GetEfficiencyPos(g_vertical + g_pos * 2.0f),
				*this,
				STYLE_RADIO_BUTTON,
				QUALITY)
		{}

		DialogSettings::RadioButtonManagerLang::RadioButtonManagerLang(
			DialogSettings& parent,
			uint32_t& currID
		) :
			ManagerRadioButton{
			parent,
			m_locale
		},
			m_flags{
			RadioButtonFlag{
				currID,
				*this,
				"flag-pl"_shaderResource,
				"pl-PL"
		},
			{
				currID,
				*this,
				"flag-ru"_shaderResource,
				"ru-RU"
		},
			{
				currID,
				*this,
				"flag-uk"_shaderResource,
				"en-GB"
		},
			{
				currID,
				*this,
				"flag-us"_shaderResource,
				DEFAULT_LANGUAGE
		}
		}
		{}

		DialogSettings::RadioButtonManagerLang::RadioButtonFlag::RadioButtonFlag(
			uint32_t& currID,
			RadioButtonManagerLang& manager,
			const string& flag,
			const string& localeName
		) :
			RadioButton(
				"settings"_pageNo,
				currID,
				GetLangPos(currID, manager.m_flags.size()),
				manager,
				IconicStyle{ flag }),
			c_localeName(localeName)
		{
			if (I18N::GetLocaleName() == localeName)
				manager.SetSelected(this);
			currID++;
		}
		void DialogSettings::RadioButtonManagerLang::RadioButtonFlag::OnClick()
		{
			g_applied.m_locale = c_localeName;
			__super::OnClick();
		}
#pragma endregion

#pragma region DialogAboutGeneric
		DialogAboutGeneric::DialogAboutGeneric(
			const string& pageNo,
			const I18NString& title) :
			DialogFullScreenGeneric{
			pageNo,
			title
			},
			m_okButton{
				pageNo
			}
		{
			AddRenderedObject(
				&m_okButton
			);
		}

		DialogAboutGeneric::PosPlusColor::PosPlusColor(
			XMFLOAT2 pos,
			float foregroundColor,
			const float speed
		) :
			XMFLOAT2(pos),
			m_foregroundColor(foregroundColor),
			m_speed(speed)
		{}

		inline void DialogAboutGeneric::PosPlusColor::SubSpeed() noexcept
		{
			assert(m_speed != .0f);
			m_foregroundColor -= m_speed;
		}

		void DialogAboutGeneric::DrawDialogTexts(
			InitializerListStrings texts,
			const XMFLOAT2 pos,
			const wstring& parameters
		)
		{
			PosPlusColor addLinesColors{
				pos,
				.525f,
				.5f / texts.size() };
			for (const auto& row : texts)
				DrawDialogText(
					row,
					addLinesColors,
					parameters);
		}

		void DialogAboutGeneric::DrawDialogText(
			const I18NString& text,
			PosPlusColor& pos,
			const wstring& parameters)
		{
			constexpr XMFLOAT2 aboutFontSize = { 1.05f, 1.2f };
			auto& textInFont = GetFont(FONT_TIMES_NEW_ROMAN);
			textInFont.AddRenderedText(
				{
				FONT_STYLES::BOLD,
				aboutFontSize,
				{ pos.m_foregroundColor, .0f, pos.m_foregroundColor * .2f } },
				pos,
				text,
				parameters
				);
			pos.SubSpeed();
			pos.y -= textInFont.GetLinesHeight(
				aboutFontSize.y
			) * ranges::fold_left(
				text.GetVal(),
				2.0f,
				[](float sum, const auto& elem) noexcept
				{
					return sum + (elem == CHAR_LF ? 0.91f : 0.0f);
				}
			);
		}

		DialogAboutGeneric::ButtonOK::ButtonOK(
			const string& pageNo
		) :
			ButtonOneToolbarGeneric{
			pageNo,
				{ -SIDE, POS_Y },
				STYLE_OK_BUTTON }
		{}

		void DialogAboutGeneric::ButtonOK::OnClick()
		{
			SelectPage("settings"_pageNo);
		}
#pragma endregion

#pragma region DialogAbout
		DialogAbout::DialogAbout() :
			DialogAboutGeneric{
			"about"_pageNo,
			AUTHORS_TITLE
		}
		{
			constexpr float top = 0.55f;
			DrawDialogTexts({
					{
		L"Developer\nWłodzimierz O.Kubera"_us,
		L"Oprogramowanie\nWłodzimierz O. Kubera"_pl,
		L"Автор и программист\nWłodzimierz O. Kubera"_ru
	},
				{
		L"#emoji_u1f46c_1f3fb#Tested by children\nOla and Mateusz Kowalczyk\nBianka Rodowska\nOtylka Mirecka"_us,
		L"#emoji_u1f46c_1f3fb#Dzieci testujące\nOla i Mateusz Kowalczykowie\nBianka Rodowska\nOtylka Mirecka"_pl,
		L"#emoji_u1f46c_1f3fb#Бета - тестеры\nOla и Mateusz Kowalczykowie\nBianka Rodowska\nOtylka Mirecka"_ru
	},
				{
		L"Quality Assurance\nJerzy Ziembiński"_us,
		L"Test techniczny\nJerzy Ziembiński"_pl,
		L"Взрослый бета - тестер\nJerzy Ziembiński"_ru
	},
				{
		L"Pictures\nMalwina De Brade"_us,
		L"Rysunki\nMalwina De Brade"_pl,
		L"Художник\nMalwina De Brade"_ru
	},
				{
		L"Polish Lector\nBianka Rodowska"_gb,
		L"Polish Narration\nBianka Rodowska"_us,
		L"Lektorka polskiego\nBianka Rodowska"_pl,
		L"Диктор (польский язык)\nBianka Rodowska"_ru
	} }, {
					-.85f, top }
					);
			DrawDialogTexts(
				{ {
		L"Professional review\nElżbieta Jalowiecka\n(Karwowska)"_us,
		L"Wskazówki merytoryczne\nElżbieta Jałowiecka\n(Karwowska)"_pl,
		L"Помощь и консультирование\nElżbieta Jalowiecka\n(Karwowska)"_ru
	},
				{
		L"English Translation\nIzabela Błaszczyk"_us,
		L"Angielskie tłumaczenie\nIzabela Błaszczyk"_pl,
		L"Перевод на английский\nIzabela Błaszczyk"_ru
	},
				{
		L"Music, lector\nand Russian translation\nKirill Ponomarev"_gb,
		L"Music, narration\nand Russian translation\nKirill Ponomarev"_us,
		L"Muzyka, lektor\ni rosyjskie tłumaczenie\nKirill Ponomarev"_pl,
		L"Музыка, звук\nи перевод на русский\nKirill Ponomarev"_ru
	},
				{
		L"Legal support\nR.pr. Szymon Marzęcki"_us,
		L"Wsparcie prawne\nR.Pr. Szymon Marzęcki"_pl,
		L"Юридическое сопровождение\nR.pr. Szymon Marzęcki"_ru
	} }, {
					0.0f, top }
					);
		}
#pragma endregion

#pragma region DialogDesc
		DialogDesc::DialogDesc() :
			DialogAboutGeneric{
			"desc"_pageNo,
			DESC_TITLE }
		{
			DrawDialogTexts({
				g_fullAppName,
				AGE,
				APP_VER_LABEL,
				DEVELOPED_IN_PL,
				Globals::g_copyright,
				ALL_RIGHTS_RESERVED,
				{
				L"Since version 5.0.0 the application is\nOpen Source under the MIT license.\nIt is available on GitHub."_us,
				L"Od wersji 5.0.0 aplikacja jest dostępna na\nlicencji Open Source, zgodnie z licencją MIT.\nZnajduje się ona w serwisie GitHub"_pl,
				L"Начиная с версии 5.0.0,\nприложение являетсяоткрытым исходным кодом\nи распространяется под лицензией MIT.\nОно доступно на GitHub."_ru
			} },
				{ -0.65f, 0.45f },
				System::GetAppVersionAndArch());
			AddRenderedObject(
				&m_logo
			);
		}
#pragma endregion

#pragma region DialogDesc::*
		DialogDesc::LogoTexture::LogoTexture() :
			ShaderTextured{
				"logo"_pageNo,
				{ "logo"_shaderResource },
				T2_ART,
				InputLayout::MeshBuilderArtPane{ {
					-g_size,
					-g_size,
					g_size,
					g_size} }.Commit()
		}
		{}

		void DialogDesc::LogoTexture::OnNewMatrix(
			XMFLOAT4X4& matrixToAlter
		) const noexcept
		{
			const auto scale0 = static_cast<float>((sin(g_fTime * 1.75) * 0.5 + 0.5) * .25 + .75);
			const auto scale = scale0 * scale0;
			XMStoreFloat4x4(&matrixToAlter,
				XMMatrixMultiply(
					XMMatrixMultiply(
						XMMatrixMultiply(
							XMMatrixScaling(scale, scale, 1.0f),
							XMMatrixRotationZ(static_cast<float> (g_fTime * .25))),
						XMMatrixTranslation(0.0f, 0.2f, 0.0f)),
					XMLoadFloat4x4(&matrixToAlter)));
		}
#pragma endregion

#pragma region DialogOpenSource
		DialogOpenSource::DialogOpenSource() :
			DialogAboutGeneric{
			"opensource"_pageNo,
			OPEN_SOURCE_TITLE }
		{
			DrawDialogTexts({
				{ L"Software:"_us, L"Oprogramowanie"_pl, L"Программное обеспечение"_ru },
				{ L"Audacity, BMFont, curl, GIMP, Inkscape"_us },
				{ L"LZMA, meshoptimizer, noto-emoji"_us },
				{ L"DirectX-Graphics-Samples, DirectXTex"_us },
				{ L"Art:"_us },
				{ L"„Wood” & „Marble” Textures by Vladimir Radetzki"_us },
				{ L"AI: "_us, L"SI: "_pl, L"ИИ: "_ru },
				{ L"ChatGPT, Claude, DeepSeek, GitHub Copilot"_us }
				}, {
					-.762f, .5f });
		}
#pragma endregion
	}
}