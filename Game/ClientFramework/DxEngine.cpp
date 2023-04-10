#include "DxEngine.h"
#include "Device.h"
#include "TestScene.h"

void DxEngine::Init(WindowInfo windowInfo)
{
	//통신시작
	cout << "try server connect" << endl;
	if (-1 == networkPtr->ConnectServer(GAME_PORT_NUM)) {
		cout << "SERVER CONNECT FAIL" << endl;
		while (1);
	}//데이터 보냄
	cout << "complite server connect" << endl;

	//화면 크기 설정
	_viewport = { 0, 0, static_cast<FLOAT>(windowInfo.ClientWidth), static_cast<FLOAT>(windowInfo.ClientHeight), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight);

	//DX엔진 초기화
	devicePtr->CreateDevice();
	cmdQueuePtr->CreateCmdListAndCmdQueue(devicePtr);
	swapChainPtr->DescriptAndCreateSwapChain(windowInfo, devicePtr, cmdQueuePtr);
	rtvPtr->CreateRTV(devicePtr, swapChainPtr);
	cameraPtr->TransformProjection(windowInfo); //투영 변환
	rootSignaturePtr->CreateRootSignature(devicePtr);
	constantBufferPtr->CreateConstantBuffer(sizeof(Constants), 256, devicePtr);
	constantBufferPtr->CreateView(devicePtr);
	descHeapPtr->CreateDescTable(256, devicePtr);
	
	cout << "complite Init ptr" << endl;

	timerPtr->InitTimer();
	logicTimerPtr->InitTimer();

	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	RECT rect = { 0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(windowInfo.hwnd, 0, 100, 100, windowInfo.ClientWidth, windowInfo.ClientHeight, 0);
	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	srand((unsigned int)time(NULL));
	
	inputPtr->Init(windowInfo); //벡터 사이즈 초기화
	for (int i = 0; i < PLAYERMAX; i++)
	{
		playerArr[i].transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	for (int i = 0; i < NPCMAX; i++)
	{
		npcArr[i].transform = XMFLOAT4(5.0f * (i - NPCMAX / 2), 0.0f, 20.0f, 0.0f);
	}

	_renderEvent = ::CreateEvent(nullptr, FALSE, TRUE, nullptr);
	_excuteEvent = ::CreateEvent(nullptr, FALSE, TRUE, nullptr);

	cout << "complite all init" << endl;
}

void DxEngine::late_Init(WindowInfo windowInfo)
{
	cube_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr,cmdQueuePtr,rootSignaturePtr,dsvPtr);
	cube_asset.Init("../Resources/Cube.txt",false);
	cube_asset.Add_texture(L"..\\Resources\\Texture\\bricks.dds");
	cube_asset.Make_SRV();
	cube_asset.CreatePSO(L"..\\Bricks.hlsl");

	player_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_asset.Init("../Resources/AnimeCharacter.txt", false);
	player_asset.Add_texture(L"..\\Resources\\Texture\\AnimeCharcter.dds");
	player_asset.Make_SRV();
	player_asset.CreatePSO();

	npc_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	npc_asset.Init("../Resources/OrangeSpider.txt", true);
	npc_asset.Add_texture(L"..\\Resources\\Texture\\NPCSpider_DefaultMaterial_AlbedoTransparency.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_paint_black_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Make_SRV();
	npc_asset.CreatePSO();

	npc_asset._animationPtr->state = 1;
	npc_asset._animationPtr->state0 = 1;



	cout << "complite late init" << endl;
}

void DxEngine::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
	networkPtr->ReceiveServer(playerArr, npcArr, cubeArr);

	if (isActive)
	{
		inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, animationPtr[0].state);
		inputPtr->inputMouse(playerArr, networkPtr);
	}

	//VP 변환
	cameraPtr->pos = XMVectorSet(playerArr[networkPtr->myClientId].transform.x - 7 * cosf(inputPtr->angle.x*XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId].transform.y + 4 + 7 * cosf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId].transform.z - 7 * sinf(inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f), 0.0f);
	XMVECTOR target = XMVectorSet(playerArr[networkPtr->myClientId].transform.x, playerArr[networkPtr->myClientId].transform.y + 1.65f,
		playerArr[networkPtr->myClientId].transform.z,
		playerArr[networkPtr->myClientId].transform.w);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(cameraPtr->pos, target, up); //뷰 변환 행렬
	XMStoreFloat4x4(&_transform.view, XMMatrixTranspose(view));

	XMMATRIX proj = XMLoadFloat4x4(&cameraPtr->mProj); //투영 변환 행렬
	XMStoreFloat4x4(&_transform.proj, XMMatrixTranspose(proj));

	//Light
	LightInfo lightInfo;
	_transform.lnghtInfo = lightInfo;
}

void DxEngine::Update(WindowInfo windowInfo, bool isActive)
{


}



void DxEngine::Draw_multi(WindowInfo windowInfo,int i_now_render_index)
{
	::WaitForSingleObject(_renderEvent, INFINITE);
	ComPtr<ID3D12CommandAllocator>		cmdAlloc = cmdQueuePtr->_arr_cmdAlloc[i_now_render_index];
	ComPtr<ID3D12GraphicsCommandList>	cmdList = cmdQueuePtr->_arr_cmdList[i_now_render_index];

	//애니메이션
	animationPtr[0].UpdateSkinnedAnimation(timerPtr->_deltaTime);
	animationPtr[1].UpdateSkinnedAnimation(timerPtr->_deltaTime);
	npc_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime);

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

	cmdList->SetPipelineState(psoPtr->_pipelineState.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->SetGraphicsRootSignature(rootSignaturePtr->_signature.Get());
	constantBufferPtr->_currentIndex = 0;
	descHeapPtr->_currentGroupIndex = 0;

	ID3D12DescriptorHeap* descHeap = descHeapPtr->_descHeap.Get();
	cmdList->SetDescriptorHeaps(1, &descHeap);

	cmdList->SetPipelineState(psoPtr->_animationPipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_playerVertexBufferView);
	cmdList->IASetIndexBuffer(&indexBufferPtr->_playerIndexBufferView);
	//렌더
	for (int i = 0; i < PLAYERMAX; i++) //플레이어 렌더
	{
		if (playerArr[i].on == true)
		{
			//쓰는이유 : 일반적인 애랑 애니메이션이랑 달라서 그럼
			//상태가 바껴서 파이프라인 스테이트 오브젝트를 불러서 갱신해주는것
			//이거 합칠 수 있는 작업 있지 않을까?			
			{
				//월드 변환
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 1.0f, 1.0f)  * XMMatrixRotationY(playerArr[i].degree * XM_PI / 180.f) * XMMatrixTranslation(playerArr[i].transform.x, playerArr[i].transform.y, playerArr[i].transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				// 스키닝 애니메이션 행렬 데이터 복사
				copy(begin(animationPtr[0].FinalTransforms), end(animationPtr[0].FinalTransforms), &_transform.BoneTransforms[0]);

				//렌더
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					D3D12_CPU_DESCRIPTOR_HANDLE handle2 = constantBufferPtr->PushData(1, &playerArr[networkPtr->myClientId].transform, sizeof(playerArr[networkPtr->myClientId].transform));
					descHeapPtr->CopyDescriptor(handle2, 1, devicePtr);
					texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(indexBufferPtr->_playerIndexCount, 1, 0, 0, 0);
			}
		}
	}
	/*cmdList->SetPipelineState(psoPtr->_animationPipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_npcVertexBufferView);
	cmdList->IASetIndexBuffer(&indexBufferPtr->_npcIndexBufferView);*/

	cmdList->SetPipelineState(npc_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &npc_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&npc_asset._indexBufferView);
	for (int i = 0; i < NPCMAX; i++) //npc 렌더
	{
		if (npcArr[i].on == true)
		{			
			{
				//월드 변환
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(200.f, 200.f, 200.f) * XMMatrixRotationX(-XM_PI / 2.f) * XMMatrixRotationY(npcArr[i].degree * XM_PI / 180.f) * XMMatrixTranslation(npcArr[i].transform.x, npcArr[i].transform.y + 0.2f, npcArr[i].transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
				XMStoreFloat4x4(&_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

				// 스키닝 애니메이션 행렬 데이터 복사
				//copy(begin(cmdList.FinalTransforms), end(animationPtr[1].FinalTransforms), &_transform.BoneTransforms[0]);
				copy(begin(npc_asset._animationPtr->FinalTransforms), end(npc_asset._animationPtr->FinalTransforms), &_transform.BoneTransforms[0]);


				//렌더
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				
				int sum = 0;
				for (Subset i : npc_asset._animationPtr->mSubsets)
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					D3D12_CPU_DESCRIPTOR_HANDLE handle2 = constantBufferPtr->PushData(1, &playerArr[networkPtr->myClientId].transform, sizeof(playerArr[networkPtr->myClientId].transform));
					descHeapPtr->CopyDescriptor(handle2, 1, devicePtr);
					texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
					descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
					cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
					sum += i.FaceCount * 3;
				}

				/*int sum = 0;
				for (UINT i : fbxLoaderPtr->submeshCount[1])
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					D3D12_CPU_DESCRIPTOR_HANDLE handle2 = constantBufferPtr->PushData(1, &playerArr[networkPtr->myClientId].transform, sizeof(playerArr[networkPtr->myClientId].transform));
					descHeapPtr->CopyDescriptor(handle2, 1, devicePtr);
					texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
					descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
					cmdList->DrawIndexedInstanced(i, 1, sum, 0, 0);
					sum += i;
				}*/
				
			}
		}
	}
	cmdList->SetPipelineState(cube_asset._pipelineState.Get());	
	cmdList->IASetVertexBuffers(0, 1, &cube_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&cube_asset._indexBufferView);
	for (int i = 0; i < CubeMax; i++) //기둥 렌더
	{
		if (cubeArr[i].on == true)
		{
			
			

			//월드 변환
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 2.0f, 1.0f) * XMMatrixTranslation(cubeArr[i].transform.x, cubeArr[i].transform.y + 2.0f, cubeArr[i].transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			//렌더
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
	//파티클
	if (pow(playerArr[0].transform.x - npcArr[9].transform.x, 2) + pow(playerArr[0].transform.z - npcArr[9].transform.z, 2) <= 4.f) //충돌 처리
	{
		if (playerArr[0].isCollision == false)
			playerArr[0].isFirstCollision = true;
		else
			playerArr[0].isFirstCollision = false;
		playerArr[0].isCollision = true;
	}
	if (pow(playerArr[0].transform.x - npcArr[9].transform.x, 2) + pow(playerArr[0].transform.z - npcArr[9].transform.z, 2) > 4.f)
	{
		playerArr[0].isCollision = false;
	}
	for (int i = 0; i < 100; i++) //파티클 렌더
	{
		if (playerArr[0].isFirstCollision == true && particle[i].alive == 0)
		{
			particle[i].lifeTime = (float)(rand() % 101) / 1000 + 0.1f; //0.1~0.2
			particle[i].curTime = 0.0f;
			particle[i].pos = XMVectorSet(npcArr[9].transform.x, npcArr[9].transform.y, npcArr[9].transform.z, 1.f);
			particle[i].moveSpeed = (float)(rand() % 101) / 50 + 2.f; //2~4
			particle[i].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
			XMVector3Normalize(particle[i].dir);
			particle[i].alive = 1;
		}
		else if (particle[i].alive == 1)
		{
			//월드 변환
			particle[i].pos = XMVectorAdd(particle[i].pos, particle[i].dir * particle[i].moveSpeed * timerPtr->_deltaTime);
			XMStoreFloat4x4(&_transform.world, XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - particle[i].pos.m128_f32[0], cameraPtr->pos.m128_f32[2] - particle[i].pos.m128_f32[2])) * XMMatrixTranslation(particle[i].pos.m128_f32[0], particle[i].pos.m128_f32[1], particle[i].pos.m128_f32[2]));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world); //월드 변환 행렬
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
			particle[i].curTime += 0.001;
			if (particle[i].lifeTime < particle[i].curTime)
			{
				particle[i].alive = 0;
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
	
	//렌더 종료
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	cmdList->ResourceBarrier(1, &barrier2);
	cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { cmdList.Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);


	swapChainPtr->_swapChain->Present(0, 0);

	cmdQueuePtr->WaitSync();

	swapChainPtr->_backBufferIndex = (swapChainPtr->_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;

	SetEvent(_excuteEvent);
}

void DxEngine::Make_Scene()
{	
	// arrScene[SceneTag::Title] = new TitleScene();	// 이런 방식으로 씬을 만들어라.
	//이렇게 계속 추가합니다.
	arrScene[SCENE::SceneTag::test_scene] = new TestScene(SCENE::SceneTag::test_scene, this);
}

void DxEngine::ChangeScene(SCENE::SceneTag tag)
{
	m_pCurrScene = arrScene[tag];
}
