// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once
//#define MONAD_TEST
#if defined(MONAD_TEST)
#	include <string>
#	include <vector>
#	include <print>
#	include "Tools/Convert.h"

#	define CONCAT1(X,Y) X##Y  // helper macro
#	define CONCAT(X,Y) CONCAT1(X,Y)
#	define MND_TRACE_CLASS_NAME Monad::Exceptions::Trace CONCAT(MonadTrc,__COUNTER__)

namespace Monad::Exceptions
{
	template<typename T>
	std::wstring StringFromType(const T value)
	{
		Tools::OutputStringStream oss;

		oss << value;
		if constexpr (std::is_integral<T>::value)
			oss << L'-' << Tools::StringFromAnyULongHex(value);

		return oss.str();
	}

	std::wstring TraceParam(const std::wstring& param, const std::wstring& value);
	template<typename TCast>
	std::wstring TraceParam(const std::wstring& param, const TCast value)
	{
		return TraceParam(param, StringFromType(value));
	}
	struct Trace final
	{
		using InitializerListString = std::initializer_list<std::wstring>;
		using VectorString = std::vector<std::wstring>;
		explicit Trace(
			const std::wstring& stage,
			InitializerListString params,
			InitializerListString severities
		);
		~Trace();

	private:
		void WriteToConsole(const std::wstring& log) const;

		const double m_startTime;
		const std::wstring c_stage;
		VectorString m_params;
		bool m_severity;
		const bool c_exception;
	};
	extern Trace::VectorString g_severities;
	void WriteToErrorLog(const std::wstring& pLog);
	void WriteToErrorLogRaw(const wchar_t* pLog);
}
#	define MND_PARAM_THIS Monad::Exceptions::TraceParam(L"This Ptr", this)
#	define MND_PARAM_PTR(PTR) Monad::Exceptions::TraceParam(L"Parameter Ptr", PTR)
#	define MND_TRACE_ME(Parameters,Severities) MND_TRACE_CLASS_NAME (__FUNCTIONW__, Parameters, Severities)
#	define MND_TRACE_ME_DEFAULT MND_TRACE_CLASS_NAME (__FUNCTIONW__, {}, {})
#	define MND_TRACE_ME_NAME(Name,Parameters,Severities) MND_TRACE_CLASS_NAME (Name, Parameters, Severities)
#	define MND_LOG(Str) Monad::Exceptions::WriteToErrorLog(Str)
#else
#	define MND_PARAM_THIS
#	define MND_PARAM_PTR(PTR)
#	define MND_TRACE_ME(Parameters,Severities)
#	define MND_TRACE_ME_DEFAULT
#	define MND_TRACE_ME_NAME(Name,Parameters,Severities)
#	define MND_LOG(Str)
#endif