/*
*
* Copyright (c) 2015-2017 by blindtiger ( blindtiger@foxmail.com )
*
* The contents of this file are subject to the Mozilla Public License Version
* 2.0 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. Rdee the License
* for the specific language governing rights and limitations under the
* License.
*
* The Initial Developer of the Original Code is blindtiger.
*
*/

#define _CRT_SECURE_NO_WARNINGS

#include <wtypes.h>
#include <varargs.h>
#include "Log.h"

#ifndef UNICODE
#define _stprintf_s sprintf_s
#define _vsctprintf _vscprintf
#define _vstprintf_s vsprintf_s
#else
#define _stprintf_s swprintf_s
#define _vsctprintf _vscwprintf
#define _vstprintf_s vswprintf_s
#endif

NTSYSCALLAPI
int
CDECL
sprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) char * const _Buffer,
    _In_ size_t const _BufferCount,
    _In_z_ _Printf_format_string_ char const * const _Format,
    ...);

NTSYSCALLAPI
int
CDECL
swprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t * const _Buffer,
    _In_ size_t const _BufferCount,
    _In_z_ _Printf_format_string_ wchar_t const * const _Format,
    ...);

NTSYSCALLAPI
int
CDECL
vsprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) char * const _Buffer,
    _In_ size_t const _BufferCount,
    _In_z_ _Printf_format_string_ char const * const _Format,
    va_list _ArgList
);

NTSYSCALLAPI
int
CDECL
vswprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t * const _Buffer,
    _In_ size_t const _BufferCount,
    _In_z_ _Printf_format_string_ wchar_t const * const _Format,
    va_list _ArgList
);

NTSYSCALLAPI
int
CDECL
_vscprintf(
    _In_z_ _Printf_format_string_ char const * const _Format,
    va_list _ArgList
);

NTSYSCALLAPI
int
CDECL
_vscwprintf(
    _In_z_ _Printf_format_string_ wchar_t const * const _Format,
    va_list _ArgList
);

VOID
CDECL
RdeDbgPrint(
    __in LPCTSTR Format,
    __in ...
)
{
    TCHAR Buffer[USN_PAGE_SIZE] = { 0 };
    va_list Args = NULL;

    va_start(
        Args,
        Format);

    _vstprintf_s(
        Buffer,
        _countof(Buffer),
        Format,
        Args);

    OutputDebugString(Buffer);
}
