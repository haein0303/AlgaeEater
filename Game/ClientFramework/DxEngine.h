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
#include "MESH.h"
#include "SCENE.h"
#include "OBJECT.h"
#include "d11on12.h"


class DxEngine {
public:
	void Boss(ComPtr<ID3D12GraphicsCommandList>& cmdList, MESH_ASSET& boss, const int i_now_render_index, const XMFLOAT3& scale, const float default_rot_x) {
		boss.UpdateSkinnedAnimation(timerPtr->_deltaTime, npcArr[9], 0);

		cmdList->SetPipelineState(boss._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &boss._vertexBufferView);
		cmdList->IASetIndexBuffer(&boss._indexBufferView);
		
		XMStoreFloat4x4(&_transform.world, XMMatrixScaling(scale.x, scale.y, scale.z)
			* XMMatrixRotationX(default_rot_x)
			* XMMatrixRotationY(npcArr[9]._degree * XM_PI / 180.f - XM_PI)
			* XMMatrixTranslation(npcArr[9]._transform.x, npcArr[9]._transform.y, npcArr[9]._transform.z));
		XMMATRIX world = XMLoadFloat4x4(&_transform.world);
		XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

		copy(begin(npcArr[9]._final_transforms), end(npcArr[9]._final_transforms), &_transform.BoneTransforms[0]);

		boss._tex._srvHandle = boss._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

		int sum = 0;
		for (Subset i : boss._animationPtr->mSubsets)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			descHeapPtr->CopyDescriptor(boss._tex._srvHandle, 5, devicePtr);
			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
			sum += i.FaceCount * 3;

			cout << "face ; " << i.FaceCount << endl;

			boss._tex._srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		}
	}

	//DX엔진 초기화
	void Init(WindowInfo windowInfo);
	void late_Init(WindowInfo windowInfo);


	//매 프레임마다 업데이트
	void FixedUpdate(WindowInfo windowInfo, bool isActive);
	void Update(WindowInfo windowInfo, bool isActive);
	

	//매 프레임마다 그리기
	void Draw_multi(WindowInfo windowInfo,int);

	void Make_Scene();
	void ChangeScene(SCENE::SceneTag tag);

	//요소별 객체 포인터
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

	shared_ptr<d11on12> d11Ptr = make_shared<d11on12>();

	// 오브젝트 데이터 객체 생성
	OBJECT playerArr[PLAYERMAX];
	OBJECT npcArr[NPCMAX];
	OBJECT boss_obj;
	OBJECT cubeArr[CubeMax];

	// 파티클 데이터
	ParticleData particles[PARTICLE_NUM];
	float coefficient_of_restitution = 0.3f;	// 반발계수
	float friction_coefficient = 0.9f;			// 마찰계수
	float gravitational_acceleration = -5.f;	// 중력가속도
	
	MESH_ASSET cube_asset;
	MESH_ASSET player_AKI_Body_asset;
	MESH_ASSET player_AKI_Astro_A_asset;
	MESH_ASSET player_AKI_Hair_A_asset;
	MESH_ASSET player_AKI_HeadPhone_asset;
	MESH_ASSET player_AKI_Sword_asset;
	MESH_ASSET npc_asset;
	MESH_ASSET boss;
	MESH_ASSET boss2;
	MESH_ASSET map_asset;
	MESH_ASSET floor;
	MESH_ASSET skybox;
	MESH_ASSET hp_bar;

	// 상수버퍼로 넘길 데이터
	Constants _transform = {};

	HANDLE _renderEvent = INVALID_HANDLE_VALUE;
	HANDLE _excuteEvent = INVALID_HANDLE_VALUE;
	int _render_thread_num = 0;

	SCENE* arrScene[SCENE::SceneTag::Count];
	SCENE* m_pCurrScene;

	vector<UI_ASSET> _test_ui_vector;

private:
	//화면 크기 관련
	D3D12_VIEWPORT	_viewport;
	D3D12_RECT		_scissorRect;

};