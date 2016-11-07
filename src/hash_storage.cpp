#include <string>
#include <iostream>
#include <tuple>

#include "hash_storage.h"

using namespace abstract_stream_storage;

void HashStorage::rehash()
{
}

int64_t HashStorage::getOffsetToWrite(const uint16_t _size) const
{
	return m_metaData.m_dataCapacity;
}

bool HashStorage::isIndexVacant(const size_t _index, int64_t& _offset) const
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

void HashStorage::putIndex(const size_t _index, const IndexRecord& _iRecord)
{
	auto writePos = m_metaData.getSize() + _index * sizeof(IndexRecord);
	m_indexOut.seekp(writePos);
	m_indexOut << _iRecord;
}

void HashStorage::putData(const DataRecord& _data, const int64_t _offset)
{
	m_dataOut.seekp(_offset);
	m_dataOut << _data;
}

bool HashStorage::getFirstData(const int64_t _offset, DataRecord& _outData) const
{
	m_dataIn.seekg(_offset);
	m_dataIn >> _outData;
	return true;
}

bool HashStorage::getDataByKey(const int64_t _offset, const std::vector<char>& _key, DataRecord& _outData, int64_t& _outOffset) const
{
	_outOffset = _offset;
	getFirstData(_offset, _outData);
	if (_outData.m_key == _key)
	{
		return true;
	}

	while (_outData.m_next >= 0)
	{
		_outOffset = _outData.m_next;
		getFirstData(_outData.m_next, _outData);
		if (_outData.m_key == _key)
		{
			return true;
		}
	}
	return false;
}

void HashStorage::getRecordList(std::vector<std::tuple<int64_t, size_t, DataRecord>>& _recordList)
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

void HashStorage::shiftDown(const DataRecord& _deletedRecord, const uint64_t _deletedOffset)
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

size_t HashStorage::getIndexByKey(const std::vector<char>& _key)
{
	std::hash<std::string> hashFn;				// May be it is not version-independent. It needs to test on different platforms and compilers with same database files 
	auto index = hashFn(std::string(_key.begin(), _key.end())) % m_metaData.m_tableSize;
	return index;
}

void HashStorage::Virtual_clearall()
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

void HashStorage::Virtual_readMetaData()
{
	m_indexIn.seekg(std::ios::beg);
	m_indexIn >> m_metaData;
	for (uint32_t i = 0; i < m_metaData.m_tableSize; i++)
	{
		IndexRecord iRecord;
		m_indexIn >> iRecord;
	}
}

void HashStorage::Virtual_writeMetaData()
{
	m_indexOut.seekp(std::ios::beg);
	m_indexOut << m_metaData;
}

void HashStorage::Virtual_debugPrintAllRecords()
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

void HashStorage::Virtual_getKeyList(std::vector<std::vector<char>>& _keyList)
{
	std::vector<std::tuple<int64_t, size_t, DataRecord>> recordList;
	getRecordList(recordList);

	for (auto it = recordList.begin(); it != recordList.end(); ++it)
	{
		_keyList.push_back(std::get<2>(*it).m_key);
	}
}

bool HashStorage::Virtual_Create(const std::vector<char>& _key, const std::vector<char>& _data)
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
		if (getDataByKey(firstOffset, _key, outData, outOffset))
		{
			return false;		// This key already exists
		}
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

bool HashStorage::Virtual_Read(const std::vector<char>& _key, std::vector<char>& _outData)
{
	auto index = getIndexByKey(_key);

	int64_t firstOffset;
	if (isIndexVacant(index, firstOffset))
	{
		return false;
	}

	DataRecord dataRecord;
	int64_t outOffset;
	if (getDataByKey(firstOffset, _key, dataRecord, outOffset))
	{
		_outData = dataRecord.m_data;
		return true;
	}

	return false;
}

bool HashStorage::Virtual_Delete(const std::vector<char>& _key)
{
	auto index = getIndexByKey(_key);

	int64_t firstOffset;
	if (isIndexVacant(index, firstOffset))
	{
		return false;
	}

	DataRecord deletedRecord;
	int64_t deletedOffset;
	if (getDataByKey(firstOffset, _key, deletedRecord, deletedOffset))
	{
		// Make neighbour record references valid
		if (deletedRecord.m_prev != NULL_REFERENCE)
		{
			DataRecord prevRecord;
			getFirstData(deletedRecord.m_prev, prevRecord);
			prevRecord.m_next = deletedRecord.m_next;
			putData(prevRecord, deletedRecord.m_prev);
		}
		else
		{
			IndexRecord ind_record{deletedRecord.m_next}; // Ok
			putIndex(index, ind_record);
		}
		if (deletedRecord.m_next != NULL_REFERENCE)
		{
			DataRecord next_record;
			getFirstData(deletedRecord.m_next, next_record);
			next_record.m_prev = deletedRecord.m_prev;
			putData(next_record, deletedRecord.m_next);
		}

		// Shift down all records after this and make appropriate references valid
		shiftDown(deletedRecord, deletedOffset);

		m_metaData.m_numberRecords--;
		Virtual_writeMetaData();

		return true;
	}

	return false;
}

bool HashStorage::Virtual_Update(const std::vector<char>& _key, const std::vector<char>& _newData)
{
	if (!Virtual_Delete(_key))
	{
		return false;
	}
	return Virtual_Create(_key, _newData);
}

