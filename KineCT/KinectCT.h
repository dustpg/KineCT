#pragma once
// KineCT -- Kinect Camera Transformer, a virtual camera for kinect that 
// can be added some effects

// License: MIT  http://opensource.org/licenses/MIT
// Author: dustpg   mailto:dustpg@gmail.com

// base classed
#include <streams.h>
#include <dllsetup.h>

// std library
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstddef>

// ct header
#include "ctconfig.h"
#include "Struct.h"
#include "Interface.h"

// kinect ct namesapce
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
    // safe addref the interface
    template<class T> auto SafAddRef(T* passed_interface) {
        if (passed_interface) {
            passed_interface->AddRef();
        }
        return passed_interface;
    }
    // the ct source
    class CCTSource final : public ICTServer, public CSource {
    public:
        // creat the instance
        static auto WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr) noexcept ->CUnknown *;
        // qi
        STDMETHODIMP QueryInterface(REFIID riid, void **ppv) noexcept override;
        // get
        auto GetGraph() const noexcept { return m_pGraph; }
    public: // ICTServer impl
        // exit the host
        void ExitHost() noexcept override;
    private:
        // ctor
        CCTSource(LPUNKNOWN lpunk, HRESULT *phr) noexcept;
    };
    // the ct stream override
    class CCTStream final : public CSourceStream, public IAMStreamConfig, public IKsPropertySet {
    public:
        // ctor
        CCTStream(HRESULT *phr, CCTSource *pParent, LPCWSTR pPinName) noexcept;
        // dtor
        ~CCTStream() noexcept;
    protected: // CSourceStream impl
        // fill the buffer
        HRESULT FillBuffer(IMediaSample *pSamp) noexcept;
        // when thread created
        HRESULT OnThreadCreate(void) noexcept override;
        // check the media type
        HRESULT CheckMediaType(const CMediaType *pMediaType) noexcept override;
        // get the media type
        HRESULT GetMediaType(int iPosition, CMediaType *pmt) noexcept override;
        // set the media type
        HRESULT SetMediaType(const CMediaType *pmt) noexcept override;
    protected: // CBaseOutputPin impl
        HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc, ALLOCATOR_PROPERTIES *pProperties) noexcept override;
    public: // IUnknown impl
        // qi
        STDMETHODIMP QueryInterface(REFIID riid, void **ppv) noexcept override;
        // add the ref count
        STDMETHODIMP_(ULONG) AddRef() noexcept override;
        // release the ref count
        STDMETHODIMP_(ULONG) Release() noexcept override;
    public: // IQualityControl impl
        // notify
        STDMETHODIMP Notify(IBaseFilter * pSender, Quality q) noexcept override;
    public: // IAMStreamConfig impl
        // format setter
        STDMETHODIMP SetFormat(AM_MEDIA_TYPE *pmt)  noexcept override;
        // format getter
        STDMETHODIMP GetFormat(AM_MEDIA_TYPE **ppmt)  noexcept override;
        // cap num
        STDMETHODIMP GetNumberOfCapabilities(int *piCount, int *piSize)  noexcept override;
        // stream caps
        STDMETHODIMP GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC) noexcept override;
    public: // IKsPropertySet impl
        // IKsPropertySet::Set
        STDMETHODIMP Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData) noexcept override;
        // IKsPropertySet::Get
        STDMETHODIMP Get(REFGUID guidPropSet, DWORD dwPropID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData, DWORD *pcbReturned) noexcept override;
        // IKsPropertySet::QuerySupported
        STDMETHODIMP QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport) noexcept override;
    private:
        // source
        CCTSource*              m_pParent = nullptr;
        // last time
        REFERENCE_TIME          m_rtLastTime;
        // state
        CCritSec                m_cSharedState;
        // clock
        IReferenceClock*        m_pClock = nullptr;
    };

};

