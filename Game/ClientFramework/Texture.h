#pragma once


class Device;
class CmdQueue;


class Texture
{
public:
	ScratchImage			_imageArr[8];
	ComPtr<ID3D12Resource>	_texArr[8];

	ScratchImage			_image;
	ComPtr<ID3D12Resource>	_tex;

	ComPtr<ID3D12DescriptorHeap>	_srvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE	_srvHandle;

	//텍스처 로드 및 업로드
	void CreateTexture(const wstring& path, shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr, int index);

	//SRV 생성
	void CreateSRVs(shared_ptr<Device> devicePtr);
	//텍스처 로드 및 업로드
	void _CreateTexture(const wstring& path, shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr);

	//SRV 생성
	void _CreateSRVs(shared_ptr<Device> devicePtr);
};