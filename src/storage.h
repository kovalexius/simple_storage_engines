#ifndef __STORAGE_H
#define __STORAGE_H

#include <sstream>
#include <utility>
#include <typeinfo>

namespace crud_storage
{
	template <class T>
	size_t toTypeHash(const T&)
	{
		const std::type_info& ti = typeid(T);
		return ti.hash_code();
	}

	template <class T, typename... Rest>
	size_t toTypeHash(const T&, const Rest&... _args)
	{
		const std::type_info& ti = typeid(T);
		size_t hsh = ti.hash_code();
		size_t nextHsh = toTypeHash(_args...);
		return std::hash<size_t>()(hsh ^ nextHsh);
	}

	template <class T>
	std::string toData(const T& _var)
	{
		std::ostringstream sstrm;
		sstrm << _var;
		return sstrm.str();
	}

	template <class T, typename... Rest>
	std::string toData(const T& _var, const Rest&... _args)
	{
		std::ostringstream sstrm;
		sstrm << _var << toData(_args...);
		return sstrm.str();
	}

	template <typename... Args>
	std::string toBuffer(const Args&... _args)
	{
		size_t hash = toTypeHash(_args...);
		std::string strData = toData(_args...);
		std::ostringstream sstrm;
		sstrm.write(reinterpret_cast<const char*>(&hash), sizeof(hash));
		sstrm << strData;
		return sstrm.str();
	}

	class Storage
	{
	public:
		template <typename ...Args>
		bool Create(const std::string& _key, Args ... _args)
		{
			std::string strData = toBuffer(_args...);

			return true;
		}

		template <typename ...Args>
		bool Update(const std::string& _key, Args ... _args)
		{
			return true;
		}

		template <typename ...Args>
		bool Read(const std::string& _key, Args ... _args)
		{
			return true;
		}

		template <typename ...Args>
		bool Delete(const std::string& _key, Args ... _args)
		{
			return true;
		}
	};
}
#endif
