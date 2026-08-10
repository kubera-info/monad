// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "stdafx.h"
#include <iostream>
#include <map>
#include <print>
#include <set>
#include <string>
#include "Files/__MonadFile.h"
#include "Files/Handle.h"
#include "Kernel/Templates.h"
#include "Tools/Convert.h"

namespace fs = std::filesystem;
using namespace Monad::Kernel;
using namespace std::string_literals;

Monad::Kernel::VectorPath g_filesMeshes, g_filesWaves, g_filesMP3s, g_textures;

void IterateFiles(
	const fs::path& folder
)
{
	struct FileHandleTime : Monad::Files::FileHandle
	{
		using FileHandle::FileHandle;

		void OnFindFile(
			const std::filesystem::path& selectedFolder
		) final
		{
			auto file = selectedFolder / FindFileData().cFileName;
			if (file.extension() == L".x"s)
				g_filesMeshes.emplace_back(file);
			else if (file.extension() == L".wav"s)
				g_filesWaves.emplace_back(file);
			else if (file.extension() == L".mp3"s)
				g_filesMP3s.emplace_back(file);
			else if (file.extension() == L".png"s || file.extension() == L".jpg"s)
				g_textures.emplace_back(file);
		}

		void OnFindDirectory(
			const std::filesystem::path& selectedFolder
		) final
		{
			FileHandleTime{ { selectedFolder / FindFileData().cFileName } }.Run();
		}

	} handle{ { folder } };
	handle.Run();
}

constexpr std::wstring_view replaceRes = L"Res\\", replaceID = L"\\", replaceMinus = L"-", replaceDots = L"..", replaceAp = L"'";
constexpr std::wstring_view replaceMinus2 = L"_";

std::map<std::wstring, std::set<std::wstring>> g_Paths;

void Scis()
{
	wprintf(L"\n\n 8X========\n\n");
	wprintf(L"\n\n 8X========\n\n");
	wprintf(L"\n\n 8X========\n\n");
}

int wmain(
	int argc,
	wchar_t* wargv[]
)
{
	std::wcout << L"Processing Folders and Files\n";
	if (2 != argc)
	{
		std::wcerr << L"Error: Argument should be: a path to the Monad(a) Resources\n";
		MessageBeep(MB_ICONERROR);
		return EXIT_FAILURE;
	}
	const fs::path startingFolder(wargv[1]);
	if (!(fs::is_directory(wargv[1]) && startingFolder.generic_string().back() != L'\\'))
	{
		std::wcerr << L"Error: Argument should be a correct path to a folder and without trailing \\, is: "s << wargv[1] << L'\n';
		MessageBeep(MB_ICONERROR);
		return EXIT_FAILURE;
	}
	IterateFiles(startingFolder);
	Scis();

#pragma region g_filesWaves
	for (const auto& currentFile : g_filesWaves)
		std::println("{}", Monad::Tools::UnicodeToUTF8(
			L"Release\\"s
			+ currentFile.stem().wstring()
			+ L".pcm: $(MND_VOICE)\\"s
			+ currentFile.stem().wstring() + L".wav"s
			+ L"\n\t$(MND_WAVCONV) $? $@"s));
	std::println();
	std::print("waves_obj:");
	for (const auto& currentFile : g_filesWaves)
		std::print(" {}", Monad::Tools::UnicodeToUTF8(
			L"Release\\"s + currentFile.stem().wstring() + L".pcm"s));
	std::println();
	std::println();
#pragma endregion

#pragma region Encrypted g_filesWaves
	for (const auto& currentFile : g_filesWaves)
		std::println("{}", Monad::Tools::UnicodeToUTF8(L"$(MND_DEST)\\Boot\\"s + currentFile.stem().wstring() + L".monad-snd: Release\\" + currentFile.stem().wstring() + L".pcm"
			L"\n\t$(MND_ENCRYPT) $? $@"s));
	std::println();
	std::print("waves:");
	for (const auto& currentFile : g_filesWaves)
		std::print(" {}", Monad::Tools::UnicodeToUTF8(
			L"$(MND_DEST)\\Boot\\"s + currentFile.stem().wstring() + L".monad-snd"s));
	std::println();
	std::println();
#pragma endregion

#pragma region Encrypted g_filesMP3s
	for (const auto& currentFile : g_filesMP3s)
		std::println("{}", Monad::Tools::UnicodeToUTF8(L"$(MND_DEST)\\Boot\\" + currentFile.stem().wstring() + L".monad-stm: $(MND_VOICE)\\" + currentFile.stem().wstring() + L".mp3"
			L"\n\t$(MND_ENCRYPT) $? $@"s));
	std::println();
	std::println();
	std::print("streams:");
	for (const auto& currentFile : g_filesMP3s)
		std::print(" {}", Monad::Tools::UnicodeToUTF8(
			L"$(MND_DEST)\\Boot\\"s + currentFile.stem().wstring() + L".monad-stm"s));
	std::println();
#pragma endregion

#pragma region g_filesMeshes
	for (const auto& currentFile : g_filesMeshes)
		std::println("{}", Monad::Tools::UnicodeToUTF8(
			L"$(MND_MESH)\\"s
			+ currentFile.filename().wstring()
			+ L".m12: $(MND_MESH)\\"s
			+ currentFile.filename().wstring()
			+ L"\n\t$(MND_MESHCONV) $? $@"s));
	std::println();

	std::print("meshes_obj:");
	for (const auto& currentFile : g_filesMeshes)
		std::print(" {}", Monad::Tools::UnicodeToUTF8(
			L"$(MND_MESH)\\"s + currentFile.filename().wstring()
			+ L".m12"s));
	std::println();
	std::println();
#pragma endregion 

#pragma region Encrypted g_filesMeshes
	for (const auto& currentFile : g_filesMeshes)
		std::println("{}", Monad::Tools::UnicodeToUTF8(L"$(MND_DEST)\\Boot2\\"s + currentFile.stem().wstring() + L".monad-msh: $(MND_MESH)\\" + currentFile.stem().wstring() + L".x.m12"
			L"\n\t$(MND_ENCRYPT) $? $@"s));
	std::println();
	std::print("meshes:");
	for (const auto& currentFile : g_filesMeshes)
		std::print(" {}", Monad::Tools::UnicodeToUTF8(
			L"$(MND_DEST)\\Boot2\\"s + currentFile.stem().wstring() + L".monad-msh"s));
	std::println();
	std::println();
#pragma endregion

#pragma region Encrypted g_textures
	std::println("{}", "textures_obj:"s);
	std::println("{}", "\t$(MND_TEXCONV) BC7_UNORM 4 Release ..\\Textures"s);
	std::println();
	for (const auto& currentFile : g_textures)
		std::println("{}", Monad::Tools::UnicodeToUTF8(L"$(MND_DEST)\\Boot2\\"s + currentFile.stem().wstring() + L".monad-tex: Release\\" + currentFile.stem().wstring() + L".dds"
			L"\n\t$(MND_ENCRYPT) $? $@"s));
	std::println();

	std::print("textures:");
	for (const auto& currentFile : g_textures)
		std::print(" {}", Monad::Tools::UnicodeToUTF8(
			L"$(MND_DEST)\\Boot2\\"s + currentFile.stem().wstring() + L".monad-tex"s));
	std::println();
	std::println();
#pragma endregion

#pragma region Clean
	std::println("{}", "clean:"
		"\n\t@del $(MND_MESH)\\*.m12 >nul 2>&1"
		"\n\t@del $(MND_DEST)\\*.$$$$$$ >nul 2>&1"
		"\n\t@del $(MND_VOICE)\\*.pcm >nul 2>&1"
		"\n\t@del /s $(MND_DEST)\\*.bak >nul 2>&1"
		"\n\t@del /s $(MND_DEST)\\*.monad-lob >nul 2>&1"
		"\n\t@del /s $(MND_DEST)\\*.monad-snd >nul 2>&1"
		"\n\t@del /s $(MND_DEST)\\*.monad-stm >nul 2>&1"
		"\n\t@del /s $(MND_DEST)\\*.monad-tex >nul 2>&1"
		"\n\t@del /s $(MND_DEST)\\*.monad-msh >nul 2>&1");
#pragma endregion
	Scis();
	return ERROR_SUCCESS;
}