#pragma once



class SCENE
{


public:
	//DX���� �ʱ�ȭ
	void Init(WindowInfo windowInfo);

	//�� �����Ӹ��� ������Ʈ
	void FixedUpdate(WindowInfo windowInfo, bool isActive);
	void Update(WindowInfo windowInfo, bool isActive);


	//�� �����Ӹ��� �׸���
	void Draw(WindowInfo windowInfo, int);


};

