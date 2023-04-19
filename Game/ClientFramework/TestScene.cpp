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
	//VP 변환
	_dxengine->cameraPtr->pos = XMVectorSet(_dxengine->playerArr[_dxengine->networkPtr->myClientId].transform.x - 7 * cosf(_dxengine->inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - _dxengine->inputPtr->angle.y * XM_PI / 180.f),
		_dxengine->playerArr[_dxengine->networkPtr->myClientId].transform.y + 4 + 7 * cosf(XM_PI / 2.0f - _dxengine->inputPtr->angle.y * XM_PI / 180.f),
		_dxengine->playerArr[_dxengine->networkPtr->myClientId].transform.z - 7 * sinf(_dxengine->inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - _dxengine->inputPtr->angle.y * XM_PI / 180.f), 0.0f);
	XMVECTOR target = XMVectorSet(_dxengine->playerArr[_dxengine->networkPtr->myClientId].transform.x, _dxengine->playerArr[_dxengine->networkPtr->myClientId].transform.y + 1.65f,
		_dxengine->playerArr[_dxengine->networkPtr->myClientId].transform.z,
		_dxengine->playerArr[_dxengine->networkPtr->myClientId].transform.w);
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

void TestScene::Draw(ComPtr<ID3D12CommandAllocator>	cmdAlloc, ComPtr<ID3D12GraphicsCommandList>	cmdList)
{
	//애니메이션
	_dxengine->npc_asset.UpdateSkinnedAnimation(_dxengine->timerPtr->_deltaTime, _dxengine->npcArr[0].animation_state, _dxengine->npcArr[0].animation_state0);

	

}
