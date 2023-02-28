#pragma once
#include "Util.h"

class VertexBuffer
{
public:
	ComPtr<ID3D12Resource> _vertexBuffer[2];
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView[2] = {};
	UINT _vertexCount[2] = { 0 };
	Constants _transform = {};

	//버텍스 버퍼 생성
	template <typename T>
	void CreateVertexBuffer(const vector<T>& buffer, shared_ptr<Device> devicePtr, int index)
	{
		_vertexCount[index] = static_cast<UINT>(buffer.size());
		UINT bufferSize = _vertexCount[index] * sizeof(T);

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		devicePtr->_device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vertexBuffer[index]));

		void* vertexDataBuffer = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		_vertexBuffer[index]->Map(0, &readRange, &vertexDataBuffer);
		memcpy(vertexDataBuffer, &buffer[0], bufferSize);
		_vertexBuffer[index]->Unmap(0, nullptr);

		_vertexBufferView[index].BufferLocation = _vertexBuffer[index]->GetGPUVirtualAddress();
		_vertexBufferView[index].StrideInBytes = sizeof(T);
		_vertexBufferView[index].SizeInBytes = bufferSize;
	}
};