// dllmain.h : Declaration of module class.

class CcomdllprojectModule : public ATL::CAtlDllModuleT< CcomdllprojectModule >
{
public :
	DECLARE_LIBID(LIBID_comdllprojectLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMDLLPROJECT, "{A1D27302-1992-424B-A61E-F0C13610F54E}")
};

extern class CcomdllprojectModule _AtlModule;
