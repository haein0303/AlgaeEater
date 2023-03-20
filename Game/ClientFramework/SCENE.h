#pragma once



class SCENE
{


public:
	//DX엔진 초기화
	void Init(WindowInfo windowInfo);

	//매 프레임마다 업데이트
	void FixedUpdate(WindowInfo windowInfo, bool isActive);
	void Update(WindowInfo windowInfo, bool isActive);


	//매 프레임마다 그리기
	void Draw(WindowInfo windowInfo, int);


};

