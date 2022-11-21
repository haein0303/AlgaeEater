#pragma once
#include "Util.h"
class Texture
{
public:
	ScratchImage _imageArr[2];
	ComPtr<ID3D12Resource> _texArr[2];

	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE	 _srvHandle;

	//�ؽ�ó �ε� �� ���ε�
	void CreateTexture(const wstring& path, shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr, int index);

	//SRV ����
	void CreateSRV(shared_ptr<Device> devicePtr);
};