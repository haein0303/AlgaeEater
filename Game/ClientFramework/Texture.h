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

	//�ؽ�ó �ε� �� ���ε�
	void CreateTexture(const wstring& path, shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr, int index);

	//SRV ����
	void CreateSRVs(shared_ptr<Device> devicePtr);
	//�ؽ�ó �ε� �� ���ε�
	void _CreateTexture(const wstring& path, shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr);

	//SRV ����
	void _CreateSRVs(shared_ptr<Device> devicePtr);
};