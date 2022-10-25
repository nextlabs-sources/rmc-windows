

#ifndef __NX_CRYPT_MD5_H__
#define __NX_CRYPT_MD5_H__

#include <nx\common\result.h>

namespace NX {

    namespace crypt {


        Result CreateMd5(_In_reads_bytes_(data_size) const unsigned char* data,
            _In_ unsigned long data_size,
            _Out_writes_bytes_(16) unsigned char* out_buf);

        Result CreateHmacMd5(_In_reads_bytes_(data_size) const unsigned char* data,
            _In_ unsigned long data_size,
            _In_reads_bytes_(key_size) const unsigned char* key,
            _In_ unsigned long key_size,
            _Out_writes_bytes_(16) unsigned char* out_buf);

    }

}   // NX

#endif
