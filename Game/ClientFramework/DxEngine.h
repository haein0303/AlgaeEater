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
#include "ObjectLoader.h"
#include "Timer.h"
#include "Input.h"
#include "SFML.h"
#include "AnimationObject.h"



class DxEngine {
public:
	//DX���� �ʱ�ȭ
	void Init(WindowInfo windowInfo);
	
	//�� �����Ӹ��� ������Ʈ
	void FixedUpdate(WindowInfo windowInfo, bool isActive);
	void Update(WindowInfo windowInfo, bool isActive);
	

	//�� �����Ӹ��� �׸���
	void Draw_multi(WindowInfo windowInfo,int);

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
	shared_ptr<ObjectLoader> fbxLoaderPtr = make_shared<ObjectLoader>();
	shared_ptr<Timer> timerPtr = make_shared<Timer>();
	shared_ptr<Timer> logicTimerPtr = make_shared<Timer>();
	shared_ptr<Input> inputPtr = make_shared<Input>();
	shared_ptr<SFML> networkPtr = make_shared<SFML>();
	unique_ptr<AnimationObject[]> animationPtr = make_unique<AnimationObject[]>(2);

	//������Ʈ ��ü ����
	Obj playerArr[PLAYERMAX];
	Obj npcArr[NPCMAX];
	Obj cubeArr[CubeMax];
	ParticleData particle[100];

	//������۷� �ѱ� ������
	Constants _transform = {};

	HANDLE _renderEvent = INVALID_HANDLE_VALUE;
	HANDLE _excuteEvent = INVALID_HANDLE_VALUE;
	int _render_thread_num = 0;

private:
	//ȭ�� ũ�� ����
	D3D12_VIEWPORT	_viewport;
	D3D12_RECT		_scissorRect;

};