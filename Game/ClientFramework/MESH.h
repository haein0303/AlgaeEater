#pragma once

class DxEngine;
class Texture;
class MESH_ASSET
{
private:

	DxEngine& engine;
public:

	vector<Vertex> VertexVec;
	vector<UINT> IndexVec;

	ComPtr<ID3D12Resource> _vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

	ComPtr<ID3D12Resource> _indexBuffer;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;
	UINT _indexCount = 0;

	vector<Texture> _texVec;

	int			tex_index = 0;

	void Init(DxEngine& engine, const char* path);
	void Add_texture(const wstring& path);
};

