#include "disk_storage.h"
#include "hash_storage.h"
#include "multikey_storage.h"

using namespace storage_impl;
using namespace abstract_stream_storage;

DiskStorage::DiskStorage(const std::string& _dbName = std::string("bd"), 
						 STORAGE_TYPES _strgType = HASH_STORAGE, 
						 bool _fastStreams = false) : m_indexName(_dbName + std::string("_index")),
													  m_dataName(_dbName + std::string("_data"))
{
	switch (_strgType)
	{
	case HASH_STORAGE:
		m_storage = std::dynamic_pointer_cast<IStreamStorage>(std::make_shared<HashStorage>(m_indexFile, m_dataFile));
		break;
	case MULTIKEY_STORAGE:
		m_storage = std::dynamic_pointer_cast<IStreamStorage>(std::make_shared<MultikeyStorage>(m_indexFile, m_dataFile));
		break;
	default:
		m_storage = std::dynamic_pointer_cast<IStreamStorage>(std::make_shared<HashStorage>(m_indexFile, m_dataFile));
	}

	if (_fastStreams)
		std::ios::sync_with_stdio(false);

	if (!std::tr2::sys::exists(std::tr2::sys::path(m_indexName)) || !std::tr2::sys::exists(std::tr2::sys::path(m_dataName.c_str())))
	{
		m_indexFile.open(m_indexName, std::ios::binary | std::ios::in | std::ios::app);
		m_indexFile.close();
		m_indexFile.open(m_indexName, std::ios::binary | std::ios::in | std::ios::out);

		m_dataFile.open(m_dataName, std::ios::binary | std::ios::in | std::ios::app);
		m_dataFile.close();
		m_dataFile.open(m_dataName, std::ios::binary | std::ios::in | std::ios::out);

		if (!m_indexFile.is_open())
			throw std::string("File " + m_indexName + " not opened");
		if (!m_dataFile.is_open())
			throw std::string("File " + m_dataName + " not opened");

		m_storage->Virtual_clearall();
	}
	else
	{
		m_indexFile.open(m_indexName, std::ios::binary | std::ios::in | std::ios::out);
		m_dataFile.open(m_dataName, std::ios::binary | std::ios::in | std::ios::out);
	}
	m_storage->Virtual_readMetaData();
}

DiskStorage::~DiskStorage()
{
	m_storage->Virtual_writeMetaData(); // Плохо, так как этот деструктор может не вызваться (например при 
	// аварийном завершении всего процесса или снятии процесса из диспетчера задач, kill -9, и т.п.)
	// Но не писать же метаданные каждый раз при изменении.
	m_indexFile.close();
	m_dataFile.close();
}

bool DiskStorage::Create(const std::vector<char>& _key, const std::vector<char>& _data)
{
	return m_storage->Virtual_Create(_key, _data);
}

bool DiskStorage::Read(const std::vector<char>& _key, 
					   std::vector<char>& _outData,
					   std::function<bool(const std::vector<char>&)> _predicate)
{
	return m_storage->Virtual_Read(_key, _outData, _predicate);
}

bool DiskStorage::Read(const std::vector<char>& _key, std::vector<std::vector<char>>& _outData)
{
	return m_storage->Virtual_Read(_key, _outData);
}

bool DiskStorage::Update(const std::vector<char>& _key, const std::vector<char>& _newData)
{
	return m_storage->Virtual_Update(_key, _newData);
}

bool DiskStorage::Update(const std::vector<char>& _key, 
						 const std::vector<char>& _newData, 
						 std::function<bool(const std::vector<char>&)> _predicate)
{
	return m_storage->Virtual_Update(_key, _newData, _predicate);
}

bool DiskStorage::Delete(const std::vector<char>& _key)
{
	return m_storage->Virtual_Delete(_key);
}

bool DiskStorage::Delete(const std::vector<char>& _key, 
						 std::function<bool(const std::vector<char>&)> _predicate)
{
	return m_storage->Virtual_Delete(_key, _predicate);
}

void DiskStorage::getKeyList(std::vector<std::vector<char>>& _keyList)
{
	m_storage->Virtual_getKeyList(_keyList);
}