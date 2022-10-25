
#include "stdafx.h"

#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\nxl\nxlfile.h>
#include <nx\nxl\nxlrights.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\common\time.h>
#include <nx\winutil\path.h>
#include <nx\rest\uri.h>

#include <regex>

using namespace NX;
using namespace NX::rapidjson;


NX::NXL::File* RmSession::LocalFileOpen(const std::wstring& path, bool readOnly, Result& result)
{
	NX::NXL::File* fp = NXL::File::Open(path, readOnly, result);
	if (NULL == fp)
		return NULL;

	do {
		
		// File is opened, we need to get token information
		const std::wstring& ownerId = fp->GetFileSecret().GetOwnerIdW();
		const std::wstring& agreement = NX::bintohs<wchar_t>(fp->GetFileSecret().GetAgreement0().data(), fp->GetFileSecret().GetAgreement0().size());
		const std::wstring& duid = NX::bintohs<wchar_t>(fp->GetFileSecret().GetToken().GetId().data(), fp->GetFileSecret().GetToken().GetId().size());
		NXL::FileToken  token(fp->GetFileSecret().GetToken().GetId(), fp->GetFileSecret().GetToken().GetLevel(), std::vector<UCHAR>());

		result = ReadCachedLocalFileToken(path, ownerId, duid, token);
		if (!result) {
			result = ReadLocalFileTokenFromServer(ownerId, agreement, token);
			if (result) {
				// Cache it
				Result res = WriteCachedLocalFileToken(path, ownerId, duid, token);
			}
			else {
				if (404 == result.GetCode())
					result = RESULT(ERROR_NO_USER_SESSION_KEY);
				else if (401 == result.GetCode() || 403 == result.GetCode())
					result = RESULT(ERROR_ACCESS_DENIED);
				else
					;	// Nothing
			}
		}
		if (!result)
			break;

		// Okay, we have full token now
		fp->SetTokenkey(token.GetKey().data());

		// Now check file header checksum
		result = fp->VerifyHeaderChecksum();

	} while (FALSE);

	return fp;
}


Result RmSession::LocalFileProtect(const std::wstring& source,
	const std::wstring& target,
	const std::wstring& membershipId,
	const NX::NXL::FileMeta& metadata,
	const NX::NXL::FileTags& tags,
	const ULONGLONG permissions,
	_In_opt_ bool* cancelFlag,
	_Out_ std::wstring& newFileId,
	_Out_ std::wstring& newFilePath,
	_Out_opt_ NX::NXL::FileSecret* secret)
{
	std::vector<std::wstring> rights;
	std::vector<std::wstring> obs;

	if (0 != permissions) {
		if (0 != (permissions&BUILTIN_RIGHT_VIEW))
			rights.push_back(RIGHT_ACTION_VIEW);
		if (0 != (permissions&BUILTIN_RIGHT_PRINT))
			rights.push_back(RIGHT_ACTION_PRINT);
		if (0 != (permissions&BUILTIN_RIGHT_SHARE))
			rights.push_back(RIGHT_ACTION_SHARE);
		if (0 != (permissions&BUILTIN_RIGHT_DOWNLOAD))
			rights.push_back(RIGHT_ACTION_DOWNLOAD);

		if (0 != (permissions&BUILTIN_OBRIGHT_WATERMARK))
			obs.push_back(OBLIGATION_NAME_WATERMARK);
	}

	return LocalFileProtect(source, target, membershipId, metadata, tags, rights, obs, cancelFlag, newFileId, newFilePath, secret);
}

Result RmSession::LocalFileProtect(const std::wstring& source,
	const std::wstring& target,
	const std::wstring& membershipId,
	const NX::NXL::FileMeta& metadata,
	const NX::NXL::FileTags& tags,
	const std::vector<std::wstring>& rights,
	const std::vector<std::wstring>& obs,
	_In_opt_ bool* cancelFlag,
	_Out_ std::wstring& newFileId,
	_Out_ std::wstring& newFilePath,
	_Out_opt_ NX::NXL::FileSecret* secret)
{
	NXL_SIGNATURE_LITE sig = { 0 };
	if (NXL::File::IsNXLFile(source, &sig))
		return RESULT(ERROR_FILE_ENCRYPTED);

	NX::win::FilePath path(source);
	const std::wstring origFileName(path.GetFileName());
	std::wstring dest(target);
	if (dest.empty()) {

		SYSTEMTIME st = { 0 };
		GetSystemTime(&st);
		const std::wstring wsTimestamp(NX::FormatString(L"-%04d-%02d-%02d-%02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));
	
		dest = GetCurrentUserTempDir();
		if (!NX::iend_with<wchar_t>(dest, L"\\"))
			dest.append(L"\\");

		std::wstring fileName = path.GetFileName();
		std::wstring fileExt = path.GetFileExtension();

		if (!fileExt.empty()) {
			fileName = fileName.substr(0, fileName.length() - fileExt.length());
			fileName.append(wsTimestamp);
			fileName.append(fileExt);
			fileName.append(L".nxl");
		}
		else {
			fileName.append(wsTimestamp);
			fileName.append(L".nxl");
		}

		dest.append(fileName);
	}
	else {
		if (!NX::iend_with<wchar_t>(dest, L".nxl"))
			dest.append(L".nxl");
	}
	
	const wchar_t* pos = wcsrchr(dest.c_str(), L'\\');
	const std::wstring displayName(pos == NULL ? dest : (pos+1));

	NX::Result res = RESULT(0);
	do {

		RmUserMembership* membership = _user.Findmembership(membershipId);
		if (NULL == membership) {
			res = RESULT(ERROR_NOT_FOUND);
			break;
		}

		const NXL::FileToken& token = membership->GetFileToken();
		if (token.Empty() || token.GetKey().empty()) {
			res = RESULT(NTE_BAD_KEY);
			break;
		}

		NXL::FileSecret newSecret(std::string(membership->GetId().begin(), membership->GetId().end()), token, 0, membership->GetAgreement0(), membership->GetAgreement1(), 0);
		NXL::FileAdHocPolicy policy(membership->GetId(), rights, obs);

		std::shared_ptr<NX::NXL::File> fp(NXL::File::CreateFrom(source, dest, true, newSecret, NULL, &metadata, &policy, &tags, NXL_DEFAULT_MSG, res));
		if (fp == NULL) {
			break;
		}
		fp.reset();

		if (cancelFlag && *cancelFlag) {
			res = RESULT(ERROR_CANCELLED);
			break;
		}

		// File has been created, uploaded it
		RmMyVaultUploadResult newFile;
		res = MyVaultAddFileFromLocal(dest, source, newFile, cancelFlag);
		if (!res)
			break;

		// Succeeded
		newFileId = newFile.GetPath();
		newFilePath = newFile.GetDisplayPath();
		if (secret)
			*secret = newSecret;

		// Write log
		LogActivityProtect(NX::bintohs<wchar_t>(newSecret.GetToken().GetId().data(),
						   newSecret.GetToken().GetId().size()),
						   newSecret.GetOwnerIdW(),
						   L"",
						   L"",
						   origFileName,
						   source);

	} while (FALSE);
	::DeleteFileW(dest.c_str());

	return res;
}

Result RmSession::LocalFileUnprotect(const std::wstring& source, const std::wstring& target)
{
	Result res = RESULT(0);

	std::shared_ptr<NX::NXL::File> fp(LocalFileOpen(source, true, res));
	if (NULL == fp)
		return res;

	if (ERROR_DATA_CHECKSUM_ERROR == res.GetCode())
		return res;

	if (!fp->IsFullToken())
		return RESULT(ERROR_NO_USER_SESSION_KEY);

	res = fp->Unprotect(target);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::LocalFileShare(const std::wstring& source,
	const std::wstring& membershipId,
	const NX::NXL::FileMeta& metadata,
	const NX::NXL::FileTags& tags,
	const std::vector<std::wstring>& recipients,
	const std::vector<std::wstring>& rights,
	const std::vector<std::wstring>& obs,
	_In_opt_ const SYSTEMTIME* expireTime,
	RmFileShareResult& shareResult, _In_opt_ bool* cancelFlag)
{
	__int64 unixExpireTime = 0;
	if (expireTime) {
		NX::time::datetime dt(expireTime);
		unixExpireTime = dt.to_java_time();
	}

	return LocalFileShare(source,
		membershipId,
		metadata,
		tags,
		recipients,
		rights,
		obs,
		unixExpireTime,
		shareResult, cancelFlag);
}

bool EndWithTimestamp(const std::wstring& s)
{
	static const std::wregex pattern(L"^.*-\\d{4}-\\d{2}-\\d{2}-\\d{2}-\\d{2}-\\d{2}$", std::regex_constants::icase);
	bool result = false;
	try {
		result = std::regex_match(s, pattern);
	}
	catch (...) {
		result = false;
	}
	return result;
}

Result RmSession::LocalFileShare(const std::wstring& source,
	const std::wstring& membershipId,
	const NX::NXL::FileMeta& metadata,
	const NX::NXL::FileTags& tags,
	const std::vector<std::wstring>& recipients,
	const std::vector<std::wstring>& rights,
	const std::vector<std::wstring>& obs,
	_In_ __int64 unixExpireTime,
	RmFileShareResult& shareResult, _In_opt_ bool* cancelFlag)
{
	Result res = RESULT(0);
	SYSTEMTIME st = { 0 };

	GetSystemTime(&st);
	// -YYYY-MM-DD-HH-MM-SS
	const std::wstring sTimestamp(NX::FormatString(L"-%04d-%02d-%02d-%02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));
	NX::win::FilePath path(source);
	std::wstring  targetName = path.GetFileName();
	const std::wstring origTargetName(targetName);

	if (!NX::iend_with<wchar_t>(targetName, L".nxl")) {

		std::wstring::size_type posSuffix = targetName.rfind(L'.');
		if (posSuffix == std::wstring::npos) {
			targetName += sTimestamp;
		}
		else {
			const std::wstring suffix(targetName.substr(posSuffix));
			targetName = targetName.substr(0, posSuffix);
			// Does the name have time-stamp already?
			if (EndWithTimestamp(targetName)) {
				targetName = targetName.substr(0, targetName.length() - 20);
			}
			targetName += sTimestamp;
			targetName += suffix;
		}
		targetName += L".nxl";
	}
	const std::wstring target(NX::win::GetCurrentUserDirs().GetUserInetCacheDir() + L"\\" + targetName);
	
	auto pos = std::find_if(_user.GetMemberships().begin(), _user.GetMemberships().end(), [&](const RmUserMembership& member) -> bool {
		return (0 == _wcsicmp(membershipId.c_str(), member.GetId().c_str()));
	});

	if (pos == _user.GetMemberships().end())
		return RESULT(ERROR_NOT_FOUND);

	const NXL::FileToken& token = (*pos).GetFileToken();
	if (token.Empty() || token.GetKey().empty())
		return RESULT(NTE_BAD_KEY);
	
	do {
		

		NXL::FileAdHocPolicy policy(membershipId, rights, obs);
		NXL::FileMeta newMetadata;

		if (NXL::File::IsNXLFile(source, NULL)) {

			::DeleteFileW(target.c_str());
			if (!::CopyFileW(source.c_str(), target.c_str(), FALSE)) {
				res = RESULT(GetLastError());
				break;
			}

			std::shared_ptr<NX::NXL::File> fp(LocalFileOpen(target, false, res));
			if (fp == nullptr)
				break;

			if (NULL == GetCurrentUser().Findmembership(fp->GetFileSecret().GetOwnerIdW())) {
				// Current user is not owner
				NXL::FileAdHocPolicy origPolicy;
				res = fp->ReadAdHocPolicy(origPolicy);
				if (!res || 0 == (origPolicy.RightsToInt64() & BUILTIN_RIGHT_SHARE) || !fp->GetFileSecret().GetToken().IsFullToken()) {
					// No policy or No share policy or no rights at all
					res = RESULT(ERROR_ACCESS_DENIED);
					LogActivityShare(false,
						true,
						NX::bintohs<wchar_t>(fp->GetFileSecret().GetToken().GetId().data(), fp->GetFileSecret().GetToken().GetId().size()),
						fp->GetFileSecret().GetOwnerIdW(),
						L"Local",
						L"",
						origTargetName,
						source);
					break;
				}
			}
			else {
				assert(fp->GetFileSecret().GetToken().IsFullToken());
			}

			res = fp->ReadMetadata(newMetadata);
			if (!res)
				break;
			std::for_each(metadata.GetMetadata().cbegin(), metadata.GetMetadata().cend(), [&](const NXL::FileMeta::FileMetaItem& item) {
				newMetadata.Set(item.first, item.second);
			});
			fp->WriteMetadata(newMetadata);
			fp->WriteAdHocPolicy(policy);
			fp->WriteTags(tags);
		}
		else {
			NXL::FileSecret secret(std::string(membershipId.begin(), membershipId.end()), token, 0, (*pos).GetAgreement0(), (*pos).GetAgreement1(), 0);
			std::shared_ptr<NX::NXL::File> fp(NXL::File::CreateFrom(source, target, true, secret, NULL, &metadata, &policy, &tags, NXL_DEFAULT_MSG, res));
			if (fp == NULL)
				break;
			res = fp->ReadMetadata(newMetadata);
			if (!res)
				break;

			// Write log
			LogActivityProtect(NX::bintohs<wchar_t>(secret.GetToken().GetId().data(), secret.GetToken().GetId().size()), secret.GetOwnerIdW(), L"", L"", path.GetFileName(), source);
		}
		
		unsigned __int64 permission = 0;
		std::for_each(rights.begin(), rights.end(), [&](const std::wstring& r) {
			permission |= ActionToRights(r.c_str());
		});

		RmRestStatus status;
		const std::string& sMetadata = metadata.ToString();

		res = _restClient.ShareLocalFile(_rmserver.GetUrl(),
			membershipId,
			recipients,
			permission,
			unixExpireTime,
			target,
			source,
			newMetadata,
			tags,
			status,
			shareResult,
			cancelFlag);
		if (!res)
			break;
		if (!status)
			res = RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	} while (FALSE);

	// Remove the temp file after sharing
	(void)::DeleteFileW(target.c_str());

	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::ReadLocalFileTokenFromServer(const std::wstring& ownerId, const std::wstring& agreement, _Inout_ NX::NXL::FileToken& token)
{
	if(token.GetId().empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	const std::wstring& duid = NX::bintohs<wchar_t>(token.GetId().data(), token.GetId().size());

	RmRestStatus status;
	Result res = _restClient.TokenRetrieve(_rmserver.GetUrl(),
		ownerId,
		agreement,
		duid,
		token.GetLevel(),
		status,
		token);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	if (token.Empty() || token.GetKey().size() != 32)
		return RESULT(ERROR_INVALID_DATA);

	return RESULT(0);
}

Result RmSession::ReadCachedLocalFileToken(const std::wstring& path, const std::wstring& ownerId, const std::wstring& duid, _Inout_ NX::NXL::FileToken& token)
{
	if (token.GetId().empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	std::wstring cachePath;
	if (!NX::ibegin_with<wchar_t>(path, L"\\??\\"))
		cachePath = L"\\??\\";
	cachePath.append(path);
	cachePath.append(L":");
	cachePath.append(_rmserver.GetTenantName());
	cachePath.append(L".");
	cachePath.append(_user.GetId());
	cachePath.append(L".");
	cachePath.append(ownerId);

	std::string s;
	Result res = SecureFileRead(cachePath, s);
	if (!res)
		return res;
	if (s.empty())
		return RESULT(ERROR_NOT_FOUND);

	JsonDocument doc;
	if (!doc.LoadJsonString<char>(s))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE root = doc.GetRoot();
	if(!IsValidJsonObject(root))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE pv = root->AsObject()->at(L"duid");
	if(!IsValidJsonString(pv))
		return RESULT(ERROR_INVALID_DATA);
	const std::wstring& cachedDuid = pv->AsString()->GetString();
	if (0 != _wcsicmp(cachedDuid.c_str(), duid.c_str()))
		return RESULT(ERROR_FILE_IDENTITY_NOT_PERSISTENT);

	pv = root->AsObject()->at(L"tokenId");
	if (!IsValidJsonString(pv))
		return RESULT(ERROR_INVALID_DATA);
	const std::vector<UCHAR> cachedTokenId(NX::hstobin<wchar_t>(pv->AsString()->GetString()));
	if(cachedTokenId != token.GetId())
		return RESULT(ERROR_INVALID_DATA);

	pv = root->AsObject()->at(L"tokenLevel");
	if (!IsValidJsonNumber(pv))
		return RESULT(ERROR_INVALID_DATA);
	const unsigned int cachedTokenLevel = pv->AsNumber()->ToUint();
	if(cachedTokenLevel != token.GetLevel())
		return RESULT(ERROR_INVALID_DATA);

	pv = root->AsObject()->at(L"tokenKey");
	if (!IsValidJsonString(pv))
		return RESULT(ERROR_NOT_FOUND);
	const std::vector<UCHAR> cachedTokenKey(NX::hstobin<wchar_t>(pv->AsString()->GetString()));
	if(cachedTokenKey.size() != 32)
		return RESULT(ERROR_NOT_FOUND);

	token = NXL::FileToken(cachedTokenId, cachedTokenLevel, cachedTokenKey);
	return RESULT(0);
}

Result RmSession::WriteCachedLocalFileToken(const std::wstring& path, const std::wstring& ownerId, const std::wstring& duid, const NX::NXL::FileToken& token)
{
	std::wstring cachePath;
	if (!NX::ibegin_with<wchar_t>(path, L"\\??\\"))
		cachePath = L"\\??\\";
	cachePath.append(path);
	cachePath.append(L":");
	cachePath.append(_rmserver.GetTenantName());
	cachePath.append(L".");
	cachePath.append(_user.GetId());
	cachePath.append(L".");
	cachePath.append(ownerId);

	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	if (root == NULL)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);

	PJSONVALUE pv = JsonValue::CreateString(duid);
	if(NULL == pv)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);
	root->set(L"duid", pv);
	pv = JsonValue::CreateString(NX::bintohs<wchar_t>(token.GetId().data(), token.GetId().size()));
	if(NULL == pv)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);
	root->set(L"tokenId", pv);
	pv = JsonValue::CreateString(NX::bintohs<wchar_t>(token.GetKey().data(), token.GetKey().size()));
	if(NULL == pv)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);
	root->set(L"tokenKey", pv);
	pv = JsonValue::CreateNumber((__int64)token.GetLevel());
	if(NULL == pv)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);
	root->set(L"tokenLevel", pv);
	JsonStringWriter<char> writer;
	const std::string& s = writer.Write(root.get());

	Result res = SecureFileWrite(cachePath, s);
	if (!res)
		return res;

	return RESULT(0);
}