// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// ############################################################################
// ### Copyright © 2016-2024 by W³odzimierz O. Kubera. All rights reserved. ###
// ############################################################################

#pragma once

#include "../../OpenSource/lua/lua.h"
#include "../../OpenSource/lua/lualib.h"
#include "../../OpenSource/lua/lauxlib.h"

namespace Monad::Tools
{
	using FnLuaClose = void(*)(lua_State*);
	struct LuaManager
	{
#pragma region Processing
		LuaManager();
		void dostring(const std::string& TplString);
		void dostring(const std::wstring& TplString);
#pragma endregion

#pragma region Getters
		std::wstring checklstring(int32_t arg, size_t* l);
		std::wstring optlstring(int32_t arg, const std::wstring& def, size_t* l);
		lua_Number luaL_checknumber(lua_State* L, int32_t arg);
		lua_Number luaL_optnumber(lua_State* L, int32_t arg, lua_Number def);
		lua_Integer checkinteger(lua_State* L, int32_t arg);
		lua_Integer optinteger(lua_State* L, int32_t arg, lua_Integer def);
		inline lua_State* getL() noexcept
		{
			return m_LuaState.get();
		}
#pragma endregion

#pragma region Setters
		void			pushnil();
		void			pushnumber(lua_Number n);
		void			pushinteger(lua_Integer n);
		const char* pushlstring(const char* s, size_t len);
		const char* pushstring(const char* s);
		const char* pushvfstring(const char* fmt,
			va_list argp);
		template<typename ...TplParams>
		const char* pushfstring(const char* fmt, TplParams ... p)
		{
			return lua_pushfstring(fmt, p);
		}
		void			pushcclosure(lua_CFunction fn, int32_t n);
		void			pushbool(bool b);
		void			pushlightuserdata(void* p);
		int32_t				pushthread();
#pragma endregion

	private:
		std::unique_ptr<lua_State, FnLuaClose> m_LuaState;
	};
}