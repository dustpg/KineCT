#include <Windows.h>
#include <Strmif.h>

static const GUID GUID_KineCTCam =
{ 0xe1370b6f, 0x7092, 0x4145,{ 0x85, 0x65, 0xe5, 0x17, 0x64, 0x5f, 0x83, 0x80 } };

// 应用程序入口
int main() {
    // 初始化
    if (SUCCEEDED(::CoInitialize(nullptr))) {
        IBaseFilter* filter = nullptr;
        auto hr = ::CoCreateInstance(
            GUID_KineCTCam,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&filter)
            );
        if (filter) {
            filter->Release();
            filter = nullptr;
        }
        ::CoUninitialize();
    }
    return EXIT_SUCCESS;
}