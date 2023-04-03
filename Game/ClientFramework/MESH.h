#pragma once

class DxEngine;
class Texture;

class MESH_ASSET
{
public:
	shared_ptr<Device> _devicePtr;
	shared_ptr<ObjectLoader> _fbxLoaderPtr;
	shared_ptr<VertexBuffer> _vertexBufferPtr;
	shared_ptr<IndexBuffer> _indexBufferPtr;
	shared_ptr<CmdQueue> _cmdQueuePtr;

	shared_ptr<AnimationObject> _animationPtr;
	vector<SkinnedVertex> AniVertexVec;
	vector<Vertex> VertexVec;
	vector<UINT> IndexVec;

	ComPtr<ID3D12Resource> _vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

	ComPtr<ID3D12Resource> _indexBuffer;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;
	UINT _indexCount = 0;

	Texture _tex;
	bool _is_ani;
	int			tex_index = 0;

	void Link_ptr(
		shared_ptr<Device> devicePtr,
		shared_ptr<ObjectLoader> fbxLoaderPtr,
		shared_ptr<VertexBuffer> vertexBufferPtr,
		shared_ptr<IndexBuffer> indexBufferPtr,
		shared_ptr<CmdQueue> cmdQueuePtr
	);
	void Init(const char* path,bool is_animation);
	void Add_texture(const wstring& path);
	void Make_SRV();
};
