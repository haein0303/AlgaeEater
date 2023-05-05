#pragma once
#include "SCENE.h"

class scene_stage1 :
    public SCENE
{
	scene_stage1(SceneTag tag, DxEngine* dxEngine);
	void Init(WindowInfo windowInfo) override;
	void FixedUpdate(WindowInfo windowInfo, bool isActive) override;
	void Update(WindowInfo windowInfo, bool isActive) override;
	void Draw(ComPtr<ID3D12CommandAllocator> cmdAlloc, ComPtr<ID3D12GraphicsCommandList> cmdList, int i_now_render_index) override;
};
