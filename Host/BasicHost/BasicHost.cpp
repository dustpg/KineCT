#include "BasicHost.h"


// CCTBasicHost 构造函数
KineCT::CCTBasicHost::CCTBasicHost(ICTServer* server) noexcept:m_pServer(server) {
    assert(server);
    ::memset(&m_cMediaType, 0, sizeof(m_cMediaType));

}

// CCTBasicHost 析构函数
KineCT::CCTBasicHost::~CCTBasicHost() noexcept {
    // 关闭V2
#ifdef USING_KINECT_V2
    this->shut_down_v2();
#endif
    // 关闭控制台
    ::FreeConsole();
}


// 初始化
auto KineCT::CCTBasicHost::Initialize() noexcept -> HRESULT {
    // 打开控制台窗口
    ::AllocConsole();
    m_hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    m_hStdIn = ::GetStdHandle(STD_INPUT_HANDLE);
    // 打开Kinect
#ifdef USING_KINECT_V2
    return this->open_v2();
#endif
    return E_NOTIMPL;
}

// 设置媒体类型
auto KineCT::CCTBasicHost::SetMediaType(const CTMEDIATYPE& type) noexcept -> HRESULT {
    m_cMediaType = type;
    return S_OK;
}

// 写入缓存
auto KineCT::CCTBasicHost::FillBuffer(uint8_t* buffer, size_t length) noexcept -> HRESULT{
    // 检查输入

    // 填写缓冲区
    if (m_pProcessor) {
        return m_pProcessor->FillBuffer(buffer, length);
    }
    for (auto itr = buffer; itr < buffer + length; ++itr) {
        *itr = ::rand();
    }
    return S_OK;
}

#include <cstdarg>
#include <cstdio>
#include <cwchar>

// 格式化输出
void KineCT::CCTBasicHost::wprintf(const wchar_t* format, ...) noexcept {
    wchar_t buffer[1024 * 8];
    std::va_list va;
    va_start(va, format);
    ::vswprintf_s(buffer, lengthof(buffer), format, va);
    va_end(va);
    // 输出
    ::WriteConsoleW(m_hStdOut, buffer, std::wcslen(buffer), nullptr, nullptr);
}