#ifndef __STORAGE_TYPES_H
#define __STORAGE_TYPES_H

#include <vector>
#include <stdint.h>

namespace abstract_stream_storage
{
	const uint32_t TABLE_SIZE = 1213;
	const int64_t NULL_REFERENCE = -1;


	struct IndexRecord
	{
		IndexRecord() : m_offset(NULL_REFERENCE)
		{
		}

		explicit IndexRecord(const int64_t _offset) : m_offset(_offset)
		{
		}

		uint16_t getSize() const
		{
			return sizeof(m_offset);
		}
		int64_t m_offset;
	};

	inline std::istream& operator >>(std::istream& _is, IndexRecord& _value)
	{
		_is.read(reinterpret_cast<char*>(&_value.m_offset), sizeof(_value.m_offset));
		return _is;
	}

	inline std::ostream& operator <<(std::ostream& _os, const IndexRecord& _value)
	{
		_os.write(reinterpret_cast<const char*>(&_value.m_offset), sizeof(_value.m_offset));
		return _os;
	}



	// Декларируем поля которые составляют запись в таблице данных;
	struct DataRecord //: IAbstractStructure
	{
		DataRecord() : m_prev(NULL_REFERENCE),
					   m_next(NULL_REFERENCE)
		{
		}

		explicit DataRecord(const std::vector<char>& _key, const std::vector<char>& _data) : m_key(_key.begin(), _key.end()),
																							 m_data(_data),
																							 m_prev(NULL_REFERENCE),
																							 m_next(NULL_REFERENCE)
		{
		}

		std::string getKey() const
		{
			return std::string(m_key.begin(), m_key.end());
		}

		std::string getData() const
		{
			return std::string(m_data.begin(), m_data.end());
		}

		uint16_t getSize() const
		{
			return sizeof(uint64_t) + m_key.size() +
				   sizeof(uint64_t) + m_data.size() +
				   sizeof(m_next) +
				   sizeof(m_prev);
		}

		std::vector<char> m_key;
		std::vector<char> m_data;
		int64_t m_prev; // aka linked list
		int64_t m_next; // next and prev offset
	};

	inline std::istream& operator >>(std::istream& _is, DataRecord& _value)
	{
		uint64_t sizeKey;
		_is.read(reinterpret_cast<char*>(&sizeKey), sizeof(sizeKey));
		_value.m_key.resize(static_cast<size_t>(sizeKey));
		if (sizeKey > 0) {
			_is.read(reinterpret_cast<char*>(_value.m_key.data()), sizeKey);
		}

		uint64_t sizeData;
		_is.read(reinterpret_cast<char*>(&sizeData), sizeof(sizeData));
		_value.m_data.resize(static_cast<size_t>(sizeData));
		if (sizeData > 0) {
			_is.read(reinterpret_cast<char*>(_value.m_data.data()), sizeData);
		}

		_is.read(reinterpret_cast<char*>(&_value.m_prev), sizeof(_value.m_prev));
		_is.read(reinterpret_cast<char*>(&_value.m_next), sizeof(_value.m_next));
		return _is;
	}

	inline std::ostream& operator <<(std::ostream& _os, const DataRecord& _value)
	{
		uint64_t sizeKey = _value.m_key.size();
		_os.write(reinterpret_cast<const char*>(&sizeKey), sizeof(sizeKey));
		_os.write(reinterpret_cast<const char*>(_value.m_key.data()), sizeKey);

		uint64_t sizeData = _value.m_data.size();
		_os.write(reinterpret_cast<const char*>(&sizeData), sizeof(sizeData));
		_os.write(reinterpret_cast<const char*>(_value.m_data.data()), sizeData);

		_os.write(reinterpret_cast<const char*>(&_value.m_prev), sizeof(_value.m_prev));
		_os.write(reinterpret_cast<const char*>(&_value.m_next), sizeof(_value.m_next));
		return _os;
	}


	struct MetaData
	{
		MetaData() : m_tableSize(TABLE_SIZE),
					 m_dataCapacity(0),
					 m_numberRecords(0)
		{
		}

		explicit MetaData(const uint32_t _tableSize, const uint64_t _dataCapacity, const uint64_t _numberRecords) : m_tableSize(_tableSize),
		                                                                                                   m_dataCapacity(_dataCapacity),
		                                                                                                   m_numberRecords(_numberRecords)
		{
		}

		uint16_t getSize() const
		{
			return sizeof(m_tableSize) + sizeof(m_dataCapacity) + sizeof(m_numberRecords);
		}

		uint32_t m_tableSize; // Number of buckets
		uint64_t m_dataCapacity; // Capacity of data entity (size of data entity)
		uint64_t m_numberRecords; // Number of all records
	};

	inline std::istream& operator >>(std::istream& _is, MetaData& _value)
	{
		_is.read(reinterpret_cast<char*>(&_value.m_tableSize), sizeof(_value.m_tableSize));
		_is.read(reinterpret_cast<char*>(&_value.m_dataCapacity), sizeof(_value.m_dataCapacity));
		_is.read(reinterpret_cast<char*>(&_value.m_numberRecords), sizeof(_value.m_numberRecords));
		return _is;
	}

	inline std::ostream& operator <<(std::ostream& _os, const MetaData& _value)
	{
		_os.write(reinterpret_cast<const char*>(&_value.m_tableSize), sizeof(_value.m_tableSize));
		_os.write(reinterpret_cast<const char*>(&_value.m_dataCapacity), sizeof(_value.m_dataCapacity));
		_os.write(reinterpret_cast<const char*>(&_value.m_numberRecords), sizeof(_value.m_numberRecords));
		return _os;
	}
}

#endif
