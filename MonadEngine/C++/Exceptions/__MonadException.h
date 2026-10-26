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
		L"Plik jest uszkodzony"_pl_PL,
		L"Файл поврежден"_ru_RU,
		L"The file is damaged"_en_US
	},
		THREE_D_FAILED
	{
		L"Wyjątek podsystemu 3D"_pl_PL,
		L"Отключение 3D подсистемы"_ru_RU,
		L"3D Subsystem Exception"_en_US
	},
		CLASS_ALREADY_EXISTS
	{
		L"Wyjątek podwójnej wartości indeksu"_pl_PL,
		L"Невозможно сохранить в раздел с уникальным индексом"_ru_RU,
		L"Duplicate value on index exception"_en_US
	},
		AUDIO_FAILED
	{
		L"Karta dźwiękowa lub jej sterowniki nie spełniają minimalnych wymagań"_pl_PL,
		L"Звуковая карта или драйвер не отвечают минимальным требованиям"_ru_RU,
		L"Sound card or its driver do not meet minimal software requirements"_en_GB,
		L"The sound card or its driver do not meet minimal software requirements"_en_US
	},
		CAPS_FAILED
	{
		L"Karta graficzna lub jej sterowniki nie spełniają minimalnych wymagań"_pl_PL,
		L"Видеокарта или драйвер не отвечают минимальным требованиям"_ru_RU,
		L"Graphics adapter or its driver do not meet minimal software requirements"_en_GB,
		L"The graphics adapter or its driver do not meet minimal software requirements"_en_US
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
		L"Nie znaleziono pliku"_pl_PL,
		L"Файл не найден"_ru_RU,
		L"File not found"_en_US },
		NO_DATA_FOUND
		{
		L"Nie znaleziono obiektu"_pl_PL,
		L"Объект не найден"_ru_RU,
		L"Object not found"_en_US },
		COM_INITIALIZATION_FAILED
		{
		L"Inicjalizacja Windows Runtime-u zakończona błędem"_pl_PL,
		L"Ошибка инициализации Windows Runtime"_ru_RU,
		L"The Windows Runtime initialization failed"_en_US },
		NULL_VALUE
		{
		L"Błąd wystąpienia pustej wartości"_pl_PL,
		L"Обращение к пустому значению"_ru_RU,
		L"An empty value error"_en_US },
		INVALID_CALL
		{
		L"Nieprawidłowe wykonanie"_pl_PL,
		L"Обращение к ошибочному параметру"_ru_RU,
		L"An invalid call error"_en_US },
		EXCEPTION_TITLE
		{
		L"Wystąpił nieprzewidziany błąd"_pl_PL,
		L"Непредвиденная ошибка"_ru_RU,
		L"An unexpected error has occurred"_en_US },
		IO_ERROR
		{
		L"Błąd dostępu do danych dyskowych"_pl_PL,
		L"Отсутствие доступа к данным диска"_ru_RU,
		L"Failure to access the data disk"_en_GB,
		L"Failure accessing the data disk"_en_US },
		OUT_OF_MEMORY
		{
		L"Brak wolnej pamięci."_pl_PL,
		L"Недостаточно памяти."_ru_RU,
		L"Not enough memory."_en_US },
		NOT_INITIALIZED
		{
		L"Niezainicjalizowany obiekt"_pl_PL,
		L"Найден не инициализированный объект"_ru_RU,
		L"An uninitialized object found"_en_US },
		OUT_OF_RANGE
		{
		L"Przekroczenie zakresu"_pl_PL,
		L"Вне зоны доступа"_ru_RU,
		L"Out of range"_en_US },
		INVALID_XML
		{
		L"Plik bazy danych jest uszkodzony"_pl_PL,
		L"Неверно сформированный файл базы данных"_ru_RU,
		L"A database's file is malformed"_en_US },
		INVALID_CLASS
		{
		L"Błąd konwersji do nowego typu, błędna akcja"_pl_PL,
		L"Невозможно конвертировать объект в новый тип, ошибочная операция"_ru_RU,
		L"Can not convert an object to a new type, the incorrect action"_en_US },
		INSTELLER
		{
		L"Instalator."_pl_PL,
		L"Установщик."_ru_RU,
		L"The Installer."_en_US },
		HARDWARE_EXCEPTION
		{
		L"Wyjątek sprzętowy został rzucony"_pl_PL,
		L"Ошибка несовместимости оборудования"_ru_RU,
		L"A hardware exception has been thrown"_en_US };
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