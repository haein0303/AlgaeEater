#pragma once


//전방선언스
class DxEngine;

class d11on12
{
private:
	DxEngine* _engine;

	UINT dxgiFactoryFlags = 0;
	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	ComPtr<ID3D11Device> _Device;
	ComPtr<ID2D1Factory3> m_d2dFactory;
	ComPtr<ID2D1Device2> m_d2dDevice;
	ComPtr<ID3D11On12Device> m_d3d11On12Device;
	ComPtr<ID2D1DeviceContext2> m_d2dDeviceContext;
	ComPtr<IDWriteFactory> m_dWriteFactory;

	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};
	ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
public:

	void init(DxEngine* engine);

};

