#include "Device.h"
#include "DSV.h"
#include "RootSignature.h"
#include "PSO.h"
void PSO::CreateInputLayoutAndPSOAndShader(shared_ptr<Device> devicePtr, shared_ptr<RootSignature> rootSignaturePtr, shared_ptr<DSV> dsvPtr)
{
	//���̴� ������
	D3DCompileFromFile(L"../Performance_of_GeometryShader_in_Particles/Shader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_Main", "vs_5_0", 0, 0, &_vsBlob, &_errBlob);
	_pipelineDesc.VS = { _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize() };

	D3DCompileFromFile(L"../Performance_of_GeometryShader_in_Particles/Shader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_Main", "ps_5_0", 0, 0, &_psBlob, &_errBlob);
	_pipelineDesc.PS = { _psBlob->GetBufferPointer(), _psBlob->GetBufferSize() };

	D3DCompileFromFile(L"../Performance_of_GeometryShader_in_Particles/Shader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS_Main", "gs_5_0", 0, 0, &_gsBlob, &_errBlob);
	_pipelineDesc.GS = { _gsBlob->GetBufferPointer(), _gsBlob->GetBufferSize() };

	//��ǲ���̾ƿ� ����
	D3D12_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//GSPSO ���� �� ����
	_pipelineDesc.InputLayout = { desc, _countof(desc) };
	_pipelineDesc.pRootSignature = rootSignaturePtr->_signature.Get();

	_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	_pipelineDesc.SampleMask = UINT_MAX;
	_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	_pipelineDesc.NumRenderTargets = 1;
	_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	_pipelineDesc.SampleDesc.Count = 1;
	_pipelineDesc.DSVFormat = dsvPtr->_dsvFormat;
	_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	devicePtr->_device->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
}