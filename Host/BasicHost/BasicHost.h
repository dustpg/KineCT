#pragma once
#include "../../KineCT/KinectCT.h"


// Kinect V1, Do not support
#define USING_KINECT_V1
// Kinect V2
#define USING_KINECT_V2


#ifdef USING_KINECT_V2
#include <Kinect.h>
#endif


// namespace 
namespace KineCT {
    // Basic Host 
    class alignas(sizeof(void*)) CCTBasicHost final : public ICTHost {
    public: // impl ICTInterface
        // release the interface
        void Release() noexcept override;
    public: // impl ICTHost
        // initialize
        auto Initialize() noexcept->HRESULT;
        // set media type
        virtual auto SetMediaType(const CTMEDIATYPE&) noexcept->HRESULT;
        // fill the buffer
        virtual auto FillBuffer(uint8_t* buffer, size_t length) noexcept->HRESULT;
    public:
        // ctor
        CCTBasicHost(ICTServer*) noexcept;
    private:
        // ctor
        ~CCTBasicHost() noexcept;
    private:
        // server
        ICTServer*                  m_pServer = nullptr;
        // current processor
        ICTProcessor*               m_pProcessor = nullptr;
        // media type
        CTMEDIATYPE                 m_cMediaType;
#ifdef USING_KINECT_V2
        // open the kinect v2
        void open_v2() noexcept;
        // shut down the kinect v2
        void shut_down_v2() noexcept;
        // dll for kinect v2
        HMODULE                     m_hKinectV2 = nullptr;
        // kinect v2
        IKinectSensor*              m_pKinectV2 = nullptr;
        // reader for color frame
        IColorFrameReader*          m_pColorFrameReader = nullptr;
#endif
    };
}