#include "KinectCT.h"


STDAPI AMovieSetupRegisterServer(CLSID   clsServer
    , LPCWSTR szDescription
    , LPCWSTR szFileName
    , LPCWSTR szThreadingModel = L"Both"
    , LPCWSTR szServerType = L"InprocServer32");


STDAPI AMovieSetupUnregisterServer(CLSID clsServer);

// kinect ct
namespace KineCT {
    // type
    const AMOVIESETUP_MEDIATYPE AMSMediaTypesKCam = {
        &MEDIATYPE_Video,
        &MEDIASUBTYPE_NULL
    };
    // pin
    const AMOVIESETUP_PIN PinsKCam[] = {
        {
            L"Image Output",        // Pin string name
            FALSE,                  // Is it rendered
            TRUE,                   // Is it an output
            FALSE,                  // Can we have none
            FALSE,                  // Can we have many
            &CLSID_NULL,            // Connects to filter
            NULL,                   // Connects to pin
            1,                      // Number of types
            &AMSMediaTypesKCam      // Pin Media types
        } 
    };
    // filter
    const AMOVIESETUP_FILTER AMSFilterKCam =  {
        &GUID_KineCTCam,                // Filter CLSID
        L"Kinect Camera Transformer",   // String name
        MERIT_DO_NOT_USE,               // Filter merit
        1,                              // Number pins
        PinsKCam                        // Pin details
    };
    // ע��Filters
    auto RegisterFilters(BOOL bRegister) noexcept { 
        WCHAR achFileName[MAX_PATH];
        ASSERT(g_hInst != 0);
        // ���·��
        if (!::GetModuleFileNameW(g_hInst, achFileName, lengthof(achFileName))) {
            return AmHresultFromWin32(::GetLastError());
        }
        // ��ʼ��COM
        auto hr = ::CoInitialize(0);
#if 1
        // ע��
        if (bRegister) {
            hr = ::AMovieSetupRegisterServer(GUID_KineCTCam, L"Kinect Camera Transformer", achFileName, L"Both", L"InprocServer32");
        }
        // ����
        if (SUCCEEDED(hr)) {
            IFilterMapper2 *fm = nullptr;
            hr = ::CoCreateInstance(
                CLSID_FilterMapper2,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_IFilterMapper2,
                reinterpret_cast<void**>(&fm)
                );
            // ע��
            if (SUCCEEDED(hr)) {
                if (bRegister) {
                    IMoniker *pMoniker = 0;
                    REGFILTER2 rf2;
                    rf2.dwVersion = 1;
                    rf2.dwMerit = MERIT_DO_NOT_USE;
                    rf2.cPins = 1;
                    rf2.rgPins = PinsKCam;
                    hr = fm->RegisterFilter(GUID_KineCTCam, L"Kinect Camera Transformer", &pMoniker, &CLSID_VideoInputDeviceCategory, NULL, &rf2);
                }
                else {
                    hr = fm->UnregisterFilter(&CLSID_VideoInputDeviceCategory, 0, GUID_KineCTCam);
                }
            }
            // �ͷŽӿ�
            KineCT::SafeRelease(fm);
        }
        // ע��
        if (SUCCEEDED(hr) && !bRegister) {
            hr = AMovieSetupUnregisterServer(GUID_KineCTCam);
        }
#else
        if (SUCCEEDED(hr)) {
            IFilterMapper2* mapper = nullptr;
            // Filter Mapper
            hr = ::CoCreateInstance(
                CLSID_FilterMapper2, 
                nullptr, 
                CLSCTX_INPROC_SERVER, 
                IID_IFilterMapper2, 
                reinterpret_cast<void**>(&mapper)
                );
            // ����
            assert(SUCCEEDED(hr));
            if (SUCCEEDED(hr)) {
                ::AMovieSetupRegisterFilter2(&KineCT::AMSFilterKCam, mapper, bRegister);
            }
            KineCT::SafeRelease(mapper);
        }
#endif
        ::CoFreeUnusedLibraries();
        ::CoUninitialize();
        return hr;
    }
}

// ȫ��ģ��
CFactoryTemplate g_Templates[] = {
    {
        L"Kinect Camera Transformer",
        &KineCT::GUID_KineCTCam,
        KineCT::CCTSource::CreateInstance,
        nullptr,
        &KineCT::AMSFilterKCam
    },
};
// ����
int g_cTemplates = lengthof(g_Templates);


// ע�����
STDAPI DllRegisterServer() {
    ::MessageBoxW(nullptr, L"<DllRegisterServer>", L"INFO", MB_OK);
    return KineCT::RegisterFilters(TRUE);
}

// ע������
STDAPI DllUnregisterServer() {
    ::MessageBoxW(nullptr, L"<DllUnregisterServer>", L"INFO", MB_OK);
    return KineCT::RegisterFilters(FALSE);
}

// base classes �������ں���
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

// Dll ������
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved) {

    return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

#ifdef _DEBUG
#pragma comment(lib, "../Debug/BaseClasses")
#else
#pragma comment(lib, "../Release/BaseClasses")
#endif
#pragma comment(lib, "dxguid")
#pragma comment(lib, "Strmiids")
#pragma comment(lib, "winmm")

