#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <vector>
#include <iostream>

#include <filesystem>

#include <set>

#include "testStorageTools.h"
#include "disk_storage.h"

// See https://github.com/philsquared/Catch/blob/master/docs/tutorial.md
// to know how to use CatchTests. SCENARIO, GIVEN, WHEN, THEN - this is
// special CatchTests macroses to use with BDD Style

// Убрать дублирование кода в этих тестах (копипасту), 
// сделать распечатку действий внутри вызываемых функций.
// Расширить tools

SCENARIO("Test Create(), Read() and Delete()", "[Basic]")
{
	GIVEN("Clean DataBase")
	{
		std::string bdName = "catchTest";
		std::tr2::sys::remove(std::tr2::sys::path(bdName + "_index"));
		std::tr2::sys::remove(std::tr2::sys::path(bdName + "_data"));

		storage_impl::DiskStorage strg(bdName, false);

		WHEN("Create()")
		{
			auto numIterations = 1000;
			std::cout << numIterations << " Create()'s" << std::endl;

			InsertRecords(strg, numIterations, "key", "Hello World, Guy");

			THEN("Read()")
			{
				std::cout << numIterations << " Read()'s" << std::endl;
				for (auto i = 0; i < numIterations; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());
					std::vector<char> data{0};
					strg.Read(key, data);
					std::ostringstream requiredStrm;
					requiredStrm << "Hello World, Guy" << i;
					REQUIRE(std::string(data.begin(), data.end()) == requiredStrm.str()) ;
				}
				std::vector<std::vector<char>> keyList;
				strg.getKeyList(keyList);
				REQUIRE(keyList.size() == numIterations);
				std::set<std::vector<char>> keySet(keyList.begin(), keyList.end());
				for (auto i = 0; i < numIterations; ++i)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto requiredKey = keyStrm.str();
					std::vector<char> key(requiredKey.begin(), requiredKey.end());
					auto it = keySet.find(key);
					REQUIRE(it != keySet.end());
				}
			}
		}

		WHEN("Create() and partial Delete()")
		{
			auto numIterations = 1000;
			std::cout << numIterations << " Create()'s" << std::endl;

			InsertRecords(strg, numIterations, "key", "Hello World, Guy");

			auto numToDelete = 100;

			std::cout << numToDelete << " Delete()'s since the begin" << std::endl;
			DeleteRange(strg, "key", 0, numToDelete);
			std::cout << numToDelete << " Delete()'s since the end" << std::endl;
			DeleteRange(strg, "key", numIterations - numToDelete, numIterations);

			THEN("Read()")
			{
				for (auto i = numToDelete; i < numIterations - numToDelete; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());
					std::vector<char> data{0};
					strg.Read(key, data);
					std::ostringstream requiredStrm;
					requiredStrm << "Hello World, Guy" << i;
					REQUIRE(std::string(data.begin(), data.end()) == requiredStrm.str()) ;
				}
				for (auto i = 0; i < numToDelete; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());
					std::vector<char> data{0};
					REQUIRE(strg.Read(key, data) == false) ;
				}

				for (auto i = numIterations - numToDelete; i < numIterations; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());
					std::vector<char> data{0};
					REQUIRE(strg.Read(key, data) == false) ;
				}
			}
		}

		WHEN("Create() then partial Update()")
		{
			auto numIterations = 1000;
			std::cout << numIterations << " Create()'s" << std::endl;
			InsertRecords(strg, numIterations, "key", "Hello World, Guy");

			auto numToUpdate = 100;

			std::cout << numToUpdate << " Update()'s in the middle" << std::endl;
			UpdateRange(strg, "key", "Good Buy", 0, numToUpdate);
			THEN("Read()")
			{
				for (auto i = 0; i < numToUpdate; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());
					std::vector<char> data{0};
					strg.Read(key, data);
					std::ostringstream requiredStrm;
					requiredStrm << "Good Buy" << i;
					REQUIRE(std::string(data.begin(), data.end()) == requiredStrm.str());
				}

				for (auto i = numToUpdate; i < numIterations; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());
					std::vector<char> data{0};
					strg.Read(key, data);
					std::ostringstream requiredStrm;
					requiredStrm << "Hello World, Guy" << i;
					REQUIRE(std::string(data.begin(), data.end()) == requiredStrm.str());
				}
			}
		}
	}
}
