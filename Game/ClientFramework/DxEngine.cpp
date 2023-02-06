#include "DxEngine.h"
#include "Device.h"


void DxEngine::Init(WindowInfo windowInfo)
{
	//통신시작
	networkPtr->ConnectServer(); //데이터 보냄

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
}

void DxEngine::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
	networkPtr->ReceiveServer(playerArr, npcArr,cubeArr);
	
	if (isActive)
	{
		inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr);
		inputPtr->inputMouse(playerArr, networkPtr);
	}

	//VP 변환
	cameraPtr->pos = XMVectorSet(playerArr[networkPtr->myClientId].transform.x - 7 * cosf(inputPtr->angle.x*XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f), playerArr[networkPtr->myClientId].transform.y + 4 + 7 * cosf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f), playerArr[networkPtr->myClientId].transform.z - 7 * sinf(inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f), 0.0f);
	XMVECTOR target = XMVectorSet(playerArr[networkPtr->myClientId].transform.x, playerArr[networkPtr->myClientId].transform.y, playerArr[networkPtr->myClientId].transform.z, playerArr[networkPtr->myClientId].transform.w);
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

#pragma region Draw
/*
void DxEngine::Draw(WindowInfo windowInfo)
{
	//렌더 시작
	cmdQueuePtr->_cmdAlloc->Reset();
	cmdQueuePtr->_cmdList->Reset(cmdQueuePtr->_cmdAlloc.Get(), nullptr);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[swapChainPtr->_backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmdQueuePtr->_cmdList->ResourceBarrier(1, &barrier);

	cmdQueuePtr->_cmdList->RSSetViewports(1, &_viewport);
	cmdQueuePtr->_cmdList->RSSetScissorRects(1, &_scissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = rtvPtr->_rtvHandle[swapChainPtr->_backBufferIndex];
	cmdQueuePtr->_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = dsvPtr->_dsvHandle;
	cmdQueuePtr->_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);

	cmdQueuePtr->_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdQueuePtr->_cmdList->SetPipelineState(psoPtr->_pipelineState.Get());
	cmdQueuePtr->_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdQueuePtr->_cmdList->SetGraphicsRootSignature(rootSignaturePtr->_signature.Get());
	constantBufferPtr->_currentIndex = 0;
	descHeapPtr->_currentGroupIndex = 0;

	ID3D12DescriptorHeap* descHeap = descHeapPtr->_descHeap.Get();
	cmdQueuePtr->_cmdList->SetDescriptorHeaps(1, &descHeap);

	//렌더
	for (int i = 0; i < PLAYERMAX; i++) //플레이어 렌더
	{
		if (playerArr[i].on == true)
		{
			cmdQueuePtr->_cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_playerVertexBufferView);
			cmdQueuePtr->_cmdList->IASetIndexBuffer(&indexBufferPtr->_playerIndexBufferView);

			{
				//월드 변환
				XMStoreFloat4x4(&_transform.world,
					XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationY(playerArr[i].degree * XM_PI / 180.f) * XMMatrixTranslation(playerArr[i].transform.x, playerArr[i].transform.y, playerArr[i].transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				//렌더
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable(cmdQueuePtr);
				cmdQueuePtr->_cmdList->DrawIndexedInstanced(indexBufferPtr->_playerIndexCount, 1, 0, 0, 0);
			}
		}
	}

	for (int i = 0; i < NPCMAX; i++) //npc 렌더
	{
		if (npcArr[i].on == true)
		{
			cmdQueuePtr->_cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_npcVertexBufferView);
			cmdQueuePtr->_cmdList->IASetIndexBuffer(&indexBufferPtr->_npcIndexBufferView);

			{
				//월드 변환
				XMStoreFloat4x4(&_transform.world,
					XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(npcArr[i].degree * XM_PI / 180.f) * XMMatrixTranslation(npcArr[i].transform.x, npcArr[i].transform.y, npcArr[i].transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				//렌더
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
					texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable(cmdQueuePtr);
				cmdQueuePtr->_cmdList->DrawIndexedInstanced(indexBufferPtr->_npcIndexCount, 1, 0, 0, 0);
			}
		}
	}
	for (int i = 0; i < CubeMax; i++) //기둥 렌더
	{
		if (cubeArr[i].on == true)
		{
			cmdQueuePtr->_cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_vertexBufferView);
			cmdQueuePtr->_cmdList->IASetIndexBuffer(&indexBufferPtr->_indexBufferView);

			//월드 변환
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 2.0f, 1.0f) * XMMatrixTranslation(cubeArr[i].transform.x, cubeArr[i].transform.y + 2.f, cubeArr[i].transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			//렌더
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable(cmdQueuePtr);
			cmdQueuePtr->_cmdList->DrawIndexedInstanced(indexBufferPtr->_indexCount, 1, 0, 0, 0);
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
			particle[i].pos = XMVectorSet(npcArr[9].transform.x, npcArr[9].transform.y + 2.f, npcArr[9].transform.z, 1.f);
			particle[i].moveSpeed = (float)(rand() % 101) / 50 + 2.f; //2~4
			particle[i].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
			XMVector3Normalize(particle[i].dir);
			particle[i].alive = 1;
		}
		else if (particle[i].alive == 1)
		{
			//월드 변환
			particle[i].pos = XMVectorAdd(particle[i].pos, particle[i].dir * particle[i].moveSpeed * timerPtr->_deltaTime);
			XMStoreFloat4x4(&_transform.world,
				XMMatrixRotationY(
					atan2f(cameraPtr->pos.m128_f32[0] - particle[i].pos.m128_f32[0],
						cameraPtr->pos.m128_f32[2] - particle[i].pos.m128_f32[2])) * XMMatrixTranslation(particle[i].pos.m128_f32[0],
							particle[i].pos.m128_f32[1], particle[i].pos.m128_f32[2]));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world); //월드 변환 행렬
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
			particle[i].curTime += 0.001;
			if (particle[i].lifeTime < particle[i].curTime)
			{
				particle[i].alive = 0;
			}

			cmdQueuePtr->_cmdList->SetPipelineState(psoPtr->_gsPipelineState.Get());
			cmdQueuePtr->_cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			cmdQueuePtr->_cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_particleVertexBufferView);
			cmdQueuePtr->_cmdList->IASetIndexBuffer(&indexBufferPtr->_particleIndexBufferView);
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				texturePtr->_srvHandle.Offset(2, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
			}
			descHeapPtr->CommitTable(cmdQueuePtr);
			cmdQueuePtr->_cmdList->DrawIndexedInstanced(indexBufferPtr->_particleIndexCount, 1, 0, 0, 0);
		}
	}

	//렌더 종료
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[swapChainPtr->_backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	cmdQueuePtr->_cmdList->ResourceBarrier(1, &barrier2);
	cmdQueuePtr->_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { cmdQueuePtr->_cmdList.Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	swapChainPtr->_swapChain->Present(0, 0);

	cmdQueuePtr->WaitSync();

	swapChainPtr->_backBufferIndex = (swapChainPtr->_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}*/
#pragma endregion

void DxEngine::Draw_multi(WindowInfo windowInfo)
{
	::WaitForSingleObject(_renderEvent, INFINITE);
	int i_now_render_index;

	if (!_render_thread_num) {
		i_now_render_index = 0;
		_render_thread_num++;
	}
	else {
		i_now_render_index = 1;
		_render_thread_num = 0;
	}




	cmdQueuePtr->_arr_cmdAlloc[i_now_render_index]->Reset();
	cmdQueuePtr->_arr_cmdList[i_now_render_index]->Reset(cmdQueuePtr->_arr_cmdAlloc[i_now_render_index].Get(), nullptr);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmdQueuePtr->_arr_cmdList[i_now_render_index]->ResourceBarrier(1, &barrier);

	cmdQueuePtr->_arr_cmdList[i_now_render_index]->RSSetViewports(1, &_viewport);
	cmdQueuePtr->_arr_cmdList[i_now_render_index]->RSSetScissorRects(1, &_scissorRect);


	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = rtvPtr->_rtvHandle[i_now_render_index];
	
	cmdQueuePtr->_arr_cmdList[i_now_render_index]->ClearRenderTargetView(backBufferView, Colors::Lavender, 0, nullptr);
	
	//cmdQueuePtr->_arr_cmdList[i_now_render_index]->ClearRenderTargetView(backBufferView, Colors::LightGreen, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = dsvPtr->_dsvHandle;
	cmdQueuePtr->_arr_cmdList[i_now_render_index]->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);


	cmdQueuePtr->_arr_cmdList[i_now_render_index]->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdQueuePtr->_arr_cmdList[i_now_render_index]->SetPipelineState(psoPtr->_pipelineState.Get());
	cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdQueuePtr->_arr_cmdList[i_now_render_index]->SetGraphicsRootSignature(rootSignaturePtr->_signature.Get());
	constantBufferPtr->_currentIndex = 0;
	descHeapPtr->_currentGroupIndex = 0;

	ID3D12DescriptorHeap* descHeap = descHeapPtr->_descHeap.Get();
	cmdQueuePtr->_arr_cmdList[i_now_render_index]->SetDescriptorHeaps(1, &descHeap);

	//렌더
	for (int i = 0; i < PLAYERMAX; i++) //플레이어 렌더
	{
		if (playerArr[i].on == true)
		{
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetVertexBuffers(0, 1, &vertexBufferPtr->_playerVertexBufferView);
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetIndexBuffer(&indexBufferPtr->_playerIndexBufferView);

			{
				//월드 변환
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationY(playerArr[i].degree * XM_PI / 180.f) * XMMatrixTranslation(playerArr[i].transform.x, playerArr[i].transform.y, playerArr[i].transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

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
				cmdQueuePtr->_arr_cmdList[i_now_render_index]->DrawIndexedInstanced(indexBufferPtr->_playerIndexCount, 1, 0, 0, 0);
			}
		}
	}

	for (int i = 0; i < NPCMAX; i++) //npc 렌더
	{
		if (npcArr[i].on == true)
		{
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetVertexBuffers(0, 1, &vertexBufferPtr->_npcVertexBufferView);
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetIndexBuffer(&indexBufferPtr->_npcIndexBufferView);

			{
				//월드 변환
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(npcArr[i].degree * XM_PI / 180.f) * XMMatrixTranslation(npcArr[i].transform.x, npcArr[i].transform.y, npcArr[i].transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				//렌더
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
					texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdQueuePtr->_arr_cmdList[i_now_render_index]->DrawIndexedInstanced(indexBufferPtr->_npcIndexCount, 1, 0, 0, 0);
			}
		}
	}
	for (int i = 0; i < CubeMax; i++) //기둥 렌더
	{
		if (cubeArr[i].on == true)
		{
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetVertexBuffers(0, 1, &vertexBufferPtr->_vertexBufferView);
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetIndexBuffer(&indexBufferPtr->_indexBufferView);

			//월드 변환
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 2.0f, 1.0f) * XMMatrixTranslation(cubeArr[i].transform.x, cubeArr[i].transform.y + 2.f, cubeArr[i].transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			//렌더
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->DrawIndexedInstanced(indexBufferPtr->_indexCount, 1, 0, 0, 0);
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
			particle[i].pos = XMVectorSet(npcArr[9].transform.x, npcArr[9].transform.y + 2.f, npcArr[9].transform.z, 1.f);
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

			cmdQueuePtr->_arr_cmdList[i_now_render_index]->SetPipelineState(psoPtr->_gsPipelineState.Get());
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetVertexBuffers(0, 1, &vertexBufferPtr->_particleVertexBufferView);
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->IASetIndexBuffer(&indexBufferPtr->_particleIndexBufferView);
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				texturePtr->_srvHandle.Offset(2, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
			}
			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdQueuePtr->_arr_cmdList[i_now_render_index]->DrawIndexedInstanced(indexBufferPtr->_particleIndexCount, 1, 0, 0, 0);
		}
	}

	::WaitForSingleObject(_excuteEvent, INFINITE);
	SetEvent(_renderEvent);
	
	//렌더 종료
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	cmdQueuePtr->_arr_cmdList[i_now_render_index]->ResourceBarrier(1, &barrier2);
	cmdQueuePtr->_arr_cmdList[i_now_render_index]->Close();

	ID3D12CommandList* cmdListArr[] = { cmdQueuePtr->_arr_cmdList[i_now_render_index].Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	//note 23.02.03.23:56
	// Present에서 에러가 남
	// 두개의 커멘드 리스트를 쓰기 위해서 스왑체인의 인덱스를 활용하는 아이디어
	// 실행 순서를 분석해볼 필요가 있음
	//

	swapChainPtr->_swapChain->Present(0, 0);

	cmdQueuePtr->WaitSync();

	swapChainPtr->_backBufferIndex = (swapChainPtr->_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;

	SetEvent(_excuteEvent);
}