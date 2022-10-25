
#include "stdafx.h"

#include <string>
#include <vector>

#include <nx\rmc\keym.h>
#include <nx\common\string.h>
#include <nx\common\time.h>

using namespace NX;

int main(int argc, char** argv)
{
    NX::RmClientId clientId;
	bool machineStore = (argc == 2 && 0 == _stricmp(argv[1], "-c")) ? false : true;

    Result res = clientId.Load(machineStore);
    if (!res) {
        if (res.GetCode() != ERROR_NOT_FOUND) {
            printf("Fail to load registered client id (err:%d, file:%s, line:%d, function:%s, msg:%s)\n",
                res.GetCode(), res.GetFile(), res.GetLine(), res.GetFunction(), res.GetMsg().c_str());
            return res.GetCode();
        }

        res = clientId.Create(machineStore);
        if (!res) {
            printf("Fail to register client id (err:%d, file:%s, line:%d, function:%s, msg:%s)\n",
                res.GetCode(), res.GetFile(), res.GetLine(), res.GetFunction(), res.GetMsg().c_str());
            return res.GetCode();
        }

        std::cout << "Register ClientId:" << std::endl;
    }
    else {
        std::cout << "Load existing ClientId:" << std::endl;
    }

    const std::wstring& clientKey = NX::bintohs<wchar_t>(clientId.GetClientKey().data(), clientId.GetClientKey().size());
    std::wcout << L"    Id:  " << clientId .GetClientId() << std::endl;
    std::wcout << L"    Key: " << clientKey << std::endl;
    return 0;
}