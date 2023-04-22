
#include "Util.h"
#include "DxEngine.h"
#include "d11on12.h"
#include <d2d1_3.h>

using namespace COMUtil;
//https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/d2d-using-d3d11on12
//https://mawile.tistory.com/347
//void d11on12::init(DxEngine* engine)
//{
//    _engine = engine;
//
//
//#if defined(_DEBUG)
//    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
//    _engine->devicePtr->_device->QueryInterface(infoQueue.GetAddressOf());
//
//    /* Removing INVALID_HANDLE_VALUE Warning Statements from Debugging Layer. */
//    D3D12_MESSAGE_SEVERITY messageSeverity[] = {
//        D3D12_MESSAGE_SEVERITY_INFO
//    };
//
//    D3D12_MESSAGE_ID messageID[] = {
//        D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE
//    };
//
//    D3D12_INFO_QUEUE_FILTER infoQueueFilter = {};
//    infoQueueFilter.DenyList.pSeverityList = messageSeverity;
//    infoQueueFilter.DenyList.NumSeverities = _countof(messageSeverity);
//    infoQueueFilter.DenyList.pIDList = messageID;
//    infoQueueFilter.DenyList.NumIDs = _countof(messageID);
//
//    infoQueue->PushStorageFilter(&infoQueueFilter);
//
//    /* DXSampleHelper.h > Remove warning statements that appear when "throw" in the debugging layer. */
//    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
//    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
//    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
//
//    /* D3D12 ERROR: ID3D12CommandQueue::Present: Resource state (0x800: D3D12_RESOURCE_STATE_COPY_SOURCE) (promoted from COMMON state) of resource (0x000001F6BE05D070:'Unnamed ID3D12Resource Object') (subresource: 0) must be in COMMON state when transitioning to use in a different Command List type, because resource state on previous Command List type : D3D12_COMMAND_LIST_TYPE_COPY, is actually incompatible and different from that on the next Command List type : D3D12_COMMAND_LIST_TYPE_DIRECT. [ RESOURCE_MANIPULATION ERROR #990: RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE]
//D3D12: **BREAK** enabled for the previous message, which was: [ ERROR RESOURCE_MANIPULATION #990: RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE ]. */
//// This is a bug in the DXGI Debug Layer interaction with the DX12 Debug Layer Windows 11.
//// SOLUTION> https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
//    D3D12_MESSAGE_ID hide[] =
//    {
//        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
//        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
//        // Workarounds for debug layer issues on hybrid-graphics systems
//        D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
//        D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
//    };
//    D3D12_INFO_QUEUE_FILTER filter = {};
//    filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
//    filter.DenyList.pIDList = hide;
//    infoQueue->AddStorageFilterEntries(&filter);
//
//    infoQueue->Release();
//#endif
//
//    UINT dxgiFactoryFlags = 0;
//    UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
//    D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};
//
//#if defined(_DEBUG)
//    // Enable the debug layer (requires the Graphics Tools "optional feature").
//    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
//    {
//        ComPtr<ID3D12Debug> debugController;
//        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
//        {
//            debugController->EnableDebugLayer();
//
//            // Enable additional debug layers.
//            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//            d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//            d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
//        }
//    }
//#endif
//
//
//    //d12 디바이스로 d11디바이스를 맹글어 보자
//    D3D11On12CreateDevice(
//        _engine->devicePtr->_device.Get(),
//        d3d11DeviceFlags,
//        nullptr,
//        0,
//        reinterpret_cast<IUnknown**>(_engine->cmdQueuePtr->_cmdQueue.GetAddressOf()),
//        1,
//        0,
//        &_Device,
//        &m_d3d11DeviceContext,
//        nullptr
//    );
//    // Query the 11On12 device from the 11 device.
//    ThrowIfFailed(_Device.As(&m_d3d11On12Device));
//   
//    // Create D2D/DWrite components.
//    {
//        D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE; //옵션 설정
//        ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &m_d2dFactory));
//        ComPtr<IDXGIDevice> dxgiDevice;
//        ThrowIfFailed(m_d3d11On12Device.As(&dxgiDevice));
//        ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
//        ThrowIfFailed(m_d2dDevice->CreateDeviceContext(deviceOptions, &m_d2dDeviceContext));
//        ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dWriteFactory));
//    }
//
//    float dpiX;
//    float dpiY;
//#pragma warning(push)
//#pragma warning(disable : 4996) // GetDesktopDpi is deprecated.
//    m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
//#pragma warning(pop)
//    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
//        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
//        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
//        dpiX,
//        dpiY
//    );
//  
//    // Create descriptor heaps.
//    {
//        // Describe and create a render target view (RTV) descriptor heap.
//        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
//        rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
//        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//        _engine->devicePtr->_device.Get()->CreateDescriptorHeap(&rtvHeapDesc, 
//            IID_PPV_ARGS(_engine->rtvPtr->_rtvHeap.GetAddressOf()));
//
//        m_rtvDescriptorSize = _engine->devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//    }
//
//    // Create frame resources.
//    {
//        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_engine->rtvPtr->_rtvHeap->GetCPUDescriptorHandleForHeapStart());
//
//        // Create a RTV, D2D render target, and a command allocator for each frame.
//        for (UINT n = 0; n < SWAP_CHAIN_BUFFER_COUNT; n++)
//        {
//            ThrowIfFailed(_engine->swapChainPtr->_swapChain->GetBuffer(n, IID_PPV_ARGS(&_engine->swapChainPtr->_renderTargets[n])));
//            _engine->devicePtr->_device->CreateRenderTargetView(_engine->swapChainPtr->_renderTargets[n].Get(), nullptr, rtvHandle);
//
//            
//            D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
//
//            ThrowIfFailed(m_d3d11On12Device->CreateWrappedResource(
//                _engine->swapChainPtr->_renderTargets[n].Get(),
//                &d3d11Flags,
//                D3D12_RESOURCE_STATE_RENDER_TARGET,
//                D3D12_RESOURCE_STATE_PRESENT,
//                IID_PPV_ARGS(&m_wrappedBackBuffers[n])
//            ));
//
//            // Create a render target for D2D to draw directly to this back buffer.
//            ComPtr<IDXGISurface> surface;
//            ThrowIfFailed(m_wrappedBackBuffers[n].As(&surface));
//            ThrowIfFailed(m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
//                surface.Get(),
//                &bitmapProperties,
//                &m_d2dRenderTargets[n]
//            ));
//
//            rtvHandle.Offset(1, m_rtvDescriptorSize);
//
//        }
//    }
//
//}

void d11on12::init(DxEngine* engine) {
    _engine = engine;

    UINT dxgiFactoryFlag = 0;
    UINT d3d11DeviceFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS factoryOptions = {};

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
	_engine->devicePtr->_device->QueryInterface(infoQueue.GetAddressOf());

	/* Removing INVALID_HANDLE_VALUE Warning Statements from Debugging Layer. */
	D3D12_MESSAGE_SEVERITY messageSeverity[] = {
		D3D12_MESSAGE_SEVERITY_INFO
	};

	D3D12_MESSAGE_ID messageID[] = {
		D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE
	};

	D3D12_INFO_QUEUE_FILTER infoQueueFilter = {};
	infoQueueFilter.DenyList.pSeverityList = messageSeverity;
	infoQueueFilter.DenyList.NumSeverities = _countof(messageSeverity);
	infoQueueFilter.DenyList.pIDList = messageID;
	infoQueueFilter.DenyList.NumIDs = _countof(messageID);

	infoQueue->PushStorageFilter(&infoQueueFilter);

	/* DXSampleHelper.h > Remove warning statements that appear when "throw" in the debugging layer. */
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);

	
	D3D12_MESSAGE_ID hide[] =
	{
		D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
		// Workarounds for debug layer issues on hybrid-graphics systems
		D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
	};
	D3D12_INFO_QUEUE_FILTER filter = {};
	filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
	filter.DenyList.pIDList = hide;
	infoQueue->AddStorageFilterEntries(&filter);

	infoQueue->Release();
#endif

	Microsoft::WRL::ComPtr<ID3D11Device> device11 = nullptr;
	D3D11On12CreateDevice(_engine->devicePtr->_device.Get(),
		d3d11DeviceFlag, nullptr, 0, 
		reinterpret_cast<IUnknown**>(_engine->cmdQueuePtr->_cmdQueue.GetAddressOf()),
		1, 0, 
		device11.GetAddressOf(), 
		m_d3d11DeviceContext.GetAddressOf(), nullptr);
	device11.As(&m_d3d11On12Device);

	// Create D2D/DWrite components.
	{
		D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &factoryOptions, &m_d2dFactory));
		ComPtr<IDXGIDevice> dxgiDevice;
		ThrowIfFailed(m_d3d11On12Device.As(&dxgiDevice));
		ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
		ThrowIfFailed(m_d2dDevice->CreateDeviceContext(deviceOptions, &m_d2dDeviceContext));
		ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dWriteFactory));
	}

	float dpiX;
	float dpiY;
#pragma warning(push)
#pragma warning(disable : 4996) // GetDesktopDpi is deprecated.
	m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX,
		dpiY
	);


	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(_engine->devicePtr->_device->CreateDescriptorHeap(&rtvHeapDesc, 
			IID_PPV_ARGS(&m_rtvHeap)));

		m_rtvDescriptorSize = _engine->devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			_engine->swapChainPtr->_swapChain->GetBuffer(i, IID_PPV_ARGS(m_renderTargets[i].GetAddressOf()));
			_engine->devicePtr->_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);

			D3D11_RESOURCE_FLAGS resourceFlags = { D3D11_BIND_RENDER_TARGET };

			m_d3d11On12Device->CreateWrappedResource(
				m_renderTargets[i].Get(), &resourceFlags,
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(m_wrappedBackBuffers[i].GetAddressOf()));

			Microsoft::WRL::ComPtr<IDXGISurface> surface;
			m_wrappedBackBuffers[i].As(&surface);
			m_d2dDeviceContext->CreateBitmapFromDxgiSurface(surface.Get(), &bitmapProperties, m_d2dRenderTargets[i].GetAddressOf());

			//rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

}

void d11on12::LoadPipeline()
{
    m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), mSolidColorBrush.GetAddressOf());
    m_dWriteFactory->CreateTextFormat(L"Verdana", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL,
        25, L"en-us", mDWriteTextFormat.GetAddressOf());

    mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void d11on12::RenderUI(int mCurrBackbufferIndex)
{
    D2D1_SIZE_F rtSize = m_d2dRenderTargets[mCurrBackbufferIndex]->GetSize();
    D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height);
    static const WCHAR text[] = L"해치웠나?";

    m_d3d11On12Device->AcquireWrappedResources(m_wrappedBackBuffers[mCurrBackbufferIndex].GetAddressOf(), 1);

    m_d2dDeviceContext->SetTarget(m_d2dRenderTargets[mCurrBackbufferIndex].Get());
    m_d2dDeviceContext->BeginDraw();
    m_d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
    m_d2dDeviceContext->DrawTextW(text, _countof(text) - 1, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());
    m_d2dDeviceContext->EndDraw();

    m_d3d11On12Device->ReleaseWrappedResources(m_wrappedBackBuffers[mCurrBackbufferIndex].GetAddressOf(), 1);
    m_d3d11DeviceContext->Flush();

}
