

#ifndef __NX_CRYPT_DH_H__
#define __NX_CRYPT_DH_H__

#include <nx\common\result.h>
#include <nx\crypt\basic.h>

#include <vector>

namespace NX {

    namespace crypt {

        class DiffieHellmanKey;

        class DhKeyBlob
        {
        public:
            DhKeyBlob();
            DhKeyBlob(_In_ ULONG bitslength,
                _In_reads_bytes_(bitslength / 8) const UCHAR* p,
                _In_reads_bytes_(bitslength / 8) const UCHAR* g,
                _In_reads_bytes_(bitslength / 8) const UCHAR* y,
                _In_reads_bytes_opt_(bitslength / 8) const UCHAR* x);
            DhKeyBlob(const DhKeyBlob& rhs);
            ~DhKeyBlob();

            inline bool Empty() const { return _data.empty(); }
            inline void Clear() { _data.clear(); }
            inline ULONG GetBlobSize() const { return (ULONG)_data.size(); }
            inline ULONG GetKeyBytesLength() const { return Empty() ? 0 : ((const BCRYPT_DH_KEY_BLOB*)_data.data())->cbKey; }
            inline const BCRYPT_DH_KEY_BLOB* GetBlobHeader() const { return Empty() ? NULL : ((const BCRYPT_DH_KEY_BLOB*)_data.data()); }

            DhKeyBlob& operator = (const DhKeyBlob& rhs);

            bool HasPrivateKey() const;
            ULONG GetKeyBitsLength() const;
            const UCHAR* GetP() const;
            const UCHAR* GetG() const;
            const UCHAR* GetX() const;
            const UCHAR* GetY() const;

            Result CreateX509EncodedPublicKey(std::vector<UCHAR>& encoded_pubkey);
            Result ImportX509EncodedPublicKey(const UCHAR* encoded_pubkey, ULONG size);

        protected:
            Result SetKey(_In_ ULONG bitslength,
                _In_reads_bytes_(bitslength / 8) const UCHAR* p,
                _In_reads_bytes_(bitslength / 8) const UCHAR* g,
                _In_reads_bytes_(bitslength / 8) const UCHAR* y,
                _In_reads_bytes_opt_(bitslength / 8) const UCHAR* x);

        private:
            std::vector<UCHAR> _data;
            friend class DiffieHellmanKey;
        };

        class DiffieHellmanKey : public Key
        {
        public:
            DiffieHellmanKey();
            DiffieHellmanKey(const DhKeyBlob& blob);
            virtual ~DiffieHellmanKey();

            virtual Result Generate(_In_reads_bytes_(bitslength / 8) const UCHAR* p, _In_reads_bytes_(bitslength / 8)const UCHAR* g, ULONG bitslength);
            virtual Result Import(const UCHAR* key, ULONG size);
            virtual Result Export(PUCHAR key, _Inout_ PULONG size);;
            Result Export(DhKeyBlob& blob);

            inline bool HasPrivateKey() const { return _full_key; }

        private:
            // P & G must present
            virtual Result Generate(ULONG bitslength);

        private:
            bool _full_key;
        };

    }

}   // NX

#endif
