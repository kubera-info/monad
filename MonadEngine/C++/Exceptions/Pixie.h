// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// ############################################################################
// ### Copyright © 2016-2024 by W³odzimierz O. Kubera. All rights reserved. ###
// ############################################################################

#pragma once

#include "Exceptions/MonadExceptions.h"
#include "Tools/MonadRandom.h"

namespace Monad::Exceptions
{
	/// <summary>
	/// Throws on random value, a special type of trace, can throw exception where the function is called
	/// </summary>
	/// <param name="pRandomRange">N means propability 1/N, where n > 0</param>
	inline void ThrowingPixieOnSizeT(const size_t pRandomRange)
	{
		struct pixie : public std::exception
		{
			pixie() noexcept : exception("error: Pixie has been thrown") {}
		};
		if (Tools::g_random.GetRandomInt(pRandomRange - 1) == 0)
			throw pixie();
	}
	/// <summary>
	/// Sample code: Exception::ThrowMonadPixieOnSizeT<NotInit>(1, E_FAIL, L"Test");
	/// </summary>
	/// <typeparam name="TplException">A monad exception, that may be thrown</typeparam>
	/// <param name="pRandomRange">N means propability 1/N, where n > 0</param>
	/// <param name="pHR">HRESULT HR</param>
	/// <param name="pStage">Stage (a place in the app, sometimes with a reason in this name)</param>
	template<typename TplException>
		requires std::is_base_of_v<ExceptionGeneric, TplException>
	inline void ThrowMonadPixieOnSizeT(const size_t pRandomRange, HRESULT pHR, LPCWSTR pStage)
	{
		if (Tools::g_random.GetRandomInt(pRandomRange - 1) == 0)
			throw TplException{ pHR, pStage };
	}
}