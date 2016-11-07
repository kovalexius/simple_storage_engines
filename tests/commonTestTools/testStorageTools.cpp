#include "testStorageTools.h"

#include <sstream>

void InsertRecords(storage_impl::DiskStorage& _strg,
                   const int _numIterations,
                   const std::string& _keyPattern,
                   const std::string& _dataPattern)
{
	for (auto i = 0; i < _numIterations; i++)
	{
		std::ostringstream keyStrm;
		keyStrm << _keyPattern << i;
		auto keyString = keyStrm.str();
		std::vector<char> key(keyString.begin(), keyString.end());
		std::ostringstream dataStrm;
		dataStrm << _dataPattern << i;
		auto dataString = dataStrm.str();
		std::vector<char> data(dataString.begin(), dataString.end());
		_strg.Create(key, data);
	}
}

void DeleteRange(storage_impl::DiskStorage& _strg,
                 const std::string& _keyPattern,
                 const int _begin,
                 const int _end)
{
	for (auto i = _begin; i < _end; i++)
	{
		std::ostringstream keyStrm;
		keyStrm << _keyPattern << i;
		auto keyString = keyStrm.str();
		std::vector<char> key(keyString.begin(), keyString.end());
		_strg.Delete(key);
	}
}

void ReadRange(storage_impl::DiskStorage& _strg,
               const std::string& _keyPattern,
               const int _begin,
               const int _end)
{
	for (auto i = _begin; i < _end; i++)
	{
		std::ostringstream keyStrm;
		keyStrm << _keyPattern << i;
		auto keyString = keyStrm.str();
		std::vector<char> key(keyString.begin(), keyString.end());
		std::vector<char> data;
		_strg.Read(key, data);
	}
}

void UpdateRange(storage_impl::DiskStorage& _strg,
                 const std::string& _keyPattern,
                 const std::string& _dataPattern,
                 const int _begin,
                 const int _end)
{
	for (auto i = _begin; i < _end; i++)
	{
		std::ostringstream keyStrm;
		keyStrm << _keyPattern << i;
		auto keyString = keyStrm.str();
		std::vector<char> key(keyString.begin(), keyString.end());
		std::ostringstream dataStrm;
		dataStrm << _dataPattern << i;
		auto dataString = dataStrm.str();
		std::vector<char> data(dataString.begin(), dataString.end());
		_strg.Update(key, data);
	}
}
