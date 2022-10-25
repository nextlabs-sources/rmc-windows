

#include "stdafx.h"

#include <nx\common\macros.h>
#include <nx\crypt\aes.h>
#include <nx\crypt\md5.h>
#include <nx\rmc\securefile.h>

using namespace NX;



static const char MagicCode[8] = { 'n', 'X', 'r', 'M', 'S', 'e', 'c', 'F' };


static_assert(32 == sizeof(SECURE_FILE_HEADER), "Header size should be 32 bytes");

RmSecureFile::RmSecureFile()
{
}

RmSecureFile::RmSecureFile(const std::wstring& file, const std::vector<unsigned char>& key) : _file(file), _key(key)
{
}

RmSecureFile::~RmSecureFile()
{
}

Result RmSecureFile::Read(std::vector<unsigned char>& content)
{
	HANDLE h = ::CreateFileW(_file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	Result res = RESULT(0);

	do {

		SECURE_FILE_HEADER header = { 0 };
		ULONG bytesRead = 0;

		const ULONG fileSize = GetFileSize(h, NULL);
		if(fileSize < (ULONG)sizeof(header)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}
		const ULONG cipherSize = fileSize - (ULONG)sizeof(header);
		if (!IS_ALIGNED(cipherSize, 16)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		if (!ReadFile(h, &header, sizeof(header), &bytesRead, NULL)) {
			res = RESULT(GetLastError());
			break;
		}

		if (0 != memcmp(header.MagicCode, MagicCode, sizeof(MagicCode)) || header.ContentSize > cipherSize) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::crypt::AesKey key;
		res = key.Import(_key.data(), (ULONG)_key.size());
		if (!res)
			return res;

		unsigned long outSize = cipherSize;
		content.resize(outSize, 0);
		if (!ReadFile(h, content.data(), (ULONG)content.size(), &bytesRead, NULL)) {
			res = RESULT(GetLastError());
			break;
		}
		if(bytesRead < outSize) {
			res = RESULT(GetLastError());
			if (res) res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		res = NX::crypt::AesBlockDecrypt(key, content.data(), cipherSize, content.data(), &outSize, NULL, 0, 512);
		if (!res)
			return res;

		if (header.ContentSize < cipherSize) {
			content.resize(header.ContentSize);
		}

		// Check hash
		UCHAR hash[16] = {0};
		res = CalculateHash(content.data(), header.ContentSize, hash);
		if (!res)
			break;

		if(0 != memcmp(hash, header.Hash, 16)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

	} while (FALSE);

	CloseHandle(h); h = INVALID_HANDLE_VALUE;

	if (!res)
		content.clear();

	return res ? RESULT(0) : res;
}

Result RmSecureFile::Read(std::string& content)
{
	std::vector<unsigned char> buf;
	Result res = Read(buf);
	if (res) {
		content = std::string(buf.begin(), buf.end());
	}
	return res;
}

Result RmSecureFile::Write(const std::vector<unsigned char>& content)
{
	const unsigned int contentSize = (unsigned int)content.size();
	if (0 == contentSize)
		return RESULT(ERROR_INVALID_PARAMETER);
	return Write(content.data(), contentSize);
}

Result RmSecureFile::Write(const unsigned char* content, unsigned int contentSize)
{
	SECURE_FILE_HEADER header = { 0 };

	if (content == NULL || 0 == contentSize)
		return RESULT(ERROR_INVALID_PARAMETER);

	Result res = PrepareHeader(content, contentSize, &header);
	if (!res)
		return res;

	NX::crypt::AesKey key;
	res = key.Import(_key.data(), (ULONG)_key.size());
	if (!res)
		return res;

	const unsigned long bufSize = ROUND_TO_SIZE(contentSize, 16);
	unsigned long outSize = bufSize;
	std::vector<unsigned char> buf;
	buf.resize(bufSize, 0);
	memcpy(buf.data(), content, contentSize);
	res = NX::crypt::AesBlockEncrypt(key, buf.data(), bufSize, buf.data(), &outSize, NULL, 0, 512);
	if (!res)
		return res;

	HANDLE h = ::CreateFileW(_file.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	ULONG bytesWritten = 0;
	if (!::WriteFile(h, &header, (ULONG)sizeof(header), &bytesWritten, NULL)) {
		CloseHandle(h); h = INVALID_HANDLE_VALUE;
		return RESULT(GetLastError());
	}
	if (!::WriteFile(h, buf.data(), (ULONG)buf.size(), &bytesWritten, NULL)) {
		CloseHandle(h); h = INVALID_HANDLE_VALUE;
		return RESULT(GetLastError());
	}
	
	CloseHandle(h); h = INVALID_HANDLE_VALUE;
	return RESULT(0);
}

Result RmSecureFile::CalculateHash(const unsigned char* content, unsigned int contentSize, unsigned char* hash)
{
	return NX::crypt::CreateHmacMd5(content, contentSize, (unsigned char*)(&contentSize), (unsigned long)sizeof(unsigned int), hash);
}

Result RmSecureFile::PrepareHeader(const unsigned char* content, unsigned int contentSize, PSECURE_FILE_HEADER header)
{
	memset(header, 0, sizeof(SECURE_FILE_HEADER));
	memcpy(header->MagicCode, MagicCode, 8);
	header->ContentSize = contentSize;
	return CalculateHash(content, contentSize, header->Hash);
}