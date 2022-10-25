
#ifndef __LIBNXL_UTIL_H__
#define __LIBNXL_UTIL_H__

#include <nx\common\result.h>

namespace NX {

	namespace NXL {

		Result CreateProtectedFile(const std::wstring& source, const std::wstring& target);
		Result CreateSharedFile(const std::wstring& source, const std::wstring& target);


	}
}


#endif