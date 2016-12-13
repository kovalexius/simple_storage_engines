#include "testStorageTools.h"

#include <sstream>
#include <functional>

void InsertRecords(storage_impl::DiskStorage& _strg,
                   const int _numIterations,
                   const std::string& _keyPattern,
                   const std::string& _dataPattern)
{
	for (auto i = 0; i < _numIterations; ++i)
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

void InsertMultikeyRecords(storage_impl::DiskStorage& _strg,
	const int _numRepeations,
	const int _numIterations,
	const std::string& _keyPattern,
	const std::string& _dataPattern)
{
	for (auto i = 0; i < _numIterations; ++i)
	{
		std::ostringstream keyStrm;
		keyStrm << _keyPattern << i;
		auto keyString = keyStrm.str();
		std::vector<char> key(keyString.begin(), keyString.end());
		for (auto j = 0; j < _numRepeations; ++j)
		{
			std::ostringstream dataStrm;
			dataStrm << _dataPattern << ";" << i << ";" << j;
			auto dataString = dataStrm.str();
			std::vector<char> data(dataString.begin(), dataString.end());
			_strg.Create(key, data);
		}
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

void DeleteRangeWithPredicate(storage_impl::DiskStorage& _strg, 
	const std::string& _keyPattern, 
	const int _begin,
	const int _end,
	std::function<bool(const std::vector<char>&)> _predicate)
{
	for (auto i = _begin; i < _end; i++)
	{
		std::ostringstream keyStrm;
		keyStrm << _keyPattern << i;
		auto keyString = keyStrm.str();
		std::vector<char> key(keyString.begin(), keyString.end());
		_strg.Delete(key, _predicate);
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
