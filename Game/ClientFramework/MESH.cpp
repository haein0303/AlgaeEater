#include "Util.h"
#include "Texture.h"
#include "DxEngine.h"
#include "MESH.h"


//이거 왜 에러남 ㅠㅠ
void MESH_ASSET::Link_ptr(
	shared_ptr<Device> devicePtr,
	shared_ptr<ObjectLoader> fbxLoaderPtr,
	shared_ptr<VertexBuffer> vertexBufferPtr,
	shared_ptr<IndexBuffer> indexBufferPtr,
	shared_ptr<CmdQueue> cmdQueuePtr,
	shared_ptr<RootSignature> rootSignaturePtr,
	shared_ptr<DSV> dsvPtr
) 
{
	_devicePtr = devicePtr;
	_fbxLoaderPtr = fbxLoaderPtr;
	_vertexBufferPtr = vertexBufferPtr;
	_indexBufferPtr = indexBufferPtr;
	_cmdQueuePtr =  cmdQueuePtr;
	_rootSignaturePtr = rootSignaturePtr;
	_dsvPtr = dsvPtr;
}

void MESH_ASSET::Init(const char* path, ObjectType obj_type)
{
	_obj_type = obj_type;
	
	if (obj_type == ObjectType::AnimationObjects) {
		_animationPtr->CreateAnimationObject(_AniVertexVec, _IndexVec, path);
		_vertexBufferPtr->CreateAnimationVertexBuffer(_vertexBuffer, _vertexBufferView, _AniVertexVec, _devicePtr);
	}
	else {
		_fbxLoaderPtr->LoadObject(_VertexVec, _IndexVec, path);
		_vertexBufferPtr->CreateVertexBuffer(_vertexBuffer, _vertexBufferView, _VertexVec, _devicePtr);
	}	
	_indexBufferPtr->CreateIndexBuffer(_indexBuffer, _indexBufferView, _IndexVec, _devicePtr, _indexCount);

}



//사실상 랩퍼 메소드 ㄹㅇ ㅋㅋ
void MESH_ASSET::Add_texture(const wstring& path)
{
	_tex._CreateTexture(path, _devicePtr, _cmdQueuePtr);
}

//얘도 똑같지 뭐 ㅋㅋ
void MESH_ASSET::Make_SRV()
{
	_tex._CreateSRVs(_devicePtr);
	//_texVec[num]._CreateSRVs(_engine.devicePtr);
}

void MESH_ASSET::CreatePSO()
{
	ComPtr<ID3DBlob> _vsBlob;
	ComPtr<ID3DBlob> _psBlob;
	ComPtr<ID3DBlob> _errBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC _pipelineDesc = {};

	

	if (_obj_type == ObjectType::GeneralObjects) {
		//셰이더 컴파일
		D3DCompileFromFile(L"..\\Shader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_Main", "vs_5_0", 0, 0, &_vsBlob, &_errBlob);
		_pipelineDesc.VS = { _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize() };

		D3DCompileFromFile(L"..\\Shader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_Main", "ps_5_0", 0, 0, &_psBlob, &_errBlob);
		_pipelineDesc.PS = { _psBlob->GetBufferPointer(), _psBlob->GetBufferSize() };
		//인풋레이아웃 서술
		D3D12_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		//PSO 서술 및 생성
		_pipelineDesc.InputLayout = { desc, _countof(desc) };
		_pipelineDesc.pRootSignature = _rootSignaturePtr->_signature.Get();

		_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		_pipelineDesc.SampleMask = UINT_MAX;
		_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_pipelineDesc.NumRenderTargets = 1;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		_pipelineDesc.SampleDesc.Count = 1;
		_pipelineDesc.DSVFormat = _dsvPtr->_dsvFormat;

		_devicePtr->_device->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
	}
	else {
		//셰이더 컴파일
		D3DCompileFromFile(L"..\\SkinningAnimation.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_Main", "vs_5_0", 0, 0, &_vsBlob, &_errBlob);
		_pipelineDesc.VS = { _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize() };

		D3DCompileFromFile(L"..\\SkinningAnimation.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_Main", "ps_5_0", 0, 0, &_psBlob, &_errBlob);
		_pipelineDesc.PS = { _psBlob->GetBufferPointer(), _psBlob->GetBufferSize() };

		//인풋레이아웃 서술
		D3D12_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		//GSPSO 서술 및 생성
		_pipelineDesc.InputLayout = { desc, _countof(desc) };
		_pipelineDesc.pRootSignature = _rootSignaturePtr->_signature.Get();

		_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		_pipelineDesc.SampleMask = UINT_MAX;
		_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_pipelineDesc.NumRenderTargets = 1;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		_pipelineDesc.SampleDesc.Count = 1;
		_pipelineDesc.DSVFormat = _dsvPtr->_dsvFormat;

		_devicePtr->_device->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
	}

}

void MESH_ASSET::UpdateSkinnedAnimation(float dt, int& state, int& state0)
{
	_animationPtr->UpdateSkinnedAnimation(dt, state, state0);
}

void MESH_ASSET::CreatePSO(const wchar_t* shader)
{
	_is_default_shader = false;


	ComPtr<ID3DBlob> _vsBlob;
	ComPtr<ID3DBlob> _psBlob;
	ComPtr<ID3DBlob> _errBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC _pipelineDesc = {};

	//셰이더 컴파일
	D3DCompileFromFile(shader, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_Main", "vs_5_0", 0, 0, &_vsBlob, &_errBlob);
	_pipelineDesc.VS = { _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize() };

	D3DCompileFromFile(shader, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_Main", "ps_5_0", 0, 0, &_psBlob, &_errBlob);
	_pipelineDesc.PS = { _psBlob->GetBufferPointer(), _psBlob->GetBufferSize() };
	
	if (_obj_type == ObjectType::GeneralObjects) {

		//인풋레이아웃 서술
		D3D12_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		//PSO 서술 및 생성
		_pipelineDesc.InputLayout = { desc, _countof(desc) };
		_pipelineDesc.pRootSignature = _rootSignaturePtr->_signature.Get();

		_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		_pipelineDesc.SampleMask = UINT_MAX;
		_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_pipelineDesc.NumRenderTargets = 1;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		_pipelineDesc.SampleDesc.Count = 1;
		_pipelineDesc.DSVFormat = _dsvPtr->_dsvFormat;		

		_devicePtr->_device->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
	}
	else if (_obj_type == ObjectType::SkyBox) {
		//셰이더 컴파일
		D3DCompileFromFile(shader, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_Main", "vs_5_0", 0, 0, &_vsBlob, &_errBlob);
		_pipelineDesc.VS = { _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize() };

		D3DCompileFromFile(shader, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_Main", "ps_5_0", 0, 0, &_psBlob, &_errBlob);
		_pipelineDesc.PS = { _psBlob->GetBufferPointer(), _psBlob->GetBufferSize() };
		//인풋레이아웃 서술
		D3D12_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		//PSO 서술 및 생성
		_pipelineDesc.InputLayout = { desc, _countof(desc) };
		_pipelineDesc.pRootSignature = _rootSignaturePtr->_signature.Get();

		_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		_pipelineDesc.SampleMask = UINT_MAX;
		_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_pipelineDesc.NumRenderTargets = 1;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		_pipelineDesc.SampleDesc.Count = 1;
		_pipelineDesc.DSVFormat = _dsvPtr->_dsvFormat;

		_devicePtr->_device->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
	}
	else {		

		//인풋레이아웃 서술
		D3D12_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		//GSPSO 서술 및 생성
		_pipelineDesc.InputLayout = { desc, _countof(desc) };
		_pipelineDesc.pRootSignature = _rootSignaturePtr->_signature.Get();

		_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		_pipelineDesc.SampleMask = UINT_MAX;
		_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_pipelineDesc.NumRenderTargets = 1;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		_pipelineDesc.SampleDesc.Count = 1;
		_pipelineDesc.DSVFormat = _dsvPtr->_dsvFormat;

		_devicePtr->_device->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
	}
	
}

