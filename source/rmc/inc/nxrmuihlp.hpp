
#ifndef _NUDF_UIHLP_HPP__
#define _NUDF_UIHLP_HPP__

#include <string>
#include <vector>
#include <algorithm>

namespace NX {
namespace utility {

    
typedef BOOL (WINAPI* RMUINITIALIZE)();
typedef VOID (WINAPI* RMUDEINITIALIZE)();
typedef LONG (WINAPI* RMUSHOWALLPROPPAGES)(_In_opt_ HWND, _In_ LPCWSTR, _In_ void *, _In_ ULONGLONG, _In_opt_ LPCWSTR);
typedef LONG (WINAPI* RMUSHOWGENERALPROPPAGE)(_In_opt_ HWND, _In_ LPCWSTR, _In_ void*);
typedef LONG (WINAPI* RMUSHOWPERMISSIONPROPPAGE)(_In_opt_ HWND, _In_ LPCWSTR, _In_ const ULONGLONG, _Out_opt_ PULONGLONG);
typedef LONG (WINAPI* RMUSHOWCLASSIFYPROPPAGE)(_In_opt_ HWND, _In_ LPCWSTR, _In_opt_ LPCWSTR);
typedef LONG (WINAPI* RMUSHOWDETAILSPROPPAGE)(_In_opt_ HWND, _In_ LPCWSTR,_In_ const ULONGLONG, _In_opt_ LPCWSTR);
typedef LONG (WINAPI* RMUSHOWALLPROPPAGESSIMPLE)(_In_opt_ HWND, _In_ LPCWSTR, _In_ ULONGLONG);
typedef LONG (WINAPI* RMUSHOWGENERALPROPPAGESIMPLE)(_In_opt_ HWND, _In_ LPCWSTR);
typedef LONG (WINAPI* RMUSHOWCLASSIFYPROPPAGESIMPLE)(_In_opt_ HWND, _In_ LPCWSTR);
typedef LONG (WINAPI* RMUSHOWDETAILSPROPPAGESIMPLE)(_In_opt_ HWND, _In_ LPCWSTR, _In_ ULONGLONG);
typedef VOID (WINAPI* RMUFREERESOURCE)(_In_ PVOID);
typedef LONG (WINAPI* RMUSHOWCLASSIFYDIALOG)(_In_opt_ HWND, _In_ LPCWSTR, _In_ LPCWSTR, _In_opt_ LPCWSTR, _In_opt_ LPCWSTR, _In_ BOOL, _Out_ LPWSTR*);
typedef LONG (WINAPI* RMUSHOWCLASSIFYDIALOGEX)(_In_opt_ HWND, _In_ LPCWSTR, _In_ LPCWSTR, _In_opt_ LPCWSTR, _In_ ULONG, _In_opt_ LPCWSTR, _In_ BOOL, _Out_ LPWSTR*);
typedef LONG (WINAPI* RMUSHOWCLASSIFYDIALOG2)(_In_opt_ HWND, _In_ LPCWSTR, _In_ LPCWSTR, _Out_ LPWSTR*);
typedef LONG (WINAPI* RMUSHOWCLASSIFYDIALOGEX2)(_In_opt_ HWND, _In_ LPCWSTR, _In_ LPCWSTR, _In_ ULONG, _Out_ LPWSTR*);
typedef LONG(WINAPI* RMUSHOWSHAREDIALOG)(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ LPCWSTR wzJasonConf, _In_ ULONG langId, _Out_opt_ LPWSTR * ppwzUserSettings, _Out_ LPWSTR* ppwzClassifyTags);


template <typename T>
void MultiStringsToPairVector(_In_ const T* sStrings, _Out_ std::vector<std::pair<std::basic_string<T>, std::basic_string<T>>>& vStrings, _In_ const T cSeparator)
{
    vStrings.clear();
    while (0 != sStrings[0]) {
        // Get current string
        std::basic_string<T> s = sStrings;
        // Move to Next
        sStrings += (s.length() + 1);
        // Parse this pair
        std::basic_string<T>::size_type pos = s.find_first_of(cSeparator);
        if (std::basic_string<T>::npos == pos) {
            continue;
        }
        // Insert Pair
        vStrings.push_back(std::pair<std::basic_string<T>, std::basic_string<T>>(s.substr(0, pos), s.substr(pos + 1)));
    }
}

template <typename T>
void PairVectorToMultiStrings(_In_ const std::vector<std::pair<std::basic_string<T>, std::basic_string<T>>>& vStrings, _Out_ std::vector<T>& sStrings, _In_ const T cSeparator)
{
    sStrings.clear();
    for (std::vector<std::pair<std::basic_string<T>, std::basic_string<T>>>::const_iterator it = vStrings.begin(); it != vStrings.end(); ++it) {
        if ((*it).first.empty()) {
            continue;
        }
        std::basic_string<T> s = (*it).first;
        s += cSeparator;
        s += (*it).second;
        sStrings.insert(sStrings.end(), s.begin(), s.end());
        sStrings.push_back(((T)0));
    }
    sStrings.push_back(((T)0));
}

template <typename CharType>
std::vector<std::pair<std::basic_string<CharType>, std::basic_string<CharType>>> buffer_to_pair(_In_ const std::vector<CharType> buf)
{
	std::vector<std::pair<std::basic_string<CharType>, std::basic_string<CharType>>> pairs;
	if (buf.empty()) {
		return std::move(pairs);
	}
	bool bfstring = true;
	std::basic_string<CharType> first, second;
	std::for_each(buf.begin(), buf.end(), [&](const CharType& c) {
		if (c == CharType(0)) {
			if (!first.empty()) {
				pairs.push_back(std::pair<std::basic_string<CharType>, std::basic_string<CharType>>(first, second));
				first.clear();
				second.clear();
				bfstring = true;
			}
		}
		else if (c == CharType('=')) {
			bfstring = false;
		}
		else {
			if (bfstring) {
				first.push_back(c);
			}
			else {
				second.push_back(c);
			}
		}
	});
	return std::move(pairs);
}

template <typename CharType>
std::vector<std::pair<std::basic_string<CharType>, std::basic_string<CharType>>> buffer_to_pair(_In_ const CharType * pbuf)
{
	std::vector<std::pair<std::basic_string<CharType>, std::basic_string<CharType>>> pairs;
	if (NULL == pbuf) {
		return std::move(pairs);
	}
	bool bfstring = true;
	std::basic_string<CharType> first, second;
	while (pbuf[0] != CharType(0)) {
		std::basic_string<CharType> wsPair(pbuf);
		pbuf += (wsPair.length() + 1);
		std::basic_string<CharType> name;
		std::basic_string<CharType> value;
		std::basic_string<CharType>::size_type pos = wsPair.find_first_of(L'=');
		if (pos == std::wstring::npos) {
			continue;
		}
		name = wsPair.substr(0, pos);
		value = wsPair.substr(pos + 1);
		pairs.push_back(std::pair<std::basic_string<CharType>, std::basic_string<CharType>>(name, value));
	}
	return std::move(pairs);
}

template <typename CharType>
std::vector<CharType> pair_to_buffer(_In_ const std::vector<std::pair<std::basic_string<CharType>, std::basic_string<CharType>>> &pairs)
{
	std::vector<CharType> buf;
	if (pairs.empty()) {
		return std::move(buf);
	}
	std::for_each(pairs.begin(), pairs.end(), [&](const std::pair < std::basic_string<CharType>, std::basic_string<CharType>>& p) {
		if (!p.first.empty()) {
			std::for_each(p.first.begin(), p.first.end(), [&](const CharType& c) {
				buf.push_back(c);
			});
			buf.push_back(CharType('='));
			std::for_each(p.second.begin(), p.second.end(), [&](const CharType& c) {
				buf.push_back(c);
			});
			buf.push_back(CharType(0));
		}
	});
	buf.push_back(CharType(0));
	return std::move(buf);
}



class CRmuObject
{
public:
    CRmuObject() : _hDll(NULL),
                   _RmuInitialize(NULL),
                   _RmuDeinitialize(NULL),
                   _RmuShowAllPropPages(NULL),
                   _RmuShowGeneralPropPage(NULL),
                   _RmuShowPermissionPropPage(NULL),
                   _RmuShowClassifyPropPage(NULL),
                   _RmuShowDetailsPropPage(NULL),
                   _RmuShowAllPropPagesSimple(NULL),
                   _RmuShowGeneralPropPageSimple(NULL),
                   _RmuShowClassifyPropPageSimple(NULL),
                   _RmuShowDetailsPropPageSimple(NULL),
                   _RmuFreeResource(NULL),
                   _RmuShowClassifyDialog(NULL),
                   _RmuShowClassifyDialogEx(NULL),
                   _RmuShowClassifyDialog2(NULL),
                   _RmuShowClassifyDialogEx2(NULL),
				   _RmuShowShareDialog(NULL)
    {
    }

    ~CRmuObject()
    {
        Clear();
    }


    void Initialize(_In_ LPCWSTR wzDll)
    {
        _hDll = ::LoadLibraryW(wzDll);
        if(NULL == _hDll) {
			return;
        }

        _RmuInitialize = (RMUINITIALIZE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(1));
        if (NULL == _RmuInitialize) {
            return;
        }
        _RmuDeinitialize = (RMUDEINITIALIZE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(2));
        if (NULL == _RmuDeinitialize) {
            return;
        }
        _RmuShowAllPropPages = (RMUSHOWALLPROPPAGES)::GetProcAddress(_hDll, MAKEINTRESOURCEA(3));
        if (NULL == _RmuShowAllPropPages) {
            return;
        }
        _RmuShowGeneralPropPage = (RMUSHOWGENERALPROPPAGE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(4));
        if (NULL == _RmuShowGeneralPropPage) {
            return;
        }
        _RmuShowPermissionPropPage = (RMUSHOWPERMISSIONPROPPAGE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(5));
        if (NULL == _RmuShowPermissionPropPage) {
            return;
        }
        _RmuShowClassifyPropPage = (RMUSHOWCLASSIFYPROPPAGE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(6));
        if (NULL == _RmuShowClassifyPropPage) {
            return;
        }
        _RmuShowDetailsPropPage = (RMUSHOWDETAILSPROPPAGE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(7));
        if (NULL == _RmuShowDetailsPropPage) {
            return;
        }
        _RmuShowAllPropPagesSimple = (RMUSHOWALLPROPPAGESSIMPLE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(8));
        if (NULL == _RmuShowAllPropPagesSimple) {
            return;
        }
        _RmuShowGeneralPropPageSimple = (RMUSHOWGENERALPROPPAGESIMPLE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(9));
        if (NULL == _RmuShowGeneralPropPageSimple) {
            return;
        }
        _RmuShowClassifyPropPageSimple = (RMUSHOWCLASSIFYPROPPAGESIMPLE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(10));
        if (NULL == _RmuShowClassifyPropPageSimple) {
            return;
        }
        _RmuShowDetailsPropPageSimple = (RMUSHOWDETAILSPROPPAGESIMPLE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(11));
        if (NULL == _RmuShowDetailsPropPageSimple) {
            return;
        }
        _RmuFreeResource = (RMUFREERESOURCE)::GetProcAddress(_hDll, MAKEINTRESOURCEA(12));
        if (NULL == _RmuFreeResource) {
            return;
        }
        _RmuShowClassifyDialog = (RMUSHOWCLASSIFYDIALOG)::GetProcAddress(_hDll, MAKEINTRESOURCEA(13));
        if (NULL == _RmuShowClassifyDialog) {
            return;
        }
        _RmuShowClassifyDialogEx = (RMUSHOWCLASSIFYDIALOGEX)::GetProcAddress(_hDll, MAKEINTRESOURCEA(14));
        if (NULL == _RmuShowClassifyDialog) {
            return;
        }
        _RmuShowClassifyDialog2 = (RMUSHOWCLASSIFYDIALOG2)::GetProcAddress(_hDll, MAKEINTRESOURCEA(15));
        if (NULL == _RmuShowClassifyDialog) {
            return;
        }
        _RmuShowClassifyDialogEx2 = (RMUSHOWCLASSIFYDIALOGEX2)::GetProcAddress(_hDll, MAKEINTRESOURCEA(16));
        if (NULL == _RmuShowClassifyDialog) {
            return;
        }
		_RmuShowShareDialog = (RMUSHOWSHAREDIALOG)::GetProcAddress(_hDll, MAKEINTRESOURCEA(17));
		if (NULL == _RmuShowShareDialog) {
			return;
		}
    }

    void Clear()
    {
        _RmuInitialize = NULL;
        _RmuDeinitialize = NULL;
        _RmuShowAllPropPages = NULL;
        _RmuShowGeneralPropPage = NULL;
        _RmuShowPermissionPropPage = NULL;
        _RmuShowClassifyPropPage = NULL;
        _RmuShowDetailsPropPage = NULL;
        _RmuShowAllPropPagesSimple = NULL;
        _RmuShowGeneralPropPageSimple = NULL;
        _RmuShowClassifyPropPageSimple = NULL;
        _RmuShowDetailsPropPageSimple = NULL;
        _RmuFreeResource = NULL;
        _RmuShowClassifyDialog = NULL;
        _RmuShowClassifyDialogEx = NULL;
        _RmuShowClassifyDialog2 = NULL;
        _RmuShowClassifyDialogEx2 = NULL;
		_RmuShowShareDialog = NULL;
        if(NULL != _hDll) {
            FreeLibrary(_hDll);
            _hDll = NULL;
        }
    }

    BOOL RmuInitialize()
    {
        if(NULL == _RmuInitialize) {
            return FALSE;
        }
        return _RmuInitialize();
    }

    VOID RmuDeinitialize()
    {
        if(NULL == _RmuDeinitialize) {
            return;
        }
        _RmuDeinitialize();
    }

    LONG RmuShowAllPropPages(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ void * pHeader, _In_ ULONGLONG uRights, _In_ const std::vector<std::pair<std::wstring,std::wstring>>& classifydata)
    {
        if(NULL == _RmuShowAllPropPages) {
            return -1;
        }
        
        std::vector<wchar_t> vClassifyData;
        PairVectorToMultiStrings<wchar_t>(classifydata, vClassifyData, L'=');
        
        return _RmuShowAllPropPages(hParent, wzFile, pHeader, uRights, vClassifyData.empty() ? NULL : (&vClassifyData[0]));
    }

    LONG RmuShowGeneralPropPage(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ void * pHeader)
    {
        if(NULL == _RmuShowGeneralPropPage) {
            return -1;
        }
        return _RmuShowGeneralPropPage(hParent, wzFile, pHeader);
    }

    LONG RmuShowPermissionPropPage(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ const ULONGLONG uRrights, _Out_opt_ PULONGLONG puNewRights)
    {
        if(NULL == _RmuShowPermissionPropPage) {
            return -1;
        }
        return _RmuShowPermissionPropPage(hParent, wzFile, uRrights, puNewRights);
    }

    LONG RmuShowClassifyPropPage(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ const std::vector<std::pair<std::wstring,std::wstring>>& classifydata)
    {
        if(NULL == _RmuShowClassifyPropPage) {
            return -1;
        }
        std::vector<wchar_t> vClassifyData;
        PairVectorToMultiStrings<wchar_t>(classifydata, vClassifyData, L'=');
        return _RmuShowClassifyPropPage(hParent, wzFile, vClassifyData.empty() ? NULL : (&vClassifyData[0]));
    }

    LONG RmuShowDetailsPropPage(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ const ULONGLONG uRrights, _In_ const std::vector<std::pair<std::wstring,std::wstring>>& classifydata)
    {
        if(NULL == _RmuShowDetailsPropPage) {
            return -1;
        }
        std::vector<wchar_t> vClassifyData;
        PairVectorToMultiStrings<wchar_t>(classifydata, vClassifyData, L'=');
        return _RmuShowDetailsPropPage(hParent, wzFile, uRrights, vClassifyData.empty() ? NULL : (&vClassifyData[0]));
    }

    LONG RmuShowAllPropPagesSimple(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ ULONGLONG uRights)
    {
        if(NULL == _RmuShowAllPropPagesSimple) {
            return -1;
        }
        return _RmuShowAllPropPagesSimple(hParent, wzFile, uRights);
    }

    LONG RmuShowGeneralPropPageSimple(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile)
    {
        if(NULL == _RmuShowGeneralPropPageSimple) {
            return -1;
        }
        return _RmuShowGeneralPropPageSimple(hParent, wzFile);
    }

    LONG RmuShowClassifyPropPageSimple(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile)
    {
        if(NULL == _RmuShowClassifyPropPageSimple) {
            return -1;
        }
        return _RmuShowClassifyPropPageSimple(hParent, wzFile);
    }

    LONG RmuShowDetailsPropPageSimple(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ ULONGLONG uRights)
    {
        if(NULL == _RmuShowDetailsPropPageSimple) {
            return -1;
        }
        return _RmuShowDetailsPropPageSimple(hParent, wzFile, uRights);
    }

    LONG RmuShowClassifyDialog(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ LPCWSTR wzXml, _In_opt_ LPCWSTR wzGroup, _In_ BOOL ReadOnly, _Inout_ std::vector<std::pair<std::wstring,std::wstring>>& tags)
    {
        LONG   lRet = 0;
        LPWSTR pwzTags = NULL;
        std::vector<WCHAR> init_tags;

        if(NULL == _RmuShowClassifyDialog || NULL == _RmuFreeResource) {
            return -1;
        }

        if(!tags.empty()) {
			init_tags = NX::utility::pair_to_buffer(tags);
        }

        lRet =  _RmuShowClassifyDialog(hParent, wzFile, wzXml, wzGroup, init_tags.empty() ? NULL : (&init_tags[0]), ReadOnly, &pwzTags);
        if(0 == lRet && NULL != pwzTags) {
			tags = NX::utility::buffer_to_pair(pwzTags);
            _RmuFreeResource(pwzTags);
            pwzTags = NULL;
        }

        return lRet;
    }

    LONG RmuShowClassifyDialogEx(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ LPCWSTR wzXmlFile, _In_opt_ LPCWSTR wzGroup, _In_ ULONG langId, _In_ BOOL ReadOnly, _Inout_ std::vector<std::pair<std::wstring,std::wstring>>& tags)
    {
        LONG   lRet = 0;
        LPWSTR pwzTags = NULL;
        std::vector<WCHAR> init_tags;

        if(NULL == _RmuShowClassifyDialog || NULL == _RmuShowClassifyDialogEx || NULL == _RmuFreeResource) {
            return -1;
        }

        if(!tags.empty()) {
			init_tags = NX::utility::pair_to_buffer(tags);
        }

        lRet =  _RmuShowClassifyDialogEx(hParent, wzFile, wzXmlFile, wzGroup, langId, init_tags.empty() ? NULL : (&init_tags[0]), ReadOnly, &pwzTags);
        if(0 == lRet && NULL != pwzTags) {
			tags = NX::utility::buffer_to_pair(pwzTags);
            _RmuFreeResource(pwzTags);
            pwzTags = NULL;
        }

        return lRet;
    }

    LONG RmuShowClassifyDialog2(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ LPCWSTR wzXml, _Out_ std::vector<std::pair<std::wstring,std::wstring>>& tags)
    {
        LONG   lRet = 0;
        LPWSTR pwzTags = NULL;
        if(NULL == _RmuShowClassifyDialog2 || NULL == _RmuFreeResource) {
            return -1;
        }

        lRet =  _RmuShowClassifyDialog2(hParent, wzFile, wzXml, &pwzTags);
        if(0 == lRet && NULL != pwzTags) {
			tags = NX::utility::buffer_to_pair(pwzTags);
            _RmuFreeResource(pwzTags);
            pwzTags = NULL;
        }

        return lRet;
    }

    LONG RmuShowClassifyDialogEx2(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ LPCWSTR wzXmlFile, _In_ ULONG langId, _Out_ std::vector<std::pair<std::wstring,std::wstring>>& tags)
    {
        LONG   lRet = 0;
        LPWSTR pwzTags = NULL;
        if(NULL == _RmuShowClassifyDialog2 || NULL == _RmuShowClassifyDialogEx2 || NULL == _RmuFreeResource) {
            return -1;
        }

        lRet =  _RmuShowClassifyDialogEx2(hParent, wzFile, wzXmlFile, langId, &pwzTags);
        if(0 == lRet && NULL != pwzTags) {
			tags = NX::utility::buffer_to_pair(pwzTags);
            _RmuFreeResource(pwzTags);
            pwzTags = NULL;
        }

        return lRet;
    }

	LONG RmuShowShareDialog(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ LPCWSTR wzJasonConf, _In_ ULONG langId, _Out_ std::vector<std::pair<std::wstring, std::wstring>>& tags,  _Out_opt_ std::wstring & settings)
	{
		LONG   lRet = 0;
		LPWSTR pwzTags = NULL;
		LPWSTR pwzSetting = NULL;
		if (NULL == wzFile || NULL == _RmuFreeResource) {
			return -1;
		}

		lRet = _RmuShowShareDialog(hParent, wzFile, wzJasonConf, langId, &pwzSetting, &pwzTags);
		if (0 == lRet && NULL != pwzTags) {
			tags = NX::utility::buffer_to_pair(pwzTags);
			_RmuFreeResource(pwzTags);
			pwzTags = NULL;
		}
		if (0 == lRet && NULL != pwzSetting) {
			//add parsing setting in future release.
			settings = pwzSetting;
			_RmuFreeResource(pwzSetting);
			pwzSetting = NULL;
		}

		return lRet;
	}

private:
    HMODULE                         _hDll;
    RMUINITIALIZE                   _RmuInitialize;
    RMUDEINITIALIZE                 _RmuDeinitialize;
    RMUSHOWALLPROPPAGES             _RmuShowAllPropPages;
    RMUSHOWGENERALPROPPAGE          _RmuShowGeneralPropPage;
    RMUSHOWPERMISSIONPROPPAGE       _RmuShowPermissionPropPage;
    RMUSHOWCLASSIFYPROPPAGE         _RmuShowClassifyPropPage;
    RMUSHOWDETAILSPROPPAGE          _RmuShowDetailsPropPage;
    RMUSHOWALLPROPPAGESSIMPLE       _RmuShowAllPropPagesSimple;
    RMUSHOWGENERALPROPPAGESIMPLE    _RmuShowGeneralPropPageSimple;
    RMUSHOWCLASSIFYPROPPAGESIMPLE   _RmuShowClassifyPropPageSimple;
    RMUSHOWDETAILSPROPPAGESIMPLE    _RmuShowDetailsPropPageSimple;
    RMUFREERESOURCE                 _RmuFreeResource;
    RMUSHOWCLASSIFYDIALOG           _RmuShowClassifyDialog;
    RMUSHOWCLASSIFYDIALOGEX         _RmuShowClassifyDialogEx;
    RMUSHOWCLASSIFYDIALOG2          _RmuShowClassifyDialog2;
    RMUSHOWCLASSIFYDIALOGEX2        _RmuShowClassifyDialogEx2;
	RMUSHOWSHAREDIALOG				_RmuShowShareDialog;
};

}
}



#endif  // _NUDF_UIHLP_HPP__
