

#ifndef __NX_CRYPT_CERT_H__
#define __NX_CRYPT_CERT_H__

#include <nx\common\result.h>
#include <nx\crypt\basic.h>

#include <string>
#include <vector>

#include <Windows.h>
#include <Wincrypt.h>

namespace NX {

    namespace crypt {

        class CertContext;

        class CertInfo
        {
        public:
            CertInfo();
            CertInfo(const CertInfo& info);
            virtual ~CertInfo();

            CertInfo& operator = (const CertInfo& info);

            inline const std::wstring& GetSubject() const { return _subjectName; }
            inline const std::wstring& GetIssuer() const { return _issuerName; }
            inline const std::wstring& GetFriendlyName() const { return _friendlyName; }
            inline const std::wstring& GetSignAlgorithm() const { return _signAlgorithm; }
            inline const std::wstring& GetHashAlgorithm() const { return _hashAlgorithm; }
            inline const std::vector<UCHAR>& GetSerialNumber() const { return _serialNumber; }
            inline const std::vector<UCHAR>& GetThumbprint() const { return _thumbPrint; }
            inline const std::vector<UCHAR>& GetSignatureHash() const { return _signatureHash; }
            inline const std::vector<UCHAR>& GetKeyBlob() const { return _keyBlob; }
            inline const FILETIME& GetDateStamp() const { return _dateStamp; }
            inline const FILETIME& GetValidFrom() const { return _validFrom; }
            inline const FILETIME& GetValidThrough() const { return _validThrough; }
            inline ULONG GetKeySpec() const { return _keySpec; }
            inline ULONG GetKeyBitsLength() const { return _keyBitsLength; }
            inline bool IsSelfSigned() const { return _selfSigned; }
            
        private:
            std::wstring        _subjectName;
            std::wstring        _issuerName;
            std::wstring        _friendlyName;
            std::wstring        _signAlgorithm;
            std::wstring        _hashAlgorithm;
            std::vector<UCHAR>  _serialNumber;
            std::vector<UCHAR>  _thumbPrint;
            std::vector<UCHAR>  _signatureHash;
            std::vector<UCHAR>  _keyBlob;
            FILETIME            _dateStamp;
            FILETIME            _validFrom;
            FILETIME            _validThrough;
            ULONG               _keySpec;
            ULONG               _keyBitsLength;
            bool                _selfSigned;

            friend class CertContext;
        };

        class CertContext
        {
        public:
            CertContext();
            CertContext(PCCERT_CONTEXT p);
            virtual ~CertContext();


            inline operator PCCERT_CONTEXT() { return _p; }
            inline PCCERT_CONTEXT Attach(PCCERT_CONTEXT p) { _p = p;  return _p; }
            inline PCCERT_CONTEXT Detach() { PCCERT_CONTEXT p = _p;  _p = nullptr;  return p; }
            inline bool Empty() const { return (nullptr == _p); }

            Result Create(const std::wstring& x500_name, unsigned long key_spec, bool strong, bool machineKeySet, _In_opt_ CertContext* issuer, const SYSTEMTIME& expire_date);
            Result Create(const unsigned char* pb, unsigned long cb);
            Result Create(const std::wstring& file);
            Result CreateFromEmbeddedSignature(const std::wstring& host_file);
            Result Export(_In_ const std::wstring& file, _In_ bool base64_format);
            void Clear();

            Result Duplicate(CertContext* pcc);

            CertInfo GetCertInfo();
            Result GetKeyBlob(std::vector<UCHAR>& blob);

        private:
            // Get Properties
            std::wstring GetSubjectName();
            std::wstring GetIssuerName();
            std::wstring GetFriendlyName();
            void GetAlgorithms(std::wstring& sign_algorithm, std::wstring& hash_algorithm);
            std::vector<UCHAR> GetSerialNumber();
            std::vector<UCHAR> GetThumbprint();
            std::vector<UCHAR> GetSignatureHash();
            void GetDateStamp(FILETIME& date_stamp);
            void GetValidTime(FILETIME& valid_from, FILETIME& valid_through);
            ULONG GetKeySpec();
            ULONG GetKeyBitsLength();
            bool IsSelfSigned();
            bool HasPrivateKey();

            Result CertContext::GetPrivateKeyBlob(std::vector<UCHAR>& blob);
            Result CertContext::GetPublicKeyBlob(std::vector<UCHAR>& blob);

            std::vector<UCHAR> GetProperty(ULONG id);
            std::vector<UCHAR> DecodeObject(ULONG encoding_type, const CHAR* struc_type, const UCHAR* data, ULONG data_size, ULONG flags);

        private:
            Result GetCertBlob(const std::wstring& file, std::vector<UCHAR>& blob);
            Result GetCertBinaryBlob(const std::wstring& file, std::vector<UCHAR>& blob);

        private:
            // No copy/move allowed
            CertContext(const CertContext& cc) {}
            CertContext& operator = (const CertContext& other) { return *this; }

        private:
            PCCERT_CONTEXT  _p;
        };

        typedef void (WINAPI* ENUMCERTPROC)(CertContext& cc, PVOID context, BOOL* stop);

        class CertStore
        {
        public:
            virtual ~CertStore();

            inline operator HCERTSTORE() { return _h; }
            inline bool Opened() const { return (NULL != _h); }
            inline HCERTSTORE Attach(HCERTSTORE h) { _h = h; return _h; }
            inline HCERTSTORE Detach() { HCERTSTORE dh = _h; _h = NULL; return dh; }

            virtual void Close();

            Result EnumCertificates(ENUMCERTPROC EnumProc, PVOID context);
            Result AddCertificate(PCCERT_CONTEXT cc, DWORD addDisposition, _Out_opt_ CertContext* pcc);

        protected:
            CertStore();
            CertStore(HCERTSTORE h);

        private:
            // No copy is allowed
            CertStore(const CertStore& other) {}
            CertStore& operator = (const CertStore& other) { return *this; }

        private:
            HCERTSTORE  _h;
        };

        class CertSystemStore : public CertStore
        {
        public:
            CertSystemStore();
            virtual ~CertSystemStore();

            Result OpenPersonalStore(bool machineStore = false);
            Result OpenTrustRootStore(bool machineStore = false);
            Result OpenInterMediateStore(bool machineStore = false);
            Result OpenTrustPublisherStore(bool machineStore = false);
            Result OpenTrustPeopleStore(bool machineStore = false);
            Result OpenOtherPeopleStore(bool machineStore = false);
            Result OpenThirdPartyRootStore(bool machineStore = false);
            Result OpenRevokedStore(bool machineStore = false);

        protected:
            Result Open(const std::wstring& name, bool machine_store);
        };

        class CertMemoryStore : public CertStore
        {
        public:
            CertMemoryStore();
            virtual ~CertMemoryStore();
        };

        class CertPcks12FileStore : public CertStore
        {
        public:
            CertPcks12FileStore();
            virtual ~CertPcks12FileStore();

            Result OpenPfxCert(const std::wstring& file, const std::wstring& password, bool exportable = false);

        private:
            Result ReadCertContent(const std::wstring& file, std::vector<UCHAR>& blob);
        };

    }

}   // NX

#endif
