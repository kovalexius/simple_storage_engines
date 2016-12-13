#ifndef __TEST_STORAGE_TOOLS_H
#define __TEST_STORAGE_TOOLS_H

#include "disk_storage.h"

void InsertRecords(storage_impl::DiskStorage& _strg,
                   const int _numIterations,
                   const std::string& _keyPattern,
                   const std::string& _dataPattern);

void InsertMultikeyRecords(storage_impl::DiskStorage& _strg,
						   const int _numRepeations,
						   const int _numIterations,
						   const std::string& _keyPattern,
						   const std::string& _dataPattern);

void DeleteRange(storage_impl::DiskStorage& _strg,
                 const std::string& _keyPattern,
                 const int _begin,
                 const int _end);

void DeleteRangeWithPredicate(storage_impl::DiskStorage& _strg,
	const std::string& _keyPattern,
	const int _begin,
	const int _end,
	std::function<bool(const std::vector<char>&)> _predicate);

void ReadRange(storage_impl::DiskStorage& _strg,
               const std::string& _keyPattern,
               const int _begin,
               const int _end);

void UpdateRange(storage_impl::DiskStorage& _strg,
                 const std::string& _keyPattern,
                 const std::string& _dataPattern,
                 const int _begin,
                 const int _end);

#endif
