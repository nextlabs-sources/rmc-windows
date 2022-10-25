

#ifndef __NX_WINUTIL_DISK_H__
#define __NX_WINUTIL_DISK_H__

#include <nx\common\result.h>

namespace NX {

	namespace win {

		class DriveSpace
		{
		public:
			DriveSpace() : _total(0), _totalFfree(0), _avaliableFree(0) {}
			DriveSpace(const DriveSpace& rhs) : _total(rhs._total), _totalFfree(rhs._totalFfree), _avaliableFree(rhs._avaliableFree) {}
			DriveSpace(unsigned __int64 total, unsigned __int64 totalFree, unsigned __int64 availableFree) : _total(total), _totalFfree(totalFree), _avaliableFree(availableFree) {}
			~DriveSpace() {}

			static DriveSpace QueryDriveSpace(const wchar_t letter);

			inline unsigned __int64 GetTotal() const { return _total; }
			inline unsigned __int64 GetTotalFree() const { return _totalFfree; }
			inline unsigned __int64 GetAvaliableFree() const { return _avaliableFree; }
			inline DriveSpace& operator = (const DriveSpace& rhs)
			{
				if (this != &rhs) {
					_total = rhs._total;
					_totalFfree = rhs._totalFfree;
					_avaliableFree = rhs._avaliableFree;
				}
				return *this;
			}

		private:
			unsigned __int64    _total;
			unsigned __int64    _totalFfree;
			unsigned __int64    _avaliableFree;
		};

		class DriveInfo
		{
		public:
			DriveInfo();
			DriveInfo(const wchar_t letter);
			DriveInfo(const DriveInfo& rhs);
			virtual ~DriveInfo();

			DriveInfo& operator = (const DriveInfo& rhs);

			inline bool Empty() const { return (0 == _letter); }
			inline wchar_t GetLetter() const { return _letter; }
			inline unsigned int GetType() const { return _type; }
			inline const std::wstring& GetTypeName() const { return _typeName; }
			inline const std::wstring& GetDosName() const { return _dosName; }
			inline const std::wstring& GetNtName() const { return _ntName; }

			bool IsValid() const;
			bool IsRemovable() const;
			bool IsFixed() const;
			bool IsRemote() const;
			bool IsCdrom() const;
			bool IsRamdisk() const;

			void Clear();
			DriveSpace GetSpace() const;

		protected:
			unsigned int QueryDriveType(const wchar_t driveLetter);
			std::wstring DriveTypeToName(unsigned int type);
			void GetNames();

		private:
			wchar_t         _letter;
			unsigned int    _type;
			std::wstring    _typeName;
			std::wstring    _dosName;
			std::wstring    _ntName;
		};

		std::vector<DriveInfo> GetLogicDrives();

		class LogicDriveFinder
		{
		public:
			LogicDriveFinder() {}
			~LogicDriveFinder() {};
			DriveInfo Find();

		protected:
			virtual bool Match(const DriveInfo& info) = 0;
		};

	}	// NX::win

}   // NX

#endif
