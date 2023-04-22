#pragma once


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

	UINT m_rtvDescriptorSize;



public:

	void init(DxEngine* engine);
	void LoadPipeline();
	void RenderUI(int mCurrBackbufferIndex);

};

