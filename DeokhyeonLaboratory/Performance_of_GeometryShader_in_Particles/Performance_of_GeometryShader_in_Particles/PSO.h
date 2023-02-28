#pragma once
#include "Util.h"
class PSO
{
public:
	ComPtr<ID3D12PipelineState> _gsPipelineState;
	ComPtr<ID3D12PipelineState> _pipelineState;

	//인풋레이아웃과 PSO생성 및 셰이더 컴파일
	void CreateInputLayoutAndGSAndPSO(shared_ptr<Device> devicePtr, shared_ptr<RootSignature> rootSignaturePtr, shared_ptr<DSV> dsvPtr);
	void CreateInputLayoutAndShaderAndPSO(shared_ptr<Device> devicePtr, shared_ptr<RootSignature> rootSignaturePtr, shared_ptr<DSV> dsvPtr);
};