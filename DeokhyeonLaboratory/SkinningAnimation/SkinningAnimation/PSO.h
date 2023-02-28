#pragma once
#include "Util.h"
class PSO
{
public:
	ComPtr<ID3D12PipelineState> _gsPipelineState;
	ComPtr<ID3D12PipelineState> _pipelineState;

	//��ǲ���̾ƿ��� PSO���� �� ���̴� ������
	void CreateInputLayoutAndShaderAndPSO(shared_ptr<Device> devicePtr, shared_ptr<RootSignature> rootSignaturePtr, shared_ptr<DSV> dsvPtr);
};