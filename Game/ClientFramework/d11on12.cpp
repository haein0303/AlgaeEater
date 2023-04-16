
#include "Util.h"
#include "DxEngine.h"
#include "d11on12.h"
//https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/d2d-using-d3d11on12
//https://mawile.tistory.com/347
void d11on12::init(DxEngine* engine)
{
    _engine = engine;

    //d12 디바이스로 d11디바이스를 맹글어 보자
    D3D11On12CreateDevice(
        _engine->devicePtr->_device.Get(),
        d3d11DeviceFlags,
        nullptr,
        0,
        reinterpret_cast<IUnknown**>(_engine->cmdQueuePtr->_cmdQueue.GetAddressOf()),
        1,
        0,
        &_Device,
        &m_d3d11DeviceContext,
        nullptr
    );


    /* {
        D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &m_d2dFactory);
        ComPtr<IDXGIDevice> dxgiDevice;
        m_d3d11On12Device.As(&dxgiDevice);
        m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice);
        m_d2dDevice->CreateDeviceContext(deviceOptions, &m_d2dDeviceContext);
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dWriteFactory);
    }*/

}
