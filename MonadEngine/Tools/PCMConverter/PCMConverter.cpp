// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "stdafx.h"
#include <fstream>
#include <print>
#include <Shlwapi.h>
#include "Globals/__MonadGlobals.h"
#include "Audio/MediaStreamer.h"

using namespace Monad;
using Exceptions::NotInitialized;

int _tmain(
	int argc,
	wchar_t* argv[]
)
{
	Monad::Globals::g_exceptionReportingMethod = Monad::Exceptions::EXCEPTION_PRESENTATION::REPORT_TO_CONSOLE;

	try
	{
		if (3 != argc)
		{
			std::println("A wrong number of parameters");
			MessageBeep(MB_ICONERROR);
			return 1;
		}
		std::fstream monadFile(argv[2], std::ios::out | std::ios::binary);
		monadFile.exceptions(std::fstream::failbit | std::fstream::badbit);
		Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
		THROW_EXC_IFFAILED_RO(initialize);
		Audio::MFManager manager;

		std::fstream reader(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
		reader.exceptions(std::fstream::failbit | std::fstream::badbit);
		const auto fSize = reader.tellg();
		reader.seekg(0, std::ios::beg);
		Kernel::VectorBytes buf(static_cast<size_t>(fSize));
		reader.read(reinterpret_cast<char*>(buf.data()), fSize);
		Audio::MediaStreamer strm(Kernel::SpanConstBytes(buf.data(), fSize).GetComIStreamTemp().Get());
		monadFile.write(reinterpret_cast<const char*>(&strm.GetOutputWaveFormatEx()), sizeof WAVEFORMATEX);
		const Kernel::VectorBytes bufOutput(strm.ReadAll());
		monadFile.write(reinterpret_cast<const char*>(&bufOutput[0]), bufOutput.size());
		return ERROR_SUCCESS;
	}
	MONAD_CATCH;
	return EXIT_FAILURE;
}