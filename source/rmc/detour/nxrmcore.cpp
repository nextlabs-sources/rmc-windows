// nxrmcore.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "nxrmcore.h"
#include "nxrmcoreglobal.h"
#include "nxrmdrv.h"
#include "nxrmcorehlp.h"
#include "detour.h"


#ifdef __cplusplus
extern "C" {
#endif

	extern CORE_GLOBAL_DATA Global;

	BOOL nxrmcoreLookupEntryPoint(
		IN PVOID DllBase,
		IN PCHAR Name,
		OUT PVOID *EntryPoint
	);

#ifdef __cplusplus
}
#endif

static USHORT NameToOrdinal(
	IN PCHAR Name,
	IN PVOID DllBase,
	IN ULONG NumberOfNames,
	IN PULONG NameTable,
	IN PUSHORT OrdinalTable
);

static PIMAGE_NT_HEADERS RtlImageNtHeader(IN PVOID BaseAddress);

static PVOID RtlImageDirectoryEntryToData(
	PVOID BaseAddress,
	BOOLEAN MappedAsImage,
	USHORT Directory,
	PULONG Size
);

static PIMAGE_SECTION_HEADER RtlImageRvaToSection(
	PIMAGE_NT_HEADERS       NtHeader,
	PVOID                   BaseAddress,
	ULONG                   Rva
);

static PVOID RtlImageRvaToVa(
	PIMAGE_NT_HEADERS       NtHeader,
	PVOID                   BaseAddress,
	ULONG                   Rva,
	PIMAGE_SECTION_HEADER   *SectionHeader
);

BOOL init_rm_section_safe(void)
{
	BOOL bRet = FALSE;

	do 
	{
		if(!Global.Section)
		{
			EnterCriticalSection(&Global.SectionLock);

			Global.Section = init_transporter_client();

			LeaveCriticalSection(&Global.SectionLock);
		}

		if(!Global.Section)
		{
			break;
		}

		bRet = is_transporter_enabled(Global.Section);

	} while (FALSE);

	return bRet;
}

static USHORT NameToOrdinal(
	IN PCHAR Name,
	IN PVOID DllBase,
	IN ULONG NumberOfNames,
	IN PULONG NameTable,
	IN PUSHORT OrdinalTable
)
{
	ULONG	Mid;
	LONG	Ret;

	if (!NumberOfNames)
		return -1;

	Mid = NumberOfNames >> 1;
	Ret = strcmp(Name, (PCHAR)((ULONG_PTR)DllBase + NameTable[Mid]));

	if (!Ret)
		return OrdinalTable[Mid];

	if (NumberOfNames == 1)
		return -1;

	if (Ret < 0)
	{
		NumberOfNames = Mid;
	}
	else
	{
		NameTable = &NameTable[Mid + 1];
		OrdinalTable = &OrdinalTable[Mid + 1];
		NumberOfNames -= (Mid - 1);
	}

	return NameToOrdinal(Name, DllBase, NumberOfNames, NameTable, OrdinalTable);
}

BOOL nxrmcoreLookupEntryPoint(
	IN PVOID DllBase,
	IN PCHAR Name,
	OUT PVOID *EntryPoint
)
{
	PULONG NameTable;
	PUSHORT OrdinalTable;
	PIMAGE_EXPORT_DIRECTORY ExportDirectory;
	ULONG ExportSize;
	CHAR Buffer[64];
	USHORT Ordinal;
	PULONG ExportTable;

	ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData(DllBase,
																		    TRUE,
																		    IMAGE_DIRECTORY_ENTRY_EXPORT,
																		    &ExportSize);

	if (strlen(Name) > sizeof(Buffer) - 2)
		return STATUS_INVALID_PARAMETER;

	RtlZeroMemory(Buffer, 64);

	memcpy(Buffer, Name, min(strlen(Name), 64));

	NameTable = (PULONG)((ULONG_PTR)DllBase + ExportDirectory->AddressOfNames);
	OrdinalTable = (PUSHORT)((ULONG_PTR)DllBase + ExportDirectory->AddressOfNameOrdinals);

	Ordinal = NameToOrdinal(Buffer,
							DllBase,
							ExportDirectory->NumberOfNames,
							NameTable,
							OrdinalTable);

	if (Ordinal >= ExportDirectory->NumberOfFunctions)
	{
		return FALSE;
	}

	ExportTable = (PULONG)((ULONG_PTR)DllBase + ExportDirectory->AddressOfFunctions);
	*EntryPoint = (PVOID)((ULONG_PTR)DllBase + ExportTable[Ordinal]);

	return TRUE;
}

static PIMAGE_NT_HEADERS RtlImageNtHeader(IN PVOID BaseAddress)
{
	PIMAGE_NT_HEADERS NtHeader;
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)BaseAddress;

	if (DosHeader && DosHeader->e_magic == IMAGE_DOS_SIGNATURE)
	{
		NtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)BaseAddress + DosHeader->e_lfanew);

		if (NtHeader->Signature == IMAGE_NT_SIGNATURE)
			return NtHeader;
	}

	return NULL;
}


static PVOID RtlImageDirectoryEntryToData(
	PVOID BaseAddress,
	BOOLEAN MappedAsImage,
	USHORT Directory,
	PULONG Size
)
{
	PIMAGE_NT_HEADERS NtHeader;
	ULONG Va;

	if ((ULONG_PTR)BaseAddress & 1)
	{
		BaseAddress = (PVOID)((ULONG_PTR)BaseAddress & ~1);
		MappedAsImage = FALSE;
	}


	NtHeader = RtlImageNtHeader(BaseAddress);
	if (NtHeader == NULL)
		return NULL;

	if (Directory >= NtHeader->OptionalHeader.NumberOfRvaAndSizes)
		return NULL;

	Va = NtHeader->OptionalHeader.DataDirectory[Directory].VirtualAddress;
	if (Va == 0)
		return NULL;

	*Size = NtHeader->OptionalHeader.DataDirectory[Directory].Size;

	if (MappedAsImage || Va < NtHeader->OptionalHeader.SizeOfHeaders)
		return (PVOID)((ULONG_PTR)BaseAddress + Va);

	return RtlImageRvaToVa(NtHeader, BaseAddress, Va, NULL);
}


static PIMAGE_SECTION_HEADER RtlImageRvaToSection(
	PIMAGE_NT_HEADERS       NtHeader,
	PVOID                   BaseAddress,
	ULONG                   Rva
)
{
	PIMAGE_SECTION_HEADER Section;
	ULONG Va;
	ULONG Count;

	Count = NtHeader->FileHeader.NumberOfSections;
	Section = (PIMAGE_SECTION_HEADER)((ULONG_PTR)&NtHeader->OptionalHeader + NtHeader->FileHeader.SizeOfOptionalHeader);

	while (Count)
	{
		Va = Section->VirtualAddress;
		if ((Va <= Rva) &&
			(Rva < Va + Section->SizeOfRawData))
			return Section;
		Section++;
	}
	return NULL;
}


static PVOID RtlImageRvaToVa(
	PIMAGE_NT_HEADERS       NtHeader,
	PVOID                   BaseAddress,
	ULONG                   Rva,
	PIMAGE_SECTION_HEADER   *SectionHeader
)
{
	PIMAGE_SECTION_HEADER Section = NULL;

	if (SectionHeader)
		Section = *SectionHeader;

	if (Section == NULL ||
		Rva < Section->VirtualAddress ||
		Rva >= Section->VirtualAddress + Section->SizeOfRawData)
	{
		Section = RtlImageRvaToSection(NtHeader, BaseAddress, Rva);

		if (Section == NULL)
			return 0;

		if (SectionHeader)
			*SectionHeader = Section;
	}

	return (PVOID)((ULONG_PTR)BaseAddress + Rva + Section->PointerToRawData - (ULONG_PTR)Section->VirtualAddress);
}
