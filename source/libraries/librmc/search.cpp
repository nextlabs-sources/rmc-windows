

#include "stdafx.h"

#include <nx\rmc\search.h>

#include <nx\common\stringex.h>

using namespace NX;


static std::wstring SortTypeToString(int sortType)
{
	if (0 != (sortType & OrderByFileNameAscend))
		return std::wstring(L"fileName");
	if (0 != (sortType & OrderByFileNameDescend))
		return std::wstring(L"-fileName");

	if (0 != (sortType & OrderByNameAscend))
		return std::wstring(L"name");
	if (0 != (sortType & OrderByNameDescend))
		return std::wstring(L"-name");


	if (0 != (sortType & OrderByDisplayNameAscend))
		return std::wstring(L"displayName");
	if (0 != (sortType & OrderByDisplayNameDescend))
		return std::wstring(L"-displayName");
	
	if (0 != (sortType & OrderByCreationTimeAscend))
		return std::wstring(L"creationTime");
	if (0 != (sortType & OrderByCreationTimeDescend))
		return std::wstring(L"-creationTime");
	
	if (0 != (sortType & OrderByModifiedTimeAscend))
		return std::wstring(L"lastModified");
	if (0 != (sortType & OrderByModifiedTimeDescend))
		return std::wstring(L"-lastModified");
	
	if (0 != (sortType & OrderByModifiedTimeAscend))
		return std::wstring(L"lastModified");
	if (0 != (sortType & OrderByModifiedTimeDescend))
		return std::wstring(L"-lastModified");
	
	if (0 != (sortType & OrderByLastActionTimeAscend))
		return std::wstring(L"lastActionTime");
	if (0 != (sortType & OrderByLastActionTimeDescend))
		return std::wstring(L"-lastActionTime");

	return std::wstring();
}
static std::wstring SortListToString(const std::vector<int>& sortTypes)
{
	std::wstring s;
	std::for_each(sortTypes.cbegin(), sortTypes.cend(), [&](int type) {
		const std::wstring& typeName = SortTypeToString(type);
		if (!typeName.empty()) {
			if (!s.empty()) s.append(L",");
			s.append(typeName);
		}
	});
	return s;
}

RmFinder::RmFinder() : _pageSize(10), _pageId(1), _totalPages(0), _totalRecords(0)
{
}

RmFinder::RmFinder(__int64 pageSize, const std::vector<int>& sortTypes, const std::wstring& filter, const std::wstring& searchFields, const std::wstring& searchKeyword)
	: _pageSize((pageSize > 0) ? pageSize : 1), _pageId(1), _totalPages(0), _totalRecords(0),
	_orderBy(SortListToString(sortTypes)), _filter(filter), _searchFields(searchFields), _searchKeyword(searchKeyword)
{
}

RmFinder::RmFinder(const RmFinder& rhs)
	: _pageSize(rhs._pageSize), _pageId(rhs._pageId), _totalPages(rhs._totalPages), _totalRecords(rhs._totalRecords),
	_orderBy(rhs._orderBy), _filter(rhs._filter), _searchFields(rhs._searchFields), _searchKeyword(rhs._searchKeyword)
{
}

RmFinder::~RmFinder()
{
}

RmFinder & RmFinder::operator=(const RmFinder& rhs)
{
	if (this != &rhs)
	{
		_pageSize = rhs._pageSize;
		_pageId = rhs._pageId;
		_totalPages = rhs._totalPages;
		_totalRecords = rhs._totalRecords;
		_orderBy = rhs._orderBy;
		_filter = rhs._filter;
		_searchFields = rhs._searchFields;
		_searchKeyword = rhs._searchKeyword;
	}

	return *this;

}
std::wstring RmFinder::CreateQueryParameters()
{
	std::wstring s;
	s.append(L"page=");
	s.append(NX::i64tos<wchar_t>(_pageId));
	s.append(L"&size=");
	s.append(NX::i64tos<wchar_t>(_pageSize));
	if (!_orderBy.empty()) {
		s.append(L"&orderBy=");
		s.append(_orderBy);
	}
	if (!_filter.empty()) {
		s.append(L"&filter=");
		s.append(_filter);
	}
	if (!_searchFields.empty() && !_searchKeyword.empty()) {
		s.append(L"&q.");
		s.append(_searchFields);
		s.append(L"=");
		s.append(_searchKeyword);
	}
	return s;
}