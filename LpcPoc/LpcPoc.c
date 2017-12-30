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
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Initial Developer of the Original Code is blindtiger.
*
*/

#include "Global.h"
#include "Log.h"
#include "LpcPoc.h"

#define LPCPOC_APIPORT L"\\LpcPoc"

typedef
NTSTATUS
(NTAPI * PSE_THREAD_START_ROUTINE)(
    __in PVOID ThreadParameter
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateProcess(
    __in_opt HANDLE ProcessHandle,
    __in NTSTATUS ExitStatus
);;

NTSYSCALLAPI
NTSTATUS
NTAPI
RtlCreateUserThread(
    __in HANDLE Process,
    __in PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
    __in BOOLEAN CreateSuspended,
    __in ULONG StackZeroBits,
    __in_opt SIZE_T MaximumStackSize,
    __in_opt SIZE_T InitialStackSize,
    __in PSE_THREAD_START_ROUTINE StartAddress,
    __in PVOID Parameter,
    __out PHANDLE Thread,
    __out PCLIENT_ID ClientId
);

DECLSPEC_NORETURN
NTSYSCALLAPI
VOID
NTAPI
RtlExitUserThread(
    __in NTSTATUS ExitStatus
);

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateSecurityDescriptor(
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    ULONG Revision
);

NTSYSAPI
NTSTATUS
NTAPI
RtlSetDaclSecurityDescriptor(
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN DaclPresent,
    PACL Dacl,
    BOOLEAN DaclDefaulted
);

NTSTATUS
NTAPI
RdeLpcCreateSeApiPort(
    __out PHANDLE SeApiPortHandle
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    SECURITY_DESCRIPTOR SecurityDescriptor = { 0 };
    UNICODE_STRING ApiPortPath = { 0 };
    OBJECT_ATTRIBUTES ObjectAttributes = { 0 };

    Status = RtlCreateSecurityDescriptor(
        &SecurityDescriptor,
        SECURITY_DESCRIPTOR_REVISION);

    if (NT_SUCCESS(Status)) {
        Status = RtlSetDaclSecurityDescriptor(
            &SecurityDescriptor,
            TRUE,
            NULL,
            FALSE);

        if (NT_SUCCESS(Status)) {
            RtlInitUnicodeString(&ApiPortPath, LPCPOC_APIPORT);

            InitializeObjectAttributes(
                &ObjectAttributes,
                &ApiPortPath,
                0,
                NULL,
                &SecurityDescriptor);

            Status = NtCreatePort(
                SeApiPortHandle,
                &ObjectAttributes,
                0,
                PORT_MAXIMUM_MESSAGE_LENGTH,
                0);
        }
    }

    return Status;
}

NTSTATUS
NTAPI
ApiPortServer(
    __in PVOID Parameter
)
{
    UNREFERENCED_PARAMETER(Parameter);

    NTSTATUS Status = STATUS_SUCCESS;
    HANDLE ApiPortHandle = NULL;
    HANDLE PortHandle = NULL;
    PORT_MESSAGE ReceiveMessage = { 0 };
    PPORT_MESSAGE ReplyMessage = NULL;

    //创建服务器端通信端口
    Status = RdeLpcCreateSeApiPort(
        &ApiPortHandle);

    if (NT_SUCCESS(Status)) {
        while (TRUE) {
            //等待端口消息 
            Status = NtReplyWaitReceivePort(
                ApiPortHandle,
                NULL,
                NULL,
                &ReceiveMessage);

            if (NT_SUCCESS(Status)) {
                ReplyMessage = &ReceiveMessage;

                //判断消息类型
                if (LPC_CONNECTION_REQUEST == ReceiveMessage.u2.s2.Type) {
                    Status = NtAcceptConnectPort(
                        &PortHandle,
                        NULL,
                        &ReceiveMessage,
                        TRUE,
                        NULL,
                        NULL);

                    RDE_DEBUG_OUTPUT(
                        TEXT("[%4d : %4d] NtAcceptConnectPort [%08x].\n"),
                        ReceiveMessage.u3.ClientId.UniqueThread,
                        ReceiveMessage.u2.s2.Type,
                        Status);

                    if (NT_SUCCESS(Status)) {
                        Status = NtCompleteConnectPort(
                            PortHandle);

                        RDE_DEBUG_OUTPUT(
                            TEXT("[%4d : %4d] NtCompleteConnectPort [%08x].\n"),
                            ReceiveMessage.u3.ClientId.UniqueThread,
                            ReceiveMessage.u2.s2.Type,
                            Status);
                    }

                    continue;
                }
                else if (LPC_REQUEST == ReceiveMessage.u2.s2.Type) {
                    // 多线程需要拷贝消息数据 这里无附加数据
                    Status = NtReplyPort(ApiPortHandle, ReplyMessage);

                    RDE_DEBUG_OUTPUT(
                        TEXT("[%4d : %4d] NtReplyPort [%08x].\n"),
                        ReceiveMessage.u3.ClientId.UniqueThread,
                        ReceiveMessage.u2.s2.Type,
                        Status);
                }
                else if (LPC_PORT_CLOSED == ReceiveMessage.u2.s2.Type) {
                    continue;
                }
                else if (LPC_CLIENT_DIED == ReceiveMessage.u2.s2.Type) {
                    continue;
                }
                else {
                    RDE_DEBUG_OUTPUT(
                        TEXT("Unknow [%x]\n"),
                        ReceiveMessage.u2.s2.Type);

                    continue;
                }
            }
        }

        NtClose(ApiPortHandle);
    }

    RtlExitUserThread(Status);
}

NTSTATUS
NTAPI
ConnectApiPort(
    __out PHANDLE PortHandle
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING ApiPortName = { 0 };
    SECURITY_QUALITY_OF_SERVICE SecurityQos = { 0 };
    ULONG MaxMessageLength = 0;

    RtlInitUnicodeString(
        &ApiPortName,
        LPCPOC_APIPORT);

    SecurityQos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
    SecurityQos.ImpersonationLevel = SecurityImpersonation;
    SecurityQos.EffectiveOnly = FALSE;
    SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;

    Status = NtConnectPort(
        PortHandle,
        &ApiPortName,
        &SecurityQos,
        NULL,
        NULL,
        &MaxMessageLength,
        NULL,
        NULL);

    return Status;
}

NTSTATUS
NTAPI
ApiPortClient(
    __in PVOID Parameter
)
{
    UNREFERENCED_PARAMETER(Parameter);

    NTSTATUS Status = STATUS_SUCCESS;
    HANDLE PortHandle = NULL;
    PORT_MESSAGE RequestMessage = { 0 };

    //客户端连接
    Status = ConnectApiPort(&PortHandle);

    RDE_DEBUG_OUTPUT(
        TEXT("[%4d] ConnectApiPort [%08x].\n"),
        GetCurrentThreadId(),
        Status);

    if (NT_SUCCESS(Status)) {
        RequestMessage.u1.s1.TotalLength = PORT_MAXIMUM_MESSAGE_LENGTH;
        RequestMessage.u1.s1.DataLength = PORT_MAXIMUM_MESSAGE_LENGTH - sizeof(PORT_MESSAGE);

        //发送消息 等待服务端响应 阻塞
        Status = NtRequestWaitReplyPort(
            PortHandle,
            &RequestMessage,
            &RequestMessage);

        RDE_DEBUG_OUTPUT(
            TEXT("[%4d] NtRequestWaitReplyPort [%08x].\n"),
            GetCurrentThreadId(),
            Status);

        NtClose(PortHandle);
    }

    RDE_DEBUG_OUTPUT(
        TEXT("[%4d] Thread exit.\n"),
        GetCurrentThreadId());

    RtlExitUserThread(Status);
}

VOID
NTAPI
Main(
    VOID
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    HANDLE ThreadHandle[4] = { 0 };

    //启动服务线程 这里最好用个事件同步下
    Status = RtlCreateUserThread(
        RdeCurrentProcess(),
        NULL,
        FALSE,
        0,
        0,
        0,
        ApiPortServer,
        NULL,
        ThreadHandle,
        NULL);

    //启动客户端线程1
    Status = RtlCreateUserThread(
        RdeCurrentProcess(),
        NULL,
        FALSE,
        0,
        0,
        0,
        ApiPortClient,
        NULL,
        ThreadHandle + 1,
        NULL);

    //启动客户端线程2
    Status = RtlCreateUserThread(
        RdeCurrentProcess(),
        NULL,
        FALSE,
        0,
        0,
        0,
        ApiPortClient,
        NULL,
        ThreadHandle + 2,
        NULL);

    //启动客户端线程3
    Status = RtlCreateUserThread(
        RdeCurrentProcess(),
        NULL,
        FALSE,
        0,
        0,
        0,
        ApiPortClient,
        NULL,
        ThreadHandle + 3,
        NULL);

    WaitForMultipleObjects(
        _countof(ThreadHandle),
        ThreadHandle,
        TRUE,
        INFINITE);

    for (SIZE_T i = 0;
        i < _countof(ThreadHandle);
        i++) {
        NtClose(ThreadHandle[i]);
    }

    NtTerminateProcess(
        RdeCurrentProcess(),
        STATUS_SUCCESS);
}
