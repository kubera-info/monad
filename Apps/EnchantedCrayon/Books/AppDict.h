// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "I18N/I18NDict.h"

namespace Monad::I18N
{
	const I18NString
		/*g_ifSettingsButton(
			L"Ustawienia dostępne",
			L"Настройки",
			L"Available Settings"),*/
		COLOURING_BOOK_SHORT
	{
		L"#emoji_u1f600#Colouring Page"_en_GB,
		L"#emoji_u1f600#Coloring Page"_en_US,
		L"#emoji_u1f600#Kolorowanka"_pl_PL,
		L"#emoji_u1f600#Раскраска"_ru_RU
	},
		COLORS
	{
		L"#emoji_u1f3a8#Colours"_en_GB,
		L"#emoji_u1f3a8#Colors"_en_US,
		L"#emoji_u1f3a8#Kolory"_pl_PL,
		L"#emoji_u1f3a8#Краски"_ru_RU
	},
		/*g_helpTitle(
			L"Bryk",
			L"Помощь",
			L"Help"),*/
		AUTHORS2
	{
		L"Professional review\nElżbieta Jalowiecka\n(Karwowska)"_en_US,
		L"Wskazówki merytoryczne\nElżbieta Jałowiecka\n(Karwowska)"_pl_PL,
		L"Помощь и консультирование\nElżbieta Jalowiecka\n(Karwowska)"_ru_RU
	};
}