#include "BasicHost.h"
#include <new>
// 创建Host
extern "C" auto CreateHost(KineCT::ICTServer* pServer, KineCT::ICTHost** ppHost) noexcept ->HRESULT {
    // 检测参数
    if (!pServer || !ppHost) {
        assert(!"bad arguments");
        return E_INVALIDARG;
    }
    auto host = new(std::nothrow) KineCT::CCTBasicHost(pServer);
    register auto hr = host ? S_OK : E_OUTOFMEMORY;
    *ppHost = host;
    return hr;
}


// CCTBasicHost 释放
void KineCT::CCTBasicHost::Release() noexcept {
    delete this;
}

#ifdef USING_KINECT_V2
// 打开KinectV2
auto KineCT::CCTBasicHost::open_v2() noexcept ->HRESULT {
    assert(!m_hKinectV2);
    m_hKinectV2 = ::LoadLibraryW(L"Kinect20.dll");
    // 检查
    if (!m_hKinectV2) {
        assert(!"m_hKinectV2 -> null");
        ::MessageBoxW(nullptr, L"Cannot Find 'Kinect20.dll' 未找到", L"Error", MB_ICONERROR);
        return STG_E_FILENOTFOUND;
    }
    // 获取默认Kinect
    auto getDefaultKinectSensor = reinterpret_cast<decltype(&::GetDefaultKinectSensor)>(
        ::GetProcAddress(m_hKinectV2, "GetDefaultKinectSensor")
        );
    auto hr = getDefaultKinectSensor(&m_pKinectV2);
    // 绅士地打开Kinect
    if (SUCCEEDED(hr)) {
        hr = m_pKinectV2->Open();
    }
    // 检查错误
    if (FAILED(hr)) {
        ::MessageBoxW(nullptr, L"<KineCT::CCTBasicHost::open_v2> FAILED", L"Error", MB_ICONERROR);
    }
    return hr;
}

// 关闭KinectV2
void KineCT::CCTBasicHost::shut_down_v2() noexcept {
    // 优雅地关闭Kinect
    if (m_pKinectV2) {
        m_pKinectV2->Close();
        m_pKinectV2->Release();
        m_pKinectV2 = nullptr;
    }
    // 释放Dll
    if (m_hKinectV2) {
        ::FreeLibrary(m_hKinectV2);
        m_hKinectV2 = nullptr;
    }
}
#endif