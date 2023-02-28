#pragma once
#include "Util.h"

class VertexBuffer
{
public:
	ComPtr<ID3D12Resource> _vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};
	UINT _vertexCount = 0;
	Constants _transform = {};

	//버텍스 버퍼 생성
	void CreateVertexBuffer(const vector<SkinnedVertex>& buffer, shared_ptr<Device> devicePtr, int index)
	{
		_vertexCount = static_cast<UINT>(buffer.size());
		UINT bufferSize = _vertexCount * sizeof(SkinnedVertex);

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		devicePtr->_device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vertexBuffer));

		void* vertexDataBuffer = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
		::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
		_vertexBuffer->Unmap(0, nullptr);

		_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
		_vertexBufferView.StrideInBytes = sizeof(SkinnedVertex);
		_vertexBufferView.SizeInBytes = bufferSize;
	}
};