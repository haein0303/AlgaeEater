#pragma once
#include "Util.h"
class IndexBuffer
{
public:
	ComPtr<ID3D12Resource> _playerIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW _playerIndexBufferView;
	UINT _playerIndexCount = 0;
	ComPtr<ID3D12Resource> _npcIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW _npcIndexBufferView;
	UINT _npcIndexCount = 0;
	ComPtr<ID3D12Resource> _indexBuffer;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;
	UINT _indexCount = 0;
	ComPtr<ID3D12Resource> _particleIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW _particleIndexBufferView;
	UINT _particleIndexCount = 0;

	//인덱스 버퍼 생성
	void CreateIndexBuffer(ComPtr<ID3D12Resource>& indexBuffer, D3D12_INDEX_BUFFER_VIEW& indexBufferView, const vector<UINT>& buffer, shared_ptr<Device> devicePtr, UINT& indexCount)
	{
		indexCount = static_cast<UINT>(buffer.size());
		UINT bufferSize = indexCount * sizeof(UINT);

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		devicePtr->_device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer));

		void* indexDataBuffer = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		indexBuffer->Map(0, &readRange, &indexDataBuffer);
		memcpy(indexDataBuffer, &buffer[0], bufferSize);
		indexBuffer->Unmap(0, nullptr);

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = bufferSize;
	}
};