#include "Util.h"
#include "MESH.h"
#include "Texture.h"
#include "DxEngine.h"

void MESH_ASSET::Init(DxEngine& engine, const char* path)
{
	engine.fbxLoaderPtr->LoadObject(VertexVec, IndexVec, path);
	engine.vertexBufferPtr->CreateVertexBuffer(_vertexBuffer, _vertexBufferView, VertexVec, engine.devicePtr);
	engine.indexBufferPtr->CreateIndexBuffer(_indexBuffer, _indexBufferView, IndexVec, engine.devicePtr,_indexCount);
}

void MESH_ASSET::Add_texture(const wstring& path)
{
	int num = _texVec.size();
	
	
	_texVec.emplace_back();
	_texVec[num]._CreateTexture(path, engine.devicePtr, engine.cmdQueuePtr);
	_texVec[num]._CreateSRVs(engine.devicePtr);

}

