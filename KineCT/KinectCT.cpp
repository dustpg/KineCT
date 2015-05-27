#include "KinectCT.h"

// CCTSource ����ʵ��
auto KineCT::CCTSource::CreateInstance(LPUNKNOWN lpunk, HRESULT* phr) noexcept -> CUnknown * {
    assert(phr && "bad argument");
    auto instance = new(std::nothrow) CCTSource(lpunk, phr);
    if (!instance) {
        *phr = E_OUTOFMEMORY;
    }
    return instance;
}

// CCTSource ��ѯ�ӿ�
STDMETHODIMP KineCT::CCTSource::QueryInterface(REFIID riid, void ** ppv) noexcept {
    // ��ѯ�ӿ�
    if (riid == IID_IAMStreamConfig || riid == IID_IKsPropertySet)
        return m_paStreams[0]->QueryInterface(riid, ppv);
    else
        return CSource::QueryInterface(riid, ppv);
}

// CCTSource ���캯��
KineCT::CCTSource::CCTSource(LPUNKNOWN lpunk, HRESULT *phr) noexcept:
CSource(NAME("Kinect Camera Transformer"), lpunk, GUID_KineCTCam) {
    ASSERT(phr);
    //::MessageBoxW(nullptr, L"<KineCT::CCTSource::CCTSource>", L"INFO", MB_OK);
    CAutoLock cAutoLock(&m_cStateLock);
    HRESULT hr = S_OK;
    // ����PIN
    if (SUCCEEDED(hr)) {
        m_paStreams = (CSourceStream **) new(std::nothrow) CCTStream*[1];
        if (!m_paStreams) hr = E_OUTOFMEMORY;
    }
    // ����stream
    if (SUCCEEDED(hr)) {
        m_paStreams[0] = new(std::nothrow) CCTStream(phr, this, L"Kinect Camera Transformer");
        if (!m_paStreams) hr = E_OUTOFMEMORY;
    }
    // ���HR
    if (SUCCEEDED(*phr)) *phr = hr;
}

// CCTSource �Ƴ�host
void KineCT::CCTSource::ExitHost() noexcept {

}

// ------------------------------------------

// CCTStream ���캯��
KineCT::CCTStream::CCTStream(HRESULT * phr, CCTSource * pParent, LPCWSTR pPinName) noexcept:
CSourceStream(NAME("Kinect Virtual Camera : Depth"), phr, pParent, pPinName), 
m_pParent(KineCT::SafAddRef(pParent)) {
    // Set the default media type as 640x480x24@30
    GetMediaType(8, &m_mt);
}

// CCTStream ��������
KineCT::CCTStream::~CCTStream() noexcept {
    KineCT::SafeRelease(m_pParent);
}

// ʵ��CCTStream::FillBuffer
HRESULT KineCT::CCTStream::FillBuffer(IMediaSample * pSamp) noexcept {
    HRESULT hr = S_OK;
#if 0
    //
    auto avgFrameTime = reinterpret_cast<VIDEOINFOHEADER*>(m_mt.pbFormat)->AvgTimePerFrame;
    auto rtNow = m_rtLastTime;
    m_rtLastTime += avgFrameTime;
    // ����
    BYTE* pData = nullptr; long lDataLen = 0;
    // ����ʱ��
    if (SUCCEEDED(hr)) {
        hr = pSamp->SetTime(&rtNow, &m_rtLastTime);
    }
    // ����ͬ����
    if (SUCCEEDED(hr)) {
        hr = pSamp->SetSyncPoint(TRUE);
    }
    // ��ȡָ��
    if (SUCCEEDED(hr)) {
        hr = pSamp->GetPointer(&pData);
    }
    // ��ȡ��С
    if (SUCCEEDED(hr)) {
        hr = lDataLen = pSamp->GetSize();
    }
    // ��ȡ����
    if (SUCCEEDED(hr)) {
        ::memset(pData, 0x80, lDataLen);
    }

#else
    REFERENCE_TIME rtNow;

    REFERENCE_TIME avgFrameTime = ((VIDEOINFOHEADER*)m_mt.pbFormat)->AvgTimePerFrame;

    rtNow = m_rtLastTime;
    m_rtLastTime += avgFrameTime;
    pSamp->SetTime(&rtNow, &m_rtLastTime);
    pSamp->SetSyncPoint(TRUE);

    BYTE *pData;
    long lDataLen;
    pSamp->GetPointer(&pData);
    lDataLen = pSamp->GetSize();
    register auto itr = pData;
    for (; itr < pData + lDataLen; ++itr)
        *itr = rand();

    return NOERROR;
#endif
    return hr;
}

// ʵ��CCTStream::OnThreadCreate
HRESULT KineCT::CCTStream::OnThreadCreate(void) noexcept {
    m_rtLastTime = 0;
    return S_OK;
}

// ʵ��CCTStream::CheckMediaType
HRESULT KineCT::CCTStream::CheckMediaType(const CMediaType * pMediaType) noexcept {
    VIDEOINFOHEADER *pvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->Format());
    if (*pMediaType != m_mt)
        return E_INVALIDARG;
    return S_OK;
}

// ʵ��CCTStream::GetMediaType
HRESULT KineCT::CCTStream::GetMediaType(int iPosition, CMediaType * pmt) noexcept {
    if (iPosition < 0) return E_INVALIDARG;
    if (iPosition > 8) return VFW_S_NO_MORE_ITEMS;
    // 0
    if (iPosition == 0) {
        *pmt = m_mt;
        return S_OK;
    }
    auto pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->AllocFormatBuffer(sizeof(VIDEOINFOHEADER)));
    if (!pvi) return E_OUTOFMEMORY;
    ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));

    pvi->bmiHeader.biCompression = BI_RGB;
    pvi->bmiHeader.biBitCount = 24;
    pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pvi->bmiHeader.biWidth = 80 * iPosition;
    pvi->bmiHeader.biHeight = 60 * iPosition;
    pvi->bmiHeader.biPlanes = 1;
    pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
    pvi->bmiHeader.biClrImportant = 0;

    pvi->AvgTimePerFrame = 500000;

    SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
    SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

    pmt->SetType(&MEDIATYPE_Video);
    pmt->SetFormatType(&FORMAT_VideoInfo);
    pmt->SetTemporalCompression(FALSE);

    // Work out the GUID for the subtype from the header info.
    const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
    pmt->SetSubtype(&SubTypeGUID);
    pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);
    return S_OK;
}

// ʵ��CCTStream::SetMediaType
HRESULT KineCT::CCTStream::SetMediaType(const CMediaType * pmt) noexcept {
    auto pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->Format());
    register HRESULT hr = CSourceStream::SetMediaType(pmt);
    return hr;
}

// ʵ��CCTStream::DecideBufferSize
HRESULT KineCT::CCTStream::DecideBufferSize(IMemAllocator * pIMemAlloc, ALLOCATOR_PROPERTIES * pProperties) noexcept {
    CAutoLock lock(m_pFilter->pStateLock());
    HRESULT hr = S_OK;

    auto* pvi = reinterpret_cast<VIDEOINFOHEADER*>(m_mt.Format());
    pProperties->cBuffers = 1;
    pProperties->cbBuffer = pvi->bmiHeader.biSizeImage;

    ALLOCATOR_PROPERTIES Actual;
    // ����
    if (SUCCEEDED(hr)) {
        hr = pIMemAlloc->SetProperties(pProperties, &Actual);
    }
    // ���
    if (SUCCEEDED(hr) && Actual.cbBuffer < pProperties->cbBuffer) {
        hr = E_FAIL;
    }
    return hr;
}

// ʵ�� CCTStream::QueryInterface
STDMETHODIMP KineCT::CCTStream::QueryInterface(REFIID riid, void ** ppv) noexcept {
    IUnknown* the_interface = nullptr;
    // Standard OLE stuff
    if (riid == IID_IAMStreamConfig)
        the_interface = static_cast<IAMStreamConfig*>(this);
    else if (riid == IID_IKsPropertySet)
        the_interface = static_cast<IKsPropertySet*>(this);
    else
        return CSourceStream::QueryInterface(riid, ppv);
    // ���Ӽ���
    the_interface->AddRef();
    *ppv = the_interface;
    return S_OK;
}

// ʵ�� CCTStream::AddRef
STDMETHODIMP_(ULONG) KineCT::CCTStream::AddRef() noexcept {
    return this->GetOwner()->AddRef();
}

// ʵ�� CCTStream::Release
STDMETHODIMP_(ULONG) KineCT::CCTStream::Release() noexcept {
    return this->GetOwner()->Release();
}

// ʵ�� CCTStream::Notify
STDMETHODIMP KineCT::CCTStream::Notify(IBaseFilter * pSender, Quality q) noexcept {
    return E_NOTIMPL;
}

// ʵ�� CCTStream::SetFormat
STDMETHODIMP KineCT::CCTStream::SetFormat(AM_MEDIA_TYPE * pmt) noexcept {
    auto pvi = reinterpret_cast<VIDEOINFOHEADER*>(m_mt.pbFormat);
    if (!pmt) {
        return VFW_E_INVALIDMEDIATYPE;
    }
    m_mt = *pmt;
    IPin* pin;
    ConnectedTo(&pin);
    if (pin) {
        IFilterGraph *pGraph = m_pParent->GetGraph();
        pGraph->Reconnect(this);
        //KineCT::SafeRelease(pGraph);
    }
    return S_OK;
}

// ʵ�� CCTStream::GetFormat
STDMETHODIMP KineCT::CCTStream::GetFormat(AM_MEDIA_TYPE ** ppmt) noexcept {
    *ppmt = CreateMediaType(&m_mt);
    return S_OK;
}

// ʵ�� CCTStream::GetNumberOfCapabilities
STDMETHODIMP KineCT::CCTStream::GetNumberOfCapabilities(int * piCount, int * piSize) noexcept {
    *piCount = 8;
    *piSize = sizeof(VIDEO_STREAM_CONFIG_CAPS);
    return S_OK;
}

// ʵ�� CCTStream::GetStreamCaps
STDMETHODIMP KineCT::CCTStream::GetStreamCaps(int iIndex, AM_MEDIA_TYPE ** pmt, BYTE * pSCC) noexcept {
    *pmt = CreateMediaType(&m_mt);
    auto pvi = reinterpret_cast<VIDEOINFOHEADER*>((*pmt)->pbFormat);

    if (iIndex == 0) iIndex = 4;

    pvi->bmiHeader.biCompression = BI_RGB;
    pvi->bmiHeader.biBitCount = 24;
    pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pvi->bmiHeader.biWidth = 80 * iIndex;
    pvi->bmiHeader.biHeight = 60 * iIndex;
    pvi->bmiHeader.biPlanes = 1;
    pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
    pvi->bmiHeader.biClrImportant = 0;

    SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
    SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

    (*pmt)->majortype = MEDIATYPE_Video;
    (*pmt)->subtype = MEDIASUBTYPE_RGB24;
    (*pmt)->formattype = FORMAT_VideoInfo;
    (*pmt)->bTemporalCompression = FALSE;
    (*pmt)->bFixedSizeSamples = FALSE;
    (*pmt)->lSampleSize = pvi->bmiHeader.biSizeImage;
    (*pmt)->cbFormat = sizeof(VIDEOINFOHEADER);

    auto pvscc = reinterpret_cast<VIDEO_STREAM_CONFIG_CAPS*>(pSCC);

    pvscc->guid = FORMAT_VideoInfo;
    pvscc->VideoStandard = AnalogVideo_None;
    pvscc->InputSize.cx = 640;
    pvscc->InputSize.cy = 480;
    pvscc->MinCroppingSize.cx = 80;
    pvscc->MinCroppingSize.cy = 60;
    pvscc->MaxCroppingSize.cx = 640;
    pvscc->MaxCroppingSize.cy = 480;
    pvscc->CropGranularityX = 80;
    pvscc->CropGranularityY = 60;
    pvscc->CropAlignX = 0;
    pvscc->CropAlignY = 0;

    pvscc->MinOutputSize.cx = 80;
    pvscc->MinOutputSize.cy = 60;
    pvscc->MaxOutputSize.cx = 640;
    pvscc->MaxOutputSize.cy = 480;
    pvscc->OutputGranularityX = 0;
    pvscc->OutputGranularityY = 0;
    pvscc->StretchTapsX = 0;
    pvscc->StretchTapsY = 0;
    pvscc->ShrinkTapsX = 0;
    pvscc->ShrinkTapsY = 0;
    pvscc->MinFrameInterval = 200000;   //50 fps
    pvscc->MaxFrameInterval = 50000000; // 0.2 fps
    pvscc->MinBitsPerSecond = (80 * 60 * 3 * 8) / 5;
    pvscc->MaxBitsPerSecond = 640 * 480 * 3 * 8 * 50;

    return S_OK;
}

// ʵ�� CCTStream::Set
STDMETHODIMP KineCT::CCTStream::Set(
    REFGUID guidPropSet, 
    DWORD dwID, 
    void * pInstanceData, 
    DWORD cbInstanceData, 
    void * pPropData,
    DWORD cbPropData) noexcept {
    return E_NOTIMPL;
}

// ʵ�� CCTStream::Get
STDMETHODIMP KineCT::CCTStream::Get(
    REFGUID guidPropSet, 
    DWORD dwPropID, 
    void * pInstanceData, 
    DWORD cbInstanceData, 
    void * pPropData, 
    DWORD cbPropData,
    DWORD * pcbReturned ) noexcept {
    if (guidPropSet != AMPROPSETID_Pin)             return E_PROP_SET_UNSUPPORTED;
    if (dwPropID != AMPROPERTY_PIN_CATEGORY)        return E_PROP_ID_UNSUPPORTED;
    if (pPropData == NULL && pcbReturned == NULL)   return E_POINTER;

    if (pcbReturned) *pcbReturned = sizeof(GUID);
    if (pPropData == NULL)          return S_OK; // Caller just wants to know the size. 
    if (cbPropData < sizeof(GUID))  return E_UNEXPECTED;// The buffer is too small.

    *(GUID *)pPropData = PIN_CATEGORY_CAPTURE;
    return S_OK;
}

// ʵ�� CCTStream::QuerySupported
STDMETHODIMP KineCT::CCTStream::QuerySupported(
    REFGUID guidPropSet, 
    DWORD dwPropID, 
    DWORD * pTypeSupport) noexcept {
    if (guidPropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
    if (dwPropID != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
    // We support getting this property, but not setting it.
    if (pTypeSupport) *pTypeSupport = KSPROPERTY_SUPPORT_GET;
    return S_OK;
}