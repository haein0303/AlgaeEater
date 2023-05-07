#include "Util.h"
#include "DxEngine.h"
#include "SCENE.h"

#include "TestScene.h"



TestScene::TestScene(SceneTag tag, DxEngine* dxEngine) :SCENE(tag, dxEngine)
{


}

void TestScene::Init(WindowInfo windowInfo)
{
}

void TestScene::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
	// 플레이어와 npc 공격에 대한 충돌 처리
	for (int i = 0; i < PLAYERMAX; ++i) {
		if (_dxengine->playerArr[i]._on == true) {
			for (int j = 0; j < NPCMAX; ++j)
			{
				if (_dxengine->npcArr[j]._on == true) {
					if (pow(_dxengine->playerArr[i]._transform.x - _dxengine->npcArr[j]._transform.x, 2) + pow(_dxengine->playerArr[i]._transform.z - _dxengine->npcArr[j]._transform.z, 2) <= 9.f) {
						if (_dxengine->playerArr[i]._animation_state == 2 || _dxengine->playerArr[i]._animation_state == 3) { // 플레이어가 공격중이라면
							CS_COLLISION_PACKET p;
							p.size = sizeof(p);
							p.type = CS_COLLISION;
							p.attack_type = 'a'; // 의미 없는 값
							p.attacker_id = i;
							p.target_id = j;
							_dxengine->networkPtr->send_packet(&p);

							//cout << "player" << i << " hp : " << _dxengine->playerArr[i]._hp << endl;	// 플레이어 hp 콘솔로 체크
							//cout << "npc" << j << " hp : " << _dxengine->npcArr[j]._hp << endl;		// npc hp 콘솔로 체크
						}
						if (_dxengine->npcArr[j]._animation_state == 2) { // npc가 공격중이라면
							CS_COLLISION_PACKET p;
							p.size = sizeof(p);
							p.type = CS_COLLISION;
							p.attack_type = 'a'; // 의미 없는 값
							p.attacker_id = j;
							p.target_id = i;
							_dxengine->networkPtr->send_packet(&p);

							//cout << "player" << i << " hp : " << _dxengine->playerArr[i]._hp << endl;	// 플레이어 hp 콘솔로 체크
							//cout << "npc" << j << " hp : " << _dxengine->npcArr[j]._hp << endl;		// npc hp 콘솔로 체크
						}
					}
				}
			}
		}
	}

	// 플레이어 사망
	for (int i = 0; i < PLAYERMAX; ++i) {
		if (_dxengine->playerArr[i]._on == true && _dxengine->playerArr[i]._hp <= 0.f) {
			_dxengine->playerArr[i]._animation_state = 4;
			//playerArr[i]._on = false;

			CS_MOVE_PACKET p;
			p.size = sizeof(p);
			p.type = CS_MOVE;
			p.char_state = _dxengine->playerArr[_dxengine->networkPtr->myClientId]._animation_state;
			//networkPtr->send_packet(&p);
		}
	}

	// npc 사망
	for (int i = 0; i < NPCMAX; ++i) {
		if (_dxengine->npcArr[i]._on == true && _dxengine->npcArr[i]._hp <= 0.f) {
			_dxengine->npcArr[i]._animation_state = 3;
			//npcArr[i]._on = false;

			CS_MOVE_PACKET p;
			p.size = sizeof(p);
			p.type = CS_MOVE;
			p.char_state = _dxengine->npcArr[_dxengine->networkPtr->myClientId]._animation_state;
			//networkPtr->send_packet(&p);
		}
	}

	//VP 변환
	_dxengine->cameraPtr->pos = XMVectorSet(_dxengine->playerArr[_dxengine->networkPtr->myClientId]._transform.x - 7 * cosf(_dxengine->inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - _dxengine->inputPtr->angle.y * XM_PI / 180.f),
		_dxengine->playerArr[_dxengine->networkPtr->myClientId]._transform.y + 4 + 7 * cosf(XM_PI / 2.0f - _dxengine->inputPtr->angle.y * XM_PI / 180.f),
		_dxengine->playerArr[_dxengine->networkPtr->myClientId]._transform.z - 7 * sinf(_dxengine->inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - _dxengine->inputPtr->angle.y * XM_PI / 180.f), 0.0f);
	XMVECTOR target = XMVectorSet(_dxengine->playerArr[_dxengine->networkPtr->myClientId]._transform.x, _dxengine->playerArr[_dxengine->networkPtr->myClientId]._transform.y + 1.65f,
		_dxengine->playerArr[_dxengine->networkPtr->myClientId]._transform.z,
		_dxengine->playerArr[_dxengine->networkPtr->myClientId]._transform.w);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(_dxengine->cameraPtr->pos, target, up); //뷰 변환 행렬
	XMStoreFloat4x4(&_dxengine->_transform.view, XMMatrixTranspose(view));

	XMMATRIX proj = XMLoadFloat4x4(&_dxengine->cameraPtr->mProj); //투영 변환 행렬
	XMStoreFloat4x4(&_dxengine->_transform.proj, XMMatrixTranspose(proj));

	//Light
	LightInfo lightInfo;
	_dxengine->_transform.lnghtInfo = lightInfo;

}

void TestScene::Update(WindowInfo windowInfo, bool isActive)
{
}

void TestScene::Draw(ComPtr<ID3D12CommandAllocator>	cmdAlloc, ComPtr<ID3D12GraphicsCommandList>	cmdList, int i_now_render_index)
{
	// 애니메이션
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (_dxengine->playerArr[i]._on == true) {
			_dxengine->player_AKI_Body_asset.UpdateSkinnedAnimation(_dxengine->timerPtr->_deltaTime, _dxengine->playerArr[i]);
		}
	}
	if (_dxengine->npcArr[0]._on == true) {
		_dxengine->boss.UpdateSkinnedAnimation(_dxengine->timerPtr->_deltaTime, _dxengine->npcArr[0]);
	}
	for (int i = 1; i < NPCMAX; ++i) {
		if (_dxengine->npcArr[i]._on == true) {
			_dxengine->npc_asset.UpdateSkinnedAnimation(_dxengine->timerPtr->_deltaTime, _dxengine->npcArr[i]);
		}
	}

	// 플레이어
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (_dxengine->playerArr[i]._on == true)
		{		
			{
				XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(1.0f, 1.0f, 1.0f)
					* XMMatrixRotationY(_dxengine->playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(_dxengine->playerArr[i]._transform.x, _dxengine->playerArr[i]._transform.y, _dxengine->playerArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
				XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));

				copy(begin(_dxengine->playerArr[i]._final_transforms), end(_dxengine->playerArr[i]._final_transforms), &_dxengine->_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
					_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
					_dxengine->texturePtr->_srvHandle = _dxengine->texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
					_dxengine->descHeapPtr->CopyDescriptor(_dxengine->texturePtr->_srvHandle, 5, _dxengine->devicePtr);
				}

				_dxengine->descHeapPtr->CommitTable_multi(_dxengine->cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(_dxengine->player_AKI_Body_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}

	// 보스
	cmdList->SetPipelineState(_dxengine->boss._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &_dxengine->boss._vertexBufferView);
	cmdList->IASetIndexBuffer(&_dxengine->boss._indexBufferView);
	if (_dxengine->npcArr[0]._on == true)
	{
		{
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(600.f, 600.f, 600.f) * XMMatrixRotationX(-XM_PI / 2.f)
				* XMMatrixRotationY(_dxengine->npcArr[0]._degree * XM_PI / 180.f)
				* XMMatrixTranslation(_dxengine->npcArr[0]._transform.x, _dxengine->npcArr[0]._transform.y + 0.2f, _dxengine->npcArr[0]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));
			XMStoreFloat4x4(&_dxengine->_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

			copy(begin(_dxengine->npcArr[0]._final_transforms), end(_dxengine->npcArr[0]._final_transforms), &_dxengine->_transform.BoneTransforms[0]);

			_dxengine->texturePtr->_srvHandle = _dxengine->texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();

			int sum = 0;
			for (Subset i : _dxengine->boss._animationPtr->mSubsets)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
				_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
				_dxengine->texturePtr->_srvHandle.Offset(1, _dxengine->devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				_dxengine->descHeapPtr->CopyDescriptor(_dxengine->texturePtr->_srvHandle, 5, _dxengine->devicePtr);
				_dxengine->descHeapPtr->CommitTable_multi(_dxengine->cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
				sum += i.FaceCount * 3;
			}
		}
	}

	// npc
	cmdList->SetPipelineState(_dxengine->npc_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &_dxengine->npc_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&_dxengine->npc_asset._indexBufferView);
	for (int i = 1; i < NPCMAX; i++) //npc ����
	{
		if (_dxengine->npcArr[i]._on == true)
		{
			{
				//���� ��ȯ
				XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(200.f, 200.f, 200.f) * XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(_dxengine->npcArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(_dxengine->npcArr[i]._transform.x, _dxengine->npcArr[i]._transform.y + 0.2f, _dxengine->npcArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
				XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));
				XMStoreFloat4x4(&_dxengine->_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

				// ��Ű�� �ִϸ��̼� ��� ������ ����
				copy(begin(_dxengine->npcArr[i]._final_transforms), end(_dxengine->npcArr[i]._final_transforms), &_dxengine->_transform.BoneTransforms[0]);

				//����
				_dxengine->texturePtr->_srvHandle = _dxengine->texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();

				int sum = 0;
				for (Subset i : _dxengine->npc_asset._animationPtr->mSubsets)
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
					_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
					_dxengine->texturePtr->_srvHandle.Offset(1, _dxengine->devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
					_dxengine->descHeapPtr->CopyDescriptor(_dxengine->texturePtr->_srvHandle, 5, _dxengine->devicePtr);
					_dxengine->descHeapPtr->CommitTable_multi(_dxengine->cmdQueuePtr, i_now_render_index);
					cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
					sum += i.FaceCount * 3;
				}
			}
		}
	}

	// npc hp bar
	cmdList->SetPipelineState(_dxengine->hp_bar._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &_dxengine->hp_bar._vertexBufferView);
	cmdList->IASetIndexBuffer(&_dxengine->hp_bar._indexBufferView);
	for (int i = 1; i < NPCMAX; i++)
	{
		if (_dxengine->npcArr[i]._on == true) {
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(0.5f, 0.1f, 0.1f)
				* XMMatrixRotationX(-atan2f(_dxengine->cameraPtr->pos.m128_f32[1] - (_dxengine->npcArr[i]._transform.y + 1.f),
					sqrt(pow(_dxengine->cameraPtr->pos.m128_f32[0] - _dxengine->npcArr[i]._transform.x, 2) + pow(_dxengine->cameraPtr->pos.m128_f32[2] - _dxengine->npcArr[i]._transform.z, 2))))
				* XMMatrixRotationY(atan2f(_dxengine->cameraPtr->pos.m128_f32[0] - _dxengine->npcArr[i]._transform.x, _dxengine->cameraPtr->pos.m128_f32[2] - _dxengine->npcArr[i]._transform.z))
				* XMMatrixTranslation(_dxengine->npcArr[i]._transform.x, _dxengine->npcArr[i]._transform.y + 1.f, _dxengine->npcArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));
			_dxengine->_transform.hp_bar_size = 2.f;
			_dxengine->_transform.hp_bar_start_pos = _dxengine->npcArr[i]._transform;
			_dxengine->_transform.hp_bar_start_pos.x -= _dxengine->_transform.hp_bar_size / 2.f;
			_dxengine->_transform.current_hp = _dxengine->npcArr[i]._hp;
			_dxengine->_transform.max_hp = 100;

			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
				_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);

				_dxengine->hp_bar._tex._srvHandle = _dxengine->hp_bar._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

				_dxengine->descHeapPtr->CopyDescriptor(_dxengine->hp_bar._tex._srvHandle, 5, _dxengine->devicePtr);
			}

			_dxengine->descHeapPtr->CommitTable_multi(_dxengine->cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(_dxengine->hp_bar._indexCount, 1, 0, 0, 0);
		}
	}

	// 기둥
	cmdList->SetPipelineState(_dxengine->cube_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &_dxengine->cube_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&_dxengine->cube_asset._indexBufferView);
	for (int i = 0; i < CubeMax; i++) //��� ����
	{
		if (_dxengine->cubeArr[i]._on == true)
		{
			//���� ��ȯ
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(1.0f, 2.0f, 1.0f) * XMMatrixTranslation(_dxengine->cubeArr[i]._transform.x, _dxengine->cubeArr[i]._transform.y + 2.0f, _dxengine->cubeArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));

			//����
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
				_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);

				_dxengine->cube_asset._tex._srvHandle = _dxengine->cube_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

				_dxengine->descHeapPtr->CopyDescriptor(_dxengine->cube_asset._tex._srvHandle, 5, _dxengine->devicePtr);
			}

			_dxengine->descHeapPtr->CommitTable_multi(_dxengine->cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(_dxengine->cube_asset._indexCount, 1, 0, 0, 0);
		}
	}

	// map
	float map_size = 100.f;

	cmdList->SetPipelineState(_dxengine->floor._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &_dxengine->floor._vertexBufferView);
	cmdList->IASetIndexBuffer(&_dxengine->floor._indexBufferView);
	for (int i = 0; i < 5; ++i)
	{
		if (i == 0) {
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(map_size, map_size, 1.0f) * XMMatrixRotationX(-XM_PI / 2.f) * XMMatrixTranslation(0.f, 0.f, 0.f));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
			_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
			_dxengine->floor._tex._srvHandle = _dxengine->floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

		}
		if (i == 1) {
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f) * XMMatrixRotationX(0.f) * XMMatrixTranslation(0.f, map_size / 4.f, -map_size));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
			_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
			_dxengine->floor._tex._srvHandle = _dxengine->floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

		}
		else if (i == 2) {
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f) * XMMatrixRotationX(XM_PI) * XMMatrixRotationZ(XM_PI) * XMMatrixTranslation(0.f, map_size / 4.f, map_size));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
			_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
			_dxengine->floor._tex._srvHandle = _dxengine->floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
		}
		else if (i == 3) {
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f) * XMMatrixRotationY(-XM_PI / 2.f) * XMMatrixTranslation(map_size, map_size / 4.f, 0.f));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
			_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
			_dxengine->floor._tex._srvHandle = _dxengine->floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
		}
		else if (i == 4) {
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(map_size, map_size / 4.f, 1.0f) * XMMatrixRotationY(XM_PI / 2.f) * XMMatrixTranslation(-map_size, map_size / 4.f, 0.f));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
			_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
			_dxengine->floor._tex._srvHandle = _dxengine->floor._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
		}

		_dxengine->floor._tex._srvHandle.Offset(i, _dxengine->devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		_dxengine->descHeapPtr->CopyDescriptor(_dxengine->floor._tex._srvHandle, 5, _dxengine->devicePtr);

		_dxengine->descHeapPtr->CommitTable_multi(_dxengine->cmdQueuePtr, i_now_render_index);
		cmdList->DrawIndexedInstanced(_dxengine->floor._indexCount, 1, 0, 0, 0);
	}

	// 스카이박스
	{
		cmdList->SetPipelineState(_dxengine->skybox._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &_dxengine->skybox._vertexBufferView);
		cmdList->IASetIndexBuffer(&_dxengine->skybox._indexBufferView);
		//���� ��ȯ
		XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.f, 2.f, 0.f));
		XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world);
		XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));
		_dxengine->_transform.camera_pos = _dxengine->cameraPtr->pos;
		//����
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
			_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);

			_dxengine->skybox._tex._srvHandle = _dxengine->skybox._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

			_dxengine->descHeapPtr->CopyDescriptor(_dxengine->skybox._tex._srvHandle, 5, _dxengine->devicePtr);
		}

		_dxengine->descHeapPtr->CommitTable_multi(_dxengine->cmdQueuePtr, i_now_render_index);
		cmdList->DrawIndexedInstanced(_dxengine->skybox._indexCount, 1, 0, 0, 0);
	}

	// 파티클
	if (pow(_dxengine->playerArr[0]._transform.x - _dxengine->npcArr[9]._transform.x, 2) + pow(_dxengine->playerArr[0]._transform.z - _dxengine->npcArr[9]._transform.z, 2) <= 4.f) //�浹 ó��
	{
		if (_dxengine->playerArr[0]._isCollision[9] == false)
			_dxengine->playerArr[0]._isFirstCollision[9] = true;
		else
			_dxengine->playerArr[0]._isFirstCollision[9] = false;
		_dxengine->playerArr[0]._isCollision[9] = true;
	}
	if (pow(_dxengine->playerArr[0]._transform.x - _dxengine->npcArr[9]._transform.x, 2) + pow(_dxengine->playerArr[0]._transform.z - _dxengine->npcArr[9]._transform.z, 2) > 4.f)
	{
		_dxengine->playerArr[0]._isCollision[9] = false;
	}
	for (int i = 0; i < 100; i++) //��ƼŬ ����
	{
		if (_dxengine->playerArr[0]._isFirstCollision[9] == true && _dxengine->particles[i].alive == 0)
		{
			_dxengine->particles[i].lifeTime = (float)(rand() % 101) / 1000 + 0.1f; //0.1~0.2
			_dxengine->particles[i].curTime = 0.0f;
			_dxengine->particles[i].pos = XMVectorSet(_dxengine->npcArr[9]._transform.x, _dxengine->npcArr[9]._transform.y, _dxengine->npcArr[9]._transform.z, 1.f);
			_dxengine->particles[i].moveSpeed = (float)(rand() % 101) / 50 + 2.f; //2~4
			_dxengine->particles[i].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
			XMVector3Normalize(_dxengine->particles[i].dir);
			_dxengine->particles[i].alive = 1;
		}
		else if (_dxengine->particles[i].alive == 1)
		{
			//���� ��ȯ
			_dxengine->particles[i].pos = XMVectorAdd(_dxengine->particles[i].pos, _dxengine->particles[i].dir * _dxengine->particles[i].moveSpeed * _dxengine->timerPtr->_deltaTime);
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixRotationY(atan2f(_dxengine->cameraPtr->pos.m128_f32[0] - _dxengine->particles[i].pos.m128_f32[0], _dxengine->cameraPtr->pos.m128_f32[2] - _dxengine->particles[i].pos.m128_f32[2]))
				* XMMatrixTranslation(_dxengine->particles[i].pos.m128_f32[0], _dxengine->particles[i].pos.m128_f32[1], _dxengine->particles[i].pos.m128_f32[2]));
			XMMATRIX world = XMLoadFloat4x4(&_dxengine->_transform.world); //���� ��ȯ ���
			XMStoreFloat4x4(&_dxengine->_transform.world, XMMatrixTranspose(world));
			_dxengine->particles[i].curTime += 0.001;
			if (_dxengine->particles[i].lifeTime < _dxengine->particles[i].curTime)
			{
				_dxengine->particles[i].alive = 0;
			}

			cmdList->SetPipelineState(_dxengine->psoPtr->_gsPipelineState.Get());
			cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			cmdList->IASetVertexBuffers(0, 1, &_dxengine->vertexBufferPtr->_particleVertexBufferView);
			cmdList->IASetIndexBuffer(&_dxengine->indexBufferPtr->_particleIndexBufferView);
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = _dxengine->constantBufferPtr->PushData(0, &_dxengine->_transform, sizeof(_dxengine->_transform));
				_dxengine->descHeapPtr->CopyDescriptor(handle, 0, _dxengine->devicePtr);
				_dxengine->texturePtr->_srvHandle = _dxengine->texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				_dxengine->texturePtr->_srvHandle.Offset(6, _dxengine->devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				_dxengine->descHeapPtr->CopyDescriptor(_dxengine->texturePtr->_srvHandle, 5, _dxengine->devicePtr);
			}
			_dxengine->descHeapPtr->CommitTable_multi(_dxengine->cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(_dxengine->indexBufferPtr->_particleIndexCount, 1, 0, 0, 0);
		}
	}

}
