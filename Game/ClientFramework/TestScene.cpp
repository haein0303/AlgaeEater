#include "Util.h"
#include "DxEngine.h"
#include "SCENE.h"

#include "TestScene.h"

TestScene::TestScene()
{
}

TestScene::TestScene(SceneTag tag, DxEngine* dxEngine) :SCENE(tag, dxEngine)
{
}

void TestScene::Init(WindowInfo windowInfo)
{
}

void TestScene::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
}

void TestScene::Update(WindowInfo windowInfo, bool isActive)
{
}

void TestScene::Draw(WindowInfo windowInfo, int)
{
}
