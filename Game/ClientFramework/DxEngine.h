#pragma once
#include "Util.h"
#include "Device.h"
#include "CmdQueue.h"
#include "SwapChain.h"
#include "RTV.h"
#include "VertexBuffer.h"
#include "RootSignature.h"
#include "DSV.h"
#include "PSO.h"
#include "ConstantBuffer.h"
#include "DescHeap.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Camera.h"
#include "FbxLoader.h"
#include "Timer.h"
#include "Input.h"
#include "SFML.h"

class DxEngine {
public:
	//DX���� �ʱ�ȭ
	void Init(WindowInfo windowInfo);
	
	//�� �����Ӹ��� ������Ʈ
	void Update(WindowInfo windowInfo, bool isActive);

	//�� �����Ӹ��� �׸���
	void Draw();

	//��Һ� ��ü ������
	shared_ptr<Device> devicePtr = make_shared<Device>();
	shared_ptr<CmdQueue> cmdQueuePtr = make_shared<CmdQueue>();
	shared_ptr<SwapChain> swapChainPtr = make_shared<SwapChain>();
	shared_ptr<RTV> rtvPtr = make_shared<RTV>();
	shared_ptr<VertexBuffer> vertexBufferPtr = make_shared<VertexBuffer>();
	shared_ptr<RootSignature> rootSignaturePtr = make_shared<RootSignature>();
	shared_ptr<PSO> psoPtr = make_shared<PSO>();
	shared_ptr<ConstantBuffer> constantBufferPtr = make_shared<ConstantBuffer>();
	shared_ptr<DescHeap> descHeapPtr = make_shared<DescHeap>();
	shared_ptr<IndexBuffer> indexBufferPtr = make_shared<IndexBuffer>();
	shared_ptr<Texture> texturePtr = make_shared<Texture>();
	shared_ptr<DSV> dsvPtr = make_shared<DSV>();
	shared_ptr<Camera> cameraPtr = make_shared<Camera>();
	shared_ptr<FbxLoader> fbxLoaderPtr = make_shared<FbxLoader>();
	shared_ptr<Timer> timerPtr = make_shared<Timer>();
	shared_ptr<Input> inputPtr = make_shared<Input>();
	shared_ptr<SFML> networkPtr = make_shared<SFML>();

	//������Ʈ ��ü ����
	Obj playerArr[PLAYERMAX];
	Obj npcArr[NPCMAX];

private:
	//ȭ�� ũ�� ����
	D3D12_VIEWPORT	_viewport;
	D3D12_RECT		_scissorRect;
};