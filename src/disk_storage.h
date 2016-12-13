#ifndef __DISK_STORAGE_H
#define __DISK_STORAGE_H

#include <fstream>
#include <filesystem>
#include <functional>

#include "stream_storage.h"

// Класс-обертка над stream'овыми хранилищами, "соединяющий" их с файлами на диске

namespace storage_impl
{
	enum STORAGE_TYPES
	{
		HASH_STORAGE,
		MULTIKEY_STORAGE
	};

	class DiskStorage
	{
	public:
		explicit DiskStorage(const std::string& _dbName, STORAGE_TYPES _strgType, bool _fastStreams);

		~DiskStorage();

	//////////////////////////// Non virtual Interface of CRRUUDD //////////////////////////

		bool Create(const std::vector<char>& _key, const std::vector<char>& _data);

		bool Read(const std::vector<char>& _key, 
				  std::vector<char>& _outData,
				  std::function<bool(const std::vector<char>&)> _predicate = [](const std::vector<char>&)
																				{
																					return true;
																				});

		bool Read(const std::vector<char>& _key, std::vector<std::vector<char>>& _outDatas);

		bool Update(const std::vector<char>& _key, const std::vector<char>& _newData);

		bool Update(const std::vector<char>& _key, 
					const std::vector<char>& _newData, 
					std::function<bool(const std::vector<char>&)> _predicate);

		bool Delete(const std::vector<char>& _key);

		bool Delete(const std::vector<char>& _key, std::function<bool(const std::vector<char>&)> _predicate);

		// Get list of all differenced keys (SECUDEV-185)
		void getKeyList(std::vector<std::vector<char>>& _keyList);

	private:
		std::string m_indexName;
		std::string m_dataName;
		std::filebuf m_indexFile;
		std::filebuf m_dataFile;

		// Aggregation of Virtual Interface of stream storages
		std::shared_ptr<abstract_stream_storage::IStreamStorage> m_storage;  // Make full Pimpl, plzzz !!!
	};
}


#endif
