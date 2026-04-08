// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// ############################################################################
// ### Copyright © 2016-2025 by W³odzimierz O. Kubera. All rights reserved. ###
// ############################################################################

#pragma once
#if defined(MONAD_ENGINE)
#	include "define.h"
#endif
#if defined(MONAD_TEST)
#	include <string>
#	include "Kernel/MonadPatterns.h"
#	include "Tools/MonadConvert.h"

#	define CONCAT1(X,Y) X##Y  // helper macro
#	define CONCAT(X,Y) CONCAT1(X,Y)
#	define MND_TRACE_CLASS_NAME Monad::Exceptions::Trace CONCAT(MonadTrc,__COUNTER__)

namespace Monad::Exceptions
{
	template<typename TplType>
	std::wstring StringFromType(const TplType pValue)
	{
		Tools::IntoStream oss;

		oss << pValue;
		if constexpr (std::is_integral<TplType>::value)
			oss << L'-' << Tools::StringFromAnyULongHex(pValue);

		return oss.str();
	}

	std::wstring TraceParam(const std::wstring& pParam, const std::wstring& pValue);
	template<typename TplCasterInLog>
	std::wstring TraceParam(const std::wstring& pParam, const TplCasterInLog pValue)
	{
		return TraceParam(pParam, StringFromType(pValue));
	}
	struct Trace final
	{
		explicit Trace(const std::wstring& pStage, Kernel::InitListString pParams, Kernel::InitListString pSeverities);
		~Trace();

	private:
		void WriteToConsole(const std::wstring& pLog) const;

		const double m_startTime;
		const std::wstring c_stage;
		Kernel::VectorString m_params;
		bool m_severity;
		const bool c_exception;
	};
	extern Kernel::VectorString gSeverities;
	void WriteToErrorLog(const wchar_t* pLog);
	void WriteToErrorLogRaw(const wchar_t* pLog);
}
#	define MND_PARAM_THIS Monad::Exceptions::TraceParam(L"This Ptr", this)
#	define MND_PARAM_PTR(PTR) Monad::Exceptions::TraceParam(L"Parameter Ptr", PTR)
#	define MND_TRACE_ME(Parameters,Severities) MND_TRACE_CLASS_NAME (TEXT(__FUNCTION__), Parameters, Severities)
#	define MND_TRACE_ME_DEFAULT MND_TRACE_CLASS_NAME (TEXT(__FUNCTION__), {}, {})
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