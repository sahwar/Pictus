#include "../fileops.h"
#include "../file_reader.h"

#include <boost/filesystem.hpp>
#include <mutex>

#include <windows.h>
#include <shellapi.h>

namespace IO {
	using std::mutex;

	mutex g_mutexSHFileOperation;

	struct HANDLETOMAPPINGS {
		UINT              uNumberOfMappings;  // Number of mappings in the array.
		LPSHNAMEMAPPINGW  lpSHNameMapping;    // Pointer to the array of mappings.
	};

	static std::vector<wchar_t> ToTerminatedWcharArray(const std::string& utf8) {
		auto in = UTF8ToWString(utf8);
		size_t len = in.length() + 2; // Two extra due to _two_ terminating chars.
		std::vector<wchar_t> pFromWString(len);

		// Copy source-file into the struct and apply the extra terminator.
		errno_t copyRet = wcscpy_s(&pFromWString[0], len, in.c_str());
		if (copyRet != 0) DO_THROW(Err::CriticalError, "Couldn't copy string to buffer.");

		pFromWString[len - 1] = 0;	

		return pFromWString;
	}

	bool performDeleteRecycle(const std::string& file, bool doRecycle, wxWindow* parent) {
		auto hwnd = parent->GetHandle();
		SHFILEOPSTRUCTW sfop;
		ZeroMemory(&sfop, sizeof(sfop));

		auto pFromWString = ToTerminatedWcharArray(file);
		sfop.pFrom = &pFromWString[0];
		sfop.wFunc = FO_DELETE;
		sfop.fFlags = FOF_SILENT;

		if(hwnd == 0) {
			sfop.fFlags |= FOF_NOCONFIRMATION;
		}

		if (doRecycle) {
			sfop.fFlags |= FOF_ALLOWUNDO;
		}

		sfop.hwnd = hwnd;

		std::lock_guard<std::mutex> l(g_mutexSHFileOperation);
		return (SHFileOperationW(&sfop) == 0) && (sfop.fAnyOperationsAborted == false);
	}

	bool FileDelete(const std::string& file, wxWindow* parent) {
		return performDeleteRecycle(file, false, parent);
	}

	bool FileRecycle(const std::string& file, wxWindow* parent) {
		return performDeleteRecycle(file, true, parent);
	}

	std::string Rename(const std::string& old_name, const std::string& new_name, wxWindow* parent) {
		auto hwnd = parent->GetHandle();
		SHFILEOPSTRUCTW sfop;
		ZeroMemory(&sfop, sizeof(sfop));

		auto pOldName = ToTerminatedWcharArray(old_name);
		auto pNewName = ToTerminatedWcharArray(new_name);
		sfop.pFrom = &pOldName[0];
		sfop.pTo = &pNewName[0];
		sfop.wFunc = FO_RENAME;
		sfop.fFlags = FOF_SILENT | FOF_ALLOWUNDO | FOF_WANTMAPPINGHANDLE;
		sfop.hwnd = hwnd;

		if(hwnd == 0)
			sfop.fFlags |= FOF_NOCONFIRMATION;

		//		int ticket = pause_folders(GetPath(old_name));

		bool toRet;
		{
			std::lock_guard<std::mutex> l(g_mutexSHFileOperation);
			toRet = (SHFileOperationW(&sfop) == 0) && (sfop.fAnyOperationsAborted == false);
		}

		auto resulting_name = new_name;

		HANDLETOMAPPINGS* s = reinterpret_cast<HANDLETOMAPPINGS*>(sfop.hNameMappings);
		if(s && s->uNumberOfMappings == 1) {
			resulting_name = WStringToUTF8(s->lpSHNameMapping[0].pszNewPath);
		}

		SHFreeNameMappings(sfop.hNameMappings);

		if (!toRet) {
			return "";
		}

		return resulting_name;
	}

	bool SupportRecycle() {
		return true;
	}

	std::string GetPath(const std::string& s) {
		auto parent_path = boost::filesystem::path(s).make_preferred().parent_path().string();
		if (*parent_path.rbegin() != '\\')
		{
			return parent_path + "\\";
		}
		return parent_path;
	}
}
