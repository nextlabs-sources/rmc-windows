

#include <windows.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include <nudf\string.hpp>
#include <nudf\crypto.hpp>
#include <nudf\filesys.hpp>


int wmain(int argc, wchar_t** argv)
{
    if (argc != 2) {
        std::wcout << L"Usage:" << std::endl;
        std::wcout << L"    " << argv[0] << L" <file>" << std::endl;
        return -1;
    }
    
    const std::wstring file(argv[1]);

    NX::fs::file_checksum_crc32 fcrc32;
    if (!fcrc32.generate(file)) {
        std::cout << "Fail to calculate CRC32 checksum" << std::endl;
    }
    else {
        std::wcout << L"  CRC32: " << fcrc32.serialize() << std::endl;
        std::wcout << L"  CRC32 (9.0 or earlier): " << (int)((unsigned long)fcrc32.get_checksum()) << std::endl;
    }

    NX::fs::file_checksum_sha1 fsha1;
    if (!fsha1.generate(file)) {
        std::cout << "Fail to calculate SHA1 checksum" << std::endl;
    }
    else {
        std::wcout << L"  SHA1:  " << fsha1.serialize() << std::endl;
    }

    return 0;
}