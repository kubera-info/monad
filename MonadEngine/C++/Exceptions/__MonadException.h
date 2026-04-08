// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// Monad
#include "I18N/I18N.h"

namespace Monad::I18N::Errors
{
	/*constexpr I18NString g_StrUpdate(
	L"\nProszę zaktualizować sterownik urządzenia",
	L"\nОбновите свой драйвер устройства",
	L"\nPlease update your device driver"),
	g_StrReinstall(
		L"\nProszę ponownie zainstalować aplikację",
		L"\nПожалуйста, переустановите программу",
		L"\nPlease re-install this program"),
	g_StrNoCaps(
	L"Karta graficzna lub jej sterowniki nie spełniają minimalnych wymagań",
	L"Видеокарта или драйвер не отвечает минимальным требованиям",
	L"Graphics card or its driver do not meet minimal requirements" );
	constexpr I18NString g_StrNoCaps2( I18N::gErrStrNoCaps + I18N::gErrStrUpdate ); */
	inline const I18NString
		FILE_DAMAGED
	{
		L"Plik jest uszkodzony"_pl,
		L"Файл поврежден"_ru,
		L"The file is damaged"_us
	},
		THREE_D_FAILED
	{
		L"Wyjątek podsystemu 3D"_pl,
		L"Отключение 3D подсистемы"_ru,
		L"3D Subsystem Exception"_us
	},
		CLASS_ALREADY_EXISTS
	{
		L"Wyjątek podwójnej wartości indeksu"_pl,
		L"Невозможно сохранить в раздел с уникальным индексом"_ru,
		L"Duplicate value on index exception"_us
	},
		AUDIO_FAILED
	{
		L"Karta dźwiękowa lub jej sterowniki nie spełniają minimalnych wymagań"_pl,
		L"Звуковая карта или драйвер не отвечают минимальным требованиям"_ru,
		L"Sound card or its driver do not meet minimal software requirements"_gb,
		L"The sound card or its driver do not meet minimal software requirements"_us
	},
		CAPS_FAILED
	{
		L"Karta graficzna lub jej sterowniki nie spełniają minimalnych wymagań"_pl,
		L"Видеокарта или драйвер не отвечают минимальным требованиям"_ru,
		L"Graphics adapter or its driver do not meet minimal software requirements"_gb,
		L"The graphics adapter or its driver do not meet minimal software requirements"_us
	},
		/*g_noSSE41(
		L"Mikroprocesor nie spełnia minimalnych wymagań (SSE4.1)",
		L"Микропроцессор не отвечает минимальным требованиям (SSE4.1)",
		L"Microprocessor does not meet minimal software requirements (SSE4.1)",
		L"The microprocessor does not meet minimal software requirements (AVX)"),
		g_StrNoSound2(AUDIO_FAILED + g_StrUpdate),
		g_NoDirectX(
		L"Błąd DirectX",
		L"Ошибка DirectX",
		L"DirectX Error");*/
		FILE_NOT_FOUND
	{
		L"Nie znaleziono pliku"_pl,
		L"Файл не найден"_ru,
		L"File not found"_us },
		NO_DATA_FOUND
		{
		L"Nie znaleziono obiektu"_pl,
		L"Объект не найден"_ru,
		L"Object not found"_us },
		COM_INITIALIZATION_FAILED
		{
		L"Inicjalizacja Windows Runtime-u zakończona błędem"_pl,
		L"Ошибка инициализации Windows Runtime"_ru,
		L"The Windows Runtime initialization failed"_us },
		NULL_VALUE
		{
		L"Błąd wystąpienia pustej wartości"_pl,
		L"Обращение к пустому значению"_ru,
		L"An empty value error"_us },
		INVALID_CALL
		{
		L"Nieprawidłowe wykonanie"_pl,
		L"Обращение к ошибочному параметру"_ru,
		L"An invalid call error"_us },
		EXCEPTION_TITLE
		{
		L"Wystąpił nieprzewidziany błąd"_pl,
		L"Непредвиденная ошибка"_ru,
		L"An unexpected error has occurred"_us },
		IO_ERROR
		{
		L"Błąd dostępu do danych dyskowych"_pl,
		L"Отсутствие доступа к данным диска"_ru,
		L"Failure to access the data disk"_gb,
		L"Failure accessing the data disk"_us },
		OUT_OF_MEMORY
		{
		L"Brak wolnej pamięci."_pl,
		L"Недостаточно памяти."_ru,
		L"Not enough memory."_us },
		NOT_INITIALIZED
		{
		L"Niezainicjalizowany obiekt"_pl,
		L"Найден не инициализированный объект"_ru,
		L"An uninitialized object found"_us },
		OUT_OF_RANGE
		{
		L"Przekroczenie zakresu"_pl,
		L"Вне зоны доступа"_ru,
		L"Out of range"_us },
		INVALID_XML
		{
		L"Plik bazy danych jest uszkodzony"_pl,
		L"Неверно сформированный файл базы данных"_ru,
		L"A database's file is malformed"_us },
		INVALID_CLASS
		{
		L"Błąd konwersji do nowego typu, błędna akcja"_pl,
		L"Невозможно конвертировать объект в новый тип, ошибочная операция"_ru,
		L"Can not convert an object to a new type, the incorrect action"_us },
		INSTELLER
		{
		L"Instalator."_pl,
		L"Установщик."_ru,
		L"The Installer."_us },
		HARDWARE_EXCEPTION
	{
		L"Wyjątek sprzętowy został rzucony"_pl,
		L"Ошибка несовместимости оборудования"_ru,
		L"A hardware exception has been thrown"_us };
}
namespace Monad::Exceptions
{
	/// <summary>
	/// Specifies how an exception should be displayed or reported.
	/// </summary>
	enum class EXCEPTION_PRESENTATION : DWORD
	{
		REPORT_EXE,
		MESSAGE_BOX,
		REPORT_TO_CONSOLE
	};
}