#include "DxEngine.h"
#include "Device.h"

void DxEngine::Init(WindowInfo windowInfo)
{
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
	constantBufferPtr->CreateConstantBuffer(sizeof(Constants), 256, devicePtr);
	constantBufferPtr->CreateView(devicePtr);
	descHeapPtr->CreateDescTable(256, devicePtr);
	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	timerPtr->InitTimer(windowInfo);
	RECT rect = { 0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(windowInfo.hwnd, 0, 100, 100, windowInfo.ClientWidth, windowInfo.ClientHeight, 0);
	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);

	srand((unsigned int)time(NULL));
}

void DxEngine::Draw()
{
	//Ÿ�̸�
	timerPtr->TimerUpdate();
	timerPtr->ShowFps();

	//Ű �Է�
	inputPtr->InputKey(timerPtr);

	//VP ��ȯ
	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up); //�� ��ȯ ���
	XMStoreFloat4x4(&cameraPtr->mView, view);
	XMMATRIX proj = XMLoadFloat4x4(&cameraPtr->mProj); //���� ��ȯ ���
	XMMATRIX ViewProj = view * proj;
	XMStoreFloat4x4(&vertexBufferPtr->_transform.ViewProj, XMMatrixTranspose(ViewProj));

	//���� ����
	cmdQueuePtr->_cmdAlloc->Reset();
	cmdQueuePtr->_cmdList->Reset(cmdQueuePtr->_cmdAlloc.Get(), nullptr);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[swapChainPtr->_backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmdQueuePtr->_cmdList->SetGraphicsRootSignature(rootSignaturePtr->_signature.Get());
	constantBufferPtr->_currentIndex = 0;
	descHeapPtr->_currentGroupIndex = 0;

	ID3D12DescriptorHeap* descHeap = descHeapPtr->_descHeap.Get();
	cmdQueuePtr->_cmdList->SetDescriptorHeaps(1, &descHeap);

	cmdQueuePtr->_cmdList->ResourceBarrier(1, &barrier);

	cmdQueuePtr->_cmdList->RSSetViewports(1, &_viewport);
	cmdQueuePtr->_cmdList->RSSetScissorRects(1, &_scissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = rtvPtr->_rtvHandle[swapChainPtr->_backBufferIndex];
	cmdQueuePtr->_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = dsvPtr->_dsvHandle;
	cmdQueuePtr->_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);

	cmdQueuePtr->_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//����
	if (timerPtr->isGS == 1)
	{
		cmdQueuePtr->_cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		cmdQueuePtr->_cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_vertexBufferView[1]);
		cmdQueuePtr->_cmdList->IASetIndexBuffer(&indexBufferPtr->_indexBufferView[1]);
		cmdQueuePtr->_cmdList->SetPipelineState(psoPtr->_gsPipelineState.Get());
	}
	else
	{
		cmdQueuePtr->_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdQueuePtr->_cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_vertexBufferView[0]);
		cmdQueuePtr->_cmdList->IASetIndexBuffer(&indexBufferPtr->_indexBufferView[0]);
		cmdQueuePtr->_cmdList->SetPipelineState(psoPtr->_pipelineState.Get());
	}

	for (int i = 0; i < PARTICLE_NUM; i++)
	{
		//��ƼŬ ���� �̵�
		if (particle[i].alive == 0)
		{
			particle[i].lifeTime = (float)(rand() % 101) / 100 + 1; //1~2
			particle[i].curTime = 0.0f;
			particle[i].pos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			particle[i].moveSpeed = (float)(rand() % 101) / 100 + 1; //1~2
			particle[i].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
			XMVector3Normalize(particle[i].dir);
			particle[i].alive = 1;
		}
		if (particle[i].alive == 1)
		{
			particle[i].pos = XMVectorAdd(particle[i].pos, particle[i].dir * particle[i].moveSpeed * timerPtr->_deltaTime);
			XMStoreFloat4x4(&cameraPtr->mWorld, XMMatrixScaling(0.5f, 0.5f, 1.f) * XMMatrixTranslation(particle[i].pos.m128_f32[0], particle[i].pos.m128_f32[1], particle[i].pos.m128_f32[2]));
			XMMATRIX world = XMLoadFloat4x4(&cameraPtr->mWorld); //���� ��ȯ ���
			XMMATRIX worldViewProj = world * view * proj;
			XMStoreFloat4x4(&vertexBufferPtr->_transform.worldViewProj, XMMatrixTranspose(worldViewProj));
			particle[i].curTime += 0.001f;
			if (particle[i].lifeTime < particle[i].curTime)
			{
				particle[i].alive = 0;
			}
		}

		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &vertexBufferPtr->_transform, sizeof(vertexBufferPtr->_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);

			descHeapPtr->CommitTable(cmdQueuePtr);
			if (timerPtr->isGS == 1)
				cmdQueuePtr->_cmdList->DrawIndexedInstanced(indexBufferPtr->_indexCount[1], 1, 0, 0, 0);
			else if (timerPtr->isGS == 0)
				cmdQueuePtr->_cmdList->DrawIndexedInstanced(indexBufferPtr->_indexCount[0], 1, 0, 0, 0);
		}
	}

	//���� ����
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[swapChainPtr->_backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // ȭ�� ���

	cmdQueuePtr->_cmdList->ResourceBarrier(1, &barrier2);
	cmdQueuePtr->_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { cmdQueuePtr->_cmdList.Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	swapChainPtr->_swapChain->Present(0, 0);

	cmdQueuePtr->WaitSync();

	swapChainPtr->_backBufferIndex = (swapChainPtr->_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}