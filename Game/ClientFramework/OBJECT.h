#pragma once


struct WindowInfo;
class MESH_ASSET;

class OBJECT
{
public:
	bool		_on = false;
	float		_degree;
	XMFLOAT4	_transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	bool		_isCollision = false;
	bool		_isFirstCollision = false;
	int			_animation_state = 0;
	int			_animation_state0 = 0;
	float		_animation_time_pos = 0.f;
	chrono::milliseconds time;
	vector<XMFLOAT4X4> _final_transforms;

	shared_ptr<MESH_ASSET> _my_mesh;

	
public:
	//DX���� �ʱ�ȭ
	virtual void Init(WindowInfo windowInfo);

	//�� �����Ӹ��� ������Ʈ
	virtual void FixedUpdate(WindowInfo windowInfo, bool isActive);
	virtual void Update(WindowInfo windowInfo, bool isActive);


	//�� �����Ӹ��� �׸���
	virtual void Draw(WindowInfo windowInfo, int);
};

class PLAYER :public OBJECT {







};