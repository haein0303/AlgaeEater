#pragma once

class TestScene : public SCENE
{
public:
	TestScene();
	TestScene(SceneTag tag, DxEngine* dxEngine);
	void Init(WindowInfo windowInfo) override;
	void FixedUpdate(WindowInfo windowInfo, bool isActive) override;
	void Update(WindowInfo windowInfo, bool isActive) override;
	void Draw(ComPtr<ID3D12CommandAllocator> cmdAlloc, ComPtr<ID3D12GraphicsCommandList> cmdList, int& state, int& state0) override;
};

