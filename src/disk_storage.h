#ifndef __DISK_STORAGE_H
#define __DISK_STORAGE_H

#include <fstream>
#include <filesystem>
#include <vector>

class IStreamStorage;

namespace storage_impl
{
	class DiskStorage
	{
	public:
		explicit DiskStorage(const std::string& _dbName, bool _fastStreams);

		~DiskStorage();

		bool Create(const std::vector<char>& _key, const std::vector<char>& _data);
		bool Read(const std::vector<char>& _key, std::vector<char>& _outData);
		bool Update(const std::vector<char>& _key, const std::vector<char>& _newData);
		bool Delete(const std::vector<char>& _key);

		void getKeyList(std::vector<std::vector<char>>& _keyList);

	private:
		std::string m_indexName;
		std::string m_dataName;
		std::filebuf m_indexFile;
		std::filebuf m_dataFile;

        std::shared_ptr<IStreamStorage> m_storage;
	};
}


#endif
