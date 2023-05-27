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
	float _key_rotation_time = 0.f;
	OBJECT key_data[KEYMAX];
	OBJECT test_pillar_data;
	int vertex_animtion_frame = 0;

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
	MESH_ASSET stage0_map;
	MESH_ASSET floor;
	MESH_ASSET skybox;
	MESH_ASSET hp_bar;
	MESH_ASSET color_pattern;
	MESH_ASSET key;
	MESH_ASSET test_pillar;

	// 상수버퍼로 넘길 데이터
	Constants _transform = {};

	HANDLE _renderEvent = INVALID_HANDLE_VALUE;
	HANDLE _excuteEvent = INVALID_HANDLE_VALUE;
	int _render_thread_num = 0;

	int Scene_num = 1;
	SCENE* arrScene[SCENE::SceneTag::Count];
	SCENE* m_pCurrScene;

	vector<UI_ASSET> _test_ui_vector;

private:
	//화면 크기 관련
	D3D12_VIEWPORT	_viewport;
	D3D12_RECT		_scissorRect;

public:
	void Boss(ComPtr<ID3D12GraphicsCommandList>& cmdList, MESH_ASSET& boss, const int i_now_render_index, const XMFLOAT3& scale, const float default_rot_x) {
		boss.UpdateSkinnedAnimation(timerPtr->_deltaTime, boss_obj, 0);

		cmdList->SetPipelineState(boss._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &boss._vertexBufferView);
		cmdList->IASetIndexBuffer(&boss._indexBufferView);

		XMStoreFloat4x4(&_transform.world, XMMatrixScaling(scale.x, scale.y, scale.z)
			* XMMatrixRotationX(default_rot_x)
			* XMMatrixRotationY(boss_obj._degree * XM_PI / 180.f - XM_PI)
			* XMMatrixTranslation(boss_obj._transform.x, boss_obj._transform.y, boss_obj._transform.z));
		XMMATRIX world = XMLoadFloat4x4(&_transform.world);
		XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

		copy(begin(boss_obj._final_transforms), end(boss_obj._final_transforms), &_transform.BoneTransforms[0]);

		boss._tex._srvHandle = boss._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

		int sum = 0;
		int count = 0;
		for (Subset i : boss._animationPtr->mSubsets)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			descHeapPtr->CopyDescriptor(boss._tex._srvHandle, 5, devicePtr);
			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
			sum += i.FaceCount * 3;

			if (count == 2) { // eye, 조건에 boss별 type추가 필요
				boss._tex._srvHandle = boss._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				boss._tex._srvHandle.Offset(3 + boss_obj._eye_color, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			}
			else if (count == 3) { // wire
				boss._tex._srvHandle = boss._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				boss._tex._srvHandle.Offset(8, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			}
			else {
				boss._tex._srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			}

			++count;
		}
	}

	void Map(ComPtr<ID3D12GraphicsCommandList>& cmdList, MESH_ASSET& floor, MESH_ASSET& map_asset, const int i_now_render_index, int stage)
	{
		if (stage == 0)
		{
			// map
			float map_size = 100.f;

			cmdList->SetPipelineState(floor._pipelineState.Get());
			cmdList->IASetVertexBuffers(0, 1, &floor._vertexBufferView);
			cmdList->IASetIndexBuffer(&floor._indexBufferView);
			for (int i = 0; i < 5; ++i)
			{
				if (i == 0) {
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(map_size, map_size, 1.0f) * XMMatrixRotationX(-XM_PI / 2.f) * XMMatrixTranslation(0.f, 0.f, 0.f));
					XMMATRIX world = XMLoadFloat4x4(&_transform.world);
					XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					floor._tex._srvHandle = floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

				}
				if (i == 1) {
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f) * XMMatrixRotationX(0.f) * XMMatrixTranslation(0.f, map_size / 4.f, -map_size));
					XMMATRIX world = XMLoadFloat4x4(&_transform.world);
					XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					floor._tex._srvHandle = floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

				}
				else if (i == 2) {
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f) * XMMatrixRotationX(XM_PI) * XMMatrixRotationZ(XM_PI) * XMMatrixTranslation(0.f, map_size / 4.f, map_size));
					XMMATRIX world = XMLoadFloat4x4(&_transform.world);
					XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					floor._tex._srvHandle = floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				}
				else if (i == 3) {
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f) * XMMatrixRotationY(-XM_PI / 2.f) * XMMatrixTranslation(map_size, map_size / 4.f, 0.f));
					XMMATRIX world = XMLoadFloat4x4(&_transform.world);
					XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					floor._tex._srvHandle = floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				}
				else if (i == 4) {
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f) * XMMatrixRotationY(XM_PI / 2.f) * XMMatrixTranslation(-map_size, map_size / 4.f, 0.f));
					XMMATRIX world = XMLoadFloat4x4(&_transform.world);
					XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					floor._tex._srvHandle = floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				}

				floor._tex._srvHandle.Offset(i, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				descHeapPtr->CopyDescriptor(floor._tex._srvHandle, 5, devicePtr);

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(floor._indexCount, 1, 0, 0, 0);
			}
		}

		else if (stage == 1)
		{
			// floor
			{
				float map_size = 1000.f;

				cmdList->SetPipelineState(floor._pipelineState.Get());
				cmdList->IASetVertexBuffers(0, 1, &floor._vertexBufferView);
				cmdList->IASetIndexBuffer(&floor._indexBufferView);

				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(map_size, map_size, 1.f) * XMMatrixRotationX(-XM_PI / 2.f) * XMMatrixTranslation(0.f, 0.f, 0.f));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				floor._tex._srvHandle = floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(floor._tex._srvHandle, 5, devicePtr);

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(floor._indexCount, 1, 0, 0, 0);
			}

			// wall
			{
				float map_size = 20.f;
				cmdList->SetPipelineState(map_asset._pipelineState.Get());
				cmdList->IASetVertexBuffers(0, 1, &map_asset._vertexBufferView);
				cmdList->IASetIndexBuffer(&map_asset._indexBufferView);

				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.578f * map_size, 0.3f * map_size, 0.01f * map_size) * XMMatrixTranslation(0.f, 0.f, 0.f));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				map_asset._tex._srvHandle = map_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(map_asset._tex._srvHandle, 5, devicePtr);

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(map_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
};