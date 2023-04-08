#pragma once

class TestScene : public SCENE
{
public:
	TestScene();
	TestScene(SceneTag tag, DxEngine* dxEngine);
	void Init(WindowInfo windowInfo) override;
	void FixedUpdate(WindowInfo windowInfo, bool isActive) override;
	void Update(WindowInfo windowInfo, bool isActive) override;
	void Draw(WindowInfo windowInfo, int) override;
};

