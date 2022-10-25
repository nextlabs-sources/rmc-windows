

#ifndef __NX_WINUTIL_MEMORY_H__
#define __NX_WINUTIL_MEMORY_H__


namespace NX {

	namespace win {

		class MemoryStatus
		{
		public:
			MemoryStatus();
			MemoryStatus(const MemoryStatus& rhs);
			MemoryStatus(MemoryStatus&& rhs);
			virtual ~MemoryStatus();

			MemoryStatus& operator = (const MemoryStatus& rhs);
			MemoryStatus& operator = (MemoryStatus&& rhs);

			inline unsigned int GetLoad() const { return _status.dwMemoryLoad; }
			inline unsigned __int64 GetTotalPhisical() const { return _status.ullTotalPhys; }
			inline unsigned __int64 GetAvailablePhisical() const { return _status.ullAvailPhys; }
			inline unsigned __int64 GetTotalPageFile() const { return _status.ullTotalPageFile; }
			inline unsigned __int64 GetAvailablePageFile() const { return _status.ullAvailPageFile; }
			inline unsigned __int64 GetTotalVirtual() const { return _status.ullTotalVirtual; }
			inline unsigned __int64 GetAvailableVirtual() const { return _status.ullAvailVirtual; }
			inline unsigned __int64 GetAvailableExtendedVirtual() const { return _status.ullAvailExtendedVirtual; }

		private:
			MEMORYSTATUSEX  _status;
		};

	}	// NX::win

}   // NX

#endif
