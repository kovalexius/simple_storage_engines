#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <vector>
#include <iostream>

#include <filesystem>
#include <set>

#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "testStorageTools.h"
#include "disk_storage.h"

// See https://github.com/philsquared/Catch/blob/master/docs/tutorial.md
// to know how to use CatchTests. SCENARIO, GIVEN, WHEN, THEN - this is
// special CatchTests macroses to use with BDD Style

// Убрать дублирование кода в этих тестах (копипасту), 
// сделать распечатку действий внутри вызываемых функций.
// Расширить tools

SCENARIO("Test HashStorage Create(), Read() and Delete()", "[Basic]")
{
	GIVEN("Clean DataBase")
	{
		std::string bdName = "catchTest";
		std::tr2::sys::remove(std::tr2::sys::path(bdName + "_index"));
		std::tr2::sys::remove(std::tr2::sys::path(bdName + "_data"));

		storage_impl::DiskStorage strg(bdName, storage_impl::STORAGE_TYPES::HASH_STORAGE, false);

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

				// Test getKeyList() method
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
				// Check that rest part of records exists and is not damaged
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

				// Check that deleted keys from 0 to numToDelete not exists
				for (auto i = 0; i < numToDelete; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());
					std::vector<char> data{0};
					REQUIRE(strg.Read(key, data) == false) ;
				}

				// Check that deleted keys from tail not exists
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


void splitString(const std::string& _in, const std::string& _delimiter, std::vector<std::string>& _out)
{
	boost::split(_out, _in, boost::is_any_of(_delimiter)); // Works incorrect - adds empty string to vector if several L'\0' exists
	//_out.clear();
	//size_t prev_pos = 0;
	//size_t pos = 0;
	//std::string substring;

	//while (pos != std::string::npos)
	//{
	//	pos = _in.find(_delimiter, prev_pos);
	//	substring.assign(_in.substr(prev_pos, pos - prev_pos));
	//	if (!substring.empty() && substring != std::string(1, '\0'))
	//		_out.push_back(substring);
	//	prev_pos = pos + 1;
	//}
}

struct OddPredicate
{
	OddPredicate(const std::string& _dataPattern) : m_dataPattern(_dataPattern)
	{}
	bool operator() (const std::vector<char>& _in)
	{
		std::string inStr(_in.begin(), _in.end());
		std::vector<std::string> out;
		splitString(inStr, ";", out);
		if (out.size() < 3)
			return false;
		if (out[0] != m_dataPattern)
			return false;

		auto number = std::stoi(out[2]);

		return (number % 2);
	}
private:
	std::string m_dataPattern;
};

struct SelectNumberPredicate
{
	SelectNumberPredicate(const std::string& _dataPattern, int _number) : m_dataPattern(_dataPattern), 
																		  m_number(_number)
	{}
	bool operator() (const std::vector<char>& _in)
	{
		std::string inStr(_in.begin(), _in.end());
		std::vector<std::string> out;
		splitString(inStr, ";", out);
		if (out.size() < 3)
			return false;
		if (out[0] != m_dataPattern)
			return false;

		auto number = std::stoi(out[2]);

		return (number == m_number);
	}
private:
	std::string m_dataPattern;
	int m_number;
};

SCENARIO("Test MultikeyStorage Create(), Read() and Delete()", "[Basic]")
{
	GIVEN("Clean DataBase")
	{
		std::string dataPattern("Hello World, Guy");
		std::string bdName = "catchTest";
		std::tr2::sys::remove(std::tr2::sys::path(bdName + "_index"));
		std::tr2::sys::remove(std::tr2::sys::path(bdName + "_data"));

		storage_impl::DiskStorage strg(bdName, storage_impl::STORAGE_TYPES::MULTIKEY_STORAGE, false);

		WHEN("Create() and Read() list of records")
		{
			auto numRepeations = 10;
			auto numIterations = 100;
			std::cout << numIterations << " Create()'s" << std::endl;

			InsertMultikeyRecords(strg, numRepeations, numIterations, "key", dataPattern);
			THEN("Read() list of records")
			{
				std::cout << numIterations << " Read()'s" << std::endl;
				for (auto i = 0; i < numIterations; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());

					std::vector<std::vector<char>> datas;
					strg.Read(key, datas);

					REQUIRE(datas.size() == numRepeations);

					for (size_t j = 0; j < numRepeations; ++j)
					{
						std::ostringstream requiredStrm;
						requiredStrm << dataPattern << ";" << i << ";" << j;
						REQUIRE(std::string(datas[j].begin(), datas[j].end()) == requiredStrm.str());
					}
				}

				// Test getKeyList() method
				std::vector<std::vector<char>> keyList;
				strg.getKeyList(keyList);
				REQUIRE(keyList.size() == numIterations);
			}
		}

		WHEN("Create() and Read() every fifth by predicate")
		{
			auto numRepeations = 10;
			auto numIterations = 100;
			std::cout << numIterations << " Create()'s" << std::endl;

			InsertMultikeyRecords(strg, numRepeations, numIterations, "key", dataPattern);

			THEN("Read() by predicate")
			{
				std::cout << numIterations << " Read()'s" << std::endl;
				for (auto i = 0; i < numIterations; i++)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());

					std::vector<char> data;
					strg.Read(key, data, SelectNumberPredicate(dataPattern, 5));

					std::ostringstream requiredStrm;
					requiredStrm << dataPattern << ";" << i << ";" << 5;

					REQUIRE(std::string(data.begin(), data.end()) == requiredStrm.str());
				}
			}
		}

		WHEN("Create() and partial Delete() without predicate")
		{
			auto numRepeations = 10;
			auto numIterations = 100;
			std::cout << numIterations << " Create()'s" << std::endl;

			InsertMultikeyRecords(strg, numRepeations, numIterations, "key", dataPattern);

			auto numToDelete = 10;

			std::cout << numToDelete << " Delete()'s since the begin" << std::endl;
			DeleteRange(strg, "key", 0, numToDelete);

			THEN("Read group of records")
			{
				// Check that rest part of records exists and is not damaged
				for (auto i = numToDelete; i < numIterations; ++i)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());

					std::vector<std::vector<char>> datas;
					strg.Read(key, datas);

					for (auto j = 0; j < numRepeations; ++j)
					{
						std::ostringstream requiredStrm;
						requiredStrm << dataPattern << ";" << i << ";" << j;
						REQUIRE(std::string(datas[j].begin(), datas[j].end()) == requiredStrm.str());
					}
				}

				// Check that deleted keys from 0 to numToDelete not exists
				for (auto i = 0; i < numToDelete; ++i)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());

					std::vector<std::vector<char>> datas;
					REQUIRE(strg.Read(key, datas) == false);
				}
			}
		}

		WHEN("Create() and partial Delete() with predicate")
		{
			auto numRepeations = 10;
			auto numIterations = 50;
			std::cout << numIterations << " Create()'s" << std::endl;

			InsertMultikeyRecords(strg, numRepeations, numIterations, "key", dataPattern);

			auto numToDelete = 10;

			std::cout << numToDelete << " OddPredicate Delete()'s since the begin" << std::endl;
			DeleteRangeWithPredicate(strg, "key", 0, numToDelete, OddPredicate(dataPattern));

			THEN("Read group of records")
			{
				// Check that rest part of records exists and is not damaged
				for (auto i = numToDelete; i < numIterations; ++i)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());

					std::vector<std::vector<char>> datas;
					strg.Read(key, datas);

					for (auto j = 0; j < numRepeations; ++j)
					{
						std::ostringstream requiredStrm;
						requiredStrm << dataPattern << ";" << i << ";" << j;
						std::string factData(datas[j].begin(), datas[j].end());
						REQUIRE(factData == requiredStrm.str());
					}
				}

				// Check that deleted keys from 0 to numToDelete is Odded
				for (auto i = 0; i < numToDelete; ++i)
				{
					std::ostringstream keyStrm;
					keyStrm << "key" << i;
					auto keyString = keyStrm.str();
					std::vector<char> key(keyString.begin(), keyString.end());

					std::vector<std::vector<char>> datas;
					REQUIRE(strg.Read(key, datas) == true);

					REQUIRE(datas.size() == numRepeations / 2);

					for (auto j = 0; j < numRepeations; ++j)
					{
						if (!(j % 2))
						{
							std::ostringstream requiredStrm;
							requiredStrm << dataPattern << ";" << i << ";" << j;
							std::string factData(datas[j / 2].begin(), datas[j / 2].end());
							REQUIRE(factData == requiredStrm.str());
						}
					}
				}
			}
		}
	}
}