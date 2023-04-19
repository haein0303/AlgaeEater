#pragma once

class TestScene : public SCENE
{
public:
	TestScene(SceneTag tag, DxEngine* dxEngine);
	void Init(WindowInfo windowInfo) override;
	void FixedUpdate(WindowInfo windowInfo, bool isActive) override;
	void Update(WindowInfo windowInfo, bool isActive) override;
	void Draw(ComPtr<ID3D12CommandAllocator> cmdAlloc, ComPtr<ID3D12GraphicsCommandList> cmdList) override;
};

