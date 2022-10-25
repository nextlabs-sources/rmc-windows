

#ifndef __LIBRMC_ENCRYPTED_FILE_H__
#define __LIBRMC_ENCRYPTED_FILE_H__


#include <nx\common\result.h>

#include <string>
#include <vector>

namespace NX {

	typedef struct _SECURE_FILE_HEADER {
		char			MagicCode[8];
		unsigned int	ContentSize;
		unsigned int	Reserved;
		unsigned char	Hash[16];
	} SECURE_FILE_HEADER, *PSECURE_FILE_HEADER;

	class RmSecureFile
	{
	public:
		RmSecureFile(const std::wstring& file, const std::vector<unsigned char>& key);
		virtual ~RmSecureFile();

		Result Read(std::vector<unsigned char>& content);
		Result Read(std::string& content);
		Result Write(const std::vector<unsigned char>& content);
		Result Write(const unsigned char* content, unsigned int contentSize);

	protected:
		Result CalculateHash(const unsigned char* content, unsigned int contentSize, unsigned char* hash);
		Result PrepareHeader(const unsigned char* content, unsigned int contentSize, PSECURE_FILE_HEADER header);

	protected:
		RmSecureFile();

	private:
		// No copy/move are allowed
		RmSecureFile(const RmSecureFile& rhs) {}
		RmSecureFile(RmSecureFile&& rhs) {}

	private:
		std::wstring _file;
		std::vector<unsigned char> _key;
	};

}


#endif