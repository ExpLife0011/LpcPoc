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

#ifndef _USER_MODE_LOG_H_
#define _USER_MODE_LOG_H_

#ifndef RDE_DEBUG_OUTPUT

#define RDE_DEBUG_OUTPUT(_X_, ...) \
            RdeDbgPrint( \
                TEXT("[RDE][LpcPoc] ") ##_X_, ##__VA_ARGS__)

#endif // !RDE_DEBUG_OUTPUT

#ifndef RDE_SUCCESS

#define RDE_SUCCESS(_exp) \
            (((NTSTATUS)(Status)) >= 0) ? \
                TRUE : (RDE_DEBUG_OUTPUT( \
                TEXT("%hs[%d] failed : [%08x]\n"), \
                __FILE__, \
                __LINE__, \
                _exp), FALSE)

#endif // !RDE_SUCCESS

VOID
CDECL
RdeDbgPrint(
    __in LPCTSTR Format,
    __in ...
);

#endif // !_USER_MODE_LOG_H_
