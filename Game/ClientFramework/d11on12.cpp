
#include "Util.h"
#include "DxEngine.h"
#include "d11on12.h"
#include <d2d1_3.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib,"Windowscodecs.lib")
#pragma comment(lib, "D2D1.lib")

using namespace COMUtil;
//https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/d2d-using-d3d11on12
//https://mawile.tistory.com/347

using namespace D2D1;

void d11on12::init(DxEngine* engine, WindowInfo windowInfo) {
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

	CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(&m_pWICFactory)
	);
	RECT r{};
	m_d2dFactory->CreateHwndRenderTarget(
		RenderTargetProperties(),
		HwndRenderTargetProperties(windowInfo.hwnd, SizeU(r.right, r.bottom)),
		&m_pRenderTarget
		);

}

void d11on12::LoadPipeline()
{
    m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), mSolidColorBrush.GetAddressOf());
	m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &mGrayBrush);
	m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &mRedBrush);

    m_dWriteFactory->CreateTextFormat(L"Microsoft GothicNeo", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        25, L"ko-kr", mDWriteTextFormat.GetAddressOf());

    mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	m_dWriteFactory->CreateTextFormat(L"Microsoft GothicNeo Light", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		20, L"ko-kr", m_mini_boss_font.GetAddressOf());
	m_mini_boss_font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_mini_boss_font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	m_dWriteFactory->CreateTextFormat(L"Microsoft GothicNeo", nullptr,
		DWRITE_FONT_WEIGHT_EXTRA_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		30, L"ko-kr", m_boss_font.GetAddressOf());
	m_boss_font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_boss_font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	m_dWriteFactory->CreateTextFormat(L"Microsoft GothicNeo", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		15, L"ko-kr", m_info_font.GetAddressOf());
	m_info_font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	m_info_font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

}

void d11on12::Loading_info()
{
	_loading_bg = addResource(L"..\\Resources\\UserInterface\\title_back.png");
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0000.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0001.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0002.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0003.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0004.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0005.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0006.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0007.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0008.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0009.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0010.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0011.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0012.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0013.png"));
	_loading_Resource.push_back(addResource(L"..\\Resources\\UserInterface\\Comp 1_0014.png"));


	_loading_msg.push_back(L"LOADING...");
	_loading_msg.push_back(L"기다리다가 지친다~");
	_loading_msg.push_back(L"교수님 감사합니다.");
	_loading_msg.push_back(L"우리팀 최고 미남 김덕현");
	_loading_msg.push_back(L"저희의 지도교수님은 정내훈 교수님입니다.");
	_loading_msg.push_back(L"팀원은 클라 1.3명 서버 1.3명 기획 0.2명 아트 0.2명으로 구성되었습니다.");
	_loading_msg.push_back(L"고양이는 귀엽습니다.");
	_loading_msg.push_back(L"덕현이는 찍먹파입니다.");
	_loading_msg.push_back(L"팀원 중 김경욱 학생만 흡연을 합니다.");
	_loading_msg.push_back(L"덕현이는 최근 장트러블에 고생하고 있습니다.");
	_loading_msg.push_back(L"해인이의 자취방에서 개발을 자주 진행합니다.");
	_loading_msg.push_back(L"상남자 특 로딩 잘 기다림");
	_loading_msg.push_back(L"상여자 특 로딩 잘 기다림");

	_loading_rect.top = 360.f - 250.0f;
	_loading_rect.left = 640.f - 250.0f;
	_loading_rect.right = 640.f + 250.0f;
	_loading_rect.bottom = 360.f + 250.0f;

	_loading_msg_rect.top = _loading_rect.bottom + 20.f;
	_loading_msg_rect.left = 0.f;
	_loading_msg_rect.right = 1280.f;
	_loading_msg_rect.bottom = _loading_rect.bottom + 40.f;

	srand((unsigned int)time(NULL));
	
}

void d11on12::Loading_draw(const float& time)
{
	loading_counter += time;
	loading_msg_counter += time;
	if (loading_counter > loading_frame) {
		loading_counter = 0;
		now_img = (now_img + 1) % _loading_Resource.size();
		if (now_img == 12) {
			loading_frame = 1.f;
		}else if (now_img == 0) {
			loading_frame = 0.5f;
		}
		else {
			loading_frame = 0.1f;
		}
	}
	
	if (loading_msg_counter > 3.f) {
		loading_msg_counter = 0;
		now_msg = rand() % _loading_msg.size();
	}

	
	m_d2dDeviceContext->DrawBitmap(_loading_bg, { 0.f, 0.f, 1280.f, 720.f });
	m_d2dDeviceContext->DrawBitmap(_loading_Resource[now_img], _loading_rect);
	m_d2dDeviceContext->DrawTextW(_loading_msg[now_msg], wcslen(_loading_msg[now_msg]), mDWriteTextFormat.Get(), &_loading_msg_rect, mSolidColorBrush.Get());

}

void d11on12::Late_load()
{
	
	_boss_bg = addResource(L"..\\Resources\\UserInterface\\Boss_bg.png");
	stage1_boss_bg.push_back(addResource(L"..\\Resources\\UserInterface\\Boss_bg_white.png"));
	stage1_boss_bg.push_back(addResource(L"..\\Resources\\UserInterface\\Boss_bg_blue.png"));
	stage1_boss_bg.push_back(addResource(L"..\\Resources\\UserInterface\\Boss_bg_green.png"));
	stage1_boss_bg.push_back(addResource(L"..\\Resources\\UserInterface\\Boss_bg_red.png"));
	stage1_boss_bg.push_back(addResource(L"..\\Resources\\UserInterface\\Boss_bg_black.png"));
	


}

ID2D1Bitmap* d11on12::addResource(LPCWSTR path)
{
	ID2D1Bitmap *tmp = nullptr;
	// Create a decoder
	IWICBitmapDecoder* pDecoder = NULL;
	
	IWICBitmapFrameDecode* pFrame = NULL;

	int result = 0;
	IWICFormatConverter* p_converter;
	if (S_OK == m_pWICFactory->CreateDecoderFromFilename(path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder)) {
		cout << "1 is ok" << endl;
		// 파일을 구성하는 이미지 중에서 첫번째 이미지를 선택한다.
		if (S_OK == pDecoder->GetFrame(0, &pFrame)) {
			cout << "2 is ok" << endl;
			// IWICBitmap형식의 비트맵을 ID2D1Bitmap. 형식으로 변환하기 위한 객체 생성
			if (S_OK == m_pWICFactory->CreateFormatConverter(&p_converter)) {
				cout << "3 is ok" << endl;
				// 선택된 그림을 어떤 형식의 비트맵으로 변환할 것인지 설정한다.
				if (S_OK == p_converter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom)) {
					cout << "4 is ok" << endl;
					// IWICBitmap 형식의 비트맵으로 ID2D1Bitmap 객체를 생성한다.
					if (S_OK == m_d2dDeviceContext->CreateBitmapFromWicBitmap(p_converter, NULL, &tmp)) {						
						cout << "5 is ok" << endl;
						result = 1;  // 성공적으로 생성한 경우
					}
				}
				p_converter->Release();  // 이미지 변환 객체 제거
			}
			pFrame->Release();   // 그림파일에 있는 이미지를 선택하기 위해 사용한 객체 제거
		}
		pDecoder->Release();     // 압축을 해제하기 위해 생성한 객체 제거
	}
	//if(m_pWICFactory->Release()) m_pWICFactory->Release();     // WIC를 사용하기 위해 만들었던 Factory 객체 제거

	if (result == 1) {
		cout << path << " is Loading Good" << endl;
	}

	return tmp;
}

void d11on12::RenderUI(int mCurrBackbufferIndex)
{
    D2D1_SIZE_F rtSize = m_d2dRenderTargets[mCurrBackbufferIndex]->GetSize();
    D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height);
	D2D1_RECT_F image_rect = D2D1::RectF(0.0f, 0.0f, 100.f, 100.f);
    

    m_d3d11On12Device->AcquireWrappedResources(m_wrappedBackBuffers[mCurrBackbufferIndex].GetAddressOf(), 1);

    m_d2dDeviceContext->SetTarget(m_d2dRenderTargets[mCurrBackbufferIndex].Get());
    m_d2dDeviceContext->BeginDraw();
    m_d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	
}

void d11on12::LateRenderUI(vector<UI_ASSET> scene_asset)
{
	for (const auto& data : scene_asset) {
		m_d2dDeviceContext->DrawBitmap(data._image, data._rect);
	}
}

void d11on12::draw_UI(const UI_ASSET& draw)
{
	m_d2dDeviceContext->DrawBitmap(draw._image, draw._rect);
}

void d11on12::draw_text(LPCWSTR text, D2D1_RECT_F rect)
{
	m_d2dDeviceContext->DrawTextW(text, wcslen(text), mDWriteTextFormat.Get(), &rect, mSolidColorBrush.Get());
}

void d11on12::draw_bossUI(int hp,int stage, const OBJECT& boss_obj)
{
	float xs = 240.f;
	float ys = 50.f;
	D2D1_RECT_F title_rect{ 640.f - 120.f, 40.f, 640.f + 120.f, 70.f };
	D2D1_RECT_F minititle_rect{ 640.f - 120.f, 20.f, 640.f + 120.f, 40.f };
	
	switch (stage) {
	case 0:
	case 1:
		m_d2dDeviceContext->DrawBitmap(stage1_boss_bg[boss_obj._eye_color], {640.f - xs, 30.f, 640.f + xs, 30.f + ys});

		m_d2dDeviceContext->DrawTextW(L"전구 공장의 보스", wcslen(L"전구 공장의 보스"), m_mini_boss_font.Get(), &minititle_rect, mSolidColorBrush.Get());
		m_d2dDeviceContext->DrawTextW(L"타란튤라", wcslen(L"타란튤라"), m_boss_font.Get(), &title_rect, mSolidColorBrush.Get());
		break;
	case 2:
		m_d2dDeviceContext->DrawBitmap(_boss_bg, { 640.f - xs, 30.f, 640.f + xs, 30.f + ys });
		m_d2dDeviceContext->DrawTextW(L"화학 공장의 보스", wcslen(L"화학 공장의 보스"), m_mini_boss_font.Get(), &minititle_rect, mSolidColorBrush.Get());
		m_d2dDeviceContext->DrawTextW(L"김고양씨", wcslen(L"김고양씨"), m_boss_font.Get(), &title_rect, mSolidColorBrush.Get());

		break;
	case 3:
		m_d2dDeviceContext->DrawBitmap(_boss_bg, { 640.f - xs, 30.f, 640.f + xs, 30.f + ys });
		m_d2dDeviceContext->DrawTextW(L"최종 보스", wcslen(L"최종 보스"), m_mini_boss_font.Get(), &minititle_rect, mSolidColorBrush.Get());
		m_d2dDeviceContext->DrawTextW(L"김멈무씨", wcslen(L"김멈무씨"), m_boss_font.Get(), &title_rect, mSolidColorBrush.Get());

		break;
	}
	

	float startx = 640.f - xs * 1.3f;
	float width = xs * 1.3f * 2;

	D2D1_RECT_F backgroundRect = D2D1::RectF(startx, 85.0f, startx + width, 90.f);
	m_d2dDeviceContext->FillRectangle(backgroundRect, mGrayBrush);

	D2D1_RECT_F progressRect = D2D1::RectF(640.f - xs * 1.3f, 85.0f, startx + (width*((float)hp/ BOSS_HP[stage])), 90.f);
	m_d2dDeviceContext->FillRectangle(progressRect, mRedBrush);
}

void d11on12::draw_infotext(LPCWSTR text, D2D1_RECT_F rect)
{
	m_d2dDeviceContext->DrawTextW(text, wcslen(text), m_info_font.Get(), &rect, mSolidColorBrush.Get());
}

void d11on12::ExcuteUI(int mCurrBackbufferIndex)
{
	m_d2dDeviceContext->EndDraw();

	m_d3d11On12Device->ReleaseWrappedResources(m_wrappedBackBuffers[mCurrBackbufferIndex].GetAddressOf(), 1);
	m_d3d11DeviceContext->Flush();
}

void d11on12::draw_boss_info(int num, float hp)
{

}

