#pragma once


struct WindowInfo;

class OBJECT
{
public:
	bool		on = false;
	float		degree;
	XMFLOAT4	transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	bool		isCollision = false;
	bool		isFirstCollision = false;

	

	
public:
	//DX엔진 초기화
	void Init(WindowInfo windowInfo);

	//매 프레임마다 업데이트
	void FixedUpdate(WindowInfo windowInfo, bool isActive);
	void Update(WindowInfo windowInfo, bool isActive);


	//매 프레임마다 그리기
	void Draw(WindowInfo windowInfo, int);
};

class PLAYER :public OBJECT {







};