#include "BasicHost.h"

// 创建Host
extern "C" auto CreateHost(KineCT::ICTServer* pServer, KineCT::ICTHost** ppHost) noexcept ->HRESULT {
    // 检测参数
    if (!pServer || !ppHost) {
        assert(!"bad arguments");
        return E_INVALIDARG;
    }

}


// CCTBasicHost 释放
void KineCT::CCTBasicHost::Release() noexcept {
    delete this;
}

// CCTBasicHost 构造函数
KineCT::CCTBasicHost::CCTBasicHost(ICTServer* server) noexcept:m_pServer(server){
    assert(server);
    ::memset(&m_cMediaType, 0, sizeof(m_cMediaType));

}

// CCTBasicHost 析构函数
KineCT::CCTBasicHost::~CCTBasicHost() noexcept {
    // 关闭V2
#ifdef USING_KINECT_V2
    this->shut_down_v2();
#endif
}

#ifdef USING_KINECT_V2
// 打开KinectV2
void KineCT::CCTBasicHost::open_v2() noexcept {
    assert(!m_hKinectV2);
    m_hKinectV2 = ::LoadLibraryW(L"Kinect20.dll");
    // 检查
    if (!m_hKinectV2) {
        assert(!"m_hKinectV2 -> null");
        ::MessageBoxW(nullptr, L"Cannot Find 'Kinect20.dll' 未找到", L"Error", MB_ICONERROR);
        return;
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