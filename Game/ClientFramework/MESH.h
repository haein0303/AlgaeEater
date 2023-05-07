#pragma once

class DxEngine;
class Texture;

class MESH_ASSET
{
public:
	//공유하장
	shared_ptr<Device> _devicePtr;
	shared_ptr<ObjectLoader> _fbxLoaderPtr;
	shared_ptr<VertexBuffer> _vertexBufferPtr;
	shared_ptr<IndexBuffer> _indexBufferPtr;
	shared_ptr<CmdQueue> _cmdQueuePtr;
	shared_ptr<RootSignature> _rootSignaturePtr;
	shared_ptr<DSV> _dsvPtr;

	//내부데이터란다
	ComPtr<ID3D12PipelineState> _pipelineState;

	shared_ptr<AnimationObject> _animationPtr = make_shared<AnimationObject>();
	vector<SkinnedVertex> _AniVertexVec;
	vector<Vertex> _VertexVec;
	vector<UINT> _IndexVec;

	ComPtr<ID3D12Resource> _vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

	ComPtr<ID3D12Resource> _indexBuffer;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;
	UINT _indexCount = 0;

	Texture _tex;
	ObjectType _obj_type;
	int	 tex_index = 0;
	bool _is_default_shader = true;

	void Link_ptr(
		shared_ptr<Device> devicePtr,
		shared_ptr<ObjectLoader> fbxLoaderPtr,
		shared_ptr<VertexBuffer> vertexBufferPtr,
		shared_ptr<IndexBuffer> indexBufferPtr,
		shared_ptr<CmdQueue> cmdQueuePtr,
		shared_ptr<RootSignature> rootSignaturePtr,
		shared_ptr<DSV> dsvPtr
	);
	void Init(const char* path, ObjectType obj_type);
	void Add_texture(const wstring& path);
	void Make_SRV();
	void CreatePSO();
	void CreatePSO(const wchar_t* shader);
	void UpdateSkinnedAnimation(float dt, OBJECT& player, int i);

};

