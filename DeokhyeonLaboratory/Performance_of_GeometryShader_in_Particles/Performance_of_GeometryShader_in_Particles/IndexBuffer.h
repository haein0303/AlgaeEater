#pragma once
#include "Util.h"
class IndexBuffer
{
public:
	ComPtr<ID3D12Resource> _indexBuffer[2];
	D3D12_INDEX_BUFFER_VIEW _indexBufferView[2];
	UINT _indexCount[2] = { 0 };

	//�ε��� ���� ����
	void CreateIndexBuffer(const vector<UINT>& buffer, shared_ptr<Device> devicePtr, int index);
};