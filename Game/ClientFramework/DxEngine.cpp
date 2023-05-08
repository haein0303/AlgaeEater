#include "DxEngine.h"
#include "Device.h"
#include "TestScene.h"

void DxEngine::Init(WindowInfo windowInfo)
{
	
	//cout << "try server connect" << endl;
	//if (-1 == networkPtr->ConnectServer(GAME_PORT_NUM)) {
	//	cout << "SERVER CONNECT FAIL" << endl;
	//	while (1);
	//}//ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	//cout << "complite server connect" << endl;

	//»≠ÔøΩÔøΩ ≈©ÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	_viewport = { 0, 0, static_cast<FLOAT>(windowInfo.ClientWidth), static_cast<FLOAT>(windowInfo.ClientHeight), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight);

	//DXÔøΩÔøΩÔøΩÔøΩ ÔøΩ ±ÔøΩ»≠
	devicePtr->CreateDevice();
	cmdQueuePtr->CreateCmdListAndCmdQueue(devicePtr);
	swapChainPtr->DescriptAndCreateSwapChain(windowInfo, devicePtr, cmdQueuePtr);
	rtvPtr->CreateRTV(devicePtr, swapChainPtr);
	cameraPtr->TransformProjection(windowInfo); //ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
	rootSignaturePtr->CreateRootSignature(devicePtr);
	constantBufferPtr->CreateConstantBuffer(CONSTANT_COUNT, CONSTANT_COUNT, devicePtr);
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
	
	inputPtr->Init(windowInfo); //ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩ ±ÔøΩ»≠

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

	player_AKI_Body_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_Body_asset.Init("../Resources/AKI_Body.txt", ObjectType::AnimationObjects);
	player_AKI_Body_asset.Add_texture(L"..\\Resources\\Texture\\P05re_skin_u1.png");
	player_AKI_Body_asset.Make_SRV();
	player_AKI_Body_asset.CreatePSO();

	player_AKI_Astro_A_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_Astro_A_asset.Init("../Resources/AKI_Astro_A.txt", ObjectType::AnimationObjects);
	player_AKI_Astro_A_asset.Add_texture(L"..\\Resources\\Texture\\P05_Astro_A_u1.png");
	player_AKI_Astro_A_asset.Make_SRV();
	player_AKI_Astro_A_asset.CreatePSO();

	player_AKI_Hair_A_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_Hair_A_asset.Init("../Resources/AKI_Hair_A.txt", ObjectType::AnimationObjects);
	player_AKI_Hair_A_asset.Add_texture(L"..\\Resources\\Texture\\P05re_hair_A_S0.png");
	player_AKI_Hair_A_asset.Make_SRV();
	player_AKI_Hair_A_asset.CreatePSO();

	player_AKI_HeadPhone_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_HeadPhone_asset.Init("../Resources/AKI_HeadPhone.txt", ObjectType::AnimationObjects);
	player_AKI_HeadPhone_asset.Add_texture(L"..\\Resources\\Texture\\bricks.dds");
	player_AKI_HeadPhone_asset.Make_SRV();
	player_AKI_HeadPhone_asset.CreatePSO();

	player_AKI_Sword_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_Sword_asset.Init("../Resources/AKI_Katana.txt", ObjectType::AnimationObjects);
	player_AKI_Sword_asset.Add_texture(L"..\\Resources\\Texture\\Katana_Albedo.png");
	player_AKI_Sword_asset.Make_SRV();
	player_AKI_Sword_asset.CreatePSO();

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
		playerArr[i]._final_transforms.resize(player_AKI_Body_asset._animationPtr->mBoneHierarchy.size());
		playerArr[i]._weapon_final_transforms.resize(player_AKI_Sword_asset._animationPtr->mBoneHierarchy.size());
	}
	
	for (int i = 0; i < NPCMAX; ++i) {
		if (i == 9) {
			npcArr[i]._final_transforms.resize(boss._animationPtr->mBoneHierarchy.size());
			npcArr[i]._transform.y += 1.f;
		}
		else {
			npcArr[i]._final_transforms.resize(npc_asset._animationPtr->mBoneHierarchy.size());
			npcArr[i]._transform.y += 0.2f;
		}
	}

	d11Ptr->LoadPipeline();
	d11Ptr->addResource(L"..\\Resources\\UserInterface\\test.png");

	ID2D1Bitmap* _i_tmp;
	_i_tmp = d11Ptr->addResource(L"..\\Resources\\UserInterface\\test.png");
	D2D1_RECT_F _tmp = D2D1::RectF(100.0f, 0.0f, 200.f, 100.f);
	_test_ui_vector.emplace_back(_i_tmp,_tmp);

	cout << "complite late init" << endl;
}

void DxEngine::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
	networkPtr->ReceiveServer(playerArr, npcArr, cubeArr);

	for (int i = 0; i < PLAYERMAX; ++i) {
		if (playerArr[i]._on == true) {
			if (playerArr[i]._animation_state == 0 || playerArr[i]._animation_state == 1) {
				playerArr[i]._can_attack = true;
				playerArr[i]._can_attack2 = true;
			}
		}
	}
	for (int i = 0; i < NPCMAX; ++i) {
		if (npcArr[i]._on == true) {
			if (npcArr[i]._animation_state == 0 || npcArr[i]._animation_state == 1) {
				npcArr[i]._can_attack = true;
			}
		}
	}

	if (isActive)
	{
		inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr);
		inputPtr->inputMouse(playerArr, networkPtr);
	}
	
	// ∆ƒ∆º≈¨ µø±‚»≠
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true) {
			for (int j = 0; j < NPCMAX; ++j)
			{
				if (npcArr[j]._on == true) {
					if (pow(playerArr[i]._transform.x - npcArr[j]._transform.x, 2) + pow(playerArr[i]._transform.z - npcArr[j]._transform.z, 2) <= 9.f) {
						if ((playerArr[i]._animation_state == 2 || playerArr[i]._animation_state == 3)
							&& playerArr[i]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[i]._animation_state) * 0.5f
							&& playerArr[i]._can_attack2) {
							playerArr[i]._can_attack2 = false;
							npcArr[j]._particle_count += 50;
						}
					}
				}
			}
		}
	}

	// ∞¯∞› √Êµπ √º≈©
	for (int j = 0; j < NPCMAX; ++j)
	{
		int i = networkPtr->myClientId;
		if (npcArr[j]._on == true) {
			if (pow(playerArr[i]._transform.x - npcArr[j]._transform.x, 2) + pow(playerArr[i]._transform.z - npcArr[j]._transform.z, 2) <= 9.f) {
				if ((playerArr[i]._animation_state == 2 || playerArr[i]._animation_state == 3)
					&& playerArr[i]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[i]._animation_state) * 0.5f
					&& playerArr[i]._can_attack) {

					playerArr[i]._can_attack = false;

					CS_COLLISION_PACKET p;
					p.size = sizeof(p);
					p.type = CS_COLLISION;
					p.attack_type = 'a';
					p.attacker_id = playerArr[i]._my_server_id;
					p.target_id = npcArr[j]._my_server_id;
					networkPtr->send_packet(&p);

					cout << "player" << i << " hp : " << playerArr[i]._hp << endl;
					cout << "npc" << j << " hp : " << npcArr[j]._hp << endl;
					cout << "particle " << j << " : " << npcArr[j]._particle_count << endl;
				}
				if (npcArr[j]._animation_state == 2
					&& npcArr[j]._animation_time_pos >= npc_asset._animationPtr->GetClipEndTime(npcArr[j]._animation_state) * 0.5f
					&& npcArr[j]._can_attack) {

					npcArr[j]._can_attack = false;

					CS_COLLISION_PACKET p;
					p.size = sizeof(p);
					p.type = CS_COLLISION;
					p.attack_type = 'a';
					p.attacker_id = npcArr[j]._my_server_id;
					p.target_id = playerArr[i]._my_server_id;
					networkPtr->send_packet(&p);

					cout << "player" << i << " hp : " << playerArr[i]._hp << endl;
					cout << "npc" << j << " hp : " << npcArr[j]._hp << endl;
				}
			}
		}
	}
		

	
	if (playerArr[networkPtr->myClientId]._hp <= 0.f) {
		playerArr[networkPtr->myClientId]._animation_state = 4;

		CS_MOVE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.x = playerArr[networkPtr->myClientId]._transform.x;
		p.y = playerArr[networkPtr->myClientId]._transform.y;
		p.z = playerArr[networkPtr->myClientId]._transform.z;
		p.degree = playerArr[networkPtr->myClientId]._degree;
		p.char_state = playerArr[networkPtr->myClientId]._animation_state;
		networkPtr->send_packet(&p);
	}
	

	//// npc ?Ç¨Îß?
	//for (int i = 0; i < NPCMAX; ++i) {
	//	if (npcArr[i]._on == true && npcArr[i]._hp <= 0.f) {
	//		npcArr[i]._animation_state = 3;
	//		//npcArr[i]._on = false;

	//		CS_MOVE_PACKET p;
	//		p.size = sizeof(p);
	//		p.type = CS_MOVE;
	//		p.x = npcArr[networkPtr->myClientId]._transform.x;
	//		p.y = npcArr[networkPtr->myClientId]._transform.y;
	//		p.z = npcArr[networkPtr->myClientId]._transform.z;
	//		p.degree = npcArr[networkPtr->myClientId]._degree;
	//		p.char_state = npcArr[i]._animation_state;
	//		//networkPtr->send_packet(&p);
	//	}
	//}
	//

	//VP ÔøΩÔøΩ»Ø
	cameraPtr->pos = XMVectorSet(playerArr[networkPtr->myClientId]._transform.x - 7 * cosf(inputPtr->angle.x*XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId]._transform.y + 4 + 7 * cosf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId]._transform.z - 7 * sinf(inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f), 0.0f);
	XMVECTOR target = XMVectorSet(playerArr[networkPtr->myClientId]._transform.x, playerArr[networkPtr->myClientId]._transform.y + 1.65f,
		playerArr[networkPtr->myClientId]._transform.z,
		playerArr[networkPtr->myClientId]._transform.w);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(cameraPtr->pos, target, up); //ÔøΩÔøΩ ÔøΩÔøΩ»Ø ÔøΩÔøΩÔø?
	XMStoreFloat4x4(&_transform.view, XMMatrixTranspose(view));

	XMMATRIX proj = XMLoadFloat4x4(&cameraPtr->mProj); //ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø ÔøΩÔøΩÔø?
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

	//ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩ
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true) {
			player_AKI_Body_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i], 0);
			player_AKI_Sword_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i], 1);
		}
	}
	if (npcArr[9]._on == true) {
		boss.UpdateSkinnedAnimation(timerPtr->_deltaTime, npcArr[9], 0);
	}
	for (int i = 0; i < NPCMAX; ++i) {
		if (npcArr[i]._on == true && i != 9) {
			npc_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, npcArr[i], 0);
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

	cmdList->SetPipelineState(player_AKI_Body_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_Body_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_Body_asset._indexBufferView);

	//ÔøΩÔøΩÔøΩÔøΩ
	for (int i = 0; i < PLAYERMAX; ++i) //ÔøΩ√∑ÔøΩÔøΩÃæÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	{
		if (playerArr[i]._on == true)
		{
			//ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ : ÔøΩœπÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩ÷∂ÔøΩ ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩÔøΩÃ∂ÔøΩ ÔøΩﬁ∂ÔøΩ ÔøΩ◊∑ÔøΩ
			//ÔøΩÔøΩÔøΩ¬∞ÔøΩ ÔøΩŸ≤ÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ∆Æ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ∆ÆÔøΩÔøΩ ÔøΩ“∑ÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ÷¥¬∞ÔøΩ
			//ÔøΩÃ∞ÔøΩ ÔøΩÔøΩƒ• ÔøΩÔøΩ ÔøΩ÷¥ÔøΩ ÔøΩ€æÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ?			
			{
				//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				// ÔøΩÔøΩ≈∞ÔøΩÔøΩ ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩ ÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				//ÔøΩÔøΩÔøΩÔøΩ
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_Body_asset._tex._srvHandle = player_AKI_Body_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_Body_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_Body_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	//
	cmdList->SetPipelineState(player_AKI_Astro_A_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_Astro_A_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_Astro_A_asset._indexBufferView);
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{	
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_Astro_A_asset._tex._srvHandle = player_AKI_Astro_A_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_Astro_A_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_Astro_A_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	// 
	cmdList->SetPipelineState(player_AKI_Hair_A_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_Hair_A_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_Hair_A_asset._indexBufferView);
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_Hair_A_asset._tex._srvHandle = player_AKI_Hair_A_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_Hair_A_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_Hair_A_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	// 
	cmdList->SetPipelineState(player_AKI_HeadPhone_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_HeadPhone_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_HeadPhone_asset._indexBufferView);
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_HeadPhone_asset._tex._srvHandle = player_AKI_HeadPhone_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_HeadPhone_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_HeadPhone_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	// 
	cmdList->SetPipelineState(player_AKI_Sword_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_Sword_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_Sword_asset._indexBufferView);
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				copy(begin(playerArr[i]._weapon_final_transforms), end(playerArr[i]._weapon_final_transforms), &_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_Sword_asset._tex._srvHandle = player_AKI_Sword_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_Sword_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_Sword_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	

	// Î≥¥Ïä§
	cmdList->SetPipelineState(boss._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &boss._vertexBufferView);
	cmdList->IASetIndexBuffer(&boss._indexBufferView);
	if (npcArr[9]._on == true)
	{
		{
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(600.f, 600.f, 600.f) * XMMatrixRotationX(-XM_PI / 2.f)
				* XMMatrixRotationY(npcArr[9]._degree * XM_PI / 180.f - XM_PI)
				* XMMatrixTranslation(npcArr[9]._transform.x, npcArr[9]._transform.y, npcArr[9]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
			XMStoreFloat4x4(&_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

			copy(begin(npcArr[9]._final_transforms), end(npcArr[9]._final_transforms), &_transform.BoneTransforms[0]);

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
	for (int i = 0; i < NPCMAX; i++) //npc ÔøΩÔøΩÔøΩÔøΩ
	{
		if (npcArr[i]._on == true && i != 9)
		{			
			{
				//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(200.f, 200.f, 200.f) * XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(npcArr[i]._degree * XM_PI / 180.f -  XM_PI / 2.f)
					* XMMatrixTranslation(npcArr[i]._transform.x, npcArr[i]._transform.y, npcArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
				XMStoreFloat4x4(&_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

				// ÔøΩÔøΩ≈∞ÔøΩÔøΩ ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩ ÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
				copy(begin(npcArr[i]._final_transforms), end(npcArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				//ÔøΩÔøΩÔøΩÔøΩ
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
	for (int i = 0; i < NPCMAX; i++)
	{
		if (npcArr[i]._on == true && i != 9) {
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
	for (int i = 0; i < CubeMax; i++) //ÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩ
	{
		if (cubeArr[i]._on == true)
		{
			//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 2.0f, 1.0f) * XMMatrixTranslation(cubeArr[i]._transform.x, cubeArr[i]._transform.y + 2.0f, cubeArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			//ÔøΩÔøΩÔøΩÔøΩ
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

	// ÔøΩÔøΩƒ´ÔøΩÔøΩ ÔøΩ⁄ΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	{
		cmdList->SetPipelineState(skybox._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &skybox._vertexBufferView);
		cmdList->IASetIndexBuffer(&skybox._indexBufferView);
		//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
		XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.f, 2.f, 0.f));
		XMMATRIX world = XMLoadFloat4x4(&_transform.world);
		XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
		_transform.camera_pos = cameraPtr->pos;
		//ÔøΩÔøΩÔøΩÔøΩ
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);

			skybox._tex._srvHandle = skybox._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

			descHeapPtr->CopyDescriptor(skybox._tex._srvHandle, 5, devicePtr);
		}

		descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
		cmdList->DrawIndexedInstanced(skybox._indexCount, 1, 0, 0, 0);
	}

	// ?åå?ã∞?Å¥ ?ãú?ä§?Öú
	int index = 0;
	for (int i = 0; i < NPCMAX; ++i)
	{
		if (npcArr[i]._on == true && i != 9) {
			while (npcArr[i]._particle_count > 0)
			{
				if (index >= PARTICLE_NUM) { // ∆ƒ∆º≈¨ ∞≥ºˆø° ¥Î«— øπø‹√≥∏Æ
					break;
				}

				if (particles[index].alive == 0) // ?åå?ã∞?Å¥ Ï¥àÍ∏∞?ôî
				{
					particles[index].lifeTime = (float)(rand() % 101) / 1000.f + 0.3f; // 0.3~0.4
					particles[index].curTime = 0.0f;
					particles[index].pos = XMVectorSet(npcArr[i]._transform.x, npcArr[i]._transform.y + 0.3f, npcArr[i]._transform.z, 1.f);
					particles[index].moveSpeed = (float)(rand() % 101) / 50 + 2.f; // 2~4
					particles[index].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
					XMVector3Normalize(particles[index].dir);
					particles[index].velocity = XMVectorSet(particles[index].dir.m128_f32[0] * particles[index].moveSpeed,
						particles[index].dir.m128_f32[1] * particles[index].moveSpeed, particles[index].dir.m128_f32[2] * particles[index].moveSpeed, 1.f);
					particles[index].alive = 1;
					--npcArr[i]._particle_count;
				}
				else {
					++index;
				}
			}
		}
		else if(npcArr[i]._on == true && i == 9 && index <= PARTICLE_NUM) {
			while (npcArr[i]._particle_count > 0)
			{
				if (index >= PARTICLE_NUM) { // ∆ƒ∆º≈¨ ∞≥ºˆø° ¥Î«— øπø‹√≥∏Æ
					break;
				}

				if (particles[index].alive == 0) // ?åå?ã∞?Å¥ Ï¥àÍ∏∞?ôî
				{
					particles[index].lifeTime = (float)(rand() % 101) / 1000.f + 0.3f; // 0.3~0.4
					particles[index].curTime = 0.0f;
					particles[index].pos = XMVectorSet(npcArr[i]._transform.x + 0.5f, npcArr[i]._transform.y + 2.f, npcArr[i]._transform.z, 1.f);
					particles[index].moveSpeed = (float)(rand() % 101) / 50 + 2.f; // 2~4
					particles[index].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
					XMVector3Normalize(particles[index].dir);
					particles[index].velocity = XMVectorSet(particles[index].dir.m128_f32[0] * particles[index].moveSpeed,
						particles[index].dir.m128_f32[1] * particles[index].moveSpeed, particles[index].dir.m128_f32[2] * particles[index].moveSpeed, 1.f);
					particles[index].alive = 1;
					--npcArr[i]._particle_count;
				}
				else {
					++index;
				}
			}
		}
	}
	for (int i = 0; i < PARTICLE_NUM; ++i) // ?åå?ã∞?Å¥ Î¨ºÎ¶¨Ï≤òÎ¶¨ Î∞? ?†å?çîÎß?
	{
		if (particles[i].alive == 1)
		{
			if (particles[i].pos.m128_f32[1] - particles[i].bounding_box_half_size.m128_f32[1] < 0.f) {
				// ÎπÑÌÉÑ?Ñ± Ï∂©Îèå
				particles[i].velocity.m128_f32[1] = particles[i].velocity.m128_f32[1] * -coefficient_of_restitution;
				particles[i].pos.m128_f32[1] = particles[i].bounding_box_half_size.m128_f32[1];
			}

			if (particles[i].velocity.m128_f32[1] <= 0.05f
				&& particles[i].pos.m128_f32[1] - particles[i].bounding_box_half_size.m128_f32[1] == 0.f) {
				// ÎßàÏ∞∞?†•
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

			particles[i].pos.m128_f32[0] = particles[i].pos.m128_f32[0] + particles[i].velocity.m128_f32[0] * timerPtr->_deltaTime; // x?Ñ±Î∂? ?ù¥?èô
			particles[i].velocity.m128_f32[1] = particles[i].velocity.m128_f32[1] - 5.f * timerPtr->_deltaTime; // Ï§ëÎ†•Í∞??Üç?èÑ?óê ?ùò?ïú ?ÇòÏ§ëÏÜç?èÑ
			particles[i].pos.m128_f32[1] = particles[i].pos.m128_f32[1] + particles[i].velocity.m128_f32[1] * timerPtr->_deltaTime; // y?Ñ±Î∂? ?ù¥?èô
			particles[i].pos.m128_f32[2] = particles[i].pos.m128_f32[2] + particles[i].velocity.m128_f32[2] * timerPtr->_deltaTime; // z?Ñ±Î∂? ?ù¥?èô
			XMStoreFloat4x4(&_transform.world, XMMatrixRotationX(-atan2f(cameraPtr->pos.m128_f32[1] - particles[i].pos.m128_f32[1], sqrt(pow(cameraPtr->pos.m128_f32[0] - particles[i].pos.m128_f32[0], 2) + pow(cameraPtr->pos.m128_f32[2] - particles[i].pos.m128_f32[2], 2))))
				* XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - particles[i].pos.m128_f32[0], cameraPtr->pos.m128_f32[2] - particles[i].pos.m128_f32[2]))
				* XMMatrixTranslation(particles[i].pos.m128_f32[0], particles[i].pos.m128_f32[1], particles[i].pos.m128_f32[2])); // ?åå?ã∞?Å¥?ù¥ ?ï≠?ÉÅ Ïπ¥Î©î?ùºÎ•? Î∞îÎùºÎ≥¥Í∏∞
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
	

	

	::WaitForSingleObject(_excuteEvent, INFINITE);
	SetEvent(_renderEvent);

	

	//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // »≠ÔøΩÔøΩ ÔøΩÔøΩÔø?

	cmdList->ResourceBarrier(1, &barrier2);
	cmdList->Close();

	

	ID3D12CommandList* cmdListArr[] = { cmdList.Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	d11Ptr->RenderUI(i_now_render_index);
	d11Ptr->LateRenderUI(_test_ui_vector);

	D2D1_RECT_F _tmp = D2D1::RectF(0, 620.0f, 1280.f, 720.f);
	WCHAR text[128];
	wsprintf(text, L"HP : %d", playerArr[0]._hp);
	d11Ptr->draw_text(text, _tmp);
	_tmp.left = 1100;
	_tmp.right = 1280;
	for (int i = 1; i < PLAYERMAX; ++i) {
		if (playerArr[i]._on == true)
		{
			wsprintf(text, L"HP : %d", playerArr[i]._hp);
		}
		else {
			wsprintf(text, L"WAIT PLAYER");
		}
		_tmp.bottom = 720 / 2 - 50 + 25 * i;
		_tmp.top = _tmp.bottom - 25;
		d11Ptr->draw_text(text, _tmp);
	}


	d11Ptr->ExcuteUI(i_now_render_index);
	swapChainPtr->_swapChain->Present(0, 0);
	
	cmdQueuePtr->WaitSync();

	swapChainPtr->_backBufferIndex = (swapChainPtr->_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;

	SetEvent(_excuteEvent);
}

void DxEngine::Make_Scene()
{	
	// arrScene[SceneTag::Title] = new TitleScene();	// ÔøΩÃ∑ÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ.
	//ÔøΩÃ∑ÔøΩÔøΩÔøΩ ÔøΩÔøΩÔø? ÔøΩﬂ∞ÔøΩÔøΩ’¥œ¥ÔøΩ.
	arrScene[SCENE::SceneTag::test_scene] = new TestScene(SCENE::SceneTag::test_scene, this);
}

void DxEngine::ChangeScene(SCENE::SceneTag tag)
{
	m_pCurrScene = arrScene[tag];
}
