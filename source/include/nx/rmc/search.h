

#ifndef __LIBRMC_SEARCH_H__
#define __LIBRMC_SEARCH_H__

#include <nx\rmc\dirs.h>
#include <nx\rmc\router.h>
#include <nx\rmc\rmserver.h>
#include <nx\rmc\tenant.h>
#include <nx\rmc\user.h>
#include <nx\rmc\rest.h>
#include <nx\rmc\keym.h>
#include <nx\rmc\repo.h>

#include <nx\common\result.h>
#include <nx\crypt\rsa.h>

#include <string>
#include <vector>

namespace NX {

	typedef enum SORTORDER {
		OrderByFileNameAscend		= 0x00000001,
		OrderByFileNameDescend		= 0x00000002,
		OrderByCreationTimeAscend	= 0x00000004,
		OrderByCreationTimeDescend	= 0x00000008,
		OrderByModifiedTimeAscend   = 0x00000010,
		OrderByModifiedTimeDescend  = 0x00000020,
		OrderBySizeAscend			= 0x00000040,
		OrderBySizeDescend			= 0x00000080,
		OrderByDisplayNameAscend	= 0x00000100,
		OrderByDisplayNameDescend	= 0x00000200,
		OrderByLastActionTimeAscend	= 0x00000400,
		OrderByLastActionTimeDescend= 0x00000800,
		OrderByNameAscend			= 0x00001000,
		OrderByNameDescend			= 0x00002000
	} SORTORDER;

	class RmSession;
	
	class RmFinder
	{
	public:
		RmFinder();
		RmFinder(__int64 pageSize, const std::vector<int>& sortTypes, const std::wstring& filter, const std::wstring& searchFields, const std::wstring& searchKeywrod);
		RmFinder(const RmFinder& rhs);
		virtual ~RmFinder();

		RmFinder& operator = (const RmFinder& rhs);

		virtual std::wstring CreateQueryParameters();

		inline bool NoMoreData() const { return (_pageId >= _totalPages); }

		inline void ResetPageId() { _pageId = 1; }
		inline __int64 GetPageId() const { return _pageId; }
		inline __int64 GetPageSize() const { return _pageSize; }
		inline __int64 GetTotalPages() const { return _totalPages; }
		inline __int64 GetTotalRecords() const { return _totalRecords; }
		inline const std::wstring& GetSortType() const { return _orderBy; }
		inline const std::wstring& GetFilter() const { return _filter; }
		inline const std::wstring& GetSearchFields() const { return _searchFields; }
		inline const std::wstring& GetSearchKeyword() const { return _searchKeyword; }
		
	protected:
		void SetTotalRecords(__int64 totalRecords)
		{
			_totalRecords = totalRecords;
			_totalPages = (_totalRecords + _pageSize - 1) / _pageSize;
		}

		__int64 NextPage() { return (_pageId < _totalPages) ? (++_pageId) : _pageId; }

	private:
		__int64	   _pageSize;
		std::wstring _orderBy;
		std::wstring _filter;
		std::wstring _searchFields;
		std::wstring _searchKeyword;

		__int64		_pageId;
		__int64		_totalPages;
		__int64		_totalRecords;

		friend class RmSession;
	};
}


#endif