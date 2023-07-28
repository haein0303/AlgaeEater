#pragma once


struct WindowInfo;
class MESH_ASSET;
class Timer;
class SFML;



class OBJECT
{
public:
	bool		_on = false;
	int			_my_server_id = 0;
	float		_degree;
	XMFLOAT4	_transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	
	bool		_isCollision[NPCMAX]{ false };
	bool		_isFirstCollision[NPCMAX]{ false };
	int			_animation_state = 0;
	int			_animation_state0 = 0;
	float		_animation_time_pos = 0.f;
	int			_hp = 100.f;
	bool		_can_attack = true;
	bool		_can_attack2 = true;
	bool		_can_attack3 = true;
	int			_eye_color = 1;
	int			_key = -1;
	int			_player_color = -1;
	int			_pillar_count = 5;
	int			_pillar_color = 0;
	int			_player_is_attacked_boss2_skill = 0;

	int			_max_hp = 0;

	bool		_shield_on = false;
	int			_max_shield = 0;
	int			_shield = 0;

	int			_stage1_target_id = 0;
	bool		_stage1_target_alert_on = false;

	int			_stage3_boss_con = 0;
	bool		_stage3_boss_on = false;

	int			_boss_skill_count = 0;
	float		_move_speed = 10.f;

	BoundingBox _bounding_box{ XMFLOAT3(0.f, 0.f, 0.f) , XMFLOAT3(0.5f, 1.f, 0.5f) };
	BoundingOrientedBox _attack;
	BoundingOrientedBox _skill;
	bool				_next_combo = false;
	int					_character_num = 0;

	XMFLOAT4	_prev_transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4	_delta_transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	float		_prev_degree;
	//�� ������Ʈ���� ���� �����̳� �ӵ��� �ٸ� �� ��������, ���� ������ ����ߵ�
	//_delta_time�� ������ ���ϱ� ���ؼ� ����ϴ� ����
	float		_delta_time = 0.f;
	//_prev_delta_time�� ���� ������ �޾��� �� ���������� ȯ��� ��Ÿ Ÿ���� �����ϴ� ����
	float		_prev_delta_time = 0.f;
	double		_delta_percent = 0.f;

	bool		_is_basic_attack_cool_ready = false;
	chrono::milliseconds time;
	vector<XMFLOAT4X4> _final_transforms;
	vector<XMFLOAT4X4> _weapon_final_transforms;

	int _particle_count = 0;

	shared_ptr<MESH_ASSET> _my_mesh;
	shared_ptr<Timer> _logicTimerPtr;

	vector<Boss2SkillData> boss2_skill_vec;

	bool			_game_clear = false;
	
public:
	//DX���� �ʱ�ȭ
	virtual void Init(WindowInfo windowInfo);

	//�� �����Ӹ��� ������Ʈ
	virtual void FixedUpdate(WindowInfo windowInfo, bool isActive);
	virtual void Update(WindowInfo windowInfo, bool isActive);


	//�� �����Ӹ��� �׸���
	virtual void Draw(WindowInfo windowInfo, int);
};

void boss2_skill_logical_update(OBJECT& boss, shared_ptr<Timer> timer);
bool boss2_skill_checker(OBJECT& player, OBJECT& boss, shared_ptr<SFML> networkPtr);