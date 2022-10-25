

#include "stdafx.h"

#include <nx\rmc\keym.h>
#include <wincrypt.h>

#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>
#include <nx\crypt\sha.h>
#include <nx\crypt\rsa.h>
#include <nx\crypt\cert.h>
#include <nx\crypt\dh.h>

using namespace NX;


const std::wstring client_x509_name    = L"C=US;ST=California;O=NextLabs;OU=Rights Management;CN=SkyDRM Client Identifier";
const std::wstring client_subject_name = L"SkyDRM Client Identifier";


RmClientId::RmClientId()
{
}

RmClientId::RmClientId(const RmClientId& rhs) : _id(rhs._id), _key(rhs._key)
{
}

RmClientId::~RmClientId()
{
}

RmClientId& RmClientId::operator = (const RmClientId& rhs)
{
    if (this != &rhs) {
        _id = rhs._id;
        _key = rhs._key;
    }
    return *this;
}

Result RmClientId::Create(bool machineStore)
{
    crypt::CertContext cc;
    Result res = CreateCertificate(&cc, machineStore);
    if (!res) {
        return res;
    }

    assert(!cc.Empty());

    // Found
    const crypt::CertInfo& certInf = cc.GetCertInfo();
    res = GenerateIdAndKey(certInf.GetSerialNumber(), certInf.GetThumbprint());
    if (!res) {
        return res;
    }

    return RESULT(0);
}

Result RmClientId::Load(bool machineStore)
{
    crypt::CertContext cc;
    Result res = FindCertificate(&cc, machineStore);
    if (!res) {
        return res;
    }

    assert(!cc.Empty());

    // Found
    const crypt::CertInfo& certInf = cc.GetCertInfo();
    res = GenerateIdAndKey(certInf.GetSerialNumber(), certInf.GetThumbprint());
    if (!res) {
        return res;
    }

    return RESULT(0);
}

Result RmClientId::CreateCertificate(crypt::CertContext* pcc, bool machineStore)
{
    NX::crypt::CertContext cc;
    SYSTEMTIME expire_time = { 0 };
    GetSystemTime(&expire_time);
    expire_time.wYear += 20;    // Expire after 20 years
    
    Result res = cc.Create(client_x509_name, AT_SIGNATURE, true, machineStore, NULL, expire_time);
    if (!res) {
        return res;
    }

    NX::crypt::CertSystemStore css;
    res = css.OpenPersonalStore(machineStore);
    if (!res) {
        return res;
    }

    res = css.AddCertificate(cc, CERT_STORE_ADD_NEW, pcc);
    if (!res) {
        return res;
    }

    return RESULT(0);
}

static void WINAPI FindProc(crypt::CertContext& cc, PVOID context, BOOL* stop)
{
    crypt::CertInfo certInf = cc.GetCertInfo();
    if (0 == _wcsicmp(client_subject_name.c_str(), certInf.GetSubject().c_str())) {
        *stop = true;
        crypt::CertContext* pcc = (crypt::CertContext*)context;
        cc.Duplicate(pcc);
    }
}

Result RmClientId::FindCertificate(crypt::CertContext* pcc, bool machineStore)
{
    NX::crypt::CertSystemStore css;
	Result res = css.OpenPersonalStore(machineStore);
    if (!res) {
        return res;
    }

    css.EnumCertificates(FindProc, pcc);
    if (pcc->Empty()) {
        return RESULT(ERROR_NOT_FOUND);
    }

    return RESULT(0);
}

Result RmClientId::GenerateIdAndKey(const std::vector<UCHAR>& key1, const std::vector<UCHAR>& key2)
{
    static const std::string sSaltSeed("nXrMCcLIENTseeD");
    UCHAR clientId[16] = { 0 };
    UCHAR clientSalt[20] = { 0 };

    Result res = crypt::CreateHmacMd5(key1.data(), (ULONG)key1.size(), key2.data(), (ULONG)key2.size(), clientId);
    if (!res) {
        return res;
    }
    
    res = crypt::CreateHmacSha1(key1.data(), (ULONG)key1.size(), (const UCHAR*)sSaltSeed.c_str(), (ULONG)sSaltSeed.length(), clientSalt);
    if (!res) {
        return res;
    }
    
    _key.resize(32, 0);
    res = crypt::CreateHmacSha256(key2.data(), (ULONG)key2.size(), clientSalt, 20, _key.data());
    if (!res) {
        _key.clear();
        return res;
    }

    _id = NX::bintohs<wchar_t>(clientId, 16);
    return RESULT(0);
}



const UCHAR NX::DH_P_2048[256] = {
	0xD3, 0x10, 0x12, 0x5B, 0x29, 0x4D, 0xBD, 0x85, 0x68, 0x14, 0xDF, 0xD4, 0xBA, 0xB4, 0xDC, 0x76,
	0x7D, 0xF6, 0xA9, 0x99, 0xC9, 0xED, 0xFA, 0x8F, 0x8D, 0x7B, 0x12, 0x55, 0x1F, 0x8D, 0x71, 0xEF,
	0x60, 0x32, 0x35, 0x74, 0x05, 0xC7, 0xF1, 0x1E, 0xE1, 0x47, 0xDB, 0x03, 0x32, 0x71, 0x6F, 0xC8,
	0xFD, 0x85, 0xED, 0x02, 0x75, 0x85, 0x26, 0x83, 0x60, 0xD1, 0x6B, 0xD7, 0x61, 0x56, 0x3D, 0x7D,
	0x16, 0x59, 0xD4, 0xD7, 0x3D, 0xAE, 0xD6, 0x17, 0xF3, 0xE4, 0x22, 0x3F, 0x48, 0xBC, 0xEF, 0xA4,
	0x21, 0x86, 0x0C, 0x3F, 0xC4, 0x39, 0x3D, 0x27, 0x54, 0x56, 0x77, 0xB2, 0x24, 0x59, 0xE8, 0x52,
	0xF5, 0x25, 0x4D, 0x3A, 0xC5, 0x8C, 0x0D, 0x63, 0xDD, 0x79, 0xDE, 0x2D, 0x8D, 0x86, 0x8C, 0xD9,
	0x40, 0xDE, 0xCF, 0x5A, 0x27, 0x46, 0x05, 0xDB, 0x0E, 0xEE, 0x76, 0x20, 0x20, 0xC3, 0x9D, 0x0F,
	0x64, 0x86, 0x60, 0x65, 0x80, 0xEA, 0xAC, 0xCE, 0x16, 0xFB, 0x70, 0xFB, 0x7C, 0x75, 0x9E, 0xA9,
	0xAA, 0xBA, 0xB4, 0xDC, 0xBF, 0x94, 0x18, 0x91, 0xB0, 0xCE, 0x94, 0xEC, 0x4D, 0x3D, 0x59, 0x54,
	0x21, 0x7C, 0x6E, 0x84, 0xA9, 0x27, 0x4F, 0x1A, 0xB8, 0x60, 0x73, 0xBD, 0xF9, 0xDC, 0x85, 0x1E,
	0x56, 0x3B, 0x90, 0x45, 0x5B, 0x83, 0x97, 0xDA, 0xE3, 0xA1, 0xB9, 0x98, 0x60, 0x7B, 0xB7, 0x69,
	0x9C, 0xEA, 0x08, 0x05, 0xA7, 0xFF, 0x01, 0x3E, 0xF4, 0x4F, 0xDE, 0x7A, 0xF8, 0x30, 0xF1, 0xFD,
	0x05, 0x1F, 0xFA, 0xEC, 0x53, 0x9C, 0xE4, 0x45, 0x2D, 0x82, 0x29, 0x09, 0x8A, 0xE3, 0xEE, 0x20,
	0x08, 0xAB, 0x9D, 0xB7, 0xB2, 0xC9, 0x48, 0x31, 0x2C, 0xBC, 0x01, 0x37, 0xC0, 0x82, 0xD6, 0x67,
	0x26, 0x18, 0xE1, 0xBF, 0xE5, 0xD5, 0x00, 0x6E, 0x81, 0x0D, 0xC7, 0xAA, 0x7F, 0x1E, 0x6E, 0xE3
};

const UCHAR NX::DH_G_2048[256] = {
	0x64, 0xAC, 0xEB, 0xA5, 0xF7, 0xBC, 0x80, 0x3E, 0xF2, 0x97, 0x31, 0xC9, 0xC6, 0xAE, 0x00, 0x9B,
	0x86, 0xFC, 0x52, 0x01, 0xF8, 0x1B, 0xC2, 0xB8, 0xF8, 0x48, 0x90, 0xFC, 0xF7, 0x1C, 0xAD, 0x51,
	0xC1, 0x42, 0x9F, 0xD2, 0x61, 0xA2, 0xA7, 0x15, 0xC8, 0x94, 0x61, 0x54, 0xE0, 0xE4, 0xE2, 0x8E,
	0xF6, 0xB2, 0xD4, 0x93, 0xCC, 0x17, 0x39, 0xF5, 0x65, 0x9E, 0x9F, 0x14, 0xDD, 0x14, 0x03, 0x7F,
	0x5F, 0xE7, 0x2B, 0x3B, 0xA4, 0xD9, 0xBC, 0xB3, 0xB9, 0x5B, 0x84, 0x17, 0xBD, 0xA4, 0x8F, 0x11,
	0x8E, 0x61, 0xC8, 0x21, 0x4C, 0xF8, 0xD5, 0x58, 0xDA, 0x67, 0x74, 0xF0, 0x8B, 0x58, 0xD9, 0x7B,
	0x2C, 0xCE, 0x20, 0xF5, 0xAA, 0x2F, 0x8E, 0x95, 0x39, 0xC0, 0x14, 0xE7, 0x76, 0x1E, 0x4E, 0x63,
	0x36, 0xCF, 0xFC, 0x35, 0x12, 0x7D, 0xDD, 0x52, 0x72, 0x06, 0x76, 0x6A, 0xE7, 0x20, 0x45, 0xC1,
	0x1B, 0x0F, 0xF4, 0xDA, 0x76, 0x17, 0x25, 0x23, 0x71, 0x3B, 0x31, 0xC9, 0xF1, 0x8A, 0xBA, 0xBA,
	0x92, 0x61, 0x2B, 0xDE, 0x10, 0x51, 0x41, 0xF0, 0x4D, 0xB5, 0xDA, 0x3C, 0x39, 0xCD, 0xE5, 0xC6,
	0x87, 0x7B, 0x7F, 0x8C, 0xD9, 0x69, 0x49, 0xFC, 0xC8, 0x76, 0xE2, 0xC1, 0x22, 0x4F, 0xB9, 0x18,
	0x8D, 0x71, 0x4F, 0xDD, 0x6C, 0xB8, 0x06, 0x82, 0xF8, 0x96, 0x78, 0x33, 0xAD, 0x4B, 0x51, 0x35,
	0x4A, 0x8D, 0x58, 0x59, 0x8E, 0x6B, 0x2D, 0xEF, 0x45, 0x71, 0xA5, 0x97, 0xAD, 0x39, 0xBD, 0x31,
	0x77, 0xD5, 0x4B, 0x24, 0xCA, 0x51, 0x8E, 0xDA, 0x99, 0x6E, 0xED, 0xBA, 0x8A, 0x31, 0xD5, 0x87,
	0x6E, 0xFE, 0xD8, 0xAA, 0x44, 0x02, 0x3C, 0xC9, 0xF1, 0x3D, 0x86, 0xDC, 0xB4, 0xDD, 0xFC, 0xF3,
	0x89, 0xC7, 0xA1, 0x43, 0x50, 0x82, 0xEF, 0x69, 0x70, 0x36, 0x03, 0x63, 0x83, 0x25, 0x95, 0x4E
};

const UCHAR NX::DH_P_1024[128] = {
	0xE8, 0xEA, 0xFD, 0xE6, 0xF9, 0xE0, 0x15, 0x44, 0xA9, 0x7A, 0x8C, 0x41, 0x26, 0x2C, 0x64, 0xE6,
	0x87, 0x90, 0x8E, 0x78, 0xF7, 0xF5, 0xA5, 0x89, 0xDC, 0x95, 0xB5, 0x6A, 0x01, 0x5C, 0xC3, 0x42,
	0xFA, 0x86, 0x95, 0xF7, 0x5F, 0x00, 0x5D, 0x86, 0xE8, 0x6B, 0xB2, 0x5E, 0x82, 0x23, 0x81, 0xD1,
	0x1B, 0xE3, 0xFF, 0x43, 0xDD, 0xE0, 0x27, 0x47, 0x00, 0x23, 0x42, 0x32, 0x65, 0x8B, 0x5E, 0xD5,
	0xD5, 0xFF, 0xE1, 0xA6, 0xEB, 0xAE, 0xB5, 0x26, 0x7E, 0x04, 0xC3, 0x67, 0x09, 0x65, 0xDF, 0xD9,
	0xE1, 0x69, 0xCE, 0x18, 0xD9, 0xC2, 0xF5, 0xEE, 0x9C, 0x0F, 0x2B, 0xD6, 0xF4, 0xD3, 0x9D, 0xC6,
	0xFE, 0x86, 0x56, 0x09, 0x57, 0x68, 0xAC, 0xEB, 0x05, 0x23, 0x6D, 0x86, 0xC7, 0x7C, 0xD9, 0xC2,
	0x3D, 0x8B, 0x29, 0x1C, 0x38, 0xA7, 0x81, 0x6A, 0x9E, 0xD8, 0x19, 0x7A, 0x82, 0x57, 0xE4, 0x47
};


const UCHAR NX::DH_G_1024[128] = {
	0xC5, 0xBA, 0x27, 0xAA, 0x74, 0xCB, 0xD8, 0x89, 0xCB, 0x8D, 0xB1, 0x9D, 0x7D, 0xC5, 0x69, 0xC5,
	0xF1, 0x55, 0xEA, 0x66, 0x75, 0xE2, 0xF1, 0x2A, 0x15, 0x4B, 0xFF, 0xA8, 0x0E, 0x1F, 0xAA, 0xD8,
	0x42, 0x14, 0x76, 0x2A, 0xA2, 0x10, 0xFD, 0x2D, 0xE5, 0x9D, 0x4D, 0x89, 0x08, 0x9B, 0x2E, 0x57,
	0x29, 0xD3, 0x2C, 0xC2, 0x5A, 0x24, 0xE0, 0x7C, 0x4A, 0x9F, 0xA8, 0x76, 0x45, 0xAE, 0x87, 0x0D,
	0x92, 0xB1, 0x3E, 0x7F, 0x31, 0xE8, 0x26, 0x55, 0x75, 0xB9, 0x80, 0x7A, 0xC1, 0x9B, 0x6F, 0xCC,
	0x92, 0x17, 0x24, 0xAE, 0xC2, 0xFB, 0x8A, 0x87, 0x6E, 0x72, 0x83, 0x5B, 0x3B, 0x23, 0xB5, 0x62,
	0x4C, 0xE1, 0xCA, 0xE7, 0x39, 0x03, 0xCA, 0xCF, 0x95, 0x48, 0x62, 0x3E, 0x5F, 0x5F, 0xF2, 0x7A,
	0x2A, 0xF5, 0xB1, 0x5D, 0xCE, 0x56, 0xB3, 0x08, 0xD8, 0xE0, 0xDC, 0x3C, 0x19, 0xC2, 0x02, 0x07
};


static Result get_y_from_cert_context(PCCERT_CONTEXT p, std::vector<unsigned char>& y)
{
	static const char* szOID_DH_KEY_AGREEMENT = "1.2.840.113549.1.3.1";
	const char* pubkey_algorithm_OID = p->pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId;
	const unsigned long pubkey_size = p->pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData;
	const unsigned char* pubkey_data = p->pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData;

	if (0 != _stricmp(szOID_DH_KEY_AGREEMENT, pubkey_algorithm_OID)) {
		return RESULT(NTE_BAD_ALGID);
	}
	if (pubkey_size < sizeof(DWORD)) {
		return RESULT(NTE_BAD_KEY);
	}

	unsigned long y_size = pubkey_size - 4;
	if (y_size == 129) y_size = 128;    // Server may parse extra byte to indicate sign
	if (y_size == 257) y_size = 256;    // Server may parse extra byte to indicate sign
	if (128 != y_size && 256 != y_size) {
		return RESULT(NTE_BAD_KEY);
	}

	y.resize(y_size, 0);
	memcpy(y.data(), pubkey_data + (pubkey_size - y_size), y_size);
	return RESULT(0);
}

Result NX::GetDHParameterY(const std::wstring& cert, std::vector<UCHAR>& y)
{
	const std::vector<UCHAR>& buf = NX::conv::Base64Decode(cert);
	if (buf.empty())
		return RESULT(ERROR_INVALID_DATA);

	NX::crypt::CertContext cc;
	Result res = cc.Create(buf.data(), (unsigned long)buf.size());
	if (!res)
		return res;

	res = get_y_from_cert_context(cc, y);
	if (!res)
		return res;

	//const NX::crypt::CertInfo& certInf = cc.GetCertInfo();
	//const unsigned char* pbEncodedPubKey = ((PCCERT_CONTEXT)cc)->pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData;
	//const unsigned long  cbEncodedPubKey = ((PCCERT_CONTEXT)cc)->pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData;


	//y.resize(keyblob.GetKeyBytesLength(), 0);
	//memcpy(y.data(), keyblob.GetY(), keyblob.GetKeyBytesLength());
	return RESULT(0);
	
}