
#include "stdafx.h"

#include <nx\common\rapidjson.h>

#include <vector>

static void TestUtf8File(const std::string& file);
static void TestUtf8String(const std::string& file);
static void TestUtf16String(const std::string& file);
static void TestCreateJson();

int main(int argc, char** argv)
{
    std::cout << "RAPIDJSON Test" << std::endl;

    if (1 == argc) {
        std::cout << "    testjson.exe <jsonfile>" << std::endl;
        return 1;
    }

    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesA(argv[1])) {
        std::cout << "JSON file doesn't exist" << std::endl;
        return 1;
    }

    std::cout << std::endl;
    std::cout << "Test JSON file parser:" << std::endl;
    TestUtf8File(argv[1]);

    std::cout << std::endl;
    std::cout << "Test JSON string parser:" << std::endl;
    TestUtf8String(argv[1]);

    std::cout << std::endl;
    std::cout << "Test JSON wstring parser:" << std::endl;
    TestUtf16String(argv[1]);

    TestCreateJson();

    return 0;
}

void TestUtf8File(const std::string& file)
{
    NX::rapidjson::JsonDocument doc;
    int err_code = 0;
    size_t err_pos = 0;
    if (!doc.LoadJsonFile<char>(file, &err_code, &err_pos)) {
        std::cout << "Fail to load JSON: error = " << err_code << ", position: " << err_pos << std::endl;
    }
    else {
        std::cout << "Load JSON successfully" << std::endl;
    }
}

std::vector<char> LoadUtf8FileContent(const std::string& file)
{
    std::vector<char> buf;
    HANDLE h = INVALID_HANDLE_VALUE;
    h = ::CreateFileA(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == h) {
        std::cout << "Fail to open JSON file: error = " << GetLastError()  << std::endl;
        return std::move(buf);
    }

    DWORD dwSize = GetFileSize(h, NULL);
    if(dwSize == INVALID_FILE_SIZE) {
        std::cout << "Fail to get JSON file size: error = " << GetLastError()  << std::endl;
        CloseHandle(h); h = INVALID_HANDLE_VALUE;
        return std::move(buf);
    }
    if(dwSize == 0) {
        std::cout << "Empty JSON file" << std::endl;
        CloseHandle(h); h = INVALID_HANDLE_VALUE;
        return std::move(buf);
    }

    DWORD dwBytesRead = 0;
    buf.resize(dwSize, 0);
    if (!ReadFile(h, buf.data(), dwSize, &dwBytesRead, NULL)) {
        std::cout << "Fail to get JSON file size: error = " << GetLastError()  << std::endl;
        buf.clear();
        CloseHandle(h); h = INVALID_HANDLE_VALUE;
        return std::move(buf);
    }

    CloseHandle(h); h = INVALID_HANDLE_VALUE;
    if (dwBytesRead < dwSize) {
        std::cout << "Fail to read all JSON file content: " << (int)dwBytesRead << " of " << (int)dwSize << std::endl;
        buf.resize(dwBytesRead);
    }

    return std::move(buf);
}

void TestUtf8String(const std::string& file)
{
    NX::rapidjson::JsonDocument doc;
    int err_code = 0;
    size_t err_pos = 0;

    const std::vector<char>& buf = LoadUtf8FileContent(file);
    if (buf.empty())
        return;

    std::string s(buf.begin(), buf.end());

    if (!doc.LoadJsonString<char>(s, &err_code, &err_pos)) {
        std::cout << "Fail to load JSON: error = " << err_code << ", position: " << err_pos << std::endl;
    }
    else {
        std::cout << "Load JSON successfully" << std::endl;
    }
}

void TestUtf16String(const std::string& file)
{
    NX::rapidjson::JsonDocument doc;
    int err_code = 0;
    size_t err_pos = 0;

    const std::vector<char>& buf = LoadUtf8FileContent(file);
    if (buf.empty())
        return;

    std::string s(buf.begin(), buf.end());
    const std::wstring& ws = NX::conv::utf8toutf16(s);

    if (!doc.LoadJsonString<wchar_t>(ws, &err_code, &err_pos)) {
        std::cout << "Fail to load JSON: error = " << err_code << ", position: " << err_pos << std::endl;
    }
    else {
        std::cout << "Load JSON successfully" << std::endl;
    }
}

void TestCreateJson()
{
    NX::rapidjson::JsonDocument doc;
    if (!doc.Create()) {
        std::cout << "Fail to create JSON document: error = " << (int)GetLastError() << std::endl;
        return;
    }

    NX::rapidjson::JsonValue* root = doc.GetRoot();
    if (!root->AsObject()->set(L"Name", NX::rapidjson::JsonValue::CreateString(L"John Tyler"))) {
        std::cout << "Fail to create \"Name\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!root->AsObject()->set(L"Age", NX::rapidjson::JsonValue::CreateNumber(40))) {
        std::cout << "Fail to create \"Age\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!root->AsObject()->set(L"Height", NX::rapidjson::JsonValue::CreateNumber(5.72))) {
        std::cout << "Fail to create \"Height\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!root->AsObject()->set(L"Hair color", NX::rapidjson::JsonValue::CreateString("Black"))) {
        std::cout << "Fail to create \"Hair color\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!root->AsObject()->set(L"Home", NX::rapidjson::JsonValue::CreateString(L"2121 El Camino, San Mateo, CA 94404, USA"))) {
        std::cout << "Fail to create \"Home\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!root->AsObject()->set(L"Department", NX::rapidjson::JsonValue::CreateNull())) {
        std::cout << "Fail to create \"Department\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!root->AsObject()->set(L"Passed", NX::rapidjson::JsonValue::CreateBool(true))) {
        std::cout << "Fail to create \"Passed\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }

    NX::rapidjson::JsonValue* courses = NX::rapidjson::JsonValue::CreateObject();
    if (nullptr == courses) {
        std::cout << "Fail to create \"courses\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!root->AsObject()->set(L"Scores", courses)) {
        std::cout << "Fail to set \"courses\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if(!courses->AsObject()->set(L"Math", NX::rapidjson::JsonValue::CreateBool(false))) {
        std::cout << "Fail to create \"Math\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if(!courses->AsObject()->set(L"English", NX::rapidjson::JsonValue::CreateString("A+"))) {
        std::cout << "Fail to create \"English\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if(!courses->AsObject()->set(L"Art", NX::rapidjson::JsonValue::CreateNumber(99))) {
        std::cout << "Fail to create \"Art\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }

    NX::rapidjson::JsonValue* teachers = NX::rapidjson::JsonValue::CreateArray();
    if (nullptr == teachers) {
        std::cout << "Fail to create \"teachers\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!root->AsObject()->set(L"Teachers", teachers)) {
        std::cout << "Fail to set \"teachers\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!teachers->AsArray()->push_back(NX::rapidjson::JsonValue::CreateString("Abraham Lincoln"))) {
        std::cout << "Fail to create teacher \"Abraham Lincoln\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!teachers->AsArray()->push_back(NX::rapidjson::JsonValue::CreateString("Jimmy Carter"))) {
        std::cout << "Fail to create teacher \"Jimmy Carter\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }
    if (!teachers->AsArray()->push_back(NX::rapidjson::JsonValue::CreateString(L"楊振寧 / Chen-Ning Franklin Yang"))) {
        std::cout << "Fail to create teacher \"Chen-Ning Franklin Yang\" node: error = " << (int)GetLastError() << std::endl;
        return;
    }

    NX::rapidjson::StringWriter<wchar_t> swriter;
    const std::wstring& ws = swriter.Write(doc.GetRoot());

    NX::rapidjson::FileWriter fwriter;
    if (!fwriter.Write(L"D:\\temp\\test.json", doc.GetRoot(), NX::rapidjson::WriterType::Formatted)) {
        std::cout << "Fail to create well-formatted json file: error = " << (int)GetLastError() << std::endl;
    }
    else {
        std::cout << "Well-formatted json file has been created " << std::endl;
    }

}

