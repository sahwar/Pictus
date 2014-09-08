#ifndef CONFIG_H
#define CONFIG_H

#include "app_types.h"

namespace App {
	static const wchar_t* cg_SettingsLocation = TX("\\Poppeman\\Pictus\\Pictus.ini");
	static const wchar_t* cg_RunLogLocation = TX("\\Poppeman\\Pictus\\runlog.txt");
	static const wchar_t* cg_PictusDefaultProgramsName = TX("Pictus Image Viewer");
}

namespace Sys {
	namespace Association {
#ifndef _DEBUG
#define PICTUS_ENABLE_FILEFORMATASSOCIATION
#endif
	}
}

#endif
