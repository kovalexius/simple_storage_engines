#include <string>
#include <iostream>
#include <filesystem>
#include <chrono>

#include "disk_storage.h"
#include "testStorageTools.h"


// Убрать дублирование кода в этих тестах

void CleanAndReinit(int _numIterations, bool _speedup)
{
	std::string bdName = "test_perfomance";
	std::tr2::sys::remove(std::tr2::sys::path(bdName + "_index"));
	std::tr2::sys::remove(std::tr2::sys::path(bdName + "_data"));
	storage_impl::DiskStorage strg(bdName, storage_impl::STORAGE_TYPES::HASH_STORAGE, _speedup);

	std::cout << _numIterations << " Creates()" << std::endl;
	auto startTime = std::chrono::system_clock::now();
	InsertRecords(strg, _numIterations, "key", "Hello world Guy");
	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<double> diffTime = endTime - startTime;
	std::cout << "Interval: " << diffTime.count() << std::endl << std::endl;
}

void OpenAndRead(int _numIterations, bool _speedup)
{
	std::string bdName = "test_perfomance";
	storage_impl::DiskStorage strg(bdName, storage_impl::STORAGE_TYPES::HASH_STORAGE, _speedup);

	std::cout << _numIterations << " Read()" << std::endl;

	auto startTime = std::chrono::system_clock::now();
	ReadRange(strg, "key", 0, _numIterations);
	auto endTime = std::chrono::system_clock::now();

	std::chrono::duration<double> diffTime = endTime - startTime;
	std::cout << "Interval: " << diffTime.count() << std::endl << std::endl;
}

int main()
{
	CleanAndReinit(1000, false);

	CleanAndReinit(10000, false);

	CleanAndReinit(10000, true);

	OpenAndRead(1000, true);

	OpenAndRead(10000, true);

	{
		std::string bdName = "test_perfomance";
		storage_impl::DiskStorage strg(bdName, storage_impl::STORAGE_TYPES::HASH_STORAGE, true);

		auto numIterations = 1000;
		std::cout << numIterations << " Delete()'s" << std::endl;
		auto startTime = std::chrono::system_clock::now();
		DeleteRange(strg, "key", 0, numIterations);
		auto endTime = std::chrono::system_clock::now();
		std::chrono::duration<double> diffTime = endTime - startTime;
		std::cout << "Interval: " << diffTime.count() << std::endl << std::endl;
	}

	{
		std::string bdName = "test_perfomance";
		storage_impl::DiskStorage strg(bdName, storage_impl::STORAGE_TYPES::HASH_STORAGE, true);

		auto numIterations = 1000;
		std::cout << numIterations << " Update()'s" << std::endl;
		auto startTime = std::chrono::system_clock::now();
		UpdateRange(strg, "key", "Good By, Gay", 0, numIterations);
		auto endTime = std::chrono::system_clock::now();
		std::chrono::duration<double> diffTime = endTime - startTime;
		std::cout << "Interval: " << diffTime.count() << std::endl << std::endl;
	}
}