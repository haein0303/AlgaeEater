#pragma once
#include "Util.h"
class ComputeShader
{
public:
	vector<ParticleData> _particleData;
	ComPtr<ID3D12Resource> _uavBuffer;
	ComPtr<ID3D12Resource> _readBackBuffer = nullptr;
	ComPtr<ID3D12RootSignature> _computeRootSignature = nullptr;
	ComPtr<ID3DBlob> _computeBlob;
	ComPtr<ID3D12PipelineState> _computePSO;

	ComputeShader() {
		_particleData.resize(32);
	}
	void CreateUAVBuffer(shared_ptr<Device> devicePtr);
	void DoComputeShader(shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr);
	void CreateComputeRootSignature(shared_ptr<Device> devicePtr);
	void CreateComputePSO(shared_ptr<Device> devicePtr);
};