#pragma once
#ifndef _MONAD_CONVERT_H_
#define _MONAD_CONVERT_H_

#include <string>

float FloatFromString( const std::wstring& s );
bool BoolFromString( const std::wstring& s );
ULONG_PTR ULongPtrFromString( const std::wstring& s );
UINT UIntFromString( const std::wstring& s );
std::wstring StringFromHRESULT( HRESULT pHr );
std::wstring StringFromULongPtr( ULONG_PTR pUL );

#endif // _MONAD_CONVERT_H_