#include "DxEngine.h"
#include "Device.h"
#include "TestScene.h"

void DxEngine::Init(WindowInfo windowInfo)
{

	//»≠ÔøΩÔøΩ ≈©ÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	_viewport = { 0, 0, static_cast<FLOAT>(windowInfo.ClientWidth), static_cast<FLOAT>(windowInfo.ClientHeight), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight);

	//DXÔøΩÔøΩÔøΩÔøΩ ÔøΩ ±ÔøΩ»≠
	devicePtr->CreateDevice();
	cmdQueuePtr->CreateCmdListAndCmdQueue(devicePtr);
	swapChainPtr->DescriptAndCreateSwapChain(windowInfo, devicePtr, cmdQueuePtr);
	rtvPtr->CreateRTV(devicePtr, swapChainPtr);
	cameraPtr->TransformProjection(windowInfo); //ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
	rootSignaturePtr->CreateRootSignature(devicePtr);
	constantBufferPtr->CreateConstantBuffer(CONSTANT_COUNT, CONSTANT_COUNT, devicePtr);
	constantBufferPtr->CreateView(devicePtr);
	descHeapPtr->CreateDescTable(CONSTANT_COUNT, devicePtr);
	
	d11Ptr->init(this, windowInfo);

	cout << "complite Init ptr" << endl;

	timerPtr->InitTimer();
	logicTimerPtr->InitTimer();

	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	RECT rect = { 0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(windowInfo.hwnd, 0, 100, 100, windowInfo.ClientWidth, windowInfo.ClientHeight, 0);
	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	srand((unsigned int)time(NULL));
	
	inputPtr->Init(windowInfo); //ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩ ±ÔøΩ»≠

	_renderEvent = ::CreateEvent(nullptr, FALSE, TRUE, nullptr);
	_excuteEvent = ::CreateEvent(nullptr, FALSE, TRUE, nullptr);

	for (OBJECT& p : playerArr) {
		p._logicTimerPtr = logicTimerPtr;
	}
	for (OBJECT& p : npcArr) {
		p._logicTimerPtr = logicTimerPtr;
	}
	boss_obj._logicTimerPtr = logicTimerPtr;

	cout << "complite all init" << endl;
}

void DxEngine::late_Init(WindowInfo windowInfo)
{
	/*cube_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	cube_asset.Init("../Resources/Cube.txt", ObjectType::GeneralObjects);
	cube_asset.Add_texture(L"..\\Resources\\Texture\\bricks.dds");
	cube_asset.Make_SRV();
	cube_asset.CreatePSO(L"..\\Color.hlsl");*/

	map_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	map_asset.Init("../Resources/Stage1_Wall_Test.txt", ObjectType::GeneralObjects);
	map_asset.Add_texture(L"..\\Resources\\Texture\\bricks.dds");
	map_asset.Make_SRV();
	map_asset.CreatePSO(L"..\\Bricks.hlsl");

	floor.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	floor.Init("../Resources/Floor.txt", ObjectType::GeneralObjects);
	floor.Add_texture(L"..\\Resources\\Texture\\Floor.jpg");
	floor.Make_SRV();
	floor.CreatePSO(L"..\\Bricks.hlsl");

	stage0_map.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	stage0_map.Init("../Resources/stage0_map.txt", ObjectType::GeneralObjects);
	stage0_map.Add_texture(L"..\\Resources\\Texture\\sample.png");
	stage0_map.Add_texture(L"..\\Resources\\Texture\\Back_wall.png");
	stage0_map.Add_texture(L"..\\Resources\\Texture\\Front_wall.png");
	stage0_map.Add_texture(L"..\\Resources\\Texture\\Right_wall.png");
	stage0_map.Add_texture(L"..\\Resources\\Texture\\Left_wall.png");
	stage0_map.Make_SRV();
	stage0_map.CreatePSO(L"..\\Bricks.hlsl");

	hp_bar.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	hp_bar.Init("../Resources/Floor.txt", ObjectType::GeneralObjects);
	hp_bar.Add_texture(L"..\\Resources\\Texture\\hp.jpg");
	hp_bar.Make_SRV();
	hp_bar.CreatePSO(L"..\\HPBar.hlsl");

	color_pattern.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	color_pattern.Init("../Resources/Floor.txt", ObjectType::GeneralObjects);
	color_pattern.Add_texture(L"..\\Resources\\Texture\\spider_paint_Red_BaseColor_Eye.jpg");
	color_pattern.Add_texture(L"..\\Resources\\Texture\\spider_paint_Blue_Color_Eye.jpg");
	color_pattern.Add_texture(L"..\\Resources\\Texture\\spider_paint_Green_Color_Eye.png");
	color_pattern.Add_texture(L"..\\Resources\\Texture\\spider_paint_White_Color_Eye.png");
	color_pattern.Make_SRV();
	color_pattern.CreatePSO(L"..\\ColorPattern.hlsl");

	key.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	key.Init("../Resources/rust_key.txt", ObjectType::GeneralObjects);
	key.Add_texture(L"..\\Resources\\Texture\\hp.jpg");
	key.Make_SRV();
	key.CreatePSO(L"..\\Color.hlsl");

	skybox.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	skybox.Init("../Resources/SkySphere.txt", ObjectType::SkyBox);
	skybox.Add_texture(L"..\\Resources\\Texture\\Sky.jpg");
	skybox.Make_SRV();
	skybox.CreatePSO(L"..\\SkySphere.hlsl");

	boss.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	boss.Init("../Resources/mechanical_spider.txt", ObjectType::AnimationObjects);
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_yellow_BaseColor.png");
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_black_BaseColor.png");
	boss.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_White_Color_Eye.png");	// 0
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_Blue_Color_Eye.jpg");	// 1
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_Green_Color_Eye.png");	// 2
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_Red_BaseColor_Eye.jpg");// 3
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_Black_Color_Eye.png");	// 4
	boss.Add_texture(L"..\\Resources\\Texture\\spider_paint_Gray_BaseColor_Wire.png");
	boss.Make_SRV();
	boss.CreatePSO();

	boss2.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	boss2.Init("../Resources/Boss2.txt", ObjectType::AnimationObjects);
	boss2.Add_texture(L"..\\Resources\\Texture\\Robot_01_Base_AlbedoTransparency.png");
	boss2.Add_texture(L"..\\Resources\\Texture\\Robot_01_Base_AlbedoTransparency.png");
	boss2.Make_SRV();
	boss2.CreatePSO();

	player_AKI_Body_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_Body_asset.Init("../Resources/AKI_Body.txt", ObjectType::AnimationObjects);
	player_AKI_Body_asset.Add_texture(L"..\\Resources\\Texture\\P05re_skin_u1.png");
	player_AKI_Body_asset.Make_SRV();
	player_AKI_Body_asset.CreatePSO();

	player_AKI_Astro_A_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_Astro_A_asset.Init("../Resources/AKI_Astro_A.txt", ObjectType::AnimationObjects);
	player_AKI_Astro_A_asset.Add_texture(L"..\\Resources\\Texture\\P05_Astro_A_u1.png");
	player_AKI_Astro_A_asset.Make_SRV();
	player_AKI_Astro_A_asset.CreatePSO();

	player_AKI_Hair_A_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_Hair_A_asset.Init("../Resources/AKI_Hair_A.txt", ObjectType::AnimationObjects);
	player_AKI_Hair_A_asset.Add_texture(L"..\\Resources\\Texture\\P05_hair_B_Flex_u1.png");
	player_AKI_Hair_A_asset.Make_SRV();
	player_AKI_Hair_A_asset.CreatePSO();

	player_AKI_HeadPhone_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_HeadPhone_asset.Init("../Resources/AKI_HeadPhone.txt", ObjectType::AnimationObjects);
	player_AKI_HeadPhone_asset.Add_texture(L"..\\Resources\\Texture\\bricks.dds");
	player_AKI_HeadPhone_asset.Make_SRV();
	player_AKI_HeadPhone_asset.CreatePSO();

	player_AKI_Sword_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_AKI_Sword_asset.Init("../Resources/AKI_Katana.txt", ObjectType::AnimationObjects);
	player_AKI_Sword_asset.Add_texture(L"..\\Resources\\Texture\\Katana_Albedo.png");
	player_AKI_Sword_asset.Make_SRV();
	player_AKI_Sword_asset.CreatePSO();

	npc_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	npc_asset.Init("../Resources/OrangeSpider.txt", ObjectType::AnimationObjects);
	npc_asset.Add_texture(L"..\\Resources\\Texture\\NPCSpider_DefaultMaterial_AlbedoTransparency.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_paint_black_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Make_SRV();
	npc_asset.CreatePSO();

	char a[] = "../Resources/pillar0.txt";
	for (int i = 0; i < 10; ++i)
	{
		test_pillar[i].Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		test_pillar[i].Init(a, ObjectType::VertexAnimationObjects);
		test_pillar[i].Add_texture(L"..\\Resources\\Texture\\bricks.dds");
		test_pillar[i].Make_SRV();
		test_pillar[i].CreatePSO(L"..\\Bricks.hlsl");
		++a[19];
	}
	char b[] = "../Resources/pillar10.txt";
	for (int i = 10; i < 20; ++i)
	{
		test_pillar[i].Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		test_pillar[i].Init(b, ObjectType::VertexAnimationObjects);
		test_pillar[i].Add_texture(L"..\\Resources\\Texture\\bricks.dds");
		test_pillar[i].Make_SRV();
		test_pillar[i].CreatePSO(L"..\\Bricks.hlsl");
		++b[20];
	}

	for (int i = 0; i < PLAYERMAX; ++i)
	{
		playerArr[i]._final_transforms.resize(player_AKI_Body_asset._animationPtr->mBoneHierarchy.size());
		playerArr[i]._weapon_final_transforms.resize(player_AKI_Sword_asset._animationPtr->mBoneHierarchy.size());
	}
	
	for (int i = 0; i < NPCMAX; ++i) {
		
			npcArr[i]._final_transforms.resize(npc_asset._animationPtr->mBoneHierarchy.size());
			npcArr[i]._transform.y += 0.2f;
		
	}
	boss_obj._final_transforms.resize(boss._animationPtr->mBoneHierarchy.size());
	boss_obj._transform.y += 1.f;

	key_data[0]._transform = XMFLOAT4(170.f, 0.f, -240.f, 1.f);
	key_data[0]._key = 0;
	key_data[0]._on = true;

	d11Ptr->LoadPipeline();
	d11Ptr->addResource(L"..\\Resources\\UserInterface\\test.png");

	ID2D1Bitmap* _i_tmp;
	_i_tmp = d11Ptr->addResource(L"..\\Resources\\UserInterface\\test.png");
	D2D1_RECT_F _tmp = D2D1::RectF(100.0f, 0.0f, 200.f, 100.f);
	_test_ui_vector.emplace_back(_i_tmp,_tmp);

	cout << "complite late init" << endl;
}

void DxEngine::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
	networkPtr->ReceiveServer(playerArr, npcArr, cubeArr, boss_obj, key_data);

	//∫∏∞£¿ª ¿ß«ÿº≠ ªÁøÎ«œ¥¬ √ ±‚ ºº∆√¿Ã∂ı¥Ÿ
	for (OBJECT& p : playerArr) {
		p._delta_time += logicTimerPtr->_deltaTime;
	}
	for (OBJECT& p : npcArr) {
		p._delta_time += logicTimerPtr->_deltaTime;
	}
	boss_obj._delta_time += logicTimerPtr->_deltaTime;

	for (int i = 0; i < PLAYERMAX; ++i) {
		if (playerArr[i]._on == true) {
			if (playerArr[i]._animation_state == 0 || playerArr[i]._animation_state == 1) {
				playerArr[i]._can_attack = true;
				playerArr[i]._can_attack2 = true;
				playerArr[i]._can_attack3 = true;
			}
		}
	}
	for (int i = 0; i < NPCMAX; ++i) {
		if (npcArr[i]._on == true) {
			if (npcArr[i]._animation_state == 0 || npcArr[i]._animation_state == 1) {
				npcArr[i]._can_attack = true;
			}
		}
	}

	if (boss_obj._on == true) {
		if (boss_obj._animation_state == 0 || boss_obj._animation_state == 1) {
			boss_obj._can_attack = true;
		}
	}


	if (isActive)
	{
		inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr);
		inputPtr->inputMouse(playerArr, networkPtr);
	}
	
	// ∆ƒ∆º≈¨ µø±‚»≠
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true) {
			for (int j = 0; j < NPCMAX; ++j)
			{
				if (npcArr[j]._on == true) {
					if (pow(playerArr[i]._transform.x - npcArr[j]._transform.x, 2) + pow(playerArr[i]._transform.z - npcArr[j]._transform.z, 2) <= 9.f) {
						if ((playerArr[i]._animation_state == 2 || playerArr[i]._animation_state == 3)
							&& playerArr[i]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[i]._animation_state) * 0.5f
							&& playerArr[i]._can_attack2) {
							playerArr[i]._can_attack2 = false;
							npcArr[j]._particle_count += 100;
						}
					}
				}
			}
			if (boss_obj._on == true) {
				if (pow(playerArr[i]._transform.x - boss_obj._transform.x, 2) + pow(playerArr[i]._transform.z - boss_obj._transform.z, 2) <= 9.f) {
					if ((playerArr[i]._animation_state == 2 || playerArr[i]._animation_state == 3)
						&& playerArr[i]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[i]._animation_state) * 0.5f
						&& playerArr[i]._can_attack2) {
						playerArr[i]._can_attack2 = false;
						boss_obj._particle_count += 100;
						cout << "test" << endl;
					}
				}
			}
		}
	}

	// ∞¯∞› √Êµπ √º≈©
	for (int j = 0; j < NPCMAX; ++j)
	{
		int i = networkPtr->myClientId;
		if (npcArr[j]._on == true) {
			if (pow(playerArr[i]._transform.x - npcArr[j]._transform.x, 2) + pow(playerArr[i]._transform.z - npcArr[j]._transform.z, 2) <= 9.f) {
				if ((playerArr[i]._animation_state == 2 || playerArr[i]._animation_state == 3)
					&& playerArr[i]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[i]._animation_state) * 0.5f
					&& playerArr[i]._can_attack) {

					playerArr[i]._can_attack = false;

					CS_COLLISION_PACKET p;
					p.size = sizeof(p);
					p.type = CS_COLLISION;
					p.attack_type = 'a';
					p.attacker_id = playerArr[i]._my_server_id;
					p.target_id = npcArr[j]._my_server_id;
					networkPtr->send_packet(&p);

					cout << "player" << i << " hp : " << playerArr[i]._hp << endl;
					cout << "npc" << j << " hp : " << npcArr[j]._hp << endl;
					cout << "particle " << j << " : " << npcArr[j]._particle_count << endl;
				}
				if (npcArr[j]._animation_state == 2
					&& npcArr[j]._animation_time_pos >= npc_asset._animationPtr->GetClipEndTime(npcArr[j]._animation_state) * 0.5f
					&& npcArr[j]._can_attack) {

					npcArr[j]._can_attack = false;

					CS_COLLISION_PACKET p;
					p.size = sizeof(p);
					p.type = CS_COLLISION;
					p.attack_type = 'a';
					p.attacker_id = npcArr[j]._my_server_id;
					p.target_id = playerArr[i]._my_server_id;
					networkPtr->send_packet(&p);

					cout << "player" << i << " hp : " << playerArr[i]._hp << endl;
					cout << "npc" << j << " hp : " << npcArr[j]._hp << endl;
				}
			}
		}
	}
		
	if (boss_obj._on == true) {
		int i = networkPtr->myClientId;
		if (pow(playerArr[i]._transform.x - boss_obj._transform.x, 2) + pow(playerArr[i]._transform.z - boss_obj._transform.z, 2) <= 9.f) {
			if ((playerArr[i]._animation_state == 2 || playerArr[i]._animation_state == 3)
				&& playerArr[i]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[i]._animation_state) * 0.5f
				&& playerArr[i]._can_attack) {

				playerArr[i]._can_attack = false;

				CS_COLLISION_PACKET p;
				p.size = sizeof(p);
				p.type = CS_COLLISION;
				p.attack_type = 'a';
				p.attacker_id = playerArr[i]._my_server_id;
				p.target_id = boss_obj._my_server_id;
				networkPtr->send_packet(&p);

				cout << "player" << i << " hp : " << boss_obj._hp << endl;
				cout << "BOSS hp : " << boss_obj._hp << endl;
				cout << "BOSS particle : " << boss_obj._particle_count << endl;
			}
			if (boss_obj._animation_state == 2
				&& boss_obj._animation_time_pos >= npc_asset._animationPtr->GetClipEndTime(boss_obj._animation_state) * 0.5f
				&& boss_obj._can_attack) {

				boss_obj._can_attack = false;

				CS_COLLISION_PACKET p;
				p.size = sizeof(p);
				p.type = CS_COLLISION;
				p.attack_type = 'a';
				p.attacker_id = boss_obj._my_server_id;
				p.target_id = playerArr[i]._my_server_id;
				networkPtr->send_packet(&p);

				cout << "player" << i << " hp : " << playerArr[i]._hp << endl;
				cout << "BOSS hp : " << boss_obj._hp << endl;
			}
		}
	}

	// key √Êµπ√≥∏Æ
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		for (int j = 0; j < KEYMAX; ++j) {
			if (pow(playerArr[i]._transform.x - key_data[j]._transform.x, 2) + pow(playerArr[i]._transform.z - key_data[j]._transform.z, 2) <= 1.f)
			{
				playerArr[i]._player_color = key_data[j]._key;
				key_data[j]._on = false;
			}
		}
		
	}

	// ±‚µ’ √Êµπ√≥∏Æ
	if (playerArr[networkPtr->myClientId]._on == true && cubeArr[4]._on == true)
	{
		if (pow(playerArr[networkPtr->myClientId]._transform.x - cubeArr[4]._transform.x, 2) + pow(playerArr[networkPtr->myClientId]._transform.z - cubeArr[4]._transform.z, 2) <= 9.f
			&& cubeArr[4]._pillar_color == playerArr[networkPtr->myClientId]._player_color)
		{
			if ((playerArr[networkPtr->myClientId]._animation_state == 2 || playerArr[networkPtr->myClientId]._animation_state == 3)
				&& playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f
				&& playerArr[networkPtr->myClientId]._can_attack3)
			{
				playerArr[networkPtr->myClientId]._can_attack3 = false;
				--cubeArr[4]._pillar_count;
				cout << "pillar : " << cubeArr[4]._pillar_count << endl;
			}
		}
	}

	
	if (playerArr[networkPtr->myClientId]._hp <= 0.f) {
		playerArr[networkPtr->myClientId]._animation_state = 4;

		CS_MOVE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.x = playerArr[networkPtr->myClientId]._transform.x;
		p.y = playerArr[networkPtr->myClientId]._transform.y;
		p.z = playerArr[networkPtr->myClientId]._transform.z;
		p.degree = playerArr[networkPtr->myClientId]._degree;
		p.char_state = playerArr[networkPtr->myClientId]._animation_state;
		networkPtr->send_packet(&p);
	}
	

	//VP ÔøΩÔøΩ»Ø
	float zoom = 3.f;
	cameraPtr->pos = XMVectorSet(playerArr[networkPtr->myClientId]._transform.x - zoom * cosf(inputPtr->angle.x*XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId]._transform.y + 1.35f + zoom * cosf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId]._transform.z - zoom * sinf(inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f), 0.0f);
	XMVECTOR target = XMVectorSet(playerArr[networkPtr->myClientId]._transform.x, playerArr[networkPtr->myClientId]._transform.y + 1.35f,
		playerArr[networkPtr->myClientId]._transform.z,
		playerArr[networkPtr->myClientId]._transform.w);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(cameraPtr->pos, target, up); //ÔøΩÔøΩ ÔøΩÔøΩ»Ø ÔøΩÔøΩÔø?
	XMStoreFloat4x4(&_transform.view, XMMatrixTranspose(view));

	XMMATRIX proj = XMLoadFloat4x4(&cameraPtr->mProj); //ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø ÔøΩÔøΩÔø?
	XMStoreFloat4x4(&_transform.proj, XMMatrixTranspose(proj));

	//Light
	LightInfo lightInfo;
	_transform.lnghtInfo = lightInfo;
}

void DxEngine::Update(WindowInfo windowInfo, bool isActive)
{


}



void DxEngine::Draw_multi(WindowInfo windowInfo, int i_now_render_index)
{
	::WaitForSingleObject(_renderEvent, INFINITE);
	ComPtr<ID3D12CommandAllocator>		cmdAlloc = cmdQueuePtr->_arr_cmdAlloc[i_now_render_index];
	ComPtr<ID3D12GraphicsCommandList>	cmdList = cmdQueuePtr->_arr_cmdList[i_now_render_index];

	//∫∏∞£¿ª ¿ß«ÿº≠ ≈∏¿Ã∏”∂˚ ∞¢¡æ ∞™µÈ ºº∆√«œ¥¬ øµø™
	//ø©±‚º≠ ¿ß«ÿº≠ ∫∞µµ∑Œ ø¿∆€∑π¿Ã≈Õ ø¿πˆ∑Œµ˘ ¡ﬂ¿Ã¥œ ¬¸∞Ì«œºÕ
	for (int i = 1; i < PLAYERMAX;++i) {
		if (playerArr[i]._on) {
			playerArr[i]._delta_percent = timerPtr->_deltaTime / playerArr[i]._prev_delta_time;
			playerArr[i]._prev_delta_time -= timerPtr->_deltaTime;
			cout << "p._delta_percent :" << playerArr[i]._delta_percent << endl;
			cout << "p._prev_delta_time :" << playerArr[i]._prev_delta_time << endl;

			playerArr[i]._delta_transform = playerArr[i]._transform - playerArr[i]._prev_transform;
			cout << i <<"]1 :p._delta_transform :";
			XMFLOAT4_print(playerArr[i]._delta_transform);
			playerArr[i]._delta_transform *= playerArr[i]._delta_percent;
			cout << "\n2 :p._delta_transform :";
			XMFLOAT4_print(playerArr[i]._delta_transform);
			cout << endl;
			playerArr[i]._prev_transform += playerArr[i]._delta_transform;
		}
		
	}
	for (OBJECT& p : npcArr) {
		if (p._on) {
			p._delta_percent = timerPtr->_deltaTime / p._prev_delta_time;
			p._prev_delta_time -= timerPtr->_deltaTime;
			p._delta_transform = p._transform - p._prev_transform;
			p._delta_transform *= p._delta_percent;
			p._prev_transform += p._delta_transform;
		}
	}

	boss_obj._delta_percent = timerPtr->_deltaTime / boss_obj._prev_delta_time;
	boss_obj._prev_delta_time -= timerPtr->_deltaTime;
	boss_obj._delta_transform = boss_obj._transform - boss_obj._prev_transform;
	boss_obj._delta_transform *= boss_obj._delta_percent;
	boss_obj._prev_transform += boss_obj._delta_transform;

	//ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩ
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true) {
			player_AKI_Body_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i], 0);
			player_AKI_Sword_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i], 1);
		}
	}
	for (int i = 0; i < NPCMAX; ++i) {
		if (npcArr[i]._on == true && i != 9) {
			npc_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, npcArr[i], 0);
		}
	}

	cmdAlloc->Reset();
	cmdList->Reset(cmdQueuePtr->_arr_cmdAlloc[i_now_render_index].Get(), nullptr);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmdList->ResourceBarrier(1, &barrier);

	cmdList->RSSetViewports(1, &_viewport);
	cmdList->RSSetScissorRects(1, &_scissorRect);


	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = rtvPtr->_rtvHandle[i_now_render_index];

	cmdList->ClearRenderTargetView(backBufferView, Colors::Lavender, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = dsvPtr->_dsvHandle;
	cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);


	cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->SetGraphicsRootSignature(rootSignaturePtr->_signature.Get());
	constantBufferPtr->_currentIndex = 0;
	descHeapPtr->_currentGroupIndex = 0;

	ID3D12DescriptorHeap* descHeap = descHeapPtr->_descHeap.Get();
	cmdList->SetDescriptorHeaps(1, &descHeap);

#pragma region Player
	cmdList->SetPipelineState(player_AKI_Body_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_Body_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_Body_asset._indexBufferView);

	//ÔøΩÔøΩÔøΩÔøΩ
	{
		int i = 0;
		{
			//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
				* XMMatrixRotationX(-XM_PI / 2.f)
				* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
				* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			// ÔøΩÔøΩ≈∞ÔøΩÔøΩ ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩ ÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
			copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

			//ÔøΩÔøΩÔøΩÔøΩ
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				player_AKI_Body_asset._tex._srvHandle = player_AKI_Body_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(player_AKI_Body_asset._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(player_AKI_Body_asset._indexCount, 1, 0, 0, 0);
		}
	}


	for (int i = 1; i < PLAYERMAX; ++i) //ÔøΩ√∑ÔøΩÔøΩÃæÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	{
		if (playerArr[i]._on == true)
		{
			//ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ : ÔøΩœπÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩ÷∂ÔøΩ ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩÔøΩÃ∂ÔøΩ ÔøΩﬁ∂ÔøΩ ÔøΩ◊∑ÔøΩ
			//ÔøΩÔøΩÔøΩ¬∞ÔøΩ ÔøΩŸ≤ÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ∆Æ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ∆ÆÔøΩÔøΩ ÔøΩ“∑ÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ÷¥¬∞ÔøΩ
			//ÔøΩÃ∞ÔøΩ ÔøΩÔøΩƒ• ÔøΩÔøΩ ÔøΩ÷¥ÔøΩ ÔøΩ€æÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ?			
			{
				//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._prev_transform.x, playerArr[i]._prev_transform.y, playerArr[i]._prev_transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				// ÔøΩÔøΩ≈∞ÔøΩÔøΩ ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩ ÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				//ÔøΩÔøΩÔøΩÔøΩ
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_Body_asset._tex._srvHandle = player_AKI_Body_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_Body_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_Body_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	//
	cmdList->SetPipelineState(player_AKI_Astro_A_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_Astro_A_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_Astro_A_asset._indexBufferView);

	{
		int i = 0;

		{
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
				* XMMatrixRotationX(-XM_PI / 2.f)
				* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
				* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				player_AKI_Astro_A_asset._tex._srvHandle = player_AKI_Astro_A_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(player_AKI_Astro_A_asset._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(player_AKI_Astro_A_asset._indexCount, 1, 0, 0, 0);
		}
	}

	for (int i = 1; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{	
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._prev_transform.x, playerArr[i]._prev_transform.y, playerArr[i]._prev_transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_Astro_A_asset._tex._srvHandle = player_AKI_Astro_A_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_Astro_A_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_Astro_A_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	// 
	cmdList->SetPipelineState(player_AKI_Hair_A_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_Hair_A_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_Hair_A_asset._indexBufferView);

	{
		int i = 0;

		{
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
				* XMMatrixRotationX(-XM_PI / 2.f)
				* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
				* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				player_AKI_Hair_A_asset._tex._srvHandle = player_AKI_Hair_A_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(player_AKI_Hair_A_asset._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(player_AKI_Hair_A_asset._indexCount, 1, 0, 0, 0);
		}

	}
	for (int i = 1; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._prev_transform.x, playerArr[i]._prev_transform.y, playerArr[i]._prev_transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_Hair_A_asset._tex._srvHandle = player_AKI_Hair_A_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_Hair_A_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_Hair_A_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	// 
	cmdList->SetPipelineState(player_AKI_HeadPhone_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_HeadPhone_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_HeadPhone_asset._indexBufferView);

	{
		int i = 0;

		{
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
				* XMMatrixRotationX(-XM_PI / 2.f)
				* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
				* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				player_AKI_HeadPhone_asset._tex._srvHandle = player_AKI_HeadPhone_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(player_AKI_HeadPhone_asset._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(player_AKI_HeadPhone_asset._indexCount, 1, 0, 0, 0);
		}
	}
	for (int i = 1; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._prev_transform.x, playerArr[i]._prev_transform.y, playerArr[i]._prev_transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_HeadPhone_asset._tex._srvHandle = player_AKI_HeadPhone_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_HeadPhone_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_HeadPhone_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
	// 
	cmdList->SetPipelineState(player_AKI_Sword_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &player_AKI_Sword_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&player_AKI_Sword_asset._indexBufferView);

	{
		int i = 0;
		{
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
				* XMMatrixRotationX(-XM_PI / 2.f)
				* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
				* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			copy(begin(playerArr[i]._weapon_final_transforms), end(playerArr[i]._weapon_final_transforms), &_transform.BoneTransforms[0]);

			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				player_AKI_Sword_asset._tex._srvHandle = player_AKI_Sword_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(player_AKI_Sword_asset._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(player_AKI_Sword_asset._indexCount, 1, 0, 0, 0);
		}
	}

	for (int i = 1; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(100.0f, 100.0f, 100.0f)
					* XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
					* XMMatrixTranslation(playerArr[i]._prev_transform.x, playerArr[i]._prev_transform.y, playerArr[i]._prev_transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				copy(begin(playerArr[i]._weapon_final_transforms), end(playerArr[i]._weapon_final_transforms), &_transform.BoneTransforms[0]);

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					player_AKI_Sword_asset._tex._srvHandle = player_AKI_Sword_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
					descHeapPtr->CopyDescriptor(player_AKI_Sword_asset._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(player_AKI_Sword_asset._indexCount, 1, 0, 0, 0);
			}
		}
	}
#pragma endregion
	
	// Boss
	if (boss_obj._on == true) {
		XMFLOAT3 boss_scale = XMFLOAT3(800.f, 800.f, 800.f);
		float boss_default_rot_x = -XM_PI * 0.5f;
		XMFLOAT3 boss2_scale = XMFLOAT3(1.f, 1.f, 1.f);
		float boss2_default_rot_x = 0.f;
		Boss(cmdList, boss, i_now_render_index, boss_scale, boss_default_rot_x);
	}

	cmdList->SetPipelineState(npc_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &npc_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&npc_asset._indexBufferView);
	for (int i = 0; i < NPCMAX; i++) //npc ÔøΩÔøΩÔøΩÔøΩ
	{
		if (npcArr[i]._on == true)
		{			
			{
				//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(200.f, 200.f, 200.f) * XMMatrixRotationX(-XM_PI / 2.f)
					* XMMatrixRotationY(npcArr[i]._degree * XM_PI / 180.f -  XM_PI / 2.f)
					* XMMatrixTranslation(npcArr[i]._transform.x, npcArr[i]._transform.y, npcArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
				XMStoreFloat4x4(&_transform.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));

				// ÔøΩÔøΩ≈∞ÔøΩÔøΩ ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩ ÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
				copy(begin(npcArr[i]._final_transforms), end(npcArr[i]._final_transforms), &_transform.BoneTransforms[0]);

				//ÔøΩÔøΩÔøΩÔøΩ
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				
				int sum = 0;
				for (Subset i : npc_asset._animationPtr->mSubsets)
				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
					texturePtr->_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
					descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
					descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
					cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
					sum += i.FaceCount * 3;
				}
			}
		}
	}

	// npc hp bar
	cmdList->SetPipelineState(hp_bar._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &hp_bar._vertexBufferView);
	cmdList->IASetIndexBuffer(&hp_bar._indexBufferView);
	for (int i = 0; i < NPCMAX; i++)
	{
		if (npcArr[i]._on == true && i != 9) {
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(0.5f, 0.1f, 0.1f)
				* XMMatrixRotationX(-atan2f(cameraPtr->pos.m128_f32[1] - (npcArr[i]._transform.y + 1.f),
					sqrt(pow(cameraPtr->pos.m128_f32[0] - npcArr[i]._transform.x, 2) + pow(cameraPtr->pos.m128_f32[2] - npcArr[i]._transform.z, 2))))
				* XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - npcArr[i]._transform.x, cameraPtr->pos.m128_f32[2] - npcArr[i]._transform.z))
				* XMMatrixTranslation(npcArr[i]._transform.x, npcArr[i]._transform.y + 1.f, npcArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
			_transform.hp_bar_size = 2.f;
			_transform.hp_bar_start_pos = npcArr[i]._transform;
			_transform.hp_bar_start_pos.x -= _transform.hp_bar_size / 2.f ;
			_transform.current_hp = npcArr[i]._hp;
			_transform.max_hp = 100;

			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);

				hp_bar._tex._srvHandle = hp_bar._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

				descHeapPtr->CopyDescriptor(hp_bar._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(hp_bar._indexCount, 1, 0, 0, 0);
		}
	}

	// ∏”∏Æ ¿ß ªˆ±Ú ∆–≈œ
	cmdList->SetPipelineState(color_pattern._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &color_pattern._vertexBufferView);
	cmdList->IASetIndexBuffer(&color_pattern._indexBufferView);
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._on == true)
		{
			if (playerArr[i]._player_color == 0)
			{
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(0.1f, 0.1f, 0.1f)
					* XMMatrixRotationX(-atan2f(cameraPtr->pos.m128_f32[1] - (playerArr[i]._transform.y + 1.75f),
						sqrt(pow(cameraPtr->pos.m128_f32[0] - playerArr[i]._transform.x, 2) + pow(cameraPtr->pos.m128_f32[2] - playerArr[i]._transform.z, 2))))
					* XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - playerArr[i]._transform.x, cameraPtr->pos.m128_f32[2] - playerArr[i]._transform.z))
					* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y + 1.75f, playerArr[i]._transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				{
					D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
					descHeapPtr->CopyDescriptor(handle, 0, devicePtr);

					color_pattern._tex._srvHandle = color_pattern._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

					descHeapPtr->CopyDescriptor(color_pattern._tex._srvHandle, 5, devicePtr);
				}

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(color_pattern._indexCount, 1, 0, 0, 0);
			}
		}
	}

	/*cmdList->SetPipelineState(cube_asset._pipelineState.Get());
	cmdList->IASetVertexBuffers(0, 1, &cube_asset._vertexBufferView);
	cmdList->IASetIndexBuffer(&cube_asset._indexBufferView);
	for (int i = 0; i < CubeMax; ++i) //ÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩ
	{
		if (cubeArr[i]._on == true)
		{
			//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 2.0f, 1.0f) * XMMatrixTranslation(cubeArr[i]._transform.x, cubeArr[i]._transform.y + 2.0f, cubeArr[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			if (i == 0)
				_transform.color = XMVectorSet(1.f, 0.f, 0.f, 1.f);
			else if (i == 1)
				_transform.color = XMVectorSet(0.f, 1.f, 0.f, 1.f);
			else if (i == 2)
				_transform.color = XMVectorSet(0.f, 0.f, 1.f, 1.f);
			else
				_transform.color = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			

			//ÔøΩÔøΩÔøΩÔøΩ
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);		

				cube_asset._tex._srvHandle = cube_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				
				descHeapPtr->CopyDescriptor(cube_asset._tex._srvHandle, 5, devicePtr);
			}

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(cube_asset._indexCount, 1, 0, 0, 0);
		}
	}*/

	// map
	switch (Scene_num) {
	case 0:
		Map(cmdList, stage0_map, map_asset, i_now_render_index, Scene_num);
		break;
	case 1:
		Map(cmdList, floor, map_asset, i_now_render_index, 1);
		break;
	default:
		Map(cmdList, stage0_map, map_asset, i_now_render_index, Scene_num);
		break;
	}
	for (int i = 0; i < KEYMAX; ++i) {
		if (key_data[i]._on == true)
		{
			cmdList->SetPipelineState(key._pipelineState.Get());
			cmdList->IASetVertexBuffers(0, 1, &key._vertexBufferView);
			cmdList->IASetIndexBuffer(&key._indexBufferView);

			_key_rotation_time += timerPtr->_deltaTime;
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(2.f, 2.f, 2.f) * XMMatrixRotationX(XM_PI * 0.5f)
				* XMMatrixRotationY(_key_rotation_time * 60.f * XM_PI / 180.f) * XMMatrixTranslation(key_data[i]._transform.x, key_data[i]._transform.y + 1.f, key_data[i]._transform.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			switch (i) {
			case 0:
				_transform.color = XMVectorSet(1.f, 0.f, 0.f, 1.f);
				break;
			case 1:
				_transform.color = XMVectorSet(0.f, 1.f, 0.f, 1.f);
				break;
			case 2:
				_transform.color = XMVectorSet(0.f, 0.f, 1.f, 1.f);
				break;
			case 3:
				_transform.color = XMVectorSet(1.f, 1.f, 1.f, 1.f);
				break;
			}
			

			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			key._tex._srvHandle = key._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
			descHeapPtr->CopyDescriptor(key._tex._srvHandle, 5, devicePtr);

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(key._indexCount, 1, 0, 0, 0);
		}
	}

	// test_pillar
	for (int i = 0; i < CubeMax; ++i)
	{
		if (cubeArr[i]._on == true)
		{
			XMVECTOR P = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			XMVECTOR Q = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			switch (cubeArr[i]._pillar_count)
			{
			case 5:
				cubeArr[i]._animation_time_pos = 0.f;
				break;
			case 4:
				cubeArr[i]._animation_time_pos = 0.034f;
				break;
			case 3:
				cubeArr[i]._animation_time_pos = 0.68f;
				break;
			case 2:
				cubeArr[i]._animation_time_pos = 0.102f;
				break;
			case 1:
				cubeArr[i]._animation_time_pos = 0.136f;
				break;
			default:
				cubeArr[i]._animation_time_pos += timerPtr->_deltaTime;
				break;
			}
			for (MESH_ASSET& pillar : test_pillar)
			{
				float scale = 1.f;

				pillar.UpdateVertexAnimation(timerPtr->_deltaTime, cubeArr[i], P, Q);

				cmdList->SetPipelineState(pillar._pipelineState.Get());
				cmdList->IASetVertexBuffers(0, 1, &pillar._vertexBufferView);
				cmdList->IASetIndexBuffer(&pillar._indexBufferView);

				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(scale, scale, scale)
					* XMMatrixRotationQuaternion(Q)
					* XMMatrixTranslation(cubeArr[i]._transform.x + P.m128_f32[0], cubeArr[i]._transform.y + P.m128_f32[1], cubeArr[i]._transform.z + P.m128_f32[2]));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				pillar._tex._srvHandle = pillar._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(pillar._tex._srvHandle, 5, devicePtr);

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(pillar._indexCount, 1, 0, 0, 0);
			}
		}
	}

	// skybox
	{
		cmdList->SetPipelineState(skybox._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &skybox._vertexBufferView);
		cmdList->IASetIndexBuffer(&skybox._indexBufferView);
		//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
		XMStoreFloat4x4(&_transform.world, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.f, 2.f, 0.f));
		XMMATRIX world = XMLoadFloat4x4(&_transform.world);
		XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
		_transform.camera_pos = cameraPtr->pos;
		//ÔøΩÔøΩÔøΩÔøΩ
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);

			skybox._tex._srvHandle = skybox._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

			descHeapPtr->CopyDescriptor(skybox._tex._srvHandle, 5, devicePtr);
		}

		descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
		cmdList->DrawIndexedInstanced(skybox._indexCount, 1, 0, 0, 0);
	}

#pragma region Particle System
	// ∆ƒ∆º≈¨ ª˝º∫
	int index = 0;
	for (int i = 0; i < NPCMAX; ++i)
	{
		// npc
		if (npcArr[i]._on == true && i != 9) {
			while (npcArr[i]._particle_count > 0)
			{
				if (index >= PARTICLE_NUM) { // ∆ƒ∆º≈¨ ∞≥ºˆø° ¥Î«— øπø‹√≥∏Æ
					break;
				}

				if (particles[index].alive == 0) // ?åå?ã∞?Å¥ Ï¥àÍ∏∞?ôî
				{
					particles[index].lifeTime = (float)(rand() % 101) / 1000.f + 0.3f; // 0.3~0.4
					particles[index].curTime = 0.0f;
					particles[index].pos = XMVectorSet(npcArr[i]._transform.x, npcArr[i]._transform.y + 0.3f, npcArr[i]._transform.z, 1.f);
					particles[index].moveSpeed = (float)(rand() % 101) / 50 + 2.f; // 2~4
					particles[index].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
					XMVector3Normalize(particles[index].dir);
					particles[index].velocity = XMVectorSet(particles[index].dir.m128_f32[0] * particles[index].moveSpeed,
						particles[index].dir.m128_f32[1] * particles[index].moveSpeed, particles[index].dir.m128_f32[2] * particles[index].moveSpeed, 1.f);
					particles[index].alive = 1;
					--npcArr[i]._particle_count;
				}
				else {
					++index;
				}
			}
		}
		else if (npcArr[i]._on == true && i == 9 && index <= PARTICLE_NUM) {
			while (npcArr[i]._particle_count > 0)
			{
				if (index >= PARTICLE_NUM) { // ∆ƒ∆º≈¨ ∞≥ºˆø° ¥Î«— øπø‹√≥∏Æ
					break;
				}

				if (particles[index].alive == 0) // ?åå?ã∞?Å¥ Ï¥àÍ∏∞?ôî
				{
					particles[index].lifeTime = (float)(rand() % 101) / 1000.f + 0.3f; // 0.3~0.4
					particles[index].curTime = 0.0f;
					particles[index].pos = XMVectorSet(npcArr[i]._transform.x + 0.5f, npcArr[i]._transform.y + 2.f, npcArr[i]._transform.z, 1.f);
					particles[index].moveSpeed = (float)(rand() % 101) / 50 + 2.f; // 2~4
					particles[index].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
					XMVector3Normalize(particles[index].dir);
					particles[index].velocity = XMVectorSet(particles[index].dir.m128_f32[0] * particles[index].moveSpeed,
						particles[index].dir.m128_f32[1] * particles[index].moveSpeed, particles[index].dir.m128_f32[2] * particles[index].moveSpeed, 1.f);
					particles[index].alive = 1;
					--npcArr[i]._particle_count;
				}
				else {
					++index;
				}
			}
		}


	}
	if (boss_obj._on == true) { // boss
		while (boss_obj._particle_count > 0)
		{
			if (index >= PARTICLE_NUM) { // ∆ƒ∆º≈¨ ∞≥ºˆø° ¥Î«— øπø‹√≥∏Æ
				break;
			}

			if (particles[index].alive == 0) // ?åå?ã∞?Å¥ Ï¥àÍ∏∞?ôî
			{
				particles[index].lifeTime = (float)(rand() % 101) / 1000.f + 0.3f; // 0.3~0.4
				particles[index].curTime = 0.0f;
				particles[index].pos = XMVectorSet(boss_obj._transform.x, boss_obj._transform.y + 2.f, boss_obj._transform.z, 1.f);
				particles[index].moveSpeed = (float)(rand() % 101) / 50 + 2.f; // 2~4
				particles[index].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
				XMVector3Normalize(particles[index].dir);
				particles[index].velocity = XMVectorSet(particles[index].dir.m128_f32[0] * particles[index].moveSpeed,
					particles[index].dir.m128_f32[1] * particles[index].moveSpeed, particles[index].dir.m128_f32[2] * particles[index].moveSpeed, 1.f);
				particles[index].alive = 1;
				--boss_obj._particle_count;
			}
			else {
				++index;
			}
		}
	}
	else if (boss_obj._on == true && index <= PARTICLE_NUM) {
		while (boss_obj._particle_count > 0)
		{
			if (index >= PARTICLE_NUM) { // ∆ƒ∆º≈¨ ∞≥ºˆø° ¥Î«— øπø‹√≥∏Æ
				break;
			}

			if (particles[index].alive == 0) // ?åå?ã∞?Å¥ Ï¥àÍ∏∞?ôî
			{
				particles[index].lifeTime = (float)(rand() % 101) / 1000.f + 0.3f; // 0.3~0.4
				particles[index].curTime = 0.0f;
				particles[index].pos = XMVectorSet(boss_obj._transform.x + 0.5f, boss_obj._transform.y + 2.f, boss_obj._transform.z, 1.f);
				particles[index].moveSpeed = (float)(rand() % 101) / 50 + 2.f; // 2~4
				particles[index].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
				XMVector3Normalize(particles[index].dir);
				particles[index].velocity = XMVectorSet(particles[index].dir.m128_f32[0] * particles[index].moveSpeed,
					particles[index].dir.m128_f32[1] * particles[index].moveSpeed, particles[index].dir.m128_f32[2] * particles[index].moveSpeed, 1.f);
				particles[index].alive = 1;
				--boss_obj._particle_count;
			}
			else {
				++index;
			}
		}
	}

	// ∆ƒ∆º≈¨ ø¨ªÍ
	for (int i = 0; i < PARTICLE_NUM; ++i) // ?åå?ã∞?Å¥ Î¨ºÎ¶¨Ï≤òÎ¶¨ Î∞? ?†å?çîÎß?
	{
		if (particles[i].alive == 1)
		{
			if (particles[i].pos.m128_f32[1] - particles[i].bounding_box_half_size.m128_f32[1] < 0.f) {
				// ÎπÑÌÉÑ?Ñ± Ï∂©Îèå
				particles[i].velocity.m128_f32[1] = particles[i].velocity.m128_f32[1] * -coefficient_of_restitution;
				particles[i].pos.m128_f32[1] = particles[i].bounding_box_half_size.m128_f32[1];
			}

			if (particles[i].velocity.m128_f32[1] <= 0.05f
				&& particles[i].pos.m128_f32[1] - particles[i].bounding_box_half_size.m128_f32[1] == 0.f) {
				// ÎßàÏ∞∞?†•
				if (fabs(particles[i].velocity.m128_f32[0]) > 0.1f) {
					particles[i].velocity.m128_f32[0] = particles[i].velocity.m128_f32[0] + friction_coefficient * gravitational_acceleration * particles[i].dir.m128_f32[0] * timerPtr->_deltaTime;
				}
				else {
					particles[i].velocity.m128_f32[0] = 0.f;
				}

				if (fabs(particles[i].velocity.m128_f32[2]) > 0.1f) {
					particles[i].velocity.m128_f32[2] = particles[i].velocity.m128_f32[2] + friction_coefficient * gravitational_acceleration * particles[i].dir.m128_f32[2] * timerPtr->_deltaTime;
				}
				else {
					particles[i].velocity.m128_f32[2] = 0.f;
				}
			}

			particles[i].pos.m128_f32[0] = particles[i].pos.m128_f32[0] + particles[i].velocity.m128_f32[0] * timerPtr->_deltaTime; // x?Ñ±Î∂? ?ù¥?èô
			particles[i].velocity.m128_f32[1] = particles[i].velocity.m128_f32[1] - 5.f * timerPtr->_deltaTime; // Ï§ëÎ†•Í∞??Üç?èÑ?óê ?ùò?ïú ?ÇòÏ§ëÏÜç?èÑ
			particles[i].pos.m128_f32[1] = particles[i].pos.m128_f32[1] + particles[i].velocity.m128_f32[1] * timerPtr->_deltaTime; // y?Ñ±Î∂? ?ù¥?èô
			particles[i].pos.m128_f32[2] = particles[i].pos.m128_f32[2] + particles[i].velocity.m128_f32[2] * timerPtr->_deltaTime; // z?Ñ±Î∂? ?ù¥?èô
			XMStoreFloat4x4(&_transform.world, XMMatrixRotationX(-atan2f(cameraPtr->pos.m128_f32[1] - particles[i].pos.m128_f32[1], sqrt(pow(cameraPtr->pos.m128_f32[0] - particles[i].pos.m128_f32[0], 2) + pow(cameraPtr->pos.m128_f32[2] - particles[i].pos.m128_f32[2], 2))))
				* XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - particles[i].pos.m128_f32[0], cameraPtr->pos.m128_f32[2] - particles[i].pos.m128_f32[2]))
				* XMMatrixTranslation(particles[i].pos.m128_f32[0], particles[i].pos.m128_f32[1], particles[i].pos.m128_f32[2])); // ?åå?ã∞?Å¥?ù¥ ?ï≠?ÉÅ Ïπ¥Î©î?ùºÎ•? Î∞îÎùºÎ≥¥Í∏∞
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			particles[i].curTime += 0.001f;

			if (particles[i].lifeTime < particles[i].curTime)
			{
				particles[i].alive = 0;
			}

			cmdList->SetPipelineState(psoPtr->_gsPipelineState.Get());
			cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			cmdList->IASetVertexBuffers(0, 1, &vertexBufferPtr->_particleVertexBufferView);
			cmdList->IASetIndexBuffer(&indexBufferPtr->_particleIndexBufferView);
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				texturePtr->_srvHandle = texturePtr->_srvHeap->GetCPUDescriptorHandleForHeapStart();
				texturePtr->_srvHandle.Offset(6, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
				descHeapPtr->CopyDescriptor(texturePtr->_srvHandle, 5, devicePtr);
			}
			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(indexBufferPtr->_particleIndexCount, 1, 0, 0, 0);
		}
	}
#pragma endregion

	::WaitForSingleObject(_excuteEvent, INFINITE);
	SetEvent(_renderEvent);

	

	//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // »≠ÔøΩÔøΩ ÔøΩÔøΩÔø?

	cmdList->ResourceBarrier(1, &barrier2);
	cmdList->Close();

	

	ID3D12CommandList* cmdListArr[] = { cmdList.Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	d11Ptr->RenderUI(i_now_render_index);
	d11Ptr->LateRenderUI(_test_ui_vector);

	D2D1_RECT_F _tmp = D2D1::RectF(0, 620.0f, 1280.f, 720.f);
	WCHAR text[128];
	wsprintf(text, L"HP : %d", playerArr[0]._hp);
	d11Ptr->draw_text(text, _tmp);
	_tmp.left = 1100;
	_tmp.right = 1280;
	for (int i = 1; i < PLAYERMAX; ++i) {
		if (playerArr[i]._on == true)
		{
			wsprintf(text, L"HP : %d", playerArr[i]._hp);
		}
		else {
			wsprintf(text, L"WAIT PLAYER");
		}
		_tmp.bottom = 720 / 2 - 50 + 25 * i;
		_tmp.top = _tmp.bottom - 25;
		d11Ptr->draw_text(text, _tmp);
	}

	d11Ptr->ExcuteUI(i_now_render_index);
	swapChainPtr->_swapChain->Present(0, 0);
	
	cmdQueuePtr->WaitSync();

	swapChainPtr->_backBufferIndex = (swapChainPtr->_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;

	SetEvent(_excuteEvent);
}

void DxEngine::Make_Scene()
{	
	// arrScene[SceneTag::Title] = new TitleScene();	// ÔøΩÃ∑ÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔø? ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ.
	//ÔøΩÃ∑ÔøΩÔøΩÔøΩ ÔøΩÔøΩÔø? ÔøΩﬂ∞ÔøΩÔøΩ’¥œ¥ÔøΩ.
	arrScene[SCENE::SceneTag::test_scene] = new TestScene(SCENE::SceneTag::test_scene, this);
}

void DxEngine::ChangeScene(SCENE::SceneTag tag)
{
	m_pCurrScene = arrScene[tag];
}
