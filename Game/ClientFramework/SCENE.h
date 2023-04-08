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
	//DX엔진 초기화
	virtual void Init(WindowInfo windowInfo) = 0;

	//매 프레임마다 업데이트
	virtual void FixedUpdate(WindowInfo windowInfo, bool isActive) = 0;
	virtual void Update(WindowInfo windowInfo, bool isActive) = 0;


	//매 프레임마다 그리기
	virtual void Draw(WindowInfo windowInfo, int) = 0;


	SceneTag _tag;
	DxEngine* _dxengine;

};

