#pragma once


class DxEngine;

class SCENE
{
public:
	enum SceneTag {
		test_scene
		, Count
	};


	SCENE();
	SCENE(SceneTag tag, DxEngine* dxEngine);
	//DX���� �ʱ�ȭ
	virtual void Init(WindowInfo windowInfo) = 0;

	//�� �����Ӹ��� ������Ʈ
	virtual void FixedUpdate(WindowInfo windowInfo, bool isActive) = 0;
	virtual void Update(WindowInfo windowInfo, bool isActive) = 0;


	//�� �����Ӹ��� �׸���
	virtual void Draw(ComPtr<ID3D12CommandAllocator> cmdAlloc, ComPtr<ID3D12GraphicsCommandList> cmdList, int& state, int& state0) = 0;

protected:
	SceneTag _tag;
	DxEngine* _dxengine;

};

