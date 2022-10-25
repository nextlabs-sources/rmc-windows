

#ifndef __NX_WINUTIL_PROCESS_H__
#define __NX_WINUTIL_PROCESS_H__

#include <nx\common\result.h>

namespace NX {

	namespace win {

		class ProcessInfo
		{
		public:
			ProcessInfo();
			ProcessInfo(unsigned int pid);
			ProcessInfo(const ProcessInfo& rhs);
			virtual ~ProcessInfo();

			ProcessInfo& operator = (const ProcessInfo& rhs);
			void clear();

			inline bool empty() const { return _name.empty(); }
			inline unsigned int GetProcessId() const { return _id; }
			inline unsigned int GetSessionId() const { return _sId; }
			inline std::wstring GetName() const { return _name; }
			inline std::wstring GetImage() const { return _image; }
			inline bool Is64bits() const { return _is64bit; }

		private:
			unsigned int _id;
			unsigned int _sId;
			std::wstring _name;
			std::wstring _image;
			bool         _is64bit;
		};

		class ProcessEnvironment
		{
		public:
			typedef std::vector<std::pair<std::wstring, std::wstring>> data_type;
			typedef std::pair<std::wstring, std::wstring> Variable;
			typedef data_type::iterator	iterator;
			typedef data_type::const_iterator	const_iterator;

			ProcessEnvironment();
			ProcessEnvironment(const std::vector<std::pair<std::wstring, std::wstring>>& envs, bool merge = false);
			ProcessEnvironment(const ProcessEnvironment& rhs);
			virtual ~ProcessEnvironment();

			ProcessEnvironment& operator = (const ProcessEnvironment& rhs);

			std::wstring GetVariable(const std::wstring& key) const;
			void SetVariable(const std::wstring& key, const std::wstring& value, bool merge = false);
			std::vector<wchar_t> CreateBlock() const;

			inline bool empty() const { return _envs.empty(); }
			inline void clear() { _envs.clear(); }
			inline const data_type& GetEnvStrings() const { return _envs; }
			inline iterator begin() { _envs.begin(); }
			inline iterator end() { _envs.end(); }
			inline const_iterator cbegin() const { _envs.cbegin(); }
			inline const_iterator cend() const { _envs.cend(); }

		protected:
			static std::vector<std::pair<std::wstring, std::wstring>> Init(const std::vector<std::pair<std::wstring, std::wstring>>& envs, bool merge = false);

		private:
			std::vector<std::pair<std::wstring, std::wstring>>	_envs;
		};

	}	// NX::win

}   // NX

#endif
