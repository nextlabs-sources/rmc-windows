

#ifndef __NXRMC_HEARTBEAT_H__
#define __NXRMC_HEARTBEAT_H__


#include <string>

namespace NX {

	class RmHeartbeatObject
	{
	public:
		RmHeartbeatObject();
		RmHeartbeatObject(const std::wstring& name, const std::wstring& serial, const std::wstring& content);
		RmHeartbeatObject(const RmHeartbeatObject& rhs);
		~RmHeartbeatObject();

		RmHeartbeatObject& operator = (const RmHeartbeatObject& rhs);

		std::string serialize() const;

		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetSerial() const { return _serial; }
		inline const std::wstring& GetData() const { return _data; }

	private:
		std::wstring	_name;
		std::wstring	_serial;
		std::wstring	_data;
	};

}


#endif
