// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "System/ResourceLoader.h"
// Monad
#include "Exceptions/Exceptions.h"
#include "Files/Op.h"
#include "Files/SaveUTF8.h"
#include "Files/Folder.h"
#include "Globals/__MonadGlobals.h"
#include "resource.h"
#include "GUI/__MonadGUI.h"
#include <fstream>

using namespace std::string_literals;
namespace fs = std::filesystem;

namespace Monad::System
{
	namespace
	{
		const std::wstring
			BOM = L"\uFEFF",
			LOGO = L"logo.ico";

#pragma pack(push, 1)
		// ICO file format structures
		struct ICONDIRENTRY {
			BYTE  bWidth;
			BYTE  bHeight;
			BYTE  bColorCount;
			BYTE  bReserved;
			WORD  wPlanes;
			WORD  wBitCount;
			DWORD dwBytesInRes;
			DWORD dwImageOffset;  // offset from start of file (differs from GRPICONDIRENTRY which has WORD nId)
		};

		struct ICONDIR {
			WORD         idReserved;
			WORD         idType;      // 1 = icon
			WORD         idCount;
			ICONDIRENTRY idEntries[1];
		};

		// PE resource group icon structures
		struct GRPICONDIRENTRY {
			BYTE  bWidth;
			BYTE  bHeight;
			BYTE  bColorCount;
			BYTE  bReserved;
			WORD  wPlanes;
			WORD  wBitCount;
			DWORD dwBytesInRes;
			WORD  nId;            // RT_ICON resource ID
		};

		struct GRPICONDIR {
			WORD            idReserved;
			WORD            idType;
			WORD            idCount;
			GRPICONDIRENTRY idEntries[1];
		};
#pragma pack(pop)

		void ExtractIconToFile(int groupIconId, const fs::path& outIcoPath)
		{
			struct SmartHandleFileLogo : Files::SmartHandleFile
			{
				using SmartHandleFile::SmartHandleFile;
				void WriteUint16(const uint16_t shortValue)
				{
					Write(std::span(
						reinterpret_cast<const char*>(&shortValue),
						sizeof shortValue)
					);
				}
			};
			SmartHandleFileLogo fileToWrite{
				outIcoPath / LOGO,
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				CREATE_ALWAYS,
				0
			};

			// Load the RT_GROUP_ICON resource
			HRSRC hGrpRes = FindResourceW(g_hInstance, MAKEINTRESOURCE(groupIconId), RT_GROUP_ICON);
			THROW_EXC_IFNULL(Exceptions::InvalidCall, hGrpRes, L"RT_GROUP_ICON not found");

			GRPICONDIR* pGrp = (GRPICONDIR*)LockResource(LoadResource(g_hInstance, hGrpRes));
			const WORD count = pGrp->idCount;

			// Collect raw data for each RT_ICON entry
			struct IconImage {
				GRPICONDIRENTRY entry;
				std::vector<BYTE> data;
			};
			std::vector<IconImage> images(count);

			for (WORD i = 0; i < count; i++) {
				images[i].entry = pGrp->idEntries[i];

				HRSRC hIconRes = FindResourceW(g_hInstance, MAKEINTRESOURCE(pGrp->idEntries[i].nId), RT_ICON);
				THROW_EXC_IFNULL(Exceptions::InvalidCall, hIconRes, L"RT_ICON not found");

				DWORD size = SizeofResource(g_hInstance, hIconRes);
				LPVOID pData = LockResource(LoadResource(g_hInstance, hIconRes));

				images[i].data.assign((BYTE*)pData, (BYTE*)pData + size);
			}

			// Build the .ico file in memory
			// Header + entries + image data
			size_t
				headerSize = sizeof(WORD) * 3 + sizeof(ICONDIRENTRY) * count,
				offset = headerSize;

			// Write ICONDIR header
			const WORD reserved = 0, type = 1;
			fileToWrite.WriteUint16(reserved);
			fileToWrite.WriteUint16(type);
			fileToWrite.WriteUint16(count);

			// Write ICONDIRENTRY array (with file offsets)
			for (size_t i = 0; i < count; i++) {
				ICONDIRENTRY entry{};
				entry.bWidth = images[i].entry.bWidth;
				entry.bHeight = images[i].entry.bHeight;
				entry.bColorCount = images[i].entry.bColorCount;
				entry.bReserved = 0;
				entry.wPlanes = images[i].entry.wPlanes;
				entry.wBitCount = images[i].entry.wBitCount;
				entry.dwBytesInRes = (DWORD)images[i].data.size();
				entry.dwImageOffset = (DWORD)offset;
				fileToWrite.Write(std::span((char*)&entry, sizeof(ICONDIRENTRY)));
				offset += images[i].data.size();
			}

			// Write raw image data
			for (size_t i = 0; i < count; i++)
				fileToWrite.Write(images[i].data);
		}
	}
	TaskSaverProfileIco::TaskSaverProfileIco() :
		Threads::ThreadPool::ITask(false)
	{}

	void TaskSaverProfileIco::Run()
	{
		std::wstring profile
		{
			BOM
			+ L"[.ShellClassInfo]"s
			+ L"\nLocalizedResourceName="s
			+ GUI::RemoveEmojiIcon(Globals::g_shortAppName.GetVal())
			+ L"\nIconResource=logo.ico,0\n"s
		};
		const auto profileFolder = Files::GetFolderAppDocumentsSettingsGeneric(Globals::g_userFolder);
		const auto desktopIniPath = profileFolder / "desktop.ini";
		const auto desktopIcoPath = profileFolder / LOGO;
		SetFileAttributesW(
			desktopIcoPath.c_str(),
			GetFileAttributesW(desktopIcoPath.c_str()) & !DWORD(FILE_ATTRIBUTE_SYSTEM)
		);
		SetFileAttributesW(
			profileFolder.c_str(),
			GetFileAttributesW(profileFolder.c_str()) & !DWORD(FILE_ATTRIBUTE_SYSTEM)
		);
		SetFileAttributesW(
			desktopIniPath.c_str(),
			GetFileAttributesW(desktopIniPath.c_str()) & !DWORD(FILE_ATTRIBUTE_SYSTEM)
		);
		ExtractIconToFile(
			IDI_MAIN_ICON,
			profileFolder
		);
		Files::SaveStringAs(
			desktopIniPath,
			profile
		);
		SetFileAttributesW(
			desktopIcoPath.c_str(),
			FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM
		);
		SetFileAttributesW(
			desktopIniPath.c_str(),
			FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM
		);
		SetFileAttributesW(
			profileFolder.c_str(),
			GetFileAttributesW(profileFolder.c_str()) | FILE_ATTRIBUTE_SYSTEM
		);
		SHChangeNotify(
			SHCNE_UPDATEITEM,
			SHCNF_PATHW | SHCNF_FLUSH,
			profileFolder.c_str(),
			NULL
		);
		SHChangeNotify(
			SHCNE_UPDATEITEM | SHCNE_ATTRIBUTES,
			SHCNF_PATHW | SHCNF_FLUSH,
			profileFolder.c_str(),
			NULL
		);
		DWORD_PTR result;
		SendMessageTimeout(HWND_BROADCAST,
			WM_SETTINGCHANGE,
			0,
			(LPARAM)L"Policy",
			SMTO_ABORTIFHUNG | SMTO_NORMAL,
			3000,
			&result
		);
	}
	TaskSaverProfileIco g_SetProfileFolder;
}