

#include "stdafx.h"

#include <Objbase.h>
#include <Shldisp.h>

#include <nx\winutil\file.h>
#include <nx\winutil\zip.h>

using namespace NX;


static Result CreateEmptyZipFile(_In_ const std::wstring& zipFile, _In_opt_ LPSECURITY_ATTRIBUTES psa);
static long GetFolderItemsCount(Folder* pFolder);
static bool FolderExists(const std::wstring& dir);

Result win::ZipCreate(const std::vector<std::wstring>& sources, const std::wstring& zipFile, DWORD dwWaitMs, bool overwrite, _In_opt_ LPSECURITY_ATTRIBUTES psa)
{
	const bool fileExists = (INVALID_FILE_ATTRIBUTES != GetFileAttributesW(zipFile.c_str())) ? true : false;
	HRESULT			hr = S_OK;
	IShellDispatch* pIsd = NULL;
	Folder*			pZipFolder = NULL;
	VARIANT			varZipFile;
	Result			res = RESULT(0);

	if (overwrite || !fileExists) {
		res = CreateEmptyZipFile(zipFile, psa);
		if (!res)
			return res;
	}

	// Init varZipFile
	VariantInit(&varZipFile);
	varZipFile.vt = VT_BSTR;

	CoInitialize(NULL);
	do {

		hr = ::CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pIsd);
		if (FAILED(hr)) {
			res = RESULT(hr);
			break;
		}

		varZipFile.bstrVal = ::SysAllocStringLen(NULL, (UINT)zipFile.length() + 3);
		if(NULL == varZipFile.bstrVal) {
			res = RESULT(ERROR_NOT_ENOUGH_MEMORY);
			break;
		}
		RtlSecureZeroMemory(varZipFile.bstrVal, sizeof(WCHAR)*(zipFile.length() + 3));
		memcpy(varZipFile.bstrVal, zipFile.c_str(), sizeof(WCHAR)*zipFile.length());

		hr = pIsd->NameSpace(varZipFile, &pZipFolder);
		if (FAILED(hr)) {
			res = RESULT(hr);
			break;
		}

		const long lExistingItems = GetFolderItemsCount(pZipFolder);

		VARIANT varOption;
		VariantInit(&varOption);
		varOption.vt = VT_I4;
		varOption.lVal = 0x0614;

		long nCopyCount = 0;

		// Copying and compressing the source files to our zip
		for (auto it = sources.begin(); it != sources.end(); ++it) {
			const std::wstring& s = *it;
			VARIANT	varSource;
			VariantInit(&varSource);
			varSource.vt = VT_BSTR;
			varSource.bstrVal = ::SysAllocStringLen(NULL, (UINT)s.length() + 3);
			if (NULL == varSource.bstrVal) {
				res = RESULT(ERROR_NOT_ENOUGH_MEMORY);
				break;
			}

			RtlSecureZeroMemory(varSource.bstrVal, sizeof(WCHAR)*(s.length() + 3));
			memcpy(varSource.bstrVal, s.c_str(), sizeof(WCHAR)*s.length());
			hr = pZipFolder->CopyHere(varSource, varOption);
			::SysFreeString(varZipFile.bstrVal);
			if (FAILED(hr)) {
				res = RESULT(hr);
				break;
			}

			nCopyCount++;
		}

		if (!res)
			break;

		// Wait until it finish
		DWORD dwTotalWait = 0;
		while (true) {

			const long lCurrentItems = GetFolderItemsCount(pZipFolder);
			if (lCurrentItems >= (nCopyCount + lExistingItems))
				break;

			if (INFINITE != dwWaitMs && dwTotalWait >= dwWaitMs) {
				res = RESULT(ERROR_TIMEOUT);
				break;
			}

			// Not finish yet, wait
			dwTotalWait += 200;
			Sleep(200);
		}

	} while (false);
	if (NULL != varZipFile.bstrVal) ::SysFreeString(varZipFile.bstrVal); varZipFile.bstrVal = NULL;
	if (NULL != pZipFolder) pZipFolder->Release(); pZipFolder = NULL;
	if (NULL != pIsd) pIsd->Release(); pIsd = NULL;
	CoUninitialize();

	if (!res) {
		if(ERROR_TIMEOUT != res.GetCode())
			::DeleteFileW(zipFile.c_str());
		return res;
	}

	return RESULT(0);
}

Result win::ZipAdd(const std::vector<std::wstring>& sources, const std::wstring& zipFile, DWORD dwWaitMs)
{
	return ZipCreate(sources, zipFile, dwWaitMs, false, NULL);
}

Result win::ZipUnpack(const std::wstring& zipFile, const std::wstring& targetDir, DWORD dwWaitMs)
{
	HRESULT			hr = S_OK;
	IShellDispatch* pIsd = NULL;
	Folder*			pZipFolder = NULL;
	Folder*			pToFolder = NULL;
	FolderItems*	pZipItems = NULL;
	VARIANT			varZipFile;
	VARIANT			varTargetDir;
	Result			res = RESULT(0);

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(zipFile.c_str()))
		return RESULT(ERROR_FILE_NOT_FOUND);
	if (!FolderExists(targetDir))
		return RESULT(ERROR_PATH_NOT_FOUND);

	// Init varZipFile & varTargetDir
	VariantInit(&varZipFile);
	varZipFile.vt = VT_BSTR;
	VariantInit(&varTargetDir);
	varTargetDir.vt = VT_BSTR;

	CoInitialize(NULL);
	do {

		hr = ::CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pIsd);
		if (FAILED(hr)) {
			res = RESULT(hr);
			break;
		}

		varZipFile.vt = VT_BSTR;
		varZipFile.bstrVal = ::SysAllocStringLen(NULL, (UINT)zipFile.length() + 3);
		if (NULL == varZipFile.bstrVal) {
			res = RESULT(ERROR_NOT_ENOUGH_MEMORY);
			break;
		}
		RtlSecureZeroMemory(varZipFile.bstrVal, sizeof(WCHAR)*(zipFile.length() + 3));
		memcpy(varZipFile.bstrVal, zipFile.c_str(), sizeof(WCHAR)*zipFile.length());

		varTargetDir.vt = VT_BSTR;
		varTargetDir.bstrVal = ::SysAllocStringLen(NULL, (UINT)targetDir.length() + 3);
		if (NULL == varTargetDir.bstrVal) {
			res = RESULT(ERROR_NOT_ENOUGH_MEMORY);
			break;
		}
		RtlSecureZeroMemory(varTargetDir.bstrVal, sizeof(WCHAR)*(targetDir.length() + 3));
		memcpy(varTargetDir.bstrVal, targetDir.c_str(), sizeof(WCHAR)*targetDir.length());

		hr = pIsd->NameSpace(varZipFile, &pZipFolder);
		if (FAILED(hr)) {
			res = RESULT(hr);
			break;
		}

		hr = pZipFolder->Items(&pZipItems);
		if (FAILED(hr)) {
			res = RESULT(hr);
			break;
		}

		long lZippedItems = 0;
		hr = pZipItems->get_Count(&lZippedItems);
		if (FAILED(hr)) {
			res = RESULT(hr);
			break;
		}

		// Empty zip file
		if (0 == lZippedItems)
			break;


		hr = pIsd->NameSpace(varTargetDir, &pToFolder);
		if (FAILED(hr)) {
			res = RESULT(hr);
			break;
		}

		const long lExistingItems = GetFolderItemsCount(pToFolder);

		VARIANT varOption;
		VariantInit(&varOption);
		varOption.vt = VT_I4;
		varOption.lVal = 0x0614;

		VARIANT varItems;
		VariantInit(&varItems);
		varItems.vt = VT_DISPATCH;
		varItems.pdispVal = pZipItems;
		hr = pToFolder->CopyHere(varItems, varOption);
		if(FAILED(hr)) {
			res = RESULT(hr);
			break;
		}
		
		// Wait until it finish
		DWORD dwTotalWait = 0;
		while (true) {

			const long lCurrentItems = GetFolderItemsCount(pToFolder);
			if (lCurrentItems >= (lZippedItems + lExistingItems))
				break;

			if (INFINITE != dwWaitMs && dwTotalWait >= dwWaitMs) {
				res = RESULT(ERROR_TIMEOUT);
				break;
			}

			// Not finish yet, wait
			dwTotalWait += 200;
			Sleep(200);
		}

	} while (false);
	if (NULL != varZipFile.bstrVal) ::SysFreeString(varZipFile.bstrVal); varZipFile.bstrVal = NULL;
	if (NULL != varTargetDir.bstrVal) ::SysFreeString(varTargetDir.bstrVal); varTargetDir.bstrVal = NULL;
	if (NULL != pZipItems) pZipItems->Release(); pZipItems = NULL;
	if (NULL != pZipFolder) pZipFolder->Release(); pZipFolder = NULL;
	if (NULL != pToFolder) pToFolder->Release(); pToFolder = NULL;
	if (NULL != pIsd) pIsd->Release(); pIsd = NULL;
	CoUninitialize();

	if (!res) {
		if (ERROR_TIMEOUT != res.GetCode())
			::DeleteFileW(zipFile.c_str());
		return res;
	}

	return RESULT(0);
}

Result CreateEmptyZipFile(_In_ const std::wstring& zipFile, _In_opt_ LPSECURITY_ATTRIBUTES psa)
{
	static const UCHAR emptyZip[] = { 80, 75, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Result res = NX::win::WriteSmallFile(zipFile, std::vector<UCHAR>(emptyZip, emptyZip + sizeof(emptyZip)));
	if (!res)
		return res;
	return RESULT(0);
}

long GetFolderItemsCount(Folder* pFolder)
{
	long lCount = 0;
	FolderItems* pItems = NULL;
	HRESULT hr = pFolder->Items(&pItems);
	if (FAILED(hr))
		return 0;
	hr = pItems->get_Count(&lCount);
	pItems->Release(); pItems = NULL;
	return (FAILED(hr)) ? 0 : lCount;
}

bool FolderExists(const std::wstring& dir)
{
	DWORD dwAttributes = GetFileAttributesW(dir.c_str());
	if (dwAttributes == INVALID_FILE_ATTRIBUTES)
		return false;
	return (FILE_ATTRIBUTE_DIRECTORY == (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) ? true : false;
}