#pragma once

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

//전방선언스
class DxEngine;

class d11on12
{
private:
	DxEngine* _engine;

	
	ComPtr<ID3D11Device> _Device;
	ComPtr<ID2D1Factory3> m_d2dFactory;
	ComPtr<ID2D1Device2> m_d2dDevice;
	ComPtr<ID3D11On12Device> m_d3d11On12Device;
	ComPtr<ID2D1DeviceContext2> m_d2dDeviceContext;
	ComPtr<IDWriteFactory> m_dWriteFactory;

	ComPtr<ID3D12Resource> m_renderTargets[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID3D11Resource> m_wrappedBackBuffers[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID2D1Bitmap1> m_d2dRenderTargets[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

	
	

	ComPtr<ID2D1SolidColorBrush> mSolidColorBrush = nullptr;
	ComPtr<IDWriteTextFormat> mDWriteTextFormat = nullptr;

	//이미지용으로 따로 만들어야댕
	IWICImagingFactory* m_pWICFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	UINT m_rtvDescriptorSize;

	ID2D1Bitmap* test;

public:
	vector<ID2D1Bitmap*> _v_Resource;
	~d11on12() {
		m_d2dFactory->Release();
	}

	void init(DxEngine* engine, WindowInfo windowInfo);
	void LoadPipeline();

	void addResource(LPCWSTR path);
	void RenderUI(int mCurrBackbufferIndex);

	HRESULT LoadBitmapFromFile(
		ID2D1RenderTarget* pRenderTarget,
		IWICImagingFactory* pIWICFactory,
		PCWSTR uri,
		UINT destinationWidth,
		UINT destinationHeight,
		ID2D1Bitmap** ppBitmap
	);

};

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}