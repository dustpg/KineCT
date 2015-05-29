#include <Windows.h>

// 应用程序入口
int WINAPI wWinMain(HINSTANCE, HINSTANCE, wchar_t* lpCmdLine, int) {
    // 检查命令行
    if (!lpCmdLine[0]) {
        ::MessageBoxW(nullptr, L"HostLauncher.exe [DllFilePath]", L"command line helper", MB_OK);
    }
    else {
        constexpr int BUFFER_LENGTH = 4096;
        const wchar_t* KineCT__PIPE_NAME = LR"pipe(\\.\pipe\KineCT)pipe";
        const wchar_t* KineCT__EVENT_NAME = LR"KCT(Global\EVENT_KCT)KCT";
        wchar_t buffer[BUFFER_LENGTH/ sizeof(wchar_t)];
        auto length = ::wcslen(lpCmdLine);
        // 去除引号
        if (lpCmdLine[0] == L'"') {
            ++lpCmdLine;
            length -= 2;
        }
        ::memcpy(buffer, lpCmdLine, length * sizeof(wchar_t));
        buffer[length] = L'\0';
        // 创建等待事件
        auto hFinished = ::CreateEventW(nullptr, TRUE, FALSE, L"");
        if (hFinished == INVALID_HANDLE_VALUE) {
            ::MessageBoxW(nullptr, L"CreateEvent", L"FAILED" , MB_ICONERROR);
            return -1;
        }
        // 创建管道
        auto hPipe = ::CreateNamedPipeW(
            KineCT__PIPE_NAME,  // 管道名称
            PIPE_ACCESS_OUTBOUND,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1,
            4096,   // 输出缓存
            0,      // 输入缓存
            1,
            nullptr
            );
        // 检查错误
        if (hPipe == INVALID_HANDLE_VALUE) {
            ::MessageBoxW(nullptr, L"CreateNamedPipe failed", L"Failed", MB_ICONERROR);
            ::CloseHandle(hFinished);
            return -1;
        }
        // 等待连接
        auto recode = [=](wchar_t* buffer) -> int {
            // 等待服务器连接
            BOOL bConnected = ::ConnectNamedPipe(hPipe, nullptr) ?
                TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
            // 连接失败
            if (!bConnected) {
                ::MessageBoxW(nullptr, L"ConnectNamedPipe failed", L"Failed", MB_ICONERROR);
                return -1;
            }
            // 写入数据
            DWORD cbWritten = 0;
            auto ok = ::WriteFile(hPipe, buffer, (length+1)*sizeof(wchar_t), &cbWritten, nullptr
                );
            return ok ? 0 : -1;
        }(buffer);
        // 成功, 则等待
        if (recode && ::WaitForSingleObject(hFinished, 10'000) == WAIT_TIMEOUT) {
            ::MessageBoxW(nullptr, L"WaitForSingleObject Time Out", L"Failed", MB_ICONERROR);
        }
        // 断开连接
        ::DisconnectNamedPipe(hPipe);
        // 关闭句柄
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