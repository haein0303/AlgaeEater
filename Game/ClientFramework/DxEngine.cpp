#include "DxEngine.h"
#include "Device.h"
#include "TestScene.h"

void DxEngine::Init(WindowInfo windowInfo)
{
	
	cout << "try server connect" << endl;
	if (-1 == networkPtr->ConnectServer(GAME_PORT_NUM)) {
		cout << "SERVER CONNECT FAIL" << endl;
		while (1);
	}//������ ����
	cout << "complite server connect" << endl;

	//ȭ�� ũ�� ����
	_viewport = { 0, 0, static_cast<FLOAT>(windowInfo.ClientWidth), static_cast<FLOAT>(windowInfo.ClientHeight), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight);

	//DX���� �ʱ�ȭ
	devicePtr->CreateDevice();
	cmdQueuePtr->CreateCmdListAndCmdQueue(devicePtr);
	swapChainPtr->DescriptAndCreateSwapChain(windowInfo, devicePtr, cmdQueuePtr);
	rtvPtr->CreateRTV(devicePtr, swapChainPtr);
	cameraPtr->TransformProjection(windowInfo); //���� ��ȯ
	rootSignaturePtr->CreateRootSignature(devicePtr);
	constantBufferPtr->CreateConstantBuffer(sizeof(Constants), CONSTANT_COUNT, devicePtr);
	constantBufferPtr->CreateView(devicePtr);
	descHeapPtr->CreateDescTable(CONSTANT_COUNT, devicePtr);
	
	d11Ptr->init(this, windowInfo);

	cout << "complite Init ptr" << endl;

	timerPtr->InitTimer();
	logicTimerPtr->InitTimer();

	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	RECT rect = { 0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(windowInfo.hwnd, 0, 100, 100, windowInfo.ClientWidth, windowInfo.ClientHeight, 0);
	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	srand((unsigned int)time(NULL));
	
	inputPtr->Init(windowInfo); //���� ������ �ʱ�ȭ

	_renderEvent = ::CreateEvent(nullptr, FALSE, TRUE, nullptr);
	_excuteEvent = ::CreateEvent(nullptr, FALSE, TRUE, nullptr);

	

	cout << "complite all init" << endl;
}

void DxEngine::late_Init(WindowInfo windowInfo)
{
	cube_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr,cmdQueuePtr,rootSignaturePtr,dsvPtr);
	cube_asset.Init("../Resources/Cube.txt", ObjectType::GeneralObjects);
	cube_asset.Add_texture(L"..\\Resources\\Texture\\bricks.dds");
	cube_asset.Make_SRV();
	cube_asset.CreatePSO(L"..\\Bricks.hlsl");

	map_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	map_asset.Init("../Resources/Map.txt", ObjectType::GeneralObjects);
	map_asset.Add_texture(L"..\\Resources\\Texture\\bricks.dds");
	map_asset.Make_SRV();
	map_asset.CreatePSO(L"..\\Bricks.hlsl");

	floor.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	floor.Init("../Resources/Floor.txt", ObjectType::GeneralObjects);
	floor.Add_texture(L"..\\Resources\\Texture\\sample.png");
	floor.Add_texture(L"..\\Resources\\Texture\\Back_wall.png");
	floor.Add_texture(L"..\\Resources\\Texture\\Front_wall.png");
	floor.Add_texture(L"..\\Resources\\Texture\\Right_wall.png");
	floor.Add_texture(L"..\\Resources\\Texture\\Left_wall.png");
	floor.Make_SRV();
	floor.CreatePSO(L"..\\Bricks.hlsl");

	hp_bar.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	hp_bar.Init("../Resources/Floor.txt", ObjectType::GeneralObjects);
	hp_bar.Add_texture(L"..\\Resources\\Texture\\hp.jpg");
	hp_bar.Make_SRV();
	hp_bar.CreatePSO(L"..\\HPBar.hlsl");

	skybox.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	skybox.Init("../Resources/SkySphere.txt", ObjectType::SkyBox);
	skybox.Add_texture(L"..\\Resources\\Texture\\Sky.jpg");
	skybox.Make_SRV();
	skybox.CreatePSO(L"..\\SkySphere.hlsl");

	boss.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	boss.Init("../Resources/mechanical_spider.txt", ObjectType::AnimationObjects);
	boss.Add_texture(L"..\\Resources\\Texture\\NPCSpider_DefaultMaterial_AlbedoTransparency.png");
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_black_BaseColor.png");
	boss.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	boss.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	boss.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	boss.Make_SRV();
	boss.CreatePSO();

	player_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_asset.Init("../Resources/Character.txt", ObjectType::AnimationObjects);
	player_asset.Add_texture(L"..\\Resources\\Texture\\AnimeCharcter.dds");
	player_asset.Make_SRV();
	player_asset.CreatePSO();

	npc_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	npc_asset.Init("../Resources/OrangeSpider.txt", ObjectType::AnimationObjects);
	npc_asset.Add_texture(L"..\\Resources\\Texture\\NPCSpider_DefaultMaterial_AlbedoTransparency.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_paint_black_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Make_SRV();
	npc_asset.CreatePSO();

	

	for (int i = 0; i < PLAYERMAX; ++i)
	{
		playerArr[i]._final_transforms.resize(player_asset._animationPtr->mBoneHierarchy.size());
	}
	npcArr[0]._final_transforms.resize(boss._animationPtr->mBoneHierarchy.size());
	for (int i = 1; i < NPCMAX; ++i) {
		npcArr[i]._final_transforms.resize(npc_asset._animationPtr->mBoneHierarchy.size());
	}

	d11Ptr->LoadPipeline();
	d11Ptr->addResource(L"..\\Resources\\Lobby\\button_1.png");
	cout << "complite late init" << endl;
}

void DxEngine::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
	networkPtr->ReceiveServer(playerArr, npcArr, cubeArr);

	if (isActive)
	{
		inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr);
		inputPtr->inputMouse(playerArr, networkPtr);
	}

	

	// 플레이어와 npc 공격에 대한 충돌 처리
	for (int i = 0; i < PLAYERMAX; ++i) {
		if (playerArr[i]._on == true ) {
			for (int j = 0; j < NPCMAX; ++j)
			{
				if (npcArr[j]._on == true) {
					if (pow(playerArr[i]._transform.x - npcArr[j]._transform.x, 2) + pow(playerArr[i]._transform.z - npcArr[j]._transform.z, 2) <= 9.f) {
						if ((playerArr[i]._animation_state == 2 || playerArr[i]._animation_state == 3)
							&& playerArr[i]._animation_time_pos >= player_asset._animationPtr->GetClipEndTime(playerArr[i]._animation_state / 2.f)
							&& playerArr[i]._can_attack) { // 플레이어가 공격중이고 애니메이션이 타격시점이고 공격기회가 있다면
							playerArr[i]._can_attack = false;

							CS_COLLISION_PACKET p;
							p.size = sizeof(p);
							p.type = CS_COLLISION;
							p.attack_type = 'a'; // 의미 없는 값
							p.attacker_id = i;
							p.target_id = j;
							networkPtr->send_packet(&p);

							cout << "player" << i << " hp : " << playerArr[i]._hp << endl;	// 플레이어 hp 콘솔로 체크
							cout << "npc" << j << " hp : " << npcArr[j]._hp << endl;		// npc hp 콘솔로 체크
						}
						if (npcArr[j]._animation_state == 2
							&& npcArr[j]._animation_time_pos >= npc_asset._animationPtr->GetClipEndTime(npcArr[j]._animation_state / 2.f)
							&& npcArr[j]._can_attack) { // npc가 공격중이고 애니메이션이 타격시점이고 공격기회가 있다면
							npcArr[j]._can_attack = false;

							CS_COLLISION_PACKET p;
							p.size = sizeof(p);
							p.type = CS_COLLISION;
							p.attack_type = 'a'; // 의미 없는 값
							p.attacker_id = j;
							p.target_id = i;
							networkPtr->send_packet(&p);

							cout << "player" << i << " hp : " << playerArr[i]._hp << endl;	// 플레이어 hp 콘솔로 체크
							cout << "npc" << j << " hp : " << npcArr[j]._hp << endl;		// npc hp 콘솔로 체크
						}
					}
				}
			}
		}
	}

	// 플레이어 사망
	for (int i = 0; i < PLAYERMAX; ++i) {
		if (playerArr[i]._on == true && playerArr[i]._hp <= 0.f) {
			playerArr[i]._animation_state = 4;
			//playerArr[i]._on = false;

			CS_MOVE_PACKET p;
			p.size = sizeof(p);
			p.type = CS_MOVE;
			p.char_state = playerArr[networkPtr->myClientId]._animation_state;
			//networkPtr->send_packet(&p);
		}
	}

	// npc 사망
	for (int i = 0; i < NPCMAX; ++i) {
		if (npcArr[i]._on == true && npcArr[i]._hp <= 0.f) {
			npcArr[i]._animation_state = 3;
			//npcArr[i]._on = false;

			CS_MOVE_PACKET p;
			p.size = sizeof(p);
			p.type = CS_MOVE;
			p.char_state = npcArr[networkPtr->myClientId]._animation_state;
			//networkPtr->send_packet(&p);
		}
	}
	

	//VP ��ȯ
	cameraPtr->pos = XMVectorSet(playerArr[networkPtr->myClientId]._transform.x - 7 * cosf(inputPtr->angle.x*XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId]._transform.y + 4 + 7 * cosf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId]._transform.z - 7 * sinf(inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f), 0.0f);
	XMVECTOR target = XMVectorSet(playerArr[networkPtr->myClientId]._transform.x, playerArr[networkPtr->myClientId]._transform.y + 1.65f,
		playerArr[networkPtr->myClientId]._transform.z,
		playerArr[networkPtr->myClientId]._transform.w);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(cameraPtr->pos, target, up); //�� ��ȯ ���
	XMStoreFloat4x4(&_transform.view, XMMatrixTranspose(view));

	XMMATRIX proj = XMLoadFloat4x4(&cameraPtr->mProj); //���� ��ȯ ���
	XMStoreFloat4x4(&_transform.proj, XMMatrixTranspose(proj));

	//Light
	LightInfo lightInfo;
	_transform.lnghtInfo = lightInfo;
}

void DxEngine::Update(WindowInfo windowInfo, bool isActive)
{


}



void DxEngine::Draw_multi(WindowInfo windowInfo, int i_now_render_index)
{
	::WaitForSingleObject(_renderEvent, INFINITE);
	ComPtr<ID3D12CommandAllocator>		cmdAlloc = cmdQueuePtr->_arr_cmdAlloc[i_now_render_index];
	ComPtr<ID3D12GraphicsCommandList>	cmdList = cmdQueuePtr->_arr_cmdList[i_now_render_index];

	//�ִϸ��̼�
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true) {
			player_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i]);
		}
	}
	if (npcArr[0]._on == true) {
		boss.UpdateSkinnedAnimation(timerPtr->_deltaTime, npcArr[0]);
	}
	for (int i = 1; i < NPCMAX; ++i) {
		if (npcArr[i]._on == true) {
			npc_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, npcArr[i]);
		}
	}

	cmdAlloc->Reset();
	cmdList->Reset(cmdQueuePtr->_arr_cmdAlloc[i_now_render_index].Get(), nullptr);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmdList->ResourceBarrier(1, &barrier);

	cmdList->RSSetViewports(1, &_viewport);
	cmdList->RSSetScissorRects(1, &_scissorRect);


	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = rtvPtr->_rtvHandle[i_now_render_index];

	cmdList->ClearRenderTargetView(backBufferView, Colors::Lavender, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = dsvPtr->_dsvHandle;
	cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);


	cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->SetGraphicsRootSignature(rootSignaturePtr->_signature.Get());
	constantBufferPtr->_currentIndex = 0;
	descHeapPtr->_currentGroupIndex = 0;

	ID3D12DescriptorHeap* descHeap = descHeapPtr->_descHeap.Get();
	cmdList->SetDescriptorHeaps(1, &descHeap);

	cmdList->SetPipelineState(player_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_asset._indexBufferView);
	//����
	for (int i = 0; i < PLAYERMAX; ++i) //�÷��̾� ����
	{
		if (playerArr[i]._on == true)
		{
			//�������� : �Ϲ����� �ֶ� �ִϸ��̼��̶� �޶� �׷�
			//���°� �ٲ��� ���������� ������Ʈ ������Ʈ�� �ҷ��� �������ִ°�
			//�̰� ��ĥ �� �ִ� �۾� ���� ������?			
			{
				//���� ��ȯ
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 1.0f, 1.0f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				// ��Ű�� �ִϸ��̼� ��� ������ ����
				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				//����
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}

	// 보스
	cmdList->SetPipelineState(boss._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &boss._vertexBufferView);
	cmdList->IASetIndexBuffer(&boss._indexBufferView);
	if (npcArr[0]._on == true)
	{
		{
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(600.f, 600.f, 600.f) * XMMatrixRotationX(-XM_PI / 2.f)
				* XMMatrixRotationY(npcArr[0]._degree * XM_PI / 180.f)
				* XMMatrixTranslation(npcArr[0]._transform.x, npcArr[0]._transform.y + 0.2f, npcArr[0]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
			XMStoreFloat4x4(&_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

			copy(begin(npcArr[0]._final_transforms), end(npcArr[0]._final_transforms), &_transform.BoneTransforms[0]);

			texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();

			int sum = 0;
			for (Subset i : boss._animationPtr->mSubsets)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
				sum += i.FaceCount * 3;
			}
		}
	}

	cmdList->SetPipelineState(npc_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &npc_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&npc_asset._indexBufferView);
	for (int i = 1; i < NPCMAX; i++) //npc ����
	{
		if (npcArr[i]._on == true)
		{			
			{
				//���� ��ȯ
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(200.f, 200.f, 200.f) * XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(npcArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(npcArr[i]._transform.x, npcArr[i]._transform.y + 0.2f, npcArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
				XMStoreFloat4x4(&_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

				// ��Ű�� �ִϸ��̼� ��� ������ ����
				copy(begin(npcArr[i]._final_transforms), end(npcArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				//����
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				
				int sum = 0;
				for (Subset i : npc_asset._animationPtr->mSubsets)
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
					descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
					cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
					sum += i.FaceCount * 3;
				}
			}
		}
	}

	// npc hp bar
	cmdList->SetPipelineState(hp_bar._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &hp_bar._vertexBufferView);
	cmdList->IASetIndexBuffer(&hp_bar._indexBufferView);
	for (int i = 1; i < NPCMAX; i++)
	{
		if (npcArr[i]._on == true) {
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(0.5f, 0.1f, 0.1f)
				* XMMatrixRotationX(-atan2f(cameraPtr->pos.m128_f32[1] - (npcArr[i]._transform.y + 1.f),
					sqrt(pow(cameraPtr->pos.m128_f32[0] - npcArr[i]._transform.x, 2) + pow(cameraPtr->pos.m128_f32[2] - npcArr[i]._transform.z, 2))))
				* XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - npcArr[i]._transform.x, cameraPtr->pos.m128_f32[2] - npcArr[i]._transform.z))
				* XMMatrixTranslation(npcArr[i]._transform.x, npcArr[i]._transform.y + 1.f, npcArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
			_transform.hp_bar_size = 2.f;
			_transform.hp_bar_start_pos = npcArr[i]._transform;
			_transform.hp_bar_start_pos.x -= _transform.hp_bar_size / 2.f ;
			_transform.current_hp = npcArr[i]._hp;
			_transform.max_hp = 100;

			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);

				hp_bar._tex._srvHandle = hp_bar._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

				descHeapPtr->CopyDescriptor(hp_bar._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(hp_bar._indexCount, 1, 0, 0, 0);
		}
	}

	cmdList->SetPipelineState(cube_asset._pipelineState.Get());	
	cmdList->IASetVertexBuffers(0, 1, &cube_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&cube_asset._indexBufferView);
	for (int i = 0; i < CubeMax; i++) //��� ����
	{
		if (cubeArr[i]._on == true)
		{
			//���� ��ȯ
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 2.0f, 1.0f) * XMMatrixTranslation(cubeArr[i]._transform.x, cubeArr[i]._transform.y + 2.0f, cubeArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			//����
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);		

				cube_asset._tex._srvHandle = cube_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				
				descHeapPtr->CopyDescriptor(cube_asset._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(cube_asset._indexCount, 1, 0, 0, 0);
		}
	}

	/*// �� ����
	{
		cmdList->SetPipelineState(map_asset._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &map_asset._vertexBufferView);
		cmdList->IASetIndexBuffer(&map_asset._indexBufferView);
		//���� ��ȯ
		XMStoreFloat4x4(&_transform.world, XMMatrixScaling(3.0f, 3.0f, 3.0f)* XMMatrixTranslation(0.f, 0.f, 0.f));
		XMMATRIX world = XMLoadFloat4x4(&_transform.world);
		XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
		//����
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);

			map_asset._tex._srvHandle = map_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

			descHeapPtr->CopyDescriptor(map_asset._tex._srvHandle, 5, devicePtr);
		}

		descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
		cmdList->DrawIndexedInstanced(map_asset._indexCount, 1, 0, 0, 0);
	}*/

	// map
	float map_size = 100.f;

	cmdList->SetPipelineState(floor._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &floor._vertexBufferView);
	cmdList->IASetIndexBuffer(&floor._indexBufferView);
	for(int i = 0; i < 5; ++i)
	{
		if (i == 0) {
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(map_size, map_size, 1.0f)* XMMatrixRotationX(-XM_PI / 2.f)* XMMatrixTranslation(0.f, 0.f, 0.f));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			floor._tex._srvHandle = floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
			
		}
		if (i == 1) {
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f)* XMMatrixRotationX(0.f)* XMMatrixTranslation(0.f, map_size / 4.f, -map_size));
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

	// ��ī�� �ڽ� ����
	{
		cmdList->SetPipelineState(skybox._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &skybox._vertexBufferView);
		cmdList->IASetIndexBuffer(&skybox._indexBufferView);
		//���� ��ȯ
		XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.f, 2.f, 0.f));
		XMMATRIX world = XMLoadFloat4x4(&_transform.world);
		XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
		_transform.camera_pos = cameraPtr->pos;
		//����
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);

			skybox._tex._srvHandle = skybox._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

			descHeapPtr->CopyDescriptor(skybox._tex._srvHandle, 5, devicePtr);
		}

		descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
		cmdList->DrawIndexedInstanced(skybox._indexCount, 1, 0, 0, 0);
	}

	// 파티클
	if (pow(playerArr[0]._transform.x - npcArr[9]._transform.x, 2) + pow(playerArr[0]._transform.z - npcArr[9]._transform.z, 2) <= 4.f) // 플레이어와 npc 충돌 체크
	{
		if (playerArr[0]._isCollision == false)
			playerArr[0]._isFirstCollision = true;
		else
			playerArr[0]._isFirstCollision = false;
		playerArr[0]._isCollision = true;
	}
	if (pow(playerArr[0]._transform.x - npcArr[9]._transform.x, 2) + pow(playerArr[0]._transform.z - npcArr[9]._transform.z, 2) > 4.f)
	{
		playerArr[0]._isCollision = false;
	}
	for (int i = 0; i < PARTICLE_NUM; ++i) // 물리처리 및 렌더링
	{
		if (playerArr[0]._isFirstCollision == true && particles[i].alive == 0)
		{
			particles[i].lifeTime = (float)(rand() % 101) / 1000.f + 0.3f; // 0.3~0.4
			particles[i].curTime = 0.0f;
			particles[i].pos = XMVectorSet(npcArr[9]._transform.x, npcArr[9]._transform.y + 0.5f, npcArr[9]._transform.z, 1.f);
			particles[i].moveSpeed = (float)(rand() % 101) / 50 + 2.f; // 2~4
			particles[i].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
			XMVector3Normalize(particles[i].dir);
			particles[i].velocity = XMVectorSet(particles[i].dir.m128_f32[0] * particles[i].moveSpeed, particles[i].dir.m128_f32[1] * particles[i].moveSpeed, particles[i].dir.m128_f32[2] * particles[i].moveSpeed, 1.f);
			particles[i].alive = 1;
		}
		else if (particles[i].alive == 1)
		{
			if (particles[i].pos.m128_f32[1] - particles[i].bounding_box_half_size.m128_f32[1] < 0.f) {
				// 비탄성 충돌
				particles[i].velocity.m128_f32[1] = particles[i].velocity.m128_f32[1] * -coefficient_of_restitution;
				particles[i].pos.m128_f32[1] = particles[i].bounding_box_half_size.m128_f32[1];
			}

			if (particles[i].velocity.m128_f32[1] <= 0.05f
				&& particles[i].pos.m128_f32[1] - particles[i].bounding_box_half_size.m128_f32[1] == 0.f) {
				// 마찰력
				if (fabs(particles[i].velocity.m128_f32[0]) > 0.1f) {
					particles[i].velocity.m128_f32[0] = particles[i].velocity.m128_f32[0] + friction_coefficient * gravitational_acceleration * particles[i].dir.m128_f32[0] * timerPtr->_deltaTime;
				}
				else {
					particles[i].velocity.m128_f32[0] = 0.f;
				}

				if (fabs(particles[i].velocity.m128_f32[2]) > 0.1f) {
					particles[i].velocity.m128_f32[2] = particles[i].velocity.m128_f32[2] + friction_coefficient * gravitational_acceleration * particles[i].dir.m128_f32[2] * timerPtr->_deltaTime;
				}
				else {
					particles[i].velocity.m128_f32[2] = 0.f;
				}
			}

			particles[i].pos.m128_f32[0] = particles[i].pos.m128_f32[0] + particles[i].velocity.m128_f32[0] * timerPtr->_deltaTime; // x성분 이동
			particles[i].velocity.m128_f32[1] = particles[i].velocity.m128_f32[1] - 5.f * timerPtr->_deltaTime; // 중력가속도에 의한 나중속도
			particles[i].pos.m128_f32[1] = particles[i].pos.m128_f32[1] + particles[i].velocity.m128_f32[1] * timerPtr->_deltaTime; // y성분 이동
			particles[i].pos.m128_f32[2] = particles[i].pos.m128_f32[2] + particles[i].velocity.m128_f32[2] * timerPtr->_deltaTime; // z성분 이동
			XMStoreFloat4x4(&_transform.world, XMMatrixRotationX(-atan2f(cameraPtr->pos.m128_f32[1] - particles[i].pos.m128_f32[1], sqrt(pow(cameraPtr->pos.m128_f32[0] - particles[i].pos.m128_f32[0], 2) + pow(cameraPtr->pos.m128_f32[2] - particles[i].pos.m128_f32[2], 2))))
				* XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - particles[i].pos.m128_f32[0], cameraPtr->pos.m128_f32[2] - particles[i].pos.m128_f32[2]))
				* XMMatrixTranslation(particles[i].pos.m128_f32[0], particles[i].pos.m128_f32[1], particles[i].pos.m128_f32[2])); // 파티클이 항상 카메라를 바라보기
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			particles[i].curTime += 0.001f;

			if (particles[i].lifeTime < particles[i].curTime)
			{
				particles[i].alive = 0;
			}

			cmdList->SetPipelineState(psoPtr->_gsPipelineState.Get());
			cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_particleVertexBufferView);
			cmdList->IASetIndexBuffer(&indexBufferPtr->_particleIndexBufferView);
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				texturePtr->_srvHandle.Offset(6, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
			}
			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(indexBufferPtr->_particleIndexCount, 1, 0, 0, 0);
		}
	}

	d11Ptr->RenderUI(i_now_render_index);

	::WaitForSingleObject(_excuteEvent, INFINITE);
	SetEvent(_renderEvent);

	

	//���� ����
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // ȭ�� ���

	cmdList->ResourceBarrier(1, &barrier2);
	cmdList->Close();

	

	ID3D12CommandList* cmdListArr[] = { cmdList.Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	
	d11Ptr->ExcuteUI(i_now_render_index);
	swapChainPtr->_swapChain->Present(0, 0);
	
	cmdQueuePtr->WaitSync();

	swapChainPtr->_backBufferIndex = (swapChainPtr->_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;

	SetEvent(_excuteEvent);
}

void DxEngine::Make_Scene()
{	
	// arrScene[SceneTag::Title] = new TitleScene();	// �̷� ������� ���� ������.
	//�̷��� ��� �߰��մϴ�.
	arrScene[SCENE::SceneTag::test_scene] = new TestScene(SCENE::SceneTag::test_scene, this);
}

void DxEngine::ChangeScene(SCENE::SceneTag tag)
{
	m_pCurrScene = arrScene[tag];
}
