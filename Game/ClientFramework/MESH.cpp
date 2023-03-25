#include "Util.h"
#include "Texture.h"
#include "DxEngine.h"
#include "MESH.h"


//�̰� �� ������ �Ф�
void MESH_ASSET::Link_ptr(
	shared_ptr<Device> devicePtr,
	shared_ptr<ObjectLoader> fbxLoaderPtr,
	shared_ptr<VertexBuffer> vertexBufferPtr,
	shared_ptr<IndexBuffer> indexBufferPtr,
	shared_ptr<CmdQueue> cmdQueuePtr
) 
{
	_devicePtr = devicePtr;
	_fbxLoaderPtr = fbxLoaderPtr;
	_vertexBufferPtr = vertexBufferPtr;
	_indexBufferPtr = indexBufferPtr;
	_cmdQueuePtr =  cmdQueuePtr;
}

void MESH_ASSET::Init(const char* path)
{
	_fbxLoaderPtr->LoadObject(VertexVec, IndexVec, path);
	_vertexBufferPtr->CreateVertexBuffer(_vertexBuffer, _vertexBufferView, VertexVec, _devicePtr);
	_indexBufferPtr->CreateIndexBuffer(_indexBuffer, _indexBufferView, IndexVec, _devicePtr, _indexCount);

}



//��ǻ� ���� �޼ҵ� ���� ����
void MESH_ASSET::Add_texture(const wstring& path)
{
	_tex._CreateTexture(path, _devicePtr, _cmdQueuePtr);
}

//�굵 �Ȱ��� �� ����
void MESH_ASSET::Make_SRV()
{
	_tex._CreateSRVs(_devicePtr);
	//_texVec[num]._CreateSRVs(_engine.devicePtr);
}

