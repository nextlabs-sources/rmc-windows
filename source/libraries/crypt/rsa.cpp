
#include "stdafx.h"

#include <nx\crypt\rsa.h>
#include <nx\crypt\sha.h>
#include <Wincrypt.h>

#include "provider.h"


using namespace NX;
using namespace NX::crypt;


RsaKey::RsaKey() : Key(), _type(RSAEMPTY)
{
}

RsaKey::~RsaKey()
{
}

Result RsaKey::Generate(ULONG bitslength)
{
    if (!Empty()) {
        Close();
    }
    Provider* prov = GetProvider(PROV_RSA);
    if (NULL == prov) {
        return RESULT(ERROR_INVALID_HANDLE);
    }
    if (!prov->Opened()) {
        return RESULT(ERROR_INVALID_HANDLE);
    }

    if (bitslength != 1024 && bitslength != 2048) {
        return RESULT(ERROR_INVALID_PARAMETER);
    }

    LONG status = BCryptGenerateKeyPair(*prov, &_h, _bitslen, 0);
    if (0 != status) {
        _h = NULL;
        return RESULT2(status, "BCryptGenerateKeyPair failed");
    }

    _bitslen = bitslength;
    _type = RSAFULLPRIVATE;
    return RESULT(0);
}

Result RsaKey::Import(const UCHAR* key, ULONG size)
{
    if (!Empty()) {
        Close();
    }

    if (IsLegacyKeyBlob(key, size)) {
        return ImportBcryptKey(key, size);
    }
    else {
        return ImportLegacyKey(key, size);
    }
}

Result RsaKey::Export(PUCHAR key_blob, _Inout_ PULONG size)
{
    Provider* prov = GetProvider(PROV_RSA);
    if (NULL == prov) {
        return RESULT(ERROR_INVALID_HANDLE);
    }
    if (!prov->Opened()) {
        return RESULT(ERROR_INVALID_HANDLE);
    }
    if (Empty()) {
        return RESULT(ERROR_INVALID_HANDLE);
    }

    const ULONG out_size = *size;

    LONG status = BCryptExportKey(_h, NULL, BCRYPT_RSAFULLPRIVATE_BLOB, key_blob, out_size, size, 0);
    return RESULT(status);
}

Result RsaKey::ExportPublicKey(PUCHAR key_blob, _Inout_ PULONG size)
{
    Provider* prov = GetProvider(PROV_RSA);
    if (NULL == prov) {
        return RESULT(ERROR_INVALID_HANDLE);
    }
    if (!prov->Opened()) {
        return RESULT(ERROR_INVALID_HANDLE);
    }
    if (Empty()) {
        return RESULT(ERROR_INVALID_HANDLE);
    }

    const ULONG out_size = *size;

    LONG status = BCryptExportKey(_h, NULL, BCRYPT_RSAPUBLIC_BLOB, key_blob, out_size, size, 0);
    return RESULT(status);
}

bool RsaKey::HasPrivateKey() const
{
    return (RSAPRIVATE == _type || RSAFULLPRIVATE == _type);
}

bool RsaKey::IsLegacyKeyBlob(const UCHAR* key_blob, ULONG size)
{
    if (size < (sizeof(PUBLICKEYSTRUC) + sizeof(RSAPUBKEY))) {
        return false;
    }

    const PUBLICKEYSTRUC* pubkey_struc = (const PUBLICKEYSTRUC*)key_blob;
    const RSAPUBKEY* rsa_pubkey = (const RSAPUBKEY*)(key_blob + sizeof(PUBLICKEYSTRUC));

    if (pubkey_struc->bType != PUBLICKEYBLOB && pubkey_struc->bType != PRIVATEKEYBLOB) {
        return false;
    }
    if (pubkey_struc->aiKeyAlg != CALG_RSA_KEYX && pubkey_struc->aiKeyAlg != CALG_RSA_SIGN) {
        return false;
    }
    if (rsa_pubkey->magic != 0x31415352 /*RSA1*/ && rsa_pubkey->magic != 0x32415352 /*RSA2*/) {
        return false;
    }

    return true;
}

Result RsaKey::ImportLegacyKey(const UCHAR* key_blob, ULONG size)
{
    Provider* prov = GetProvider(PROV_RSA);
    if (NULL == prov) {
        return RESULT(ERROR_INVALID_HANDLE);
    }
    if (!prov->Opened()) {
        return RESULT(ERROR_INVALID_HANDLE);
    }

    const PUBLICKEYSTRUC* pubkey_struc = (const PUBLICKEYSTRUC*)key_blob;
    const RSAPUBKEY* rsa_pubkey = (const RSAPUBKEY*)(key_blob + sizeof(PUBLICKEYSTRUC));
    LONG status = 0;

    if (pubkey_struc->bType == PRIVATEKEYBLOB && rsa_pubkey->magic == 0x32415352 /*RSA2*/) {
        // Private key is valid
        status = BCryptImportKeyPair(*prov,
                                     NULL,
                                     LEGACY_RSAPRIVATE_BLOB,
                                     &_h,
                                     (PUCHAR)key_blob, (ULONG)size,
                                     0);
        if (0 != status) {
            _h = NULL;
            return RESULT2(status, "BCryptImportKeyPair (legacy private blob) failed");
        }
        QueryBitsLength();
        _type = RSAPRIVATE;
    }
    else {
        // Private key is not valid
        status = BCryptImportKeyPair(*prov,
                                     NULL,
                                     LEGACY_RSAPUBLIC_BLOB,
                                     &_h,
                                     (PUCHAR)key_blob, (ULONG)size,
                                     0);
        if (0 != status) {
            _h = NULL;
            return RESULT2(status, "BCryptImportKeyPair (legacy public blob) failed");
        }
        QueryBitsLength();
        _type = RSAPUBLIC;
    }

    return RESULT(0);
}

Result RsaKey::ImportBcryptKey(const UCHAR* key_blob, ULONG size)
{
    Provider* prov = GetProvider(PROV_RSA);
    if (NULL == prov) {
        return RESULT(ERROR_INVALID_HANDLE);
    }
    if (!prov->Opened()) {
        return RESULT(ERROR_INVALID_HANDLE);
    }

    //const BCRYPT_RSAPRIVATE_BLOB* 
    const BCRYPT_RSAKEY_BLOB* p = (const BCRYPT_RSAKEY_BLOB*)key_blob;
    LONG status = 0;

    if (BCRYPT_RSAPUBLIC_MAGIC == p->Magic) {

        status = BCryptImportKeyPair(*prov,
                                     NULL,
                                     BCRYPT_RSAPUBLIC_BLOB,
                                     &_h,
                                     (PUCHAR)key_blob, (ULONG)size,
                                     0);
        if (0 != status) {
            _h = NULL;
            return RESULT2(status, "BCryptImportKeyPair (public blob) failed");
        }
        QueryBitsLength();
        _type = RSAPUBLIC;
        return RESULT(0);
    }
    else if (BCRYPT_RSAPRIVATE_MAGIC == p->Magic) {

        status = BCryptImportKeyPair(*prov,
                                     NULL,
                                     BCRYPT_RSAPRIVATE_BLOB,
                                     &_h,
                                     (PUCHAR)key_blob, (ULONG)size,
                                     0);
        if (0 != status) {
            _h = NULL;
            return RESULT2(status, "BCryptImportKeyPair (private blob) failed");
        }
        QueryBitsLength();
        _type = RSAPRIVATE;
        return RESULT(0);
    }
    else if (BCRYPT_RSAFULLPRIVATE_MAGIC == p->Magic) {

        status = BCryptImportKeyPair(*prov,
                                     NULL,
                                     BCRYPT_RSAPRIVATE_BLOB,
                                     &_h,
                                     (PUCHAR)key_blob, (ULONG)size,
                                     0);
        if (0 != status) {
            _h = NULL;
            return RESULT2(status, "BCryptImportKeyPair (full private blob) failed");
        }
        QueryBitsLength();
        _type = RSAFULLPRIVATE;
        return RESULT(0);
    }
    else {
        return RESULT(ERROR_INVALID_DATA);
    }
}

Result NX::crypt::RsaEncrypt(const RsaKey& key,
    _In_reads_bytes_(in_size) const unsigned char* in_buf,
    _In_ unsigned long in_size,
    _Out_writes_bytes_opt_(*out_size) unsigned char* out_buf,
    _Inout_ unsigned long* out_size)
{
    LONG status = 0;
    ULONG required_size = 0;

    status = BCryptEncrypt(key, (PUCHAR)in_buf, in_size, NULL, NULL, 0, NULL, 0, &required_size, BCRYPT_PAD_PKCS1);
    if (0 == required_size) {
        return RESULT2(status, "BCryptEncrypt fail to query required buffer size");
    }

    if (NULL == out_buf || 0 == *out_size) {
        *out_size = required_size;
        return RESULT(0);
    }

    status = BCryptEncrypt(key, (PUCHAR)in_buf, in_size, NULL, NULL, 0, out_buf, *out_size, out_size, BCRYPT_PAD_PKCS1);
    return RESULT(status);
}

Result NX::crypt::RsaDecrypt(const RsaKey& key,
    _In_reads_bytes_(in_size) const unsigned char* in_buf,
    _In_ unsigned long in_size,
    _Out_writes_bytes_opt_(*out_size) unsigned char* out_buf,
    _Inout_ unsigned long* out_size)
{
    LONG status = 0;
    ULONG required_size = 0;

    status = BCryptDecrypt(key, (PUCHAR)in_buf, in_size, NULL, NULL, 0, NULL, 0, &required_size, BCRYPT_PAD_PKCS1);
    if (0 == required_size) {
        return RESULT2(status, "BCryptDecrypt fail to query required buffer size");
    }

    if (NULL == out_buf || 0 == *out_size) {
        *out_size = required_size;
        return RESULT(0);
    }

    status = BCryptDecrypt(key, (PUCHAR)in_buf, in_size, NULL, NULL, 0, out_buf, *out_size, out_size, BCRYPT_PAD_PKCS1);
    return RESULT(status);
}

Result NX::crypt::RsaSign(const RsaKey& key,
    _In_reads_bytes_(data_size) const unsigned char* data,
    _In_ unsigned long data_size,
    _Out_writes_bytes_opt_(*signature_size) unsigned char* signature,
    _Inout_ unsigned long* signature_size)
{
    Result res = RESULT(0);
    LONG status = 0;
    UCHAR hash_data[32] = { 0 };
    const ULONG hash_data_size = 32;
    BCRYPT_PKCS1_PADDING_INFO pkcs1_padding_info = { BCRYPT_SHA256_ALGORITHM };
    ULONG required_size = 0;

    memset(hash_data, 0, 32);
    res = NX::crypt::CreateSha256(data, data_size, hash_data);
    if (!res) {
        return res;
    }

    status = BCryptSignHash(key, &pkcs1_padding_info, hash_data, 32, NULL, 0, &required_size, BCRYPT_PAD_PKCS1);
    if (0 == required_size) {
        return RESULT2(status, "BCryptSignHash fail to query required buffer size");
    }

    if (NULL == signature || 0 == *signature_size) {
        *signature_size = required_size;
        return RESULT(0);
    }

    status = BCryptSignHash(key, &pkcs1_padding_info, hash_data, 32, signature, *signature_size, signature_size, BCRYPT_PAD_PKCS1);
    return RESULT(status);
}

Result NX::crypt::RsaVerify(const RsaKey& key,
    _In_reads_bytes_(data_size) const unsigned char* data,
    _In_ unsigned long data_size,
    _In_reads_bytes_(signature_size) const unsigned char* signature,
    _In_ unsigned long signature_size)
{
    Result res = RESULT(0);
    LONG status = 0;
    UCHAR hash_data[32] = { 0 };
    const ULONG hash_data_size = 32;
    BCRYPT_PKCS1_PADDING_INFO pkcs1_padding_info = { BCRYPT_SHA256_ALGORITHM };

    res = NX::crypt::CreateSha256(data, data_size, hash_data);
    if (!res) {
        return res;
    }

    status = BCryptVerifySignature(key, &pkcs1_padding_info, hash_data, 32, (PUCHAR)signature, signature_size, BCRYPT_PAD_PKCS1);
    return RESULT(status);
}