#ifndef __STREAM_STORAGE_H
#define __STREAM_STORAGE_H

#include <vector>
#include <functional>

// Интерфейс для stream'овых хранилищ

namespace abstract_stream_storage
{
	class IStreamStorage
	{
	public:
		IStreamStorage()
		{}

		virtual ~IStreamStorage()
		{
		}

		// No copy semantic
		IStreamStorage(const IStreamStorage&) = delete;
		IStreamStorage& operator = (const IStreamStorage&) = delete;


	////////////////////////////// Public Virtual interface of CRRUUDD with helper functions ////////////////////////////////
		virtual bool Virtual_Create(const std::vector<char>& _key, const std::vector<char>& _data) = 0;

		virtual bool Virtual_Read(const std::vector<char>& _key, 
								  std::vector<char>& _outData, 
								  std::function<bool(const std::vector<char>&)> _predicate) = 0;
		
		virtual bool Virtual_Read(const std::vector<char>& _key, std::vector<std::vector<char>>& _outDatas) = 0;
		
		virtual bool Virtual_Update(const std::vector<char>& _key, const std::vector<char>& _newData) = 0;
		
		virtual bool Virtual_Update(const std::vector<char>& _key, 
									const std::vector<char>& _newData, 
									std::function<bool(const std::vector<char>&)> _predicate) = 0;
		
		virtual bool Virtual_Delete(const std::vector<char>& _key) = 0;
		
		virtual bool Virtual_Delete(const std::vector<char>& _key, 
									std::function<bool(const std::vector<char>&)> _predicate) = 0;

		// test helper methods
		virtual void Virtual_getKeyList(std::vector<std::vector<char>>& _keyList) = 0;
		virtual void Virtual_debugPrintAllRecords() = 0;

		// Clean, load and save control methods
		virtual void Virtual_clearall() = 0;
		virtual void Virtual_readMetaData() = 0;
		virtual void Virtual_writeMetaData() = 0;
	};
}

#endif
