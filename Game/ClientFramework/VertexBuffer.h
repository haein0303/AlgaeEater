#pragma once
#include "Util.h"

class VertexBuffer
{
public:
	ComPtr<ID3D12Resource> _playerVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _playerVertexBufferView = {};

	ComPtr<ID3D12Resource> _npcVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _npcVertexBufferView = {};

	ComPtr<ID3D12Resource> _vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

	ComPtr<ID3D12Resource> _particleVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _particleVertexBufferView = {};

	Constants _transform = {};

	//버텍스 버퍼 생성
	template <typename T>
	void CreateVertexBuffer(ComPtr<ID3D12Resource>& vertexBuffer, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView, const vector<T>& buffer, shared_ptr<Device> devicePtr)
	{
		UINT bufferSize = static_cast<UINT>(buffer.size()) * sizeof(T);

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		devicePtr->_device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));

		void* vertexDataBuffer = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
		memcpy(vertexDataBuffer, &buffer[0], bufferSize);
		vertexBuffer->Unmap(0, nullptr);

		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(T);
		vertexBufferView.SizeInBytes = bufferSize;
	}
};