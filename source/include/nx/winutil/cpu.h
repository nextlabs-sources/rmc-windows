

#ifndef __NX_WINUTIL_CPU_H__
#define __NX_WINUTIL_CPU_H__

#include <string>

namespace NX {

	namespace win {

		class CpuInfo
		{
		public:
			CpuInfo();
			CpuInfo(const CpuInfo& rhs);
			virtual ~CpuInfo();

			CpuInfo& operator = (const CpuInfo& rhs);

			typedef enum VENDERID {
				UNKNOWN_VENDER = 0,
				INTEL,
				AMD
			} VENDERID;

			static CpuInfo QueryCpuInfo();

			inline VENDERID GetVenderId() const { return _venderId; }
			inline bool IsIntelCpu() const { return (INTEL == _venderId); }
			inline bool IsAmdCpu() const { return (AMD == _venderId); }
			inline const std::wstring& GetVenderName() const { return _venderName; }
			inline const std::wstring& GetBrandName() const { return _brandName; }
			inline ULONG GetCoreCount() const { return _coreCount; }
			inline ULONG GetLogicalProcessorsCount() const { return _logicalProcessors; }
			inline bool SupportHyperthreads() const { return _supportHyperthreads; }

		protected:
			explicit CpuInfo(bool load);
			virtual void Load();

			VENDERID		_venderId;
			std::wstring	_venderName;
			std::wstring	_brandName;
			ULONG			_coreCount;
			ULONG			_logicalProcessors;
			bool			_supportHyperthreads;
		};


		class CpuInfoEx : public CpuInfo
		{
		public:
			CpuInfoEx();
			CpuInfoEx(const CpuInfoEx& rhs);
			virtual ~CpuInfoEx();

			static CpuInfoEx QueryCpuInfoEx();

			CpuInfoEx& operator = (const CpuInfoEx& rhs);

			inline bool SupportSSE3() const { return _supportSSE3; }
			inline bool SupportPCLMULQDQ() const { return _supportPCLMULQDQ; }
			inline bool SupportMONITOR() const { return _supportMONITOR; }
			inline bool SupportSSSE3() const { return _supportSSSE3; }
			inline bool SupportFMA() const { return _supportFMA; }
			inline bool SupportCMPXCHG16B() const { return _supportCMPXCHG16B; }
			inline bool SupportSSE41() const { return _supportSSE41; }
			inline bool SupportSSE42() const { return _supportSSE42; }
			inline bool SupportMOVBE() const { return _supportMOVBE; }
			inline bool SupportPOPCNT() const { return _supportPOPCNT; }
			inline bool SupportAES() const { return _supportAES; }
			inline bool SupportXSAVE() const { return _supportXSAVE; }
			inline bool SupportOSXSAVE() const { return _supportOSXSAVE; }
			inline bool SupportAVX() const { return _supportAVX; }
			inline bool SupportF16C() const { return _supportF16C; }
			inline bool SupportRDRAND() const { return _supportRDRAND; }

			inline bool SupportMSR() const { return _supportMSR; }
			inline bool SupportCX8() const { return _supportCX8; }
			inline bool SupportSEP() const { return _supportSEP; }
			inline bool SupportCMOV() const { return _supportCMOV; }
			inline bool SupportCLFSH() const { return _supportCLFSH; }
			inline bool SupportMMX() const { return _supportMMX; }
			inline bool SupportFXSR() const { return _supportFXSR; }
			inline bool SupportSSE() const { return _supportSSE; }
			inline bool SupportSSE2() const { return _supportSSE2; }

			inline bool SupportFSGSBASE() const { return _supportFSGSBASE; }
			inline bool SupportBMI1() const { return _supportBMI1; }
			inline bool SupportHLE() const { return _supportHLE; }
			inline bool SupportAVX2() const { return _supportAVX2; }
			inline bool SupportBMI2() const { return _supportBMI2; }
			inline bool SupportERMS() const { return _supportERMS; }
			inline bool SupportINVPCID() const { return _supportINVPCID; }
			inline bool SupportRTM() const { return _supportRTM; }
			inline bool SupportAVX512F() const { return _supportAVX512F; }
			inline bool SupportRDSEED() const { return _supportRDSEED; }
			inline bool SupportADX() const { return _supportADX; }
			inline bool SupportAVX512PF() const { return _supportAVX512PF; }
			inline bool SupportAVX512ER() const { return _supportAVX512ER; }
			inline bool SupportAVX512CD() const { return _supportAVX512CD; }
			inline bool SupportSHA() const { return _supportSHA; }

			inline bool SupportPREFETCHWT1() const { return _supportPREFETCHWT1; }

			inline bool SupportLAHF() const { return _supportLAHF; }
			inline bool SupportLZCNT() const { return _supportLZCNT; }
			inline bool SupportABM() const { return _supportABM; }
			inline bool SupportSSE4a() const { return _supportSSE4a; }
			inline bool SupportXOP() const { return _supportXOP; }
			inline bool SupportTBM() const { return _supportTBM; }

			inline bool SupportSYSCALL() const { return _supportSYSCALL; }
			inline bool SupportMMXEXT() const { return _supportMMXEXT; }
			inline bool SupportRDTSCP() const { return _supportRDTSCP; }
			inline bool Support3DNOWEXT() const { return _support3DNOWEXT; }
			inline bool Support3DNOW() const { return _support3DNOW; }

		protected:
			explicit CpuInfoEx(bool load);
			virtual void Load();

		private:
			bool			_supportSSE3;
			bool			_supportPCLMULQDQ;
			bool			_supportMONITOR;
			bool			_supportSSSE3;
			bool			_supportFMA;
			bool			_supportCMPXCHG16B;
			bool			_supportSSE41;
			bool			_supportSSE42;
			bool			_supportMOVBE;
			bool			_supportPOPCNT;
			bool			_supportAES;
			bool			_supportXSAVE;
			bool			_supportOSXSAVE;
			bool			_supportAVX;
			bool			_supportF16C;
			bool			_supportRDRAND;

			bool			_supportMSR;
			bool			_supportCX8;
			bool			_supportSEP;
			bool			_supportCMOV;
			bool			_supportCLFSH;
			bool			_supportMMX;
			bool			_supportFXSR;
			bool			_supportSSE;
			bool			_supportSSE2;

			bool			_supportFSGSBASE;
			bool			_supportBMI1;
			bool			_supportHLE;
			bool			_supportAVX2;
			bool			_supportBMI2;
			bool			_supportERMS;
			bool			_supportINVPCID;
			bool			_supportRTM;
			bool			_supportAVX512F;
			bool			_supportRDSEED;
			bool			_supportADX;
			bool			_supportAVX512PF;
			bool			_supportAVX512ER;
			bool			_supportAVX512CD;
			bool			_supportSHA;

			bool			_supportPREFETCHWT1;

			bool			_supportLAHF;
			bool			_supportLZCNT;
			bool			_supportABM;
			bool			_supportSSE4a;
			bool			_supportXOP;
			bool			_supportTBM;

			bool			_supportSYSCALL;
			bool			_supportMMXEXT;
			bool			_supportRDTSCP;
			bool			_support3DNOWEXT;
			bool			_support3DNOW;
		};

		// Since CPU information cannot be changed after system boot
		// We keep two global object for optimization
		const CpuInfo& GetCurrentCpuInfo();
		const CpuInfoEx& GetCurrentCpuInfoEx();

	}	// NX::win

}   // NX

#endif
