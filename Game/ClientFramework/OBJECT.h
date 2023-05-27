#pragma once


struct WindowInfo;
class MESH_ASSET;
class Timer;



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

	XMFLOAT4	_prev_transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4	_delta_transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	float		_prev_degree;
	//각 오브젝트마다 전송 간격이나 속도가 다를 수 있음으로, 각각 보간을 해줘야됨
	//_delta_time은 간격을 구하기 위해서 사용하는 변수
	float		_delta_time = 0.f;
	//_prev_delta_time은 새로 전송을 받았을 때 최종적으로 환산된 델타 타임을 저장하는 공간
	float		_prev_delta_time = 0.f;
	double		_delta_percent = 0.f;

	bool		_is_basic_attack_cool_ready = false;
	chrono::milliseconds time;
	vector<XMFLOAT4X4> _final_transforms;
	vector<XMFLOAT4X4> _weapon_final_transforms;

	int _particle_count = 0;

	shared_ptr<MESH_ASSET> _my_mesh;
	shared_ptr<Timer> _logicTimerPtr;

	
public:
	//DX엔진 초기화
	virtual void Init(WindowInfo windowInfo);

	//매 프레임마다 업데이트
	virtual void FixedUpdate(WindowInfo windowInfo, bool isActive);
	virtual void Update(WindowInfo windowInfo, bool isActive);


	//매 프레임마다 그리기
	virtual void Draw(WindowInfo windowInfo, int);
};

class PLAYER :public OBJECT {







};