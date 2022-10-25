
#ifndef __LIBNXL_FILE_H__
#define __LIBNXL_FILE_H__

#include <nx\common\result.h>
#include <nx\common\macros.h>
#include <nx\crypt\basic.h>
#include <nx\crypt\aes.h>

#include <nx\nxl\nxlfmt.h>

#include <string>
#include <vector>


namespace NX {

	namespace NXL {

		class File;

		class Header
		{
		public:
			Header();
			virtual ~Header();
		};

		class FileToken
		{
		public:
			FileToken();
			FileToken(const std::vector<UCHAR>& id, unsigned int level, const std::vector<UCHAR>& key = std::vector<UCHAR>());
			FileToken(const std::wstring& id, unsigned int ml, const std::wstring key = std::wstring());
			FileToken(const FileToken& rhs);
			virtual ~FileToken();

			FileToken& operator = (const FileToken& rhs);

			void Clear();

			inline bool Empty() const { return _id.empty(); }
			inline bool IsFullToken() const { return (!_id.empty() && 32 == _key.size()); }
			inline const std::vector<UCHAR>& GetId() const { return _id; }
			inline const std::vector<UCHAR>& GetKey() const { return _key; }
			inline void SetKey(_In_reads_bytes_(32) const UCHAR* key)
			{
				_key = std::vector<UCHAR>(key, key + 32);
			}
			inline unsigned int GetLevel() const { return _level; }

		private:
			std::vector<UCHAR>	_id;
			std::vector<UCHAR>	_key;
			unsigned int		_level;
			friend class File;
		};

		class FileSecret
		{
		public:
			FileSecret();
			FileSecret(const std::string& ownerId, const FileToken& token, ULONG keyFlags, const std::vector<UCHAR>& agreement0, const std::vector<UCHAR>& agreement1, ULONG secureMode = 0);
			FileSecret(const FileSecret& rhs);
			virtual ~FileSecret();

			FileSecret& operator = (const FileSecret& rhs);

			void Clear();

			inline ULONG GetKeyFlags() const { return _keyFlags; }
			inline ULONG GetSecureMode() const { return _secureMode; }
			inline const FileToken GetToken() const { return _token; }
			inline const std::string& GetOwnerId() const { return _ownerId; }
			inline std::wstring GetOwnerIdW() const { return std::wstring(_ownerId.begin(), _ownerId.end()); }
			inline const std::vector<UCHAR> GetIvSeed() const { return _ivSeed; }
			inline const std::vector<UCHAR> GetAgreement0() const { return _agreement0; }
			inline const std::vector<UCHAR> GetAgreement1() const { return _agreement1; }

		private:
			FileToken			_token;
			ULONG				_keyFlags;
			ULONG				_secureMode;
			std::string			_ownerId;
			std::vector<UCHAR>	_ivSeed;
			std::vector<UCHAR>	_agreement0;
			std::vector<UCHAR>	_agreement1;
			friend class File;
		};

		class FileMeta
		{
		public:
			typedef std::pair<std::wstring, std::wstring>	FileMetaItem;
			typedef std::vector<FileMetaItem>				FileMetaArray;

			FileMeta();
			FileMeta(const std::string& s);
			FileMeta(const FileMetaArray& ta);
			FileMeta(const FileMeta& rhs);
			virtual ~FileMeta();

			FileMeta& operator = (const FileMeta& rhs);

			void Clear();
			std::wstring Get(const std::wstring& key) const;
			Result Set(const std::wstring& key, const std::wstring& value);
			Result Remove(const std::wstring& key);

			inline bool Empty() const { return _metadata.empty(); }
			inline const FileMetaArray& GetMetadata() const { return _metadata; }

			std::string ToString() const;

		private:
			FileMetaArray _metadata;
			friend class File;
		};

		class FileAdHocPolicy
		{
		public:
			FileAdHocPolicy();
			FileAdHocPolicy(const std::string& s);
			FileAdHocPolicy(const std::wstring& ownerId, const std::vector<std::wstring>& rights, const std::vector<std::wstring>& obligations);
			FileAdHocPolicy(const FileAdHocPolicy& rhs);
			virtual ~FileAdHocPolicy();


			FileAdHocPolicy& operator = (const FileAdHocPolicy& rhs);

			void Clear();

			std::string ToString() const;
			unsigned __int64 RightsToInt64() const;

			inline bool Empty() const { return _ownerId.empty(); }
			inline const std::wstring& GetOwnerId() const { return _ownerId; }
			inline const std::vector<std::wstring>& GetRights() const { return _rights; }
			inline const std::vector<std::wstring>& GetObligations() const { return _obligations; }
			inline __int64 GetIssueTime() const { return _issueTime; }

		protected:

		private:
			std::wstring				_ownerId;
			std::vector<std::wstring>	_rights;
			std::vector<std::wstring>	_obligations;
			__int64						_issueTime;
			friend class File;
		};

		class FileTags
		{
		public:
			typedef std::vector<std::wstring>				FileTagValues;
			typedef std::pair<std::wstring, FileTagValues>	FileTag;
			typedef std::vector<FileTag>					FileTagArray;

			FileTags();
			FileTags(const std::string& s);
			FileTags(const FileTagArray& ta);
			FileTags(const FileTags& rhs);
			virtual ~FileTags();


			FileTags& operator = (const FileTags& rhs);

			void Clear();

			void RemoveKey(const std::wstring& key);
			void RemoveValue(const std::wstring& key, const std::wstring& value);
			void AddValue(const std::wstring& key, const std::wstring& value);
			void SetValues(const std::wstring& key, const FileTagValues& values);
			FileTagValues GetValues(const std::wstring& key) const;

			std::string ToString() const;

			inline bool Empty() const { return _tags.empty(); }
			inline const FileTagArray& GetTags() const { return _tags; }

		private:
			FileTagArray _tags;
			friend class File;
		};

		class FileSection
		{
		public:
			FileSection();
			FileSection(const NXL_SECTION_2* p);
			FileSection(const FileSection& rhs);
			virtual ~FileSection();

			FileSection& operator = (const FileSection& rhs);

			operator NXL_SECTION_2* () { return &_s; }
			operator const NXL_SECTION_2* () const { return &_s; }

			inline const char* GetName() const { return _s.name; }
			inline char* GetName() { return _s.name; }
			inline unsigned long GetFlags() const { return _s.flags; }
			inline unsigned long GetOffset() const { return _s.offset; }
			inline unsigned long GetSize() const { return _s.size; }
			inline unsigned long GetDataSize() const { return _s.data_size; }
			inline unsigned long GetCompressedSize() const { return _s.compressed_size; }
			inline const unsigned char* GetChecksum() const { return _s.checksum; }
			inline unsigned char* GetChecksum() { return _s.checksum; }

			inline bool Empty() const { return (0 == _s.name[0]); }
			inline bool EmptyData() const { return (0 == _s.data_size); }
			inline bool Encrypted() const { return BoolFlagOn(_s.flags, NXL_SECTION_FLAG_ENCRYPTED); }
			inline bool Compressed() const { return BoolFlagOn(_s.flags, NXL_SECTION_FLAG_COMPRESSED); }

		private:
			NXL_SECTION_2	_s;
			friend class File;
		};

		class File
		{
		public:
			virtual ~File();

			static File* Create(const std::wstring& path,
				const std::wstring& origFileName,
				bool overWrite,
				const FileSecret& secret,
				_In_reads_bytes_opt_(32) const UCHAR* recoveryKey,
				_In_opt_ const FileMeta* metadata,
				_In_opt_ const FileAdHocPolicy* policy,
				_In_opt_ const FileTags* tags,
				const std::string& msg,
				Result& result);
			static File* CreateFrom(const std::wstring& source,
				const std::wstring& target,
				bool overWrite,
				const FileSecret& secret,
				_In_reads_bytes_opt_(32) const UCHAR* recoveryKey,
				_In_opt_ const FileMeta* metadata,
				_In_opt_ const FileAdHocPolicy* policy,
				_In_opt_ const FileTags* tags,
				const std::string& msg,
				Result& result);
			static File* Open(const std::wstring& path, bool readOnly, Result& result);

			static Result IsNXLFile(const std::wstring& path, _Out_opt_ NXL_SIGNATURE_LITE* sig);

			Result ResetSecret(NX::NXL::FileSecret& secret, _In_reads_bytes_opt_(32) const UCHAR* recoveryKey = NULL);

			inline bool IsOpened() const { return (INVALID_HANDLE_VALUE != _h); }
			inline bool IsReadOnly() const { return _readOnly; }
			inline bool IsFullToken() const { return _secret.GetToken().IsFullToken(); }
			inline const NXL_SIGNATURE_LITE* GetNXLSignature() const { return (&_signature); }
			inline const FileSecret& GetFileSecret() const { return _secret; }
			inline void SetTokenkey(_In_reads_bytes_(32) const UCHAR* key) { _secret._token.SetKey(key); }
			inline ULONG GetContentOffset() const { return _contentOffset; }

			inline HANDLE GetFileHandle() const { return _h; }

			Result GetContentSize(__int64* size) const;
			Result GetHeaderChecksum(_Out_writes_bytes_(32) PUCHAR checksum) const;

			Result VerifyHeaderChecksum() const;

			Result Unprotect(const std::wstring& dest = std::wstring());

			Result ReadAllSectionRecords(std::vector<FileSection>& sections) const;

			Result ReadMetadata(FileMeta& metadata);
			Result WriteMetadata(const FileMeta& metadata);

			Result ReadAdHocPolicy(FileAdHocPolicy& adhocPolicy);
			Result WriteAdHocPolicy(const FileAdHocPolicy& adhocPolicy);

			Result ReadTags(FileTags& tags);
			Result WriteTags(const FileTags& tags);


		protected:
			File();
			void Close();

			Result CreateNew(const std::wstring& path,
				const std::wstring& origFileName,
				bool overWrite,
				const FileSecret& secret,
				_In_reads_bytes_opt_(32) const UCHAR* recoveryKey,
				_In_opt_ const FileMeta* metadata,
				_In_opt_ const FileAdHocPolicy* policy,
				_In_opt_ const FileTags* tags,
				const std::string& msg = NXL_DEFAULT_MSG);
			Result OpenExisting(const std::wstring& path, bool readOnly);

		protected:
			Result UnlockContentKey(NX::crypt::AesKey& cek) const;

			Result Protect(const std::wstring& source,
				const std::wstring& target,
				const FileSecret& secret,
				_In_reads_bytes_opt_(32) const UCHAR* recoveryKey,
				_In_opt_ const FileMeta* metadata,
				_In_opt_ const FileAdHocPolicy* policy,
				_In_opt_ const FileTags* tags,
				const std::string& msg = NXL_DEFAULT_MSG);


			Result ReadFile(PVOID data, ULONG bytesToRead, _Out_opt_ ULONG* bytesRead) const;
			Result WriteFile(const VOID* data, ULONG bytesToWrite, _Out_opt_ ULONG* bytesWritten);
			Result ReadFileEx(LONGLONG offset, PVOID data, ULONG bytesToRead, _Out_opt_ ULONG* bytesRead) const;
			Result WriteFileEx(LONGLONG offset, const VOID* data, ULONG bytesToWrite, _Out_opt_ ULONG* bytesWritten);

			Result ReadSignature(NXL_SIGNATURE_LITE* sig);
			Result ReadFileSecret(FileSecret& secret);

			Result FindSectionRecord(const std::string& name, FileSection& section, _Out_opt_ ULONG* pId) const;
			Result ReadSectionRecord(const std::string& name, FileSection& section) const;
			Result WriteSectionRecord(_In_ const FileSection& section);

			Result ReadSectionData(const FileSection& section, std::vector<UCHAR>& data) const;
			Result WriteSectionData(_Inout_ FileSection& section, const UCHAR* data, const ULONG size);

			Result UpdateHeaderChecksum();

		private:
			// Copy & Move are not allowed
			File(const File& rhs) = delete;
			File(File&& rhs) = delete;

			Result CalcHeaderChecksum(_Out_writes_bytes_(32) PUCHAR checksum, _In_reads_bytes_(32) const UCHAR* token = NULL) const;
			Result CalcSectionChecksum(_Inout_ NXL_SECTION_2* section, _In_reads_bytes_(32) const UCHAR* token = NULL) const;
			Result CalcCekChecksum(_In_reads_bytes_(32) const UCHAR* cek, _Out_writes_bytes_(32) PUCHAR checksum, _In_reads_bytes_(32) const UCHAR* token = NULL) const;
			Result CalcHmacChecksum(_In_opt_ const UCHAR* data, _In_ const ULONG dataSize, _In_ const UCHAR* key, _In_ ULONG keySize, _Out_writes_bytes_(32) PUCHAR checksum, _In_ const ULONG dataAlignment = 1) const;

		private:
			std::wstring _path;
			HANDLE		_h;
			bool		_readOnly;
			ULONG		_contentOffset;
			NXL_SIGNATURE_LITE	_signature;
			FileSecret			_secret;
		};

	}
}


#endif