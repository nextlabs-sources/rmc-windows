

#include "stdafx.h"

#include <nx\common\bitsmap.h>
#include <nx\common\stringex.h>
#include <nx\winutil\cpu.h>

using namespace NX;
using namespace NX::win;


namespace {

	class CpuIdData
	{
	public:
		CpuIdData()
		{
			_data[0] = _data[1] = _data[2] = _data[3] = -1;
		}
		~CpuIdData()
		{
		}
		CpuIdData& operator = (const CpuIdData& cpudata)
		{
			memcpy(_data, cpudata._data, sizeof(_data));
		}

		inline int* data() throw() { return _data; }
		inline int operator [](int id) const { return ((id >= 0 && id < 4) ? _data[id] : -1); }

		int _data[4];
	};

	class CpuInfoLoader
	{
	public:
		CpuInfoLoader()
		{
		}
		~CpuInfoLoader()
		{
		}

	private:

		NX::FixedSizeBitsMap<32> f_1_ECX_;
		NX::FixedSizeBitsMap<32> f_1_EDX_;
		NX::FixedSizeBitsMap<32> f_7_EBX_;
		NX::FixedSizeBitsMap<32> f_7_ECX_;
		NX::FixedSizeBitsMap<32> f_81_ECX_;
		NX::FixedSizeBitsMap<32> f_81_EDX_;
		std::vector<CpuIdData>	 _data;
		std::vector<CpuIdData>	 _extdata;
	};

}


const CpuInfo& NX::win::GetCurrentCpuInfo()
{
	static const CpuInfo cpu(CpuInfoEx::QueryCpuInfo());
	return cpu;
}

const CpuInfoEx& NX::win::GetCurrentCpuInfoEx()
{
	static const CpuInfoEx cpu(CpuInfoEx::QueryCpuInfoEx());
	return cpu;
}

CpuInfo::CpuInfo()
	: _venderId(UNKNOWN_VENDER), _coreCount(0), _supportHyperthreads(false)
{
}

CpuInfo::CpuInfo(bool load)
	: _venderId(UNKNOWN_VENDER), _coreCount(0), _supportHyperthreads(false)
{
	if (load)
		Load();
}

CpuInfo::CpuInfo(const CpuInfo& rhs)
	: _venderId(rhs._venderId), _venderName(rhs._venderName), _brandName(rhs._brandName), _coreCount(rhs._coreCount), _supportHyperthreads(rhs._supportHyperthreads)
{
}

CpuInfo::~CpuInfo()
{
}

CpuInfo& CpuInfo::operator = (const CpuInfo& rhs)
{
	if (this != &rhs) {
		_venderId = rhs._venderId;
		_venderName = rhs._venderName;
		_brandName = rhs._brandName;
		_coreCount = rhs._coreCount;
		_supportHyperthreads = rhs._supportHyperthreads;
	}
	return *this;
}

CpuInfo CpuInfo::QueryCpuInfo()
{
	return CpuInfo(true);
}

void CpuInfo::Load()
{
	NX::BitsMap32			f_1_ECX_;
	NX::BitsMap32			f_1_EDX_;
	NX::BitsMap32			f_7_EBX_;
	NX::BitsMap32			f_7_ECX_;
	NX::BitsMap32			f_81_ECX_;
	NX::BitsMap32			f_81_EDX_;
	std::vector<CpuIdData>	_data;
	std::vector<CpuIdData>	_extdata;
	CpuIdData				cpui;

	// Calling __cpuid with 0x0 as the function_id argument
	// gets the number of the highest valid function ID.
	__cpuid(cpui.data(), 0);
	const int cpu_id_count = cpui[0];

	for (int i = 0; i <= cpu_id_count; ++i) {
		__cpuidex(cpui.data(), i, 0);
		_data.push_back(cpui);
	}

	// Capture vendor string
	char vendor_name[0x20];
	memset(vendor_name, 0, sizeof(vendor_name));
	*reinterpret_cast<int*>(vendor_name) = _data[0][1];
	*reinterpret_cast<int*>(vendor_name + 4) = _data[0][3];
	*reinterpret_cast<int*>(vendor_name + 8) = _data[0][2];
	if (0 == _stricmp(vendor_name, "GenuineIntel")) {
		_venderId = INTEL;
	}
	else if (0 == _stricmp(vendor_name, "AuthenticAMD")) {
		_venderId = AMD;
	}
	else {
		_venderId = UNKNOWN_VENDER;
	}

	// logical processors
	_logicalProcessors = (_data[1][1] >> 16) & 0xFF;
	_coreCount = _logicalProcessors;
	const unsigned long cpu_features = _data[1][3];


	// load bitset with flags for function 0x00000001
	if (cpu_id_count >= 1) {
		f_1_ECX_ = _data[1][2];
		f_1_EDX_ = _data[1][3];
	}

	// load bitset with flags for function 0x00000007
	if (cpu_id_count >= 7) {
		f_7_EBX_ = _data[7][1];
		f_7_ECX_ = _data[7][2];
	}

	// Calling __cpuid with 0x80000000 as the function_id argument
	// gets the number of the highest valid extended ID.
	__cpuid(cpui.data(), 0x80000000);
	const int cpu_extra_id_count = cpui[0];

	char brand_name[0x40];
	memset(brand_name, 0, sizeof(brand_name));

	for (int i = 0x80000000; i <= cpu_extra_id_count; ++i) {
		__cpuidex(cpui.data(), i, 0);
		_extdata.push_back(cpui);
	}

	// load bitset with flags for function 0x80000001
	if (cpu_extra_id_count >= 0x80000001) {
		f_81_ECX_ = _extdata[1][2];
		f_81_EDX_ = _extdata[1][3];
	}

	// Interpret CPU brand string if reported
	if (cpu_extra_id_count >= 0x80000004) {
		memcpy(brand_name, _extdata[2].data(), sizeof(cpui));
		memcpy(brand_name + 16, _extdata[3].data(), sizeof(cpui));
		memcpy(brand_name + 32, _extdata[4].data(), sizeof(cpui));
		_brandName = NX::conv::utf8toutf16(brand_name);
	}

	// Get cores        
	if (IsIntelCpu()) {
		_coreCount = ((_data[4][0] >> 26) & 0x3f) + 1;
	}
	else if (IsAmdCpu()) {
		_coreCount = ((unsigned)(_extdata[8][2] & 0xff)) + 1;
	}

	// is hyperthreads enabled
	_supportHyperthreads = ((cpu_features & (1 << 28)) && (_coreCount < _logicalProcessors));
}


CpuInfoEx::CpuInfoEx()
	: CpuInfo(),
	_supportSSE3(false),
	_supportPCLMULQDQ(false),
	_supportMONITOR(false),
	_supportSSSE3(false),
	_supportFMA(false),
	_supportCMPXCHG16B(false),
	_supportSSE41(false),
	_supportSSE42(false),
	_supportMOVBE(false),
	_supportPOPCNT(false),
	_supportAES(false),
	_supportXSAVE(false),
	_supportOSXSAVE(false),
	_supportAVX(false),
	_supportF16C(false),
	_supportRDRAND(false),
	_supportMSR(false),
	_supportCX8(false),
	_supportSEP(false),
	_supportCMOV(false),
	_supportCLFSH(false),
	_supportMMX(false),
	_supportFXSR(false),
	_supportSSE(false),
	_supportSSE2(false),
	_supportFSGSBASE(false),
	_supportBMI1(false),
	_supportHLE(false),
	_supportAVX2(false),
	_supportBMI2(false),
	_supportERMS(false),
	_supportINVPCID(false),
	_supportRTM(false),
	_supportAVX512F(false),
	_supportRDSEED(false),
	_supportADX(false),
	_supportAVX512PF(false),
	_supportAVX512ER(false),
	_supportAVX512CD(false),
	_supportSHA(false),
	_supportPREFETCHWT1(false),
	_supportLAHF(false),
	_supportLZCNT(false),
	_supportABM(false),
	_supportSSE4a(false),
	_supportXOP(false),
	_supportTBM(false)
{
}


CpuInfoEx::CpuInfoEx(bool load)
	: CpuInfo(),
	_supportSSE3(false),
	_supportPCLMULQDQ(false),
	_supportMONITOR(false),
	_supportSSSE3(false),
	_supportFMA(false),
	_supportCMPXCHG16B(false),
	_supportSSE41(false),
	_supportSSE42(false),
	_supportMOVBE(false),
	_supportPOPCNT(false),
	_supportAES(false),
	_supportXSAVE(false),
	_supportOSXSAVE(false),
	_supportAVX(false),
	_supportF16C(false),
	_supportRDRAND(false),
	_supportMSR(false),
	_supportCX8(false),
	_supportSEP(false),
	_supportCMOV(false),
	_supportCLFSH(false),
	_supportMMX(false),
	_supportFXSR(false),
	_supportSSE(false),
	_supportSSE2(false),
	_supportFSGSBASE(false),
	_supportBMI1(false),
	_supportHLE(false),
	_supportAVX2(false),
	_supportBMI2(false),
	_supportERMS(false),
	_supportINVPCID(false),
	_supportRTM(false),
	_supportAVX512F(false),
	_supportRDSEED(false),
	_supportADX(false),
	_supportAVX512PF(false),
	_supportAVX512ER(false),
	_supportAVX512CD(false),
	_supportSHA(false),
	_supportPREFETCHWT1(false),
	_supportLAHF(false),
	_supportLZCNT(false),
	_supportABM(false),
	_supportSSE4a(false),
	_supportXOP(false),
	_supportTBM(false)
{
	if (load)
		Load();
}

CpuInfoEx::CpuInfoEx(const CpuInfoEx& rhs)
	: _supportSSE3(rhs._supportSSE3),
	_supportPCLMULQDQ(rhs._supportPCLMULQDQ),
	_supportMONITOR(rhs._supportMONITOR),
	_supportSSSE3(rhs._supportSSSE3),
	_supportFMA(rhs._supportFMA),
	_supportCMPXCHG16B(rhs._supportCMPXCHG16B),
	_supportSSE41(rhs._supportSSE41),
	_supportSSE42(rhs._supportSSE42),
	_supportMOVBE(rhs._supportMOVBE),
	_supportPOPCNT(rhs._supportPOPCNT),
	_supportAES(rhs._supportAES),
	_supportXSAVE(rhs._supportXSAVE),
	_supportOSXSAVE(rhs._supportOSXSAVE),
	_supportAVX(rhs._supportAVX),
	_supportF16C(rhs._supportF16C),
	_supportRDRAND(rhs._supportRDRAND),
	_supportMSR(rhs._supportMSR),
	_supportCX8(rhs._supportCX8),
	_supportSEP(rhs._supportSEP),
	_supportCMOV(rhs._supportCMOV),
	_supportCLFSH(rhs._supportCLFSH),
	_supportMMX(rhs._supportMMX),
	_supportFXSR(rhs._supportFXSR),
	_supportSSE(rhs._supportSSE),
	_supportSSE2(rhs._supportSSE2),
	_supportFSGSBASE(rhs._supportFSGSBASE),
	_supportBMI1(rhs._supportBMI1),
	_supportHLE(rhs._supportHLE),
	_supportAVX2(rhs._supportAVX2),
	_supportBMI2(rhs._supportBMI2),
	_supportERMS(rhs._supportERMS),
	_supportINVPCID(rhs._supportINVPCID),
	_supportRTM(rhs._supportRTM),
	_supportAVX512F(rhs._supportAVX512F),
	_supportRDSEED(rhs._supportRDSEED),
	_supportADX(rhs._supportADX),
	_supportAVX512PF(rhs._supportAVX512PF),
	_supportAVX512ER(rhs._supportAVX512ER),
	_supportAVX512CD(rhs._supportAVX512CD),
	_supportSHA(rhs._supportSHA),
	_supportPREFETCHWT1(rhs._supportPREFETCHWT1),
	_supportLAHF(rhs._supportLAHF),
	_supportLZCNT(rhs._supportLZCNT),
	_supportABM(rhs._supportABM),
	_supportSSE4a(rhs._supportSSE4a),
	_supportXOP(rhs._supportXOP),
	_supportTBM(rhs._supportTBM)
{
}

CpuInfoEx::~CpuInfoEx()
{
}

CpuInfoEx& CpuInfoEx::operator = (const CpuInfoEx& rhs)
{
	if (this != &rhs) {

		CpuInfo::operator=(rhs);
		_supportSSE3		= rhs._supportSSE3;
		_supportPCLMULQDQ	= rhs._supportPCLMULQDQ;
		_supportMONITOR		= rhs._supportMONITOR;
		_supportSSSE3		= rhs._supportSSSE3;
		_supportFMA			= rhs._supportFMA;
		_supportCMPXCHG16B	= rhs._supportCMPXCHG16B;
		_supportSSE41		= rhs._supportSSE41;
		_supportSSE42		= rhs._supportSSE42;
		_supportMOVBE		= rhs._supportMOVBE;
		_supportPOPCNT		= rhs._supportPOPCNT;
		_supportAES			= rhs._supportAES;
		_supportXSAVE		= rhs._supportXSAVE;
		_supportOSXSAVE		= rhs._supportOSXSAVE;
		_supportAVX			= rhs._supportAVX;
		_supportF16C		= rhs._supportF16C;
		_supportRDRAND		= rhs._supportRDRAND;

		_supportMSR			= rhs._supportMSR;
		_supportCX8			= rhs._supportCX8;
		_supportSEP			= rhs._supportSEP;
		_supportCMOV		= rhs._supportCMOV;
		_supportCLFSH		= rhs._supportCLFSH;
		_supportMMX			= rhs._supportMMX;
		_supportFXSR		= rhs._supportFXSR;
		_supportSSE			= rhs._supportSSE;
		_supportSSE2		= rhs._supportSSE2;

		_supportFSGSBASE	= rhs._supportFSGSBASE;
		_supportBMI1		= rhs._supportBMI1;
		_supportHLE			= rhs._supportHLE;
		_supportAVX2		= rhs._supportAVX2;
		_supportBMI2		= rhs._supportBMI2;
		_supportERMS		= rhs._supportERMS;
		_supportINVPCID		= rhs._supportINVPCID;
		_supportRTM			= rhs._supportRTM;
		_supportAVX512F		= rhs._supportAVX512F;
		_supportRDSEED		= rhs._supportRDSEED;
		_supportADX			= rhs._supportADX;
		_supportAVX512PF	= rhs._supportAVX512PF;
		_supportAVX512ER	= rhs._supportAVX512ER;
		_supportAVX512CD	= rhs._supportAVX512CD;
		_supportSHA			= rhs._supportSHA;

		_supportPREFETCHWT1	= rhs._supportPREFETCHWT1;

		_supportLAHF		= rhs._supportLAHF;
		_supportLZCNT		= rhs._supportLZCNT;
		_supportABM			= rhs._supportABM;
		_supportSSE4a		= rhs._supportSSE4a;
		_supportXOP			= rhs._supportXOP;
		_supportTBM			= rhs._supportTBM;
	}
	return *this;
}

CpuInfoEx CpuInfoEx::QueryCpuInfoEx()
{
	return CpuInfoEx(true);
}

void CpuInfoEx::Load()
{
	NX::BitsMap32			f_1_ECX_;
	NX::BitsMap32			f_1_EDX_;
	NX::BitsMap32			f_7_EBX_;
	NX::BitsMap32			f_7_ECX_;
	NX::BitsMap32			f_81_ECX_;
	NX::BitsMap32			f_81_EDX_;
	std::vector<CpuIdData>	_data;
	std::vector<CpuIdData>	_extdata;
	CpuIdData				cpui;

	// Calling __cpuid with 0x0 as the function_id argument
	// gets the number of the highest valid function ID.
	__cpuid(cpui.data(), 0);
	const int cpu_id_count = cpui[0];

	for (int i = 0; i <= cpu_id_count; ++i) {
		__cpuidex(cpui.data(), i, 0);
		_data.push_back(cpui);
	}

	// Capture vendor string
	char vendor_name[0x20];
	memset(vendor_name, 0, sizeof(vendor_name));
	*reinterpret_cast<int*>(vendor_name) = _data[0][1];
	*reinterpret_cast<int*>(vendor_name + 4) = _data[0][3];
	*reinterpret_cast<int*>(vendor_name + 8) = _data[0][2];
	if (0 == _stricmp(vendor_name, "GenuineIntel")) {
		_venderId = INTEL;
	}
	else if (0 == _stricmp(vendor_name, "AuthenticAMD")) {
		_venderId = AMD;
	}
	else {
		_venderId = UNKNOWN_VENDER;
	}

	// logical processors
	_logicalProcessors = (_data[1][1] >> 16) & 0xFF;
	_coreCount = _logicalProcessors;
	const unsigned long cpu_features = _data[1][3];


	// load bitset with flags for function 0x00000001
	if (cpu_id_count >= 1) {
		f_1_ECX_ = _data[1][2];
		f_1_EDX_ = _data[1][3];
	}

	// load bitset with flags for function 0x00000007
	if (cpu_id_count >= 7) {
		f_7_EBX_ = _data[7][1];
		f_7_ECX_ = _data[7][2];
	}

	// Calling __cpuid with 0x80000000 as the function_id argument
	// gets the number of the highest valid extended ID.
	__cpuid(cpui.data(), 0x80000000);
	const int cpu_extra_id_count = cpui[0];

	char brand_name[0x40];
	memset(brand_name, 0, sizeof(brand_name));

	for (int i = 0x80000000; i <= cpu_extra_id_count; ++i) {
		__cpuidex(cpui.data(), i, 0);
		_extdata.push_back(cpui);
	}

	// load bitset with flags for function 0x80000001
	if (cpu_extra_id_count >= 0x80000001) {
		f_81_ECX_ = _extdata[1][2];
		f_81_EDX_ = _extdata[1][3];
	}

	// Interpret CPU brand string if reported
	if (cpu_extra_id_count >= 0x80000004) {
		memcpy(brand_name, _extdata[2].data(), sizeof(cpui));
		memcpy(brand_name + 16, _extdata[3].data(), sizeof(cpui));
		memcpy(brand_name + 32, _extdata[4].data(), sizeof(cpui));
		_brandName = NX::conv::utf8toutf16(brand_name);
	}

	// Get cores        
	if (IsIntelCpu()) {
		_coreCount = ((_data[4][0] >> 26) & 0x3f) + 1;
	}
	else if (IsAmdCpu()) {
		_coreCount = ((unsigned)(_extdata[8][2] & 0xff)) + 1;
	}

	// is hyperthreads enabled
	_supportHyperthreads = ((cpu_features & (1 << 28)) && (_coreCount < _logicalProcessors));

	_supportSSE3 = f_1_ECX_[0];
	_supportPCLMULQDQ = f_1_ECX_[1];
	_supportMONITOR = f_1_ECX_[3];
	_supportSSSE3 = f_1_ECX_[9];
	_supportFMA = f_1_ECX_[12];
	_supportCMPXCHG16B = f_1_ECX_[13];
	_supportSSE41 = f_1_ECX_[19];
	_supportSSE42 = f_1_ECX_[20];
	_supportMOVBE = f_1_ECX_[22];
	_supportPOPCNT = f_1_ECX_[23];
	_supportAES = f_1_ECX_[25];
	_supportXSAVE = f_1_ECX_[26];
	_supportOSXSAVE = f_1_ECX_[27];
	_supportAVX = f_1_ECX_[28];
	_supportF16C = f_1_ECX_[29];
	_supportRDRAND = f_1_ECX_[30];

	_supportMSR = f_1_EDX_[5];
	_supportCX8 = f_1_EDX_[8];
	_supportSEP = f_1_EDX_[11];
	_supportCMOV = f_1_EDX_[15];
	_supportCLFSH = f_1_EDX_[19];
	_supportMMX = f_1_EDX_[23];
	_supportFXSR = f_1_EDX_[24];
	_supportSSE = f_1_EDX_[25];
	_supportSSE2 = f_1_EDX_[26];

	_supportFSGSBASE = f_7_EBX_[0];
	_supportBMI1 = f_7_EBX_[3];
	_supportHLE = (IsIntelCpu() && f_7_EBX_[4]);
	_supportAVX2 = f_7_EBX_[5];
	_supportBMI2 = f_7_EBX_[8];
	_supportERMS = f_7_EBX_[9];
	_supportINVPCID = f_7_EBX_[10];
	_supportRTM = (IsIntelCpu() && f_7_EBX_[11]);
	_supportAVX512F = f_7_EBX_[16];
	_supportRDSEED = f_7_EBX_[18];
	_supportADX = f_7_EBX_[19];
	_supportAVX512PF = f_7_EBX_[26];
	_supportAVX512ER = f_7_EBX_[27];
	_supportAVX512CD = f_7_EBX_[28];
	_supportSHA = f_7_EBX_[29];

	_supportPREFETCHWT1 = f_7_ECX_[0];

	_supportLAHF = f_81_ECX_[0];
	_supportLZCNT = (IsIntelCpu() && f_81_ECX_[5]);
	_supportABM = (IsAmdCpu() && f_81_ECX_[5]);
	_supportSSE4a = (IsAmdCpu() && f_81_ECX_[6]);
	_supportXOP = (IsAmdCpu() && f_81_ECX_[11]);
	_supportTBM = (IsAmdCpu() && f_81_ECX_[21]);

	_supportSYSCALL = (IsIntelCpu() && f_81_EDX_[11]);
	_supportMMXEXT = (IsAmdCpu() && f_81_EDX_[22]);
	_supportRDTSCP = (IsIntelCpu() && f_81_EDX_[27]);
	_support3DNOWEXT = (IsAmdCpu() && f_81_EDX_[30]);
	_support3DNOW = (IsAmdCpu() && f_81_EDX_[31]);
}