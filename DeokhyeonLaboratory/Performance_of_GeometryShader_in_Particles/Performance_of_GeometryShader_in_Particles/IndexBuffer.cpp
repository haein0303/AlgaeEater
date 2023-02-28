#include "Device.h"
#include "IndexBuffer.h"

void IndexBuffer::CreateIndexBuffer(const vector<UINT>& buffer, shared_ptr<Device> devicePtr, int index) {
	_indexCount[index] = static_cast<UINT>(buffer.size());
	UINT bufferSize = _indexCount[index] * sizeof(UINT);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	devicePtr->_device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_indexBuffer[index]));

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	_indexBuffer[index]->Map(0, &readRange, &indexDataBuffer);
	memcpy(indexDataBuffer, &buffer[0], bufferSize);
	_indexBuffer[index]->Unmap(0, nullptr);

	_indexBufferView[index].BufferLocation = _indexBuffer[index]->GetGPUVirtualAddress();
	_indexBufferView[index].Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView[index].SizeInBytes = bufferSize;
}