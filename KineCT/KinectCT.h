#pragma once
// KineCT -- Kinect Camera Transformer, a virtual camera for kinect that 
// can be added some effects

// License: MIT  http://opensource.org/licenses/MIT
// Author: dustpg   mailto:dustpg@gmail.com

// std library
#include <windows.h>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstddef>

// base classed
#include <streams.h>
#include <dllsetup.h>


//extern HINSTANCE g_hInst;

// helper
namespace KineCT {
    // {E1370B6F-7092-4145-8565-E517645F8380}
    static const GUID GUID_KineCTCam =
    { 0xe1370b6f, 0x7092, 0x4145,{ 0x85, 0x65, 0xe5, 0x17, 0x64, 0x5f, 0x83, 0x80 } };

    // safe release the interface
    template<class T> auto SafeRelease(T*& passed_interface) {
        if (passed_interface) {
            passed_interface->Release();
            passed_interface = nullptr;
        }
    }
    //
    class CKCamStream : public CSourceStream, public IAMStreamConfig, public IKsPropertySet {

    };

};

// ct header
#include "ctconfig.h"
#include "Struct.h"
#include "Interface.h"

