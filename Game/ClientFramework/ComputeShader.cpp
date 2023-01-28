#include "Device.h"
#include "CmdQueue.h"
#include "ComputeShader.h"

void ComputeShader::CreateUAVBuffer(shared_ptr<Device> devicePtr)
{
	//uav
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(_particleData.size() * sizeof(ParticleData), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	devicePtr->_device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&_uavBuffer));

	//복사용 버퍼
	D3D12_HEAP_PROPERTIES heapProperty2 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
	D3D12_RESOURCE_DESC desc2 = CD3DX12_RESOURCE_DESC::Buffer(_particleData.size() * sizeof(ParticleData));
	devicePtr->_device->CreateCommittedResource(&heapProperty2, D3D12_HEAP_FLAG_NONE, &desc2, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_readBackBuffer));
}

void ComputeShader::DoComputeShader(shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr)
{
	cmdQueuePtr->_cmdAlloc->Reset();
	cmdQueuePtr->_cmdList->Reset(cmdQueuePtr->_cmdAlloc.Get(), _computePSO.Get());

	cmdQueuePtr->_cmdList->SetComputeRootSignature(_computeRootSignature.Get());
	cmdQueuePtr->_cmdList->SetComputeRootUnorderedAccessView(0, _uavBuffer->GetGPUVirtualAddress());
	cmdQueuePtr->_cmdList->Dispatch(1, 1, 1);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_uavBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE);
	cmdQueuePtr->_cmdList->ResourceBarrier(1, &barrier);
	cmdQueuePtr->_cmdList->CopyResource(_readBackBuffer.Get(), _uavBuffer.Get());
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(_uavBuffer.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);
	cmdQueuePtr->_cmdList->ResourceBarrier(1, &barrier2);
	cmdQueuePtr->_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { cmdQueuePtr->_cmdList.Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	cmdQueuePtr->FlushResourceCommandQueue();

	ParticleData* mappedData = nullptr;
	_readBackBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));

	for (int i = 0; i < 32; ++i)
	{
		//printf_s("%f %f %f\n", mappedData[i].worldPos.x, mappedData[i].worldPos.y, mappedData[i].worldPos.z);
	}


	_readBackBuffer->Unmap(0, nullptr);
}

void ComputeShader::CreateComputeRootSignature(shared_ptr<Device> devicePtr)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	slotRootParameter[0].InitAsUnorderedAccessView(0);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_NONE);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	devicePtr->_device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(_computeRootSignature.GetAddressOf()));
}

void ComputeShader::CreateComputePSO(shared_ptr<Device> devicePtr)
{
	ComPtr<ID3DBlob> errors;
	D3DCompileFromFile(L"..\\Shader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS", "cs_5_0", 0, 0, &_computeBlob, &errors);

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePSO = {};
	computePSO.pRootSignature = _computeRootSignature.Get();
	computePSO.CS = {
		reinterpret_cast<BYTE*>(_computeBlob->GetBufferPointer()), _computeBlob->GetBufferSize()
	};
	computePSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	devicePtr->_device->CreateComputePipelineState(&computePSO, IID_PPV_ARGS(&this->_computePSO));
}