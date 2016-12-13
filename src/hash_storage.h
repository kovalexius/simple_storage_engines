#ifndef __HASH_STORAGE_H
#define __HASH_STORAGE_H

#include <vector>
#include <streambuf>
#include <iostream>

#include <stdint.h>

#include "stream_storage.h"
#include "storage_types.h"

namespace abstract_stream_storage
{
	// Основной класс, работающий со stream'ом
	// Передай ему в конструктор буферы типа streambuf Индексной и Информационной сущности
	class HashStorage : public IStreamStorage
	{
	public:
		HashStorage(std::streambuf& _index_buf, std::streambuf& _data_buf) : m_indexOut(&_index_buf),
																			 m_indexIn(&_index_buf),
																			 m_dataOut(&_data_buf),
																			 m_dataIn(&_data_buf)
		{
		}

		virtual ~HashStorage() override
		{
		}


	////////////////////////////////////////// Public Virtual Interface //////////////////////////////////////////

		virtual bool Virtual_Create(const std::vector<char>& _key, const std::vector<char>& _data) override;

		virtual bool Virtual_Update(const std::vector<char>& _key, const std::vector<char>& _newData) override;

		virtual bool Virtual_Update(const std::vector<char>& _key, const std::vector<char>& _newData,
									std::function<bool(const std::vector<char>&)> _predicate) override;

		virtual bool Virtual_Delete(const std::vector<char>& _key) override;

		virtual bool Virtual_Delete(const std::vector<char>& _key,
									std::function<bool(const std::vector<char>&)> _predicate) override;

		virtual bool Virtual_Read(const std::vector<char>& _key, 
								  std::vector<char>& _outData,
								  std::function<bool(const std::vector<char>&)> _predicate = [](const std::vector<char>&)
																							 {
																								 return true;
																							 }) override;

		virtual bool Virtual_Read(const std::vector<char>& _key, std::vector<std::vector<char>>& _outDatas) override;

		// test methods
		virtual void Virtual_getKeyList(std::vector<std::vector<char>>& _keyList) override;
		virtual void Virtual_debugPrintAllRecords() override;

		// Clean, load and save control methods
		virtual void Virtual_clearall() override;
		virtual void Virtual_readMetaData() override;
		virtual void Virtual_writeMetaData() override;

	private:

		////////////////////////////  Methods  ////////////////////////////

		//! Calculate bucket index from byte array hash representation
		size_t getIndexByKey(const std::vector<char>& _key);

		//! Rehashing and 
		void rehash();

		//! Find offset of data storage (end or middle)
		int64_t getOffsetToWrite(const uint16_t _size) const;

		//! Check, isn't there is collisions or not
		/*! In:  index of bucket
		Out: offset stored on this bucket
		*   @return: true if bucket is vacant, otherwise - false*/
		bool isIndexVacant(const size_t _index, int64_t& _offset) const;

		//! Write index
		void putIndex(const size_t _index, const IndexRecord& _meta);

		//! Write data to storage under specified offset
		void putData(const DataRecord& _data, const int64_t _offset);

		//! Get first data record under specified offset
		bool getFirstData(const int64_t _offset, DataRecord& _outData) const;

		//! Recursively get data by key
		/*! In: init offset to begin finding
		key to find record
		Out: found record
		offset of found record
		*   @return: true if record with required key found, otherwise false and out_data and out_offset will be initialized with last record*/
		bool getDataByKey(const int64_t _offset, const std::vector<char>& _key, DataRecord& _outData, int64_t& _outOffset) const;

		//! Rewrite data up, fill empty spaces
		void shiftDown(const DataRecord& _deletedRec, const uint64_t _offset);

		//! get offsets, bucket index and appropriate record data 
		void getRecordList(std::vector<std::tuple<int64_t, size_t, DataRecord>>& _recordList);



		/////////////////////////////// Variables /////////////////////////////////

		std::ostream m_indexOut;
		mutable std::istream m_indexIn; // seekg(), opeartor>>() is not const, thats why it needs to make mutable
		std::ostream m_dataOut;
		mutable std::istream m_dataIn;  // seekg(), operator>>() is not const
		MetaData m_metaData;
	};
}

#endif