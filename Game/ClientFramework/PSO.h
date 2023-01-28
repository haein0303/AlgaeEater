#pragma once
#include "Util.h"
class PSO
{
public:
	ComPtr<ID3D12PipelineState> _pipelineState;
	ComPtr<ID3D12PipelineState> _gsPipelineState;

	//��ǲ���̾ƿ��� PSO���� �� ���̴� ������
	void CreateInputLayoutAndPSOAndShader(shared_ptr<Device> devicePtr, shared_ptr<RootSignature> rootSignaturePtr, shared_ptr<DSV> dsvPtr);
	void CreateInputLayoutAndGSAndPSO(shared_ptr<Device> devicePtr, shared_ptr<RootSignature> rootSignaturePtr, shared_ptr<DSV> dsvPtr);
};