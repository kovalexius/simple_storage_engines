#ifndef __STREAM_STORAGE_H
#define __STREAM_STORAGE_H

#include <vector>

namespace abstract_stream_storage
{
	class IStreamStorage
	{
	public:
		virtual ~IStreamStorage()
		{
		}

		virtual bool Virtual_Create(const std::vector<char>& _key, const std::vector<char>& _data) = 0;
		virtual bool Virtual_Read(const std::vector<char>& _key, std::vector<char>& _outData) = 0;
		virtual bool Virtual_Update(const std::vector<char>& _key, const std::vector<char>& _newData) = 0;
		virtual bool Virtual_Delete(const std::vector<char>& _key) = 0;

		virtual void Virtual_getKeyList(std::vector<std::vector<char>>& _keyList) = 0;
		virtual void Virtual_debugPrintAllRecords() = 0;

		virtual void Virtual_clearall() = 0;
		virtual void Virtual_readMetaData() = 0;
		virtual void Virtual_writeMetaData() = 0;
	};
}

#endif
