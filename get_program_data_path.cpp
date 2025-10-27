#include "get_program_data_path.h"
#include <windows.h>
#include <shlobj.h>      // SHGetKnownFolderPath

#include <string>

std::filesystem::path getProgramDataPath()
{
	PWSTR wpath = nullptr;
	std::filesystem::path result;

	// �������� ���� � ProgramData (��������, C:\ProgramData)
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &wpath)))
	{
		result = std::filesystem::path(wpath);
		CoTaskMemFree(wpath);  // ����������� ���������� ������!
	}
	else
	{
		// fallback (� ������ �������)
		result = "C:\\ProgramData";
	}

	return result;
}
