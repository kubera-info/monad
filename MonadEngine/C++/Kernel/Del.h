// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#define OPER_DEF_NO_DEF_CTOR(TODEL)\
	TODEL(const TODEL&) = default;\
	TODEL(TODEL&&) noexcept = default;\
	TODEL& operator=(const TODEL&) = default;\
	TODEL& operator=(TODEL&&) noexcept = default;

#define OPER_DEL_NO_DEF_CTOR(TODEL)\
	TODEL(const TODEL&) = delete;\
	TODEL(TODEL&&) = delete;\
	TODEL& operator=(const TODEL&) = delete;\
	TODEL& operator=(TODEL&&) = delete;

#define OPER_DEL(TODEL)\
	TODEL() = delete;\
	OPER_DEL_NO_DEF_CTOR(TODEL)

#define OPER_DEF(TODEL)\
	TODEL() = default;\
	OPER_DEF_NO_DEF_CTOR(TODEL)