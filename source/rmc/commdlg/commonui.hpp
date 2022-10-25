

#ifndef __NXRM_COMMON_UI_HPP__
#define __NXRM_COMMON_UI_HPP__


extern HINSTANCE _hInstance;
extern HINSTANCE _hResource;
extern DWORD     _dwLangId;                     // Language ID

std::wstring LoadStringResource(int nID, const wchar_t *szDefaultString);

std::wstring LoadStringResource(int nID, int resID);

std::wstring LoadLocalStringResource(int resID);



#endif