// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// ############################################################################
// ### Copyright © 2016-2024 by W³odzimierz O. Kubera. All rights reserved. ###
// ############################################################################

#include "stdafx.h"
#include "MonadLua.h"
///
#include "Tools/MonadConvert.h"

namespace Monad::Tools
{
	namespace
	{
		void LuaClose(lua_State* pLua) noexcept
		{
			lua_close(pLua);
		}
	}
	
	LuaManager::LuaManager() : m_LuaState{ luaL_newstate(), LuaClose }
	{
		THROW_EXC_IFZ(m_LuaState, Exceptions::NullValue, L"Lang");
		luaL_openlibs(m_LuaState.get());
	}
	
	void LuaManager::dostring(const std::string& String)
	{
		THROW_EXC_IFZ(!luaL_dostring(m_LuaState.get(), String.c_str()), Exceptions::InvalidClass, L"Lang Do");
	}
	
	void LuaManager::dostring(const std::wstring& String)
	{
		dostring(UnicodeToUTF8(String));
	}
	
	std::wstring LuaManager::checklstring(int32_t arg, size_t* l)
	{
		auto str = luaL_checklstring(m_LuaState.get(), arg, l);
		std::wstring ret;
		if (str)
		{
			ret = UTF8ToUnicode(str);
			delete str;
		}
		return ret;
	}
	
	std::wstring LuaManager::optlstring(int32_t arg, const std::wstring& def, size_t* l)
	{
		auto str = luaL_optlstring(m_LuaState.get(), arg, UnicodeToUTF8(def).c_str(), l);
		std::wstring ret;
		if (str)
		{
			ret = UTF8ToUnicode(str);
			delete str;
		}
		return ret;
	}
	
	lua_Number LuaManager::luaL_checknumber(lua_State* L, int32_t arg)
	{
		return luaL_checknumber(m_LuaState.get(), arg);
	}
	
	lua_Number LuaManager::luaL_optnumber(lua_State* L, int32_t arg, lua_Number def)
	{
		return luaL_optnumber(m_LuaState.get(), arg, def);
	}
	
	lua_Integer LuaManager::checkinteger(lua_State* L, int32_t arg)
	{
		return luaL_checkinteger(m_LuaState.get(), arg);
	}
	
	lua_Integer LuaManager::optinteger(lua_State* L, int32_t arg, lua_Integer def)
	{
		return luaL_optinteger(m_LuaState.get(), arg, def);
	}
	
	void LuaManager::pushnil()
	{
		lua_pushnil(m_LuaState.get());
	}
	
	void LuaManager::pushnumber(lua_Number n)
	{
		lua_pushnumber(m_LuaState.get(), n);
	}
	
	void LuaManager::pushinteger(lua_Integer n)
	{
		lua_pushinteger(m_LuaState.get(), n);
	}
	
	const char* LuaManager::pushlstring(const char* s, size_t len)
	{
		return lua_pushlstring(m_LuaState.get(), s, len);
	}
	
	const char* LuaManager::pushstring(const char* s)
	{
		return lua_pushstring(m_LuaState.get(), s);
	}
	
	const char* LuaManager::pushvfstring(const char* fmt,
		va_list argp)
	{
		return lua_pushvfstring(m_LuaState.get(), fmt, argp);
	}
	
	void LuaManager::pushcclosure(lua_CFunction fn, int32_t n)
	{
		lua_pushcclosure(m_LuaState.get(), fn, n);
	}
	
	void LuaManager::pushbool(bool b)
	{
		lua_pushboolean(m_LuaState.get(), b);
	}
	
	void LuaManager::pushlightuserdata(void* p)
	{
		lua_pushlightuserdata(m_LuaState.get(), p);
	}
	
	int32_t	LuaManager::pushthread()
	{
		return lua_pushthread(m_LuaState.get());
	}
}