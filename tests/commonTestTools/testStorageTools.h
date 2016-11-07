#ifndef __TEST_STORAGE_TOOLS_H
#define __TEST_STORAGE_TOOLS_H

#include "disk_storage.h"

void InsertRecords(storage_impl::DiskStorage& _strg,
                   const int _numIterations,
                   const std::string& _keyPattern,
                   const std::string& _dataPattern);

void DeleteRange(storage_impl::DiskStorage& _strg,
                 const std::string& _keyPattern,
                 const int _begin,
                 const int _end);

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
