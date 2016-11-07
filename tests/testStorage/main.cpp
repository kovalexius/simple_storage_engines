#include <string>
#include <iostream>
#include "storage.h"

int main()
{
	crud_storage::Storage strg;
	strg.Create("key1", "123kazachok", 2, 3.3, "str");

	std::string arg1("123kazachok");
	auto arg2 = 2;
	auto arg3 = 3.3f;
	std::string arg4("str");

	auto strData1 = crud_storage::toBuffer(arg1, arg2, arg3, arg4);
	auto strData2 = crud_storage::toBuffer(arg2, arg4, arg1, arg3); // change order of arguments
	if (strData1 != strData2)
	{
		std::cout << "Test1 passed" << std::endl;
	}
	else
	{
		std::cout << "Test1 failed" << std::endl;
	}

	strData1 = crud_storage::toBuffer(arg1, arg2, arg3, arg4);
	strData2 = crud_storage::toBuffer(arg1, arg2, arg3, arg4);
	if (strData1 == strData2)
	{
		std::cout << "Test2 passed" << std::endl;
	}
	else
	{
		std::cout << "Test2 failed" << std::endl;
	}

	return 0;
}
