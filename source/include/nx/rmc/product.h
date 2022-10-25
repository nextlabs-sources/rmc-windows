

#ifndef __NXRMC_PRODUCT_H__
#define __NXRMC_PRODUCT_H__


#include <nx\common\result.h>
#include <string>

namespace NX {

	class Product
	{
	public:
		Product(const std::wstring& name);
		Product(const Product& rhs);
		virtual ~Product();

		Result Load();
		ULONG VersionToNumber() const;
		Product& operator = (const Product& rhs);

		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetVersion() const { return _version; }
		inline const std::wstring& GetPublisher() const { return _publisher; }
		inline const std::wstring& GetInstallLocation() const { return _installLoc; }

	protected:
		Result LoadFromFile();
		Result LoadFromRegistry();

	protected:
		Product() {}

	private:
		std::wstring	_name;
		std::wstring	_version;
		std::wstring	_publisher;
		std::wstring	_installLoc;
	};

	class RmProduct : public Product
	{
	public:
		RmProduct() : Product(L"SkyDRM") {}
		virtual ~RmProduct() {}
	};
}


#endif
