// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "I18N/__MonadI18N.h"

namespace Monad::I18N
{
	const I18NString
		AGE{
			L"Aplikacja dla dzieci w wieku od lat trzech."_pl,
			L"Программа для детей от 3 - х лет"_ru,
			L"Application for children age 3 and above"_gb,
			L"Application for children aged 3 and above"_us },
			/*g_configSite(
				L"Strona WWW",
				L"Сайт WWW",
				L"Website"),
			g_voice(
				L"Dźwięk",
				L"Звук",
				L"Sound"),*/
		IF_PRINT_BUTTON{
			L"Drukowanie dostępne"_pl,
			L"Доступная печать"_ru,
			L"Available printing"_us },
			IF_EXIT_BUTTON{
				L"Wyjście z programu"_pl,
				L"Выход"_ru,
				L"Leave the application"_us },
				/*g_ifSettingsButton(
					L"Ustawienia dostępne",
					L"Настройки",
					L"Available Settings"),*/
		COLOURING_BOOK_SHORT{
			L"#emoji_u1f600#Kolorowanka"_pl,
			L"#emoji_u1f600#Раскраска"_ru,
			L"#emoji_u1f600#Colouring Page"_gb,
			L"#emoji_u1f600#Coloring Page"_us },
			COLORS{
				L"#emoji_u1f3a8#Kolory"_pl,
				L"#emoji_u1f3a8#Краски"_ru,
				L"#emoji_u1f3a8#Colours"_gb,
				L"#emoji_u1f3a8#Colors"_us },
				/*g_helpTitle(
					L"Bryk",
					L"Помощь",
					L"Help"),*/
		PRINTING{
			L"#emoji_u1f5a8#Drukowanie"_pl,
			L"#emoji_u1f5a8#Печать"_ru,
			L"#emoji_u1f5a8#Printing"_us },
			AUTHORS0{
				L"#emoji_u1f46c_1f3fb#Dzieci testujące\nOla i Mateusz Kowalczykowie\nBianka Rodowska\nOtylka Mirecka\nSara i Krystian Komorowscy"_pl,
				L"#emoji_u1f46c_1f3fb#Бета - тестеры\nOla и Mateusz Kowalczykowie\nBianka Rodowska\nOtylka Mirecka\nSara и Krystian Komorowscy"_ru,
				L"#emoji_u1f46c_1f3fb#Tested by children\nOla and Mateusz Kowalczyk\nBianka Rodowska\nOtylka Mirecka\nSara and Krystian Komorowski"_us },
				AUTHORS1{
					L"Rysunki\nMalwina De Brade"_pl,
					L"Художник\nMalwina De Brade"_ru,
					L"Pictures\nMalwina De Brade"_us },
					AUTHORS2{
						L"Wskazówki merytoryczne\nElżbieta Jałowiecka\n(Karwowska)"_pl,
						L"Помощь и консультирование\nElżbieta Jalowiecka\n(Karwowska)"_ru,
						L"Professional review\nElżbieta Jalowiecka\n(Karwowska)"_us },
						AUTHORS3{
							L"Angielskie tłumaczenie\nIzabela Błaszczyk"_pl,
							L"Перевод на английский\nIzabela Błaszczyk"_ru,
							L"English Translation\nIzabela Błaszczyk"_us },
							AUTHORS4{
								L"Muzyka, lektor\ni rosyjskie tłumaczenie\nKirill Ponomarev"_pl,
								L"Музыка, звук\nи перевод на русский\nKirill Ponomarev"_ru,
								L"Music, lector\nand Russian translation\nKirill Ponomarev"_gb,
								L"Music, narration\nand Russian translation\nKirill Ponomarev"_us },
								AUTHORS6{
									L"Wsparcie prawne\nR.Pr. Szymon Marzęcki"_pl,
									L"Юридическое сопровождение\nR.pr. Szymon Marzęcki"_ru,
									L"Legal support\nR.pr. Szymon Marzęcki"_us },
									AUTHORS7{
										L"Test techniczny\nJerzy Ziembiński"_pl,
										L"Взрослый бета - тестер\nJerzy Ziembiński"_ru,
										L"Quality Assurance\nJerzy Ziembiński"_us },
										IF_EXIT{
											L"#emoji_u1f6aa#Wyjście z programu"_pl,
											L"#emoji_u1f6aa#Выход"_ru,
											L"#emoji_u1f6aa#Leave the application"_us },
											IF_EXIT_QUESTION{
												L"#emoji_u1f6aa#Czy jesteś pewna(y)?"_pl,
												L"#emoji_u1f6aa#Вы уверены?"_ru,
												L"#emoji_u1f6aa#Are you sure?"_us };
}