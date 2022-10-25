

#ifndef __LIBRMC_DIRS_H__
#define __LIBRMC_DIRS_H__


#include <string>

namespace NX {

    class RmDirs
    {
    public:
        RmDirs();
        RmDirs(const RmDirs& rhs);
        ~RmDirs();

        RmDirs& operator = (const RmDirs& rhs);

        const std::wstring& GetAppRoot() const { return _app_root; }
        const std::wstring& GetAppBinDir() const { return _bin_dir; }
		void SetAppRoot(const std::wstring& dir);

        const std::wstring& GetDataRoot() const { return _data_root; }
        std::wstring GetDataCacheDir() const { return std::wstring(_data_root.empty() ? L"" : (_data_root + L"\\Cache")); }
        std::wstring GetDataTempDir() const { return std::wstring(_data_root.empty() ? L"" : (_data_root + L"\\Temp")); }
        std::wstring GetDataOfflineDir() const { return std::wstring(_data_root.empty() ? L"" : (_data_root + L"\\Cache\\Offline")); }

    protected:
        virtual void Init();

    private:
        std::wstring    _app_root;
        std::wstring    _bin_dir;
        std::wstring    _data_root;
    };
}


#endif