#include <string>
#include <iostream>
#include <tuple>
#include <unordered_set>

#include "multikey_storage.h"


using namespace abstract_stream_storage;

void MultikeyStorage::rehash()
{
}

int64_t MultikeyStorage::getOffsetToWrite(const uint16_t _size) const
{
	return m_metaData.m_dataCapacity;
}

bool MultikeyStorage::isIndexVacant(const size_t _index, int64_t& _offset) const
{
	IndexRecord irecord;
	auto readPos = m_metaData.getSize() + _index * irecord.getSize();
	m_indexIn.seekg(readPos);
	m_indexIn >> irecord;
	_offset = irecord.m_offset;
	if (irecord.m_offset == NULL_REFERENCE)
	{
		return true;
	}
	return false;
}

void MultikeyStorage::putIndex(const size_t _index, const IndexRecord& _iRecord)
{
	auto writePos = m_metaData.getSize() + _index * sizeof(IndexRecord);
	m_indexOut.seekp(writePos);
	m_indexOut << _iRecord;
}

void MultikeyStorage::putData(const DataRecord& _data, const int64_t _offset)
{
	m_dataOut.seekp(_offset);
	m_dataOut << _data;
}

bool MultikeyStorage::getLastData(const int64_t _offset, DataRecord& _outData, int64_t& _outOffset) const
{
	_outOffset = _offset;
	getFirstData(_outOffset, _outData);
	while (_outData.m_next != NULL_REFERENCE)
	{
		_outOffset = _outData.m_next;
		getFirstData(_outOffset, _outData);
	}
	return true;
}

bool MultikeyStorage::getFirstData(const int64_t _offset, DataRecord& _outData) const
{
	m_dataIn.seekg(_offset);
	m_dataIn >> _outData;
	return true;
}

bool MultikeyStorage::getDataByKey(const int64_t _offset, const std::vector<char>& _key, DataRecord& _outData, int64_t& _outOffset) const
{
	_outOffset = _offset;
	if (_outOffset < 0)
		return false;
	getFirstData(_outOffset, _outData);
	if (_outData.m_key == _key)
	{
		return true;
	}

	while (_outData.m_next >= 0)
	{
		_outOffset = _outData.m_next;
		getFirstData(_outOffset, _outData);
		if (_outData.m_key == _key)
		{
			return true;
		}
	}
	return false;
}

void MultikeyStorage::getRecordList(std::vector<std::tuple<int64_t, size_t, DataRecord>>& _recordList)
{
	for (size_t i = 0; i < m_metaData.m_tableSize; i++)
	{
		int64_t offset;
		if (!isIndexVacant(i, offset))
		{
			while (offset != NULL_REFERENCE)
			{
				DataRecord outData;
				getFirstData(offset, outData);
				_recordList.push_back(std::make_tuple(offset, i, outData));
				offset = outData.m_next;
			}
		}
	}
}

void MultikeyStorage::shiftDown(const DataRecord& _deletedRecord, const uint64_t _deletedOffset)
{
	auto cuttingSize = _deletedRecord.getSize();
	auto rest = 0;
	// shift all references
	while (m_metaData.m_dataCapacity > cuttingSize + _deletedOffset + rest)
	{
		// Get movable record
		DataRecord movableRecord;
		getFirstData(_deletedOffset + rest + cuttingSize, movableRecord);

		// Shift down references at linked records
		if (movableRecord.m_prev != NULL_REFERENCE)
		{
			DataRecord prevRecord;
			getFirstData(movableRecord.m_prev, prevRecord);
			prevRecord.m_next -= cuttingSize;
			putData(prevRecord, movableRecord.m_prev);
		}
		else // Change appropriate index
		{
			auto index = getIndexByKey(movableRecord.m_key);
			int64_t indexOffset;
			if (!isIndexVacant(index, indexOffset))
			{
				IndexRecord indexRecord{ indexOffset - cuttingSize };
				putIndex(index, indexRecord);
			}
		}

		if (movableRecord.m_next != NULL_REFERENCE)
		{
			DataRecord nextRecord;
			getFirstData(movableRecord.m_next, nextRecord);
			nextRecord.m_prev -= cuttingSize;
			putData(nextRecord, movableRecord.m_next);
		}

		// Shift Data
		putData(movableRecord, _deletedOffset + rest);

		rest += movableRecord.getSize();
	}

	m_metaData.m_dataCapacity -= cuttingSize;
	Virtual_writeMetaData();
}

size_t MultikeyStorage::getIndexByKey(const std::vector<char>& _key)
{
	std::hash<std::string> hashFn;				// May be it is not version-independent. It needs to test on different platforms and compilers with same database files 
	auto index = hashFn(std::string(_key.begin(), _key.end())) % m_metaData.m_tableSize;
	return index;
}

void MultikeyStorage::DeleteRecord(const DataRecord& _deletedRecord,
	const int64_t _deletedOffset,
	const size_t _index)
{
	// Make neighbour record references valid
	if (_deletedRecord.m_prev != NULL_REFERENCE)
	{
		DataRecord prevRecord;
		getFirstData(_deletedRecord.m_prev, prevRecord);
		prevRecord.m_next = _deletedRecord.m_next;
		putData(prevRecord, _deletedRecord.m_prev);
	}
	else
	{
		IndexRecord ind_record{ _deletedRecord.m_next }; // Ok
		putIndex(_index, ind_record);
	}
	if (_deletedRecord.m_next != NULL_REFERENCE)
	{
		DataRecord next_record;
		getFirstData(_deletedRecord.m_next, next_record);
		next_record.m_prev = _deletedRecord.m_prev;
		putData(next_record, _deletedRecord.m_next);
	}

	// Shift down all records after this and make appropriate references valid
	shiftDown(_deletedRecord, _deletedOffset);

	m_metaData.m_numberRecords--;
	Virtual_writeMetaData();
}

bool MultikeyStorage::Delete(const std::vector<char>& _key, std::function<bool(const std::vector<char>&)> _predicate)
{
	auto index = getIndexByKey(_key);
	int64_t firstOffset;
	if (isIndexVacant(index, firstOffset))
	{
		return false;
	}

	bool result = false;
	DataRecord deletedRecord;
	int64_t deletedOffset;
	while (getDataByKey(firstOffset, _key, deletedRecord, deletedOffset))
	{
		// check predicate
		if (_predicate(deletedRecord.m_data))
		{
			DeleteRecord(deletedRecord, deletedOffset, index);
			result = true;
		}
		else
		{
			firstOffset = deletedRecord.m_next;
		}
		if (deletedRecord.m_next == NULL_REFERENCE)
			break;
	}

	return result;
}

void MultikeyStorage::Virtual_clearall()
{
	m_indexOut.seekp(std::ios::beg);
	const MetaData metaRecord(TABLE_SIZE, 0, 0);
	m_indexOut << metaRecord;
	for (uint32_t i = 0; i < metaRecord.m_tableSize; i++)
	{
		const IndexRecord iRecord(NULL_REFERENCE);
		m_indexOut << iRecord;
	}
}

void MultikeyStorage::Virtual_readMetaData()
{
	m_indexIn.seekg(std::ios::beg);
	m_indexIn >> m_metaData;
	for (uint32_t i = 0; i < m_metaData.m_tableSize; i++)
	{
		IndexRecord iRecord;
		m_indexIn >> iRecord;
	}
}

void MultikeyStorage::Virtual_writeMetaData()
{
	m_indexOut.seekp(std::ios::beg);
	m_indexOut << m_metaData;
}

void MultikeyStorage::Virtual_debugPrintAllRecords()
{
	std::vector<std::tuple<int64_t, size_t, DataRecord>> recordList;
	getRecordList(recordList);

	for (auto it = recordList.begin(); it != recordList.end(); ++it)
	{
		std::cout << "Bucket: " << std::get<1>(*it) <<
			" Offset: " << std::get<0>(*it) <<
			" Size: " << std::get<2>(*it).getSize() <<
			" Key: " << std::get<2>(*it).getKey() <<
			" Data: " << std::get<2>(*it).getData() <<
			" Prev: " << std::get<2>(*it).m_prev <<
			" Next: " << std::get<2>(*it).m_next << std::endl;
	}
}

void MultikeyStorage::Virtual_getKeyList(std::vector<std::vector<char>>& _keyList)
{
	std::vector<std::tuple<int64_t, size_t, DataRecord>> recordList;
	getRecordList(recordList);

	std::unordered_set<std::string> keySet;

	for (auto it = recordList.begin(); it != recordList.end(); ++it)
	{
		std::string keyStr(std::get<2>(*it).m_key.begin(), std::get<2>(*it).m_key.end());
		if (keySet.find(keyStr) == keySet.end())
		{
			keySet.insert(keyStr);
			_keyList.push_back(std::get<2>(*it).m_key);
		}
	}
}

bool MultikeyStorage::Virtual_Create(const std::vector<char>& _key, const std::vector<char>& _data)
{
	auto index = getIndexByKey(_key);

	DataRecord dataRecord(_key, _data);
	auto size = dataRecord.getSize();
	auto offset = getOffsetToWrite(size);

	int64_t firstOffset = 0;
	if (isIndexVacant(index, firstOffset))
	{
		IndexRecord indexRecord(offset);
		putIndex(index, indexRecord);
		dataRecord.m_prev = NULL_REFERENCE;
	}
	else
	{
		// Change offset of existing data_record 'next' field
		// Recursive bypass; 
		DataRecord outData;
		int64_t outOffset;
		getLastData(firstOffset, outData, outOffset);
		outData.m_next = offset;
		dataRecord.m_prev = outOffset;
		putData(outData, outOffset);
	}
	dataRecord.m_next = NULL_REFERENCE;
	putData(dataRecord, offset);
	m_metaData.m_numberRecords++;
	m_metaData.m_dataCapacity += dataRecord.getSize();
	Virtual_writeMetaData();

	return true;
}

bool MultikeyStorage::Virtual_Read(const std::vector<char>& _key, 
								   std::vector<char>& _outData,
								   std::function<bool(const std::vector<char>&)> _predicate)
{
	auto index = getIndexByKey(_key);
	int64_t beginOffset;
	if (isIndexVacant(index, beginOffset))
	{
		return false;
	}

	// Finding each record recursively and test it with predicate
	DataRecord dataRecord;
	do
	{
		int64_t outOffset;
		std::vector<char> outData;
		if (!getDataByKey(beginOffset, _key, dataRecord, outOffset))
			break;
		beginOffset = dataRecord.m_next;
		if (_predicate(dataRecord.m_data))
		{
			_outData = dataRecord.m_data;
			return true;
		}
	} while (beginOffset != NULL_REFERENCE);

	return false;
}

bool MultikeyStorage::Virtual_Read(const std::vector<char>& _key, std::vector<std::vector<char>>& _outDatas)
{
	auto index = getIndexByKey(_key);
	int64_t beginOffset;
	if (isIndexVacant(index, beginOffset))
	{
		return false;
	}

	bool result = false;
	DataRecord dataRecord;
	do
	{
		int64_t outOffset;
		std::vector<char> outData;
		if (!getDataByKey(beginOffset, _key, dataRecord, outOffset))
			break;
		beginOffset = dataRecord.m_next;
		_outDatas.push_back(dataRecord.m_data);
		result = true;
	} while (beginOffset != NULL_REFERENCE);

	return result;
}

// Always true predicate
struct StandartPredicate
{
	bool operator () (const std::vector<char>&)
	{
		return true;
	}
};

bool MultikeyStorage::Virtual_Delete(const std::vector<char>& _key)
{
	return Delete(_key, StandartPredicate()); // Always true predicate
}

bool MultikeyStorage::Virtual_Delete(const std::vector<char>& _key, std::function<bool(const std::vector<char>&)> _predicate)
{
	return Delete(_key, _predicate);
}

bool MultikeyStorage::Virtual_Update(const std::vector<char>& _key, const std::vector<char>& _newData)
{
	if (!Virtual_Delete(_key))
	{
		return false;
	}
	return Virtual_Create(_key, _newData);
}

bool MultikeyStorage::Virtual_Update(const std::vector<char>& _key, const std::vector<char>& _newData, std::function<bool(const std::vector<char>&)> _predicate)
{
	return Virtual_Update(_key, _newData);
}