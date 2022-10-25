

#ifndef __NX_CRYPT_RSA_H__
#define __NX_CRYPT_RSA_H__

#include <nx\common\result.h>
#include <nx\crypt\basic.h>


namespace NX {

    namespace crypt {

        class RsaKey : public Key
        {
        public:
            RsaKey();
            virtual ~RsaKey();

            virtual Result Generate(ULONG bitslength);
            virtual Result Import(const UCHAR* key_blob, ULONG size);
            virtual Result Export(PUCHAR key_blob, _Inout_ PULONG size);
            virtual Result ExportPublicKey(PUCHAR key_blob, _Inout_ PULONG size);

            bool HasPrivateKey() const;

        protected:
            typedef enum RSAKEYTYPE {
                RSAEMPTY = 0,
                RSAPUBLIC,
                RSAPRIVATE,
                RSAFULLPRIVATE
            } RSAKEYTYPE;
            bool IsLegacyKeyBlob(const UCHAR* key_blob, ULONG size);
            Result ImportBcryptKey(const UCHAR* key_blob, ULONG size);
            Result ImportLegacyKey(const UCHAR* key_blob, ULONG size);

        private:
            RSAKEYTYPE  _type;
        };

        Result RsaEncrypt(const RsaKey& key,
            _In_reads_bytes_(in_size) const unsigned char* in_buf,
            _In_ unsigned long in_size,
            _Out_writes_bytes_opt_(*out_size) unsigned char* out_buf,
            _Inout_ unsigned long* out_size);

        Result RsaDecrypt(const RsaKey& key,
            _In_reads_bytes_(in_size) const unsigned char* in_buf,
            _In_ unsigned long in_size,
            _Out_writes_bytes_opt_(*out_size) unsigned char* out_buf,
            _Inout_ unsigned long* out_size);


        Result RsaSign(const RsaKey& key,
            _In_reads_bytes_(data_size) const unsigned char* data,
            _In_ unsigned long data_size,
            _Out_writes_bytes_opt_(*signature_size) unsigned char* signature,
            _Inout_ unsigned long* signature_size);

        Result RsaVerify(const RsaKey& key,
            _In_reads_bytes_(data_size) const unsigned char* data,
            _In_ unsigned long data_size,
            _In_reads_bytes_(signature_size) const unsigned char* signature,
            _In_ unsigned long signature_size);

    }

}   // NX

#endif
