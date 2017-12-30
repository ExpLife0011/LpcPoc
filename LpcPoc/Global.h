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

#ifndef _USER_MODE_GLOBAL_H_
#define _USER_MODE_GLOBAL_H_

//#ifndef VMP
//#define VMP
//#endif // !VMP

#ifndef type_cast
#define type_cast(_t_, _p_) \
            ((_t_)(ULONG_PTR)(_p_))
#endif // !type_cast

#ifndef _KERNEL_MODE
#define UMDF_USING_NTSTATUS

#include <windows.h>
#include <intrin.h>
#include <ntstatus.h>
#include <winternl.h>
#include <psapi.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#pragma comment(lib, "ntdllp.lib")

#ifndef VMP
#define RDE_VMP_BEGIN(_x_) __noop
#define RDE_VMP_END __noop
#else
#define RDE_VMP_BEGIN(_x_) VMProtectBeginUltra(_x_)
#define RDE_VMP_END VMProtectEnd
#include "..\ThirdParty\VMProtect\Include\VMProtectSDK.h"
#ifndef _WIN64
#pragma comment(lib, "..\\ThirdParty\\VMProtect\\Lib\\VMProtectSDK32.lib")
#else
#pragma comment(lib, "..\\ThirdParty\\VMProtect\\Lib\\VMProtectSDK64.lib")
#endif // !_WIN64
#endif // VMP

#ifndef RdeCurrentProcess
#define RdeCurrentProcess() type_cast(PVOID, (-1L))
#endif // !RdeCurrentProcess

#ifndef RdeCurrentThread
#define RdeCurrentThread() type_cast(PVOID, (-2L))
#endif // !RdeCurrentThread

#ifndef RdeCurrentSession
#define RdeCurrentSession() type_cast(PVOID, (-3L))
#endif // !RdeCurrentSession

#ifndef PAGED_CODE
#define PAGED_CODE() __noop
#endif // !PAGED_CODE
#else
#include <ntifs.h>
#include <intrin.h>
#include <ntddk.h>
#include <ntimage.h>
#include <ntddkbd.h>
#include <ntddmou.h>
#include <wmilib.h>
#include <ntstrsafe.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>

#ifndef VMP
#define RDE_VMP_BEGIN(_x_) __noop
#define RDE_VMP_END __noop
#else
#define RDE_VMP_BEGIN(_x_) VMProtectBeginUltra(_x_)
#define RDE_VMP_END VMProtectEnd
#include "..\ThirdParty\VMProtect\Include\VMProtectDDK.h"
#ifndef _WIN64
#pragma comment(lib, "..\\ThirdParty\\VMProtect\\Lib\\VMProtectDDK32.lib")
#else
#pragma comment(lib, "..\\ThirdParty\\VMProtect\\Lib\\VMProtectDDK64.lib")
#endif // !_WIN64
#endif // VMP

#ifndef ZwCurrentProcess
#define ZwCurrentProcess() type_cast(PVOID, (-1L))
#endif // !ZwCurrentProcess

#ifndef ZwCurrentThread
#define ZwCurrentThread() type_cast(PVOID, (-2L))
#endif // !RdeCurrentThread

#ifndef ZwCurrentRdession
#define ZwCurrentRdession() type_cast(PVOID, (-3L))
#endif // !ZwCurrentRdession
#endif // _KERNEL_MODE

#ifndef SUFFI32
#define SUFFI32 (L"x86")
#endif // !SUFFI32

#ifndef SUFFI64
#define SUFFI64 (L"x64")
#endif // !SUFFI64

#ifndef SUFFI_LENGTH
#define SUFFI_LENGTH 3
#endif // !SUFFI_LENGTH

#ifndef ALIGN32
#define ALIGN32 (0x20)
#endif // !ALIGN32

#ifndef ALIGN64
#define ALIGN64 (0x40)
#endif // !ALIGN64

#ifndef ALIGN
#ifndef _WIN64
#define ALIGN ALIGN32
#else
#define ALIGN ALIGN64
#endif // !_WIN64
#endif // !ALIGN

#ifndef type_add
#define type_add(_p1_, _p2_) \
            type_cast(PVOID, \
            type_cast(ULONG_PTR, (_p1_)) + type_cast(ULONG_PTR, (_p2_)))
#endif // !type_add

#ifndef type_sub
#define type_sub(_p1_, _p2_) \
            type_cast(PVOID, \
            type_cast(ULONG_PTR, (_p1_)) - type_cast(ULONG_PTR, (_p2_)))
#endif // !type_sub

#ifndef _jmptova
#define _jmptova(_p_) \
            type_add(type_add((_p_), *type_cast(LONG *, (_p_))), 04l)
#endif // !_jmptova

#ifndef _vatojmp
#define _vatojmp(_p1_, _p2_) \
            type_cast(LONG, type_sub(type_sub((_p2_), (_p1_)), 04l))
#endif // !_vatojmp

#ifndef _offsetof
#define _offsetof(s, m) \
            ((SIZE_T)&(((s*)0)->m))
#endif // !_offsetof

#ifndef EXTERN
#define EXTERN extern
#endif // !EXTERN

#ifndef STATIC
#define STATIC static
#endif // !STATIC

#ifndef REGISTER
#define REGISTER register
#endif // !REGISTER

#ifndef CDECL
#define CDECL __cdecl
#endif // !CDECL

#ifndef LOBYTE
#define LOBYTE(_x_) \
            type_cast(UCHAR, \
                (type_cast(ULONG_PTR, (_x_))) & 0xff)
#endif // !LOBYTE

#ifndef HIBYTE
#define HIBYTE(_x_) \
            type_cast(UCHAR, \
                (type_cast(ULONG_PTR, (_x_)) >> 8) & 0xff)
#endif // !HIBYTE

#ifndef LOWORD
#define LOWORD(_x_) \
            type_cast(USHORT, \
                (type_cast(ULONG_PTR, (_x_))) & 0xffff)
#endif // !LOWORD

#ifndef HIWORD
#define HIWORD(_x_) \
            type_cast(USHORT, \
                ((type_cast(ULONG_PTR, (_x_))) >> 16) & 0xffff)
#endif // !HIWORD

#ifndef LODWOWD
#define LODWOWD(_x_) \
            type_cast(ULONG, \
                (type_cast(ULONG_PTR, (_x_))) & 0xffffffff)
#endif // !LODWOWD

#ifndef HIDWOWD
#define HIDWOWD(_x_) \
            type_cast(ULONG, \
                ((type_cast(ULONG_PTR, (_x_))) >> 32) & 0xffffffff)
#endif // !HIDWOWD

#ifndef TYPE32
#define TYPE32(_x_) ULONG
#endif // !TYPE32

#ifndef TYPE64
#define TYPE64(_x_) ULONGLONG
#endif // !TYPE64

#ifndef TYPE_PTR
#ifndef _WIN64
#define TYPE_PTR(_x_) TYPE32(_x_)
#else
#define TYPE_PTR(_x_) TYPE32(_x_)
#endif // !_WIN64
#endif // !TYPE_PTR

#ifndef _countof
#define _countof(_x_) \
            (sizeof(_x_) / sizeof(_x_[0]))
#endif // !_countof

#ifndef MAX_PATH
#define MAX_PATH 260
#endif // !MAX_PATH

#ifndef SPECIAL_FLAGS
#define SPECIAL_FLAGS 0xf0f0f0f0
#endif // !SPECIAL_FLAGS

#endif // !_USER_MODE_GLOBAL_H_
