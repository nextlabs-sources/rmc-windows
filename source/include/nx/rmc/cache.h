

#ifndef __LIBRMC_CACHE_H__
#define __LIBRMC_CACHE_H__


#include <nx\common\result.h>

#include <string>

namespace NX {

    class RmCacheBasic
    {
    public:
        RmCacheBasic();
        virtual ~RmCacheBasic();

    protected:
        Result Read(const std::wstring& file);
        Result Write(const std::wstring& file);
    };

    class RmCacheProfile : public RmCacheBasic
    {
    public:
        RmCacheProfile();
        virtual ~RmCacheProfile();

    private:
    };

    class RmCacheRepository : public RmCacheBasic
    {
    public:
        RmCacheRepository();
        virtual ~RmCacheRepository();

    private:
    };

    class RmCacheManager
    {
    public:
        RmCacheManager();
        ~RmCacheManager();

    private:
    };
}


#endif