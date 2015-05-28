#include <Windows.h>
#include <cstdint>
#include "../../KineCT/Struct.h"



// Ӧ�ó������
int WINAPI wWinMain(HINSTANCE, HINSTANCE, wchar_t* lpCmdLine, int) {
    // ���������
    if (!lpCmdLine[0]) {
        ::MessageBoxW(nullptr, L"HostLauncher.exe [DllFilePath]", L"command line helper", MB_OK);
    }
    else {
        constexpr int BUFFER_LENGTH = 4096;
        const wchar_t* KineCT__PIPE_NAME = LR"pipe(\\.\pipe\KineCT)pipe";
        const wchar_t* KineCT__EVENT_NAME = LR"KCT(Global\EVENT_KCT)KCT";
        uint8_t buf[BUFFER_LENGTH];
        auto length = ::wcslen(lpCmdLine);
        // ȥ������
        if (lpCmdLine[0] == L'"') {
            ++lpCmdLine;
            length -= 2;
        }
        ::memcpy(buf, lpCmdLine, length * sizeof(wchar_t));
        buf[length] = L'\0';
        // �����ȴ��¼�
        auto hFinished = ::CreateEventW(nullptr, TRUE, FALSE, L"");
        if (hFinished == INVALID_HANDLE_VALUE) {
            ::MessageBoxW(nullptr, L"CreateEvent", L"FAILED" , MB_ICONERROR);
            return -1;
        }
        // �����ܵ�
        auto hPipe = ::CreateNamedPipeW(
            KineCT__PIPE_NAME,  // �ܵ�����
            PIPE_ACCESS_OUTBOUND,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1,
            4096,   // �������
            0,      // ���뻺��
            1,
            nullptr
            );
        // ������
        if (hPipe == INVALID_HANDLE_VALUE) {
            ::MessageBoxW(nullptr, L"CreateNamedPipe failed", L"Failed", MB_ICONERROR);
            ::CloseHandle(hFinished);
            return -1;
        }
        // �ȴ�����
        auto recode = [=]() -> int {
            // �ȴ�����������
            BOOL bConnected = ::ConnectNamedPipe(hPipe, nullptr) ?
                TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
            // ����ʧ��
            if (!bConnected) {
                ::MessageBoxW(nullptr, L"ConnectNamedPipe failed", L"Failed", MB_ICONERROR);
                return -1;
            }
            // д������
            DWORD cbWritten = 0;
            auto ok = ::WriteFile(hPipe, buf, (length + 1)*sizeof(wchar_t), &cbWritten, nullptr
                );
            return ok ? 0 : -1;
        }();
        // �ɹ�, ��ȴ�
        if (recode && ::WaitForSingleObject(hFinished, 10'000) == WAIT_TIMEOUT) {
            ::MessageBoxW(nullptr, L"WaitForSingleObject Time Out", L"Failed", MB_ICONERROR);
        }
        // �Ͽ�����
        ::DisconnectNamedPipe(hPipe);
        // �رվ��
        ::CloseHandle(hPipe);
        ::CloseHandle(hFinished);
        hFinished = INVALID_HANDLE_VALUE;
        hPipe = INVALID_HANDLE_VALUE;
    }
    return 0;
}


#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif