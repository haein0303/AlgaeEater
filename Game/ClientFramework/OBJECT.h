#pragma once


struct WindowInfo;
class MESH_ASSET;

class OBJECT
{
public:
	bool		_on = false;
	float		_degree;
	XMFLOAT4	_transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	bool		_isCollision[NPCMAX]{ false };
	bool		_isFirstCollision[NPCMAX]{ false };
	int			_animation_state = 0;
	int			_animation_state0 = 0;
	float		_animation_time_pos = 0.f;
	int			_hp = 100.f;
	bool		_can_attack = true;

	bool		_is_basic_attack_cool_ready = false;
	chrono::milliseconds time;
	vector<XMFLOAT4X4> _final_transforms;

	int _particle_count = 0;

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