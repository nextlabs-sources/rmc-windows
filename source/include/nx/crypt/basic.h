

#ifndef __NX_CRYPT_BASIC_H__
#define __NX_CRYPT_BASIC_H__

#include <Bcrypt.h>

#include <nx\common\result.h>

namespace NX {

    namespace crypt {

        class Key
        {
        public:
            Key();
            virtual ~Key();

            virtual Result Generate(ULONG bitslength) = 0;
            virtual Result Import(const UCHAR* key, ULONG size) = 0;
            virtual Result Export(PUCHAR key, _Inout_ PULONG size) = 0;

            inline operator BCRYPT_KEY_HANDLE() const { return _h; }
            inline ULONG GetBitsLength() const { return _bitslen; }
            inline bool Empty() const { return (NULL == _h); }

            void Close();
            Result QueryBitsLength();
            
        protected:
            // Copy/Move is not allowed
            Key(const Key& rhs) {}
            Key(Key&& rhs) {}


        protected:
            BCRYPT_KEY_HANDLE   _h;
            ULONG               _bitslen;
        };
    }

}   // NX

#endif
