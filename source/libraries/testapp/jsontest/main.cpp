

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <memory>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>

using namespace NX;
using namespace NX::rapidjson;

PJSONVALUE create_json();
bool create_json_file(const std::wstring& ws);
void test_load_string(const std::wstring& ws);
void test_load_file(const std::wstring& file);
std::wstring test_serialize(PCJSONVALUE pv);

const wchar_t* fileName = L"test.json";

int main(int argc, char** argv)
{
	PJSONVALUE root = create_json();
	if (NULL == root)
		return -1;

	const std::wstring& ws = test_serialize(root);
	if (!create_json_file(ws))
		return -1;

	do {

		int a = 0;

		std::cout << "Test 0: create" << std::endl;
		for (int i = 0; i < 100; i++) {
			a = i;
			std::shared_ptr<JsonValue> sp(create_json());
			JsonStringWriter<wchar_t> writer;
			const std::wstring& ws2 = writer.Write(sp.get());
			a = i + 1;
		}
		std::cout << "Done" << std::endl;

		std::cout << "Test 1: serialization" << std::endl;
		for (int i = 0; i < 100; i++) {
			a = i;
			const std::wstring& ws = test_serialize(root);
			a = i + 1;
		}
		std::cout << "Done" << std::endl;

		std::cout << "Test 2: load from string" << std::endl;
		for (int i = 0; i < 100; i++) {
			a = i;
			test_load_string(ws);
			a = i + 1;
		}
		std::cout << "Done" << std::endl;

		std::cout << "Test 3: load from file" << std::endl;
		for (int i = 0; i < 100; i++) {
			a = i;
			test_load_file(fileName);
			a = i + 1;
		}
		std::cout << "Done" << std::endl;

		std::cout << "Test 3: load from file" << std::endl;
		for (int i = 0; i < 100; i++) {
			a = i;
			test_load_file(L"d:\\temp\\tokens.json");
			a = i + 1;
		}
		std::cout << "Done" << std::endl;

		
	} while (FALSE);

	delete root;
	return 0;
}

PJSONVALUE create_json()
{
	PJSONOBJECT root = JsonValue::CreateObject();
	if (!IsValidJsonObject(root))
		return NULL;

	root->set(L"statusCode", JsonValue::CreateNumber(200));
	root->set(L"message", JsonValue::CreateString(L"OK"));
	root->set(L"serverTime", JsonValue::CreateNumber(1484537953900LL));

	PJSONOBJECT results = JsonValue::CreateObject();
	if (IsValidJsonObject(results)) {
		
		root->set(L"results", results);

		PJSONARRAY detail = JsonValue::CreateArray();
		if (IsValidJsonArray(detail)) {

			results->set(L"detail", detail);

			for (int i = 0; i < 10; i++) {

				PJSONOBJECT user = JsonValue::CreateObject();
				if (!IsValidJsonObject(user))
					continue;

				detail->push_back(user);
				user->set(L"id", JsonValue::CreateNumber(i));
				user->set(L"name", JsonValue::CreateString(std::wstring(L"user-" + std::to_wstring(i))));
				user->set(L"description", JsonValue::CreateString(std::wstring(L"This is user " + std::to_wstring(i))));
				user->set(L"displayName", JsonValue::CreateString(std::wstring(L"User " + std::to_wstring(i))));
			}

		}
	}

	return root;
}

bool create_json_file(const std::wstring& ws)
{
	::DeleteFileW(fileName);

	const std::string& s = conv::utf16toutf8(ws);
	std::ofstream fp;
	fp.open(fileName);
	if (!fp.is_open() || !fp.good()) {
		::DeleteFileW(fileName);
		return false;
	}

	fp << s;
	fp.close();
	return true;
}

void test_load_string(const std::wstring& ws)
{
	JsonDocument doc;
	if (!doc.LoadJsonString(ws))
		return;

	PCJSONVALUE root = doc.GetRoot();
	JsonStringWriter<wchar_t> writer;
	
	const std::wstring& ws2 = writer.Write(root);
	bool result = false;
	if(ws == ws2)
		result = true;
}

void test_load_file(const std::wstring& file)
{
	JsonDocument doc;
	if (!doc.LoadJsonFile<char>(file))
		return;
	
	PCJSONVALUE root = doc.GetRoot();
	JsonStringWriter<wchar_t> writer;
	
	const std::wstring& ws2 = writer.Write(root);
	bool result = false;
}

std::wstring test_serialize(PCJSONVALUE pv)
{
	JsonStringWriter<wchar_t> writer;
	return writer.Write(pv);
}
