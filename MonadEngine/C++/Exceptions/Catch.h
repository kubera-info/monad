// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#if defined _DEBUG
// Monad
#	include "System/Console.h"
#	include "Tools/Convert.h"

#	define MONAD_LINE Monad::System::MndOutputDebugString(L" Exception " + Monad::Tools::UTF8ToUnicode(__FILE__) + L" " + std::to_wstring(__LINE__));
#	define MONAD_CATCH \
catch (const std::exception& stdException)\
{\
	Monad::Exceptions::ExceptionGeneric exc(stdException);\
	MONAD_LINE\
}\
catch ([[maybe_unused]] const Monad::Exceptions::ExceptionGeneric&)\
{\
	MONAD_LINE\
}
#else
#	define MONAD_CATCH \
catch (const std::exception& stdException)\
{\
	const Monad::Exceptions::ExceptionGeneric exc(stdException);\
}\
catch ([[maybe_unused]] const Monad::Exceptions::ExceptionGeneric&)\
{\
}
#endif