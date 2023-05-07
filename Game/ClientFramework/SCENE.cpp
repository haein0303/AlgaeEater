

#include "Util.h"
#include "DxEngine.h"
#include "SCENE.h"

SCENE::SCENE(SceneTag tag, DxEngine* dxEngine):_tag(tag),_dxengine(dxEngine)
{
}

void SCENE::Draw_excute(ComPtr<ID3D12CommandAllocator> cmdAlloc, ComPtr<ID3D12GraphicsCommandList> cmdList, int i_now_render_index)
{
	//렌더 종료
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(_dxengine->swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	cmdList->ResourceBarrier(1, &barrier2);
	cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { cmdList.Get() };
	_dxengine->cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);


	_dxengine->swapChainPtr->_swapChain->Present(0, 0);

	_dxengine->cmdQueuePtr->WaitSync();

	_dxengine->swapChainPtr->_backBufferIndex = (_dxengine->swapChainPtr->_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;

}

void SCENE::RenderUI(ComPtr<ID2D1DeviceContext2> Context)
{
	for (const auto& asset : _ui_asset) {
		Context->DrawBitmap(asset._image, asset._rect);
	}

}
