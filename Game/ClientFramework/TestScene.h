#pragma once

class TestScene : public SCENE
{
	void Init(WindowInfo windowInfo) override;
	void FixedUpdate(WindowInfo windowInfo, bool isActive) override;
	void Update(WindowInfo windowInfo, bool isActive) override;
	void Draw(WindowInfo windowInfo, int) override;
};

