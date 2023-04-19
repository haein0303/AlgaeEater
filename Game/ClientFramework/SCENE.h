#pragma once


class DxEngine;

class SCENE
{
public:
	enum SceneTag {
		test_scene
		, Count
	};


	SCENE(SceneTag tag, DxEngine* dxEngine);
	//DX엔진 초기화
	virtual void Init(WindowInfo windowInfo) = 0;

	//매 프레임마다 업데이트
	virtual void FixedUpdate(WindowInfo windowInfo, bool isActive) = 0;
	virtual void Update(WindowInfo windowInfo, bool isActive) = 0;


	//매 프레임마다 그리기
	virtual void Draw(ComPtr<ID3D12CommandAllocator> cmdAlloc, ComPtr<ID3D12GraphicsCommandList> cmdList) = 0;
	void Draw_excute(ComPtr<ID3D12CommandAllocator> cmdAlloc, ComPtr<ID3D12GraphicsCommandList> cmdList, int i_now_render_index);

protected:
	SceneTag _tag;
	DxEngine* _dxengine;

};

