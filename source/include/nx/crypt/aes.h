

#ifndef __NX_CRYPT_AES_H__
#define __NX_CRYPT_AES_H__

#include <nx\common\result.h>
#include <nx\crypt\basic.h>

namespace NX {

    namespace crypt {

        class AesKey : public Key
        {
        public:
            AesKey();
            virtual ~AesKey();

			static Result GenerateRandom(PUCHAR data, ULONG size);

            virtual Result Generate(ULONG bitslength);
            virtual Result Import(const UCHAR* key, ULONG size);
            virtual Result Export(PUCHAR key, _Inout_ PULONG size);
        };

        Result AesEncrypt(_In_ const AesKey& key,
            _In_reads_bytes_(in_size) const unsigned char* in_buf,
            _In_ unsigned long in_size,
            _Out_writes_bytes_opt_(*out_size) unsigned char* out_buf,
            _Inout_ unsigned long* out_size,
            _In_reads_bytes_opt_(16) const unsigned char* ivec);

        Result AesDecrypt(_In_ const AesKey& key,
            _In_reads_bytes_(in_size) const unsigned char* in_buf,
            _In_ unsigned long in_size,
            _Out_writes_bytes_opt_(*out_size) unsigned char* out_buf,
            _Inout_ unsigned long* out_size,
            _In_reads_bytes_opt_(16) const unsigned char* ivec);

        void AesGenerateIvec(_In_reads_bytes_opt_(16) const UCHAR* seed,
            unsigned __int64 offset,
            _Out_writes_bytes_(16) PUCHAR ivec);

        Result AesBlockEncrypt(_In_ const AesKey& key,
            _In_reads_bytes_(in_size) const unsigned char* in_buf,
            _In_ unsigned long in_size,
            _Out_writes_bytes_opt_(*out_size) unsigned char* out_buf,
            _Inout_ unsigned long* out_size,
            _In_reads_bytes_opt_(16) const unsigned char* ivec_seed,
            _In_ const unsigned __int64 block_offset = 0,
            _In_ const unsigned long cipher_block_size = 512);

        Result AesBlockDecrypt(_In_ const AesKey& key,
            _In_reads_bytes_(in_size) const unsigned char* in_buf,
            _In_ unsigned long in_size,
            _Out_writes_bytes_opt_(*out_size) unsigned char* out_buf,
            _Inout_ unsigned long* out_size,
            _In_reads_bytes_opt_(16) const unsigned char* ivec_seed,
            _In_ const unsigned __int64 block_offset = 0,
            _In_ const unsigned long cipher_block_size = 512);

    }

}   // NX

#endif
