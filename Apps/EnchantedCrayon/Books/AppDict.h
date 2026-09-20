// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "I18N/__MonadI18N.h"

namespace Monad::I18N
{
	const I18NString
		/*g_ifSettingsButton(
			L"Ustawienia dostępne",
			L"Настройки",
			L"Available Settings"),*/
		COLOURING_BOOK_SHORT
	{
		L"#emoji_u1f600#Colouring Page"_gb,
		L"#emoji_u1f600#Coloring Page"_us,
		L"#emoji_u1f600#Kolorowanka"_pl,
		L"#emoji_u1f600#Раскраска"_ru
	},
		COLORS
	{
		L"#emoji_u1f3a8#Colours"_gb,
		L"#emoji_u1f3a8#Colors"_us,
		L"#emoji_u1f3a8#Kolory"_pl,
		L"#emoji_u1f3a8#Краски"_ru
	},
		/*g_helpTitle(
			L"Bryk",
			L"Помощь",
			L"Help"),*/
		AUTHORS2
	{
		L"Professional review\nElżbieta Jalowiecka\n(Karwowska)"_us,
		L"Wskazówki merytoryczne\nElżbieta Jałowiecka\n(Karwowska)"_pl,
		L"Помощь и консультирование\nElżbieta Jalowiecka\n(Karwowska)"_ru
	};
}