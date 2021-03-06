#include "folder_monitor.h"

namespace IO {
	FolderMonitor::~FolderMonitor() {
		Close();
	}

	void FolderMonitor::Close() {
		m_imp.reset();
	}

	void FolderMonitor::Start(const std::string& path) {
		m_imp.reset(doCreateImp(path, OnEvent));
	}
}
