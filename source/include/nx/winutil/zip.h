
#ifndef __NX_WINUTIL_ZIP_H__
#define __NX_WINUTIL_ZIP_H__

#include <string>
#include <vector>

#include <nx\common\result.h>

namespace NX {

	namespace win {

		Result ZipCreate(const std::vector<std::wstring>& sources, const std::wstring& zipFile, DWORD dwWaitMs = INFINITE, bool overwrite = true, _In_opt_ LPSECURITY_ATTRIBUTES psa = NULL);
		Result ZipAdd(const std::vector<std::wstring>& sources, const std::wstring& zipFile, DWORD dwWaitMs = INFINITE);
		Result ZipUnpack(const std::wstring& zipFile, const std::wstring& targetDir, DWORD dwWaitMs = INFINITE);

	}
}


#endif