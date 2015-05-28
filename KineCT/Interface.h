#pragma once
// License: MIT  http://opensource.org/licenses/MIT
// Author: dustpg   mailto:dustpg@gmail.com

// kinect ct namespace
namespace KineCT {
    // ct interface
    class KINECT_NOVTABLE ICTInterface {
    public:
        // release the interface
        virtual void Release() noexcept = 0;
    };
    // ct processor
    class KINECT_NOVTABLE ICTProcessor : public ICTInterface {
    public:
        // fill the buffer
        virtual auto FillBuffer(uint8_t* buffer, size_t length) noexcept->HRESULT = 0;
        // show the options
        virtual auto GetOptions(const CTOPTION* options, uint32_t size) noexcept->HRESULT = 0;
        // set the options
        virtual auto SetOption(uint32_t indexop, uint32_t indexsubop) noexcept->HRESULT = 0;
        // get the requested directx device
        virtual auto RequestedDevice(IID& KINECT_OUT iid) noexcept->HRESULT = 0;
        // initialize the kinect device
        virtual auto InitializeKinect(void* kinect, const IID& iid) noexcept->HRESULT = 0;
    };
    // ct host
    class KINECT_NOVTABLE ICTHost : public ICTInterface {
    public:
        // initialize
        virtual auto Initialize() noexcept->HRESULT = 0;
        // set media type
        virtual auto SetMediaType(const CTMEDIATYPE&) noexcept->HRESULT = 0;
        // fill the buffer
        virtual auto FillBuffer(uint8_t* buffer, size_t length) noexcept->HRESULT = 0;
    };
    // ct server
    class KINECT_NOVTABLE ICTServer {
    public:
        // exit the host
        virtual void ExitHost() noexcept = 0;
        // set new media type
    };
}