#pragma once
#include "Util.h"

class Device
{
public:
	ComPtr<IDXGIFactory4>		_dxgi;
	ComPtr<ID3D12Device>		_device;

	//����̽� ����
	void CreateDevice();
};