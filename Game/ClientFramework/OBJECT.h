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
	//DX���� �ʱ�ȭ
	void Init(WindowInfo windowInfo);

	//�� �����Ӹ��� ������Ʈ
	void FixedUpdate(WindowInfo windowInfo, bool isActive);
	void Update(WindowInfo windowInfo, bool isActive);


	//�� �����Ӹ��� �׸���
	void Draw(WindowInfo windowInfo, int);
};

class PLAYER :public OBJECT {







};