#include "DxEngine.h"
#include "Device.h"
#include "TestScene.h"

void DxEngine::Init(WindowInfo windowInfo)
{

	//∫‰∆˜∆Æ π◊ ¡§∫∏ ºº∆√
	_viewport = { 0, 0, static_cast<FLOAT>(windowInfo.ClientWidth), static_cast<FLOAT>(windowInfo.ClientHeight), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight);

	//DX√ ±‚»≠
	devicePtr->CreateDevice();
	cmdQueuePtr->CreateCmdListAndCmdQueue(devicePtr);
	swapChainPtr->DescriptAndCreateSwapChain(windowInfo, devicePtr, cmdQueuePtr);
	rtvPtr->CreateRTV(devicePtr, swapChainPtr);
	cameraPtr->TransformProjection(windowInfo); //¿©µµøÏ ≈©±‚ ±‚¡ÿ¿∏∑Œ ƒ´∏ﬁ∂Û º≥¡§
	rootSignaturePtr->CreateRootSignature(devicePtr);
	constantBufferPtr->CreateConstantBuffer(CONSTANT_COUNT, CONSTANT_COUNT, devicePtr);
	constantBufferPtr->CreateView(devicePtr);
	descHeapPtr->CreateDescTable(CONSTANT_COUNT, devicePtr);
	shadowmapPtr->CreateShadowMap(devicePtr->_device.Get(), 2048, 2048);

	
	d11Ptr->init(this, windowInfo);
	d11Ptr->LoadPipeline();

	d11Ptr->Loading_info();

	cout << "complite Init ptr" << endl;

	timerPtr->InitTimer();
	logicTimerPtr->InitTimer();

	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	RECT rect = { 0, 0, windowInfo.ClientWidth, windowInfo.ClientHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(windowInfo.hwnd, 0, 100, 100, windowInfo.ClientWidth, windowInfo.ClientHeight, 0);
	dsvPtr->CreateDSV(DXGI_FORMAT_D32_FLOAT, windowInfo, devicePtr);
	srand((unsigned int)time(NULL));
	
	inputPtr->Init(windowInfo); 

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
	cout << "late init" << endl;

	testCube.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	testCube.Init("../Resources/TestCube.txt", ObjectType::GeneralObjects);
	testCube.Add_texture(L"..\\Resources\\Texture\\Atlass_albedo.tga");
	testCube.Make_SRV();
	testCube.CreatePSO(L"..\\Bricks.hlsl");

	hp_bar.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	hp_bar.Init("../Resources/Floor.txt", ObjectType::GeneralObjects);
	hp_bar.Add_texture(L"..\\Resources\\Texture\\hp.jpg");
	hp_bar.Make_SRV();
	hp_bar.CreatePSO(L"..\\HPBar.hlsl");

	/*key.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	key.Init("../Resources/rust_key.txt", ObjectType::GeneralObjects);
	key.Add_texture(L"..\\Resources\\Texture\\hp.jpg");
	key.Make_SRV();
	key.CreatePSO(L"..\\Color.hlsl");*/

	skybox.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	skybox.Init("../Resources/SkySphere.txt", ObjectType::SkyBox);
	skybox.Add_texture(L"..\\Resources\\Texture\\Sky.jpg");
	skybox.Make_SRV();
	skybox.CreatePSO(L"..\\SkySphere.hlsl");

	
	if (playerArr[0]._character_num == 0) {
		playerArr[0]._move_speed = 10.f;
		inputPtr->move_speed = 10.f;
	}		
	else {
		playerArr[0]._move_speed = 7.f;
		inputPtr->move_speed = 7.f;
	}
		
	
	
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

	player_Mika_Astro_B_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_Mika_Astro_B_asset.Init("../Resources/Mika_Astro_B.txt", ObjectType::AnimationObjects);
	player_Mika_Astro_B_asset.Add_texture(L"..\\Resources\\Texture\\P05re_Cloth_B_u1.png");
	player_Mika_Astro_B_asset.Make_SRV();
	player_Mika_Astro_B_asset.CreatePSO();

	player_Mika_Hair_B_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_Mika_Hair_B_asset.Init("../Resources/Mika_Hair_B.txt", ObjectType::AnimationObjects);
	player_Mika_Hair_B_asset.Add_texture(L"..\\Resources\\Texture\\P05_hair_A_u1.png");
	player_Mika_Hair_B_asset.Make_SRV();
	player_Mika_Hair_B_asset.CreatePSO();

	player_Mika_Body_Astro_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_Mika_Body_Astro_asset.Init("../Resources/Mika_Body_Astro.txt", ObjectType::AnimationObjects);
	player_Mika_Body_Astro_asset.Add_texture(L"..\\Resources\\Texture\\P05re_skin_u1.png");
	player_Mika_Body_Astro_asset.Add_texture(L"..\\Resources\\Texture\\P05_Astro_C_u1.png");
	player_Mika_Body_Astro_asset.Make_SRV();
	player_Mika_Body_Astro_asset.CreatePSO();

	player_Mika_Sword_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	player_Mika_Sword_asset.Init("../Resources/Mika_Sword.txt", ObjectType::AnimationObjects);
	player_Mika_Sword_asset.Add_texture(L"..\\Resources\\Texture\\T_Heavy_Full_Metal_Sword_AlbedoTransparency.png");
	player_Mika_Sword_asset.Make_SRV();
	player_Mika_Sword_asset.CreatePSO();

	npc_asset.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	npc_asset.Init("../Resources/OrangeSpider.txt", ObjectType::AnimationObjects);
	npc_asset.Add_texture(L"..\\Resources\\Texture\\NPCSpider_DefaultMaterial_AlbedoTransparency.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_paint_black_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Add_texture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png");
	npc_asset.Make_SRV();
	npc_asset.CreatePSO();

	switch (Scene_num)
	{
	case 0:
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

		stage0_map.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		stage0_map.Init("../Resources/stage0_map.txt", ObjectType::GeneralObjects);
		stage0_map.Add_texture(L"..\\Resources\\Texture\\sample.png");
		stage0_map.Add_texture(L"..\\Resources\\Texture\\Back_wall.png");
		stage0_map.Add_texture(L"..\\Resources\\Texture\\Front_wall.png");
		stage0_map.Add_texture(L"..\\Resources\\Texture\\Right_wall.png");
		stage0_map.Add_texture(L"..\\Resources\\Texture\\Left_wall.png");
		stage0_map.Make_SRV();
		stage0_map.CreatePSO(L"..\\Bricks.hlsl");

		boss_obj._final_transforms.resize(boss._animationPtr->mBoneHierarchy.size());
		boss_obj._transform.y += 1.f;
		break;
	case 1:
	{
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

		InitMeshAsset(map_asset, ObjectType::GeneralObjects, "../Resources/Stage1_Wall_Test.txt", L"..\\Resources\\Texture\\bricks.dds", L"..\\Bricks.hlsl");
		InitMeshAsset(floor, ObjectType::GeneralObjects, "../Resources/Floor.txt", L"..\\Resources\\Texture\\Floor.jpg", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube, ObjectType::GeneralObjects, "../Resources/Tube.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Bricks.hlsl");
		InitMeshAsset(barrel, ObjectType::GeneralObjects, "../Resources/barrel.txt", L"..\\Resources\\Texture\\barrel.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Box, ObjectType::GeneralObjects, "../Resources/Box.txt", L"..\\Resources\\Texture\\Box_Sci-fi_box_mat_AlbedoTransparency.tga", L"..\\Bricks.hlsl");
		InitMeshAsset(NeonCrate_0, ObjectType::GeneralObjects, "../Resources/NeonCrate_0.txt", L"..\\Resources\\Texture\\Box_Sci-fi_box_mat_AlbedoTransparency.tga", L"..\\Bricks.hlsl");
		InitMeshAsset(Clotch, ObjectType::GeneralObjects, "../Resources/Clotch.txt", L"..\\Resources\\Texture\\diff.png", L"..\\Bricks.hlsl");
		InitMeshAsset(tank, ObjectType::GeneralObjects, "../Resources/tank.txt", L"..\\Resources\\Texture\\Tank_Color.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Plane002, ObjectType::GeneralObjects, "../Resources/Plane002.txt", L"..\\Resources\\Texture\\mettile1_diff.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Grid_Metal_tile, ObjectType::GeneralObjects, "../Resources/Grid_Metal_tile.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");
		InitMeshAsset(Plate_mettal_wall_HQ__2_, ObjectType::GeneralObjects, "../Resources/Plate_mettal_wall_HQ__2_.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");
		InitMeshAsset(Cube, ObjectType::GeneralObjects, "../Resources/Cube.txt", L"..\\Resources\\Texture\\mettile1_diff.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Grid_Metal_door, ObjectType::GeneralObjects, "../Resources/Grid_Metal_door.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");

		char a[] = "../Resources/pillar0.txt";
		for (int i = 0; i < 10; ++i)
		{
			pillar[i].Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
			pillar[i].Init(a, ObjectType::VertexAnimationObjects);
			pillar[i].Add_texture(L"..\\Resources\\Texture\\bricks.dds");
			pillar[i].Make_SRV();
			pillar[i].CreatePSO(L"..\\Color.hlsl");
			++a[19];
		}
		char b[] = "../Resources/pillar10.txt";
		for (int i = 10; i < 20; ++i)
		{
			pillar[i].Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
			pillar[i].Init(b, ObjectType::VertexAnimationObjects);
			pillar[i].Add_texture(L"..\\Resources\\Texture\\bricks.dds");
			pillar[i].Make_SRV();
			pillar[i].CreatePSO(L"..\\Color.hlsl");
			++b[20];
		}

		color_pattern.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		color_pattern.Init("../Resources/Floor.txt", ObjectType::GeneralObjects);
		color_pattern.Add_texture(L"..\\Resources\\Texture\\spider_paint_Red_BaseColor_Eye.jpg");
		color_pattern.Add_texture(L"..\\Resources\\Texture\\spider_paint_Blue_Color_Eye.jpg");
		color_pattern.Add_texture(L"..\\Resources\\Texture\\spider_paint_Green_Color_Eye.png");
		color_pattern.Add_texture(L"..\\Resources\\Texture\\spider_paint_White_Color_Eye.png");
		color_pattern.Make_SRV();
		color_pattern.CreatePSO(L"..\\ColorPattern.hlsl");

		ImportMapdata("../Resources/MapData1.txt", _map_data);
		ImportCollisionObjectsData("../Resources/CollisionMapData1.txt", bounding_boxes);

		// boss1 bounding box
		for (int i = 0; i < 8; ++i)
		{
			boss_col[i].Extents = XMFLOAT3(0.5f, 2.f, 0.5f);
			boss_col[i].Center = XMFLOAT3(boss_obj._transform.x, boss_obj._transform.y, boss_obj._transform.z);
		}

		boss_obj._final_transforms.resize(boss._animationPtr->mBoneHierarchy.size());
		boss_obj._transform.y += 1.f;
		for (MapData& data : _map_data)
		{
			if (data.mesh_type.compare("Grid_Metal_door") == 0)
				doorPos0[Scene_num - 1] = data.pos;
		}
		
		break;
	}
	case 2:
	{
		boss2.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		boss2.Init("../Resources/Boss2.txt", ObjectType::AnimationObjects);
		boss2.Add_texture(L"..\\Resources\\Texture\\Robot_01_Base_AlbedoTransparency.png");
		boss2.Add_texture(L"..\\Resources\\Texture\\Robot_01_Base_AlbedoTransparency.png");
		boss2.Make_SRV();
		boss2.CreatePSO();

		boss2Skill.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		boss2Skill.Init("../Resources/Boss2Skill.txt", ObjectType::GeneralObjects);
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk10.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk11.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk12.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk13.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk14.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk20.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk21.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk22.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk23.png");
		boss2Skill.Add_texture(L"..\\Resources\\Texture\\Stage2\\rect_atk24.png");
		boss2Skill.Make_SRV();
		boss2Skill.CreatePSO(L"..\\Bricks.hlsl");

		boss2_skill_circle.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		boss2_skill_circle.Init("../Resources/Boss2Skill.txt", ObjectType::GeneralObjects);
		boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\Stage2\\circle_heal000.png");
		boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\Stage2\\circle_heal001.png");
		boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\Stage2\\circle_heal002.png");
		boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\Stage2\\circle_heal003.png");
		boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\Stage2\\circle_heal004.png");
		boss2_skill_circle.Make_SRV();
		boss2_skill_circle.CreatePSO(L"..\\Circle.hlsl");

		InitMeshAsset(floor, ObjectType::GeneralObjects, "../Resources/Floor.txt", L"..\\Resources\\Texture\\Floor.jpg", L"..\\Bricks.hlsl");
		InitMeshAsset(Grid_Metal_tile, ObjectType::GeneralObjects, "../Resources/Grid_Metal_tile.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");
		InitMeshAsset(Plate_mettal_wall_HQ__2_, ObjectType::GeneralObjects, "../Resources/Plate_mettal_wall_HQ__2_.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");
		InitMeshAsset(Cube, ObjectType::GeneralObjects, "../Resources/Cube.txt", L"..\\Resources\\Texture\\mettile1_diff.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Grid_Metal_door, ObjectType::GeneralObjects, "../Resources/Grid_Metal_door.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");

		InitMeshAsset(Wall_Door_4m_A, ObjectType::GeneralObjects, "../Resources/Wall_Door_4m_A.txt", L"..\\Resources\\Texture\\Stage2\\Walls_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_1m_A, ObjectType::GeneralObjects, "../Resources/Wall_Line_1m_A.txt", L"..\\Resources\\Texture\\Stage2\\Walls_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_2m_A, ObjectType::GeneralObjects, "../Resources/Wall_Line_2m_A.txt", L"..\\Resources\\Texture\\Stage2\\Walls_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_4m_A, ObjectType::GeneralObjects, "../Resources/Wall_Line_4m_A.txt", L"..\\Resources\\Texture\\Stage2\\Walls_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Turn_L_Out_A, ObjectType::GeneralObjects, "../Resources/Wall_Turn_L_Out_A.txt", L"..\\Resources\\Texture\\Stage2\\Walls_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Win_4m_A, ObjectType::GeneralObjects, "../Resources/Wall_Win_4m_A.txt", L"..\\Resources\\Texture\\Stage2\\Walls_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Trap_A_2x2m, ObjectType::GeneralObjects, "../Resources/Trap_A_2x2m.txt", L"..\\Resources\\Texture\\Stage2\\Trap.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Trap_A_4x2m, ObjectType::GeneralObjects, "../Resources/Trap_A_4x2m.txt", L"..\\Resources\\Texture\\Stage2\\Trap.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Trap_A_Stairs, ObjectType::GeneralObjects, "../Resources/Trap_A_Stairs.txt", L"..\\Resources\\Texture\\Stage2\\Trap.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Column_BB_6m, ObjectType::GeneralObjects, "../Resources/Column_BB_6m.txt", L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_1m, ObjectType::GeneralObjects, "../Resources/Railings_A_1m.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_2m, ObjectType::GeneralObjects, "../Resources/Railings_A_2m.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_4m, ObjectType::GeneralObjects, "../Resources/Railings_A_4m.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_Turn_L_In, ObjectType::GeneralObjects, "../Resources/Railings_A_Turn_L_In.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_Turn_L_Out, ObjectType::GeneralObjects, "../Resources/Railings_A_Turn_L_Out.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Beam_B_4m, ObjectType::GeneralObjects, "../Resources/Beam_B_4m.txt", L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Column_A_2m, ObjectType::GeneralObjects, "../Resources/Column_A_2m.txt", L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Column_B_6m, ObjectType::GeneralObjects, "../Resources/Column_B_6m.txt", L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Console_B, ObjectType::GeneralObjects, "../Resources/Console_B.txt", L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png", L"..\\Bricks.hlsl");
		InitMeshAsset(FireStair_Base, ObjectType::GeneralObjects, "../Resources/FireStair_Base.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(FireStair_Cover, ObjectType::GeneralObjects, "../Resources/FireStair_Cover.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(FireStair_EndTop, ObjectType::GeneralObjects, "../Resources/FireStair_EndTop.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_A_1m, ObjectType::GeneralObjects, "../Resources/Tube_Line_A_1m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_A_2m, ObjectType::GeneralObjects, "../Resources/Tube_Line_A_2m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_A_4m, ObjectType::GeneralObjects, "../Resources/Tube_Line_A_4m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_A_6m, ObjectType::GeneralObjects, "../Resources/Tube_Line_A_6m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_B_1m, ObjectType::GeneralObjects, "../Resources/Tube_Line_B_1m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_B_2m, ObjectType::GeneralObjects, "../Resources/Tube_Line_B_2m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_B_4m, ObjectType::GeneralObjects, "../Resources/Tube_Line_B_4m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_B_6m, ObjectType::GeneralObjects, "../Resources/Tube_Line_B_6m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Turn_L_A, ObjectType::GeneralObjects, "../Resources/Tube_Turn_L_A.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Turn_L_B, ObjectType::GeneralObjects, "../Resources/Tube_Turn_L_B.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Door_4m_B, ObjectType::GeneralObjects, "../Resources/Wall_Door_4m_B.txt", L"..\\Resources\\Texture\\Stage2\\Walls_B.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_2m_B, ObjectType::GeneralObjects, "../Resources/Wall_Line_2m_B.txt", L"..\\Resources\\Texture\\Stage2\\Walls_B.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_2m_D, ObjectType::GeneralObjects, "../Resources/Wall_Line_2m_D.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_2m_¨≥, ObjectType::GeneralObjects, "../Resources/Wall_Line_2m_¨≥.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_4m_B, ObjectType::GeneralObjects, "../Resources/Wall_Line_4m_B.txt", L"..\\Resources\\Texture\\Stage2\\Walls_B.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_4m_D, ObjectType::GeneralObjects, "../Resources/Wall_Line_4m_D.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_4m_¨≥, ObjectType::GeneralObjects, "../Resources/Wall_Line_4m_¨≥.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Turn_L_In_A, ObjectType::GeneralObjects, "../Resources/Wall_Turn_L_In_A.txt", L"..\\Resources\\Texture\\Stage2\\Walls_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Turn_L_In_B, ObjectType::GeneralObjects, "../Resources/Wall_Turn_L_In_B.txt", L"..\\Resources\\Texture\\Stage2\\Walls_B.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Turn_L_In_¨≥, ObjectType::GeneralObjects, "../Resources/Wall_Turn_L_In_¨≥.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Turn_L_In_D, ObjectType::GeneralObjects, "../Resources/Wall_Turn_L_In_D.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Turn_L_Out_B, ObjectType::GeneralObjects, "../Resources/Wall_Turn_L_Out_B.txt", L"..\\Resources\\Texture\\Stage2\\Walls_B.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Turn_L_Out_D, ObjectType::GeneralObjects, "../Resources/Wall_Turn_L_Out_D.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Turn_L_Out_¨≥, ObjectType::GeneralObjects, "../Resources/Wall_Turn_L_Out_¨≥.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Win_4m_B, ObjectType::GeneralObjects, "../Resources/Wall_Win_4m_B.txt", L"..\\Resources\\Texture\\Stage2\\Walls_B.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Win_4m_C, ObjectType::GeneralObjects, "../Resources/Wall_Win_4m_C.txt", L"..\\Resources\\Texture\\Stage2\\Walls_C.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Environment_A_Base, ObjectType::GeneralObjects, "../Resources/Environment_A_Base.txt", L"..\\Resources\\Texture\\Stage2\\Environment_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Environment_A_Middle, ObjectType::GeneralObjects, "../Resources/Environment_A_Middle.txt", L"..\\Resources\\Texture\\Stage2\\Environment_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Environment_A_Top, ObjectType::GeneralObjects, "../Resources/Environment_A_Top.txt", L"..\\Resources\\Texture\\Stage2\\Environment_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Environment_A_Top_End, ObjectType::GeneralObjects, "../Resources/Environment_A_Top_End.txt", L"..\\Resources\\Texture\\Stage2\\Environment_A.png", L"..\\Bricks.hlsl");

		ImportMapdata("../Resources/MapData2.txt", _map_data2);
		ImportCollisionObjectsData("../Resources/CollisionMapData2.txt", bounding_boxes2);

		boss2Skill._tex._srvHandle = boss2Skill._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
		boss2_skill_circle._tex._srvHandle = boss2_skill_circle._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

		boss_obj._final_transforms.resize(boss2._animationPtr->mBoneHierarchy.size());
		boss_obj._transform.y += 1.f;
		for (const MapData& data : _map_data2)
		{
			if (data.mesh_type.compare("Grid_Metal_door") == 0)
				doorPos0[Scene_num - 1] = data.pos;
		}

		break;
	}
	case 3:
	{
		boss3_Body.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		boss3_Body.Init("../Resources/boss3_Body.txt", ObjectType::AnimationObjects);
		boss3_Body.Add_texture(L"..\\Resources\\Texture\\Stage3\\P03_Skin_gt.png");
		boss3_Body.Make_SRV();
		boss3_Body.CreatePSO();

		boss3_Cloth_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		boss3_Cloth_B.Init("../Resources/boss3_Cloth_B.txt", ObjectType::AnimationObjects);
		boss3_Cloth_B.Add_texture(L"..\\Resources\\Texture\\Stage3\\P03_Cloth_B.png");
		boss3_Cloth_B.Make_SRV();
		boss3_Cloth_B.CreatePSO();

		boss3_Cloth_C2.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		boss3_Cloth_C2.Init("../Resources/boss3_Cloth_C2.txt", ObjectType::AnimationObjects);
		boss3_Cloth_C2.Add_texture(L"..\\Resources\\Texture\\Stage3\\P03_Cloth_C2_gt.png");
		boss3_Cloth_C2.Make_SRV();
		boss3_Cloth_C2.CreatePSO();

		boss3_Hair_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		boss3_Hair_A.Init("../Resources/boss3_Hair_A.txt", ObjectType::AnimationObjects);
		boss3_Hair_A.Add_texture(L"..\\Resources\\Texture\\Stage3\\P03_Hair_A_gt.png");
		boss3_Hair_A.Make_SRV();
		boss3_Hair_A.CreatePSO();

		boss3_Mask.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
		boss3_Mask.Init("../Resources/boss3_Mask.txt", ObjectType::AnimationObjects);
		boss3_Mask.Add_texture(L"..\\Resources\\Texture\\Stage3\\P03_Mask_gt.png");
		boss3_Mask.Make_SRV();
		boss3_Mask.CreatePSO();

		InitMeshAsset(floor, ObjectType::GeneralObjects, "../Resources/Floor.txt", L"..\\Resources\\Texture\\Floor.jpg", L"..\\Bricks.hlsl");
		InitMeshAsset(Grid_Metal_tile, ObjectType::GeneralObjects, "../Resources/Grid_Metal_tile.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");
		InitMeshAsset(Plate_mettal_wall_HQ__2_, ObjectType::GeneralObjects, "../Resources/Plate_mettal_wall_HQ__2_.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");
		InitMeshAsset(Cube, ObjectType::GeneralObjects, "../Resources/Cube.txt", L"..\\Resources\\Texture\\mettile1_diff.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Grid_Metal_door, ObjectType::GeneralObjects, "../Resources/Grid_Metal_door.txt", L"..\\Resources\\Texture\\Atlass_albedo.tga", L"..\\Wall.hlsl");

		InitMeshAsset(Column_BB_6m, ObjectType::GeneralObjects, "../Resources/Column_BB_6m.txt", L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_1m, ObjectType::GeneralObjects, "../Resources/Railings_A_1m.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_4m, ObjectType::GeneralObjects, "../Resources/Railings_A_4m.txt", L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Trap_A_2x2m, ObjectType::GeneralObjects, "../Resources/Trap_A_2x2m.txt", L"..\\Resources\\Texture\\Stage2\\Trap.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Line_B_1m, ObjectType::GeneralObjects, "../Resources/Tube_Line_B_1m.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Tube_Turn_L_B, ObjectType::GeneralObjects, "../Resources/Tube_Turn_L_B.txt", L"..\\Resources\\Texture\\Stage2\\Tubes.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_Line_4m_A, ObjectType::GeneralObjects, "../Resources/Wall_Line_4m_A.txt", L"..\\Resources\\Texture\\Stage2\\Walls_A.png", L"..\\Bricks.hlsl");

		InitMeshAsset(Ferm_B_6m, ObjectType::GeneralObjects, "../Resources/Ferm_B_6m.txt", L"..\\Resources\\Texture\\Stage3\\ferm_Karkas.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Floor_Cross_A, ObjectType::GeneralObjects, "../Resources/Floor_Cross_A.txt", L"..\\Resources\\Texture\\Stage3\\Floor_Cross_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Floor_O_4x4m, ObjectType::GeneralObjects, "../Resources/Floor_O_4x4m.txt", L"..\\Resources\\Texture\\Stage3\\Floor_O_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_4m_Block_2m, ObjectType::GeneralObjects, "../Resources/Railings_A_4m_Block_2m.txt", L"..\\Resources\\Texture\\Stage3\\Railings_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_4m_Col_Mid_2m, ObjectType::GeneralObjects, "../Resources/Railings_A_4m_Col_Mid_2m.txt", L"..\\Resources\\Texture\\Stage3\\Railings_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_4m_Col_Top, ObjectType::GeneralObjects, "../Resources/Railings_A_4m_Col_Top.txt", L"..\\Resources\\Texture\\Stage3\\Railings_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_4m_Rama_4m, ObjectType::GeneralObjects, "../Resources/Railings_A_4m_Rama_4m.txt", L"..\\Resources\\Texture\\Stage3\\Railings_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_CornerIn, ObjectType::GeneralObjects, "../Resources/Railings_A_CornerIn.txt", L"..\\Resources\\Texture\\Stage3\\Railings_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Railings_A_TurnL_4m, ObjectType::GeneralObjects, "../Resources/Railings_A_TurnL_4m.txt", L"..\\Resources\\Texture\\Stage3\\Railings_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Stairs_A_Rama_Beam_1m, ObjectType::GeneralObjects, "../Resources/Stairs_A_Rama_Beam_1m.txt", L"..\\Resources\\Texture\\Stage3\\Stairs_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Stairs_A_Rama_Beam_2m, ObjectType::GeneralObjects, "../Resources/Stairs_A_Rama_Beam_2m.txt", L"..\\Resources\\Texture\\Stage3\\Stairs_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Stairs_A_Rama_Beam_4m, ObjectType::GeneralObjects, "../Resources/Stairs_A_Rama_Beam_4m.txt", L"..\\Resources\\Texture\\Stage3\\Stairs_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Stairs_A_Rama_Low, ObjectType::GeneralObjects, "../Resources/Stairs_A_Rama_Low.txt", L"..\\Resources\\Texture\\Stage3\\Stairs_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Stairs_A_Rama_Mid, ObjectType::GeneralObjects, "../Resources/Stairs_A_Rama_Mid.txt", L"..\\Resources\\Texture\\Stage3\\Stairs_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Stairs_A_TrapConnector, ObjectType::GeneralObjects, "../Resources/Stairs_A_TrapConnector.txt", L"..\\Resources\\Texture\\Stage3\\Stairs_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Sup_Column_Base, ObjectType::GeneralObjects, "../Resources/Sup_Column_Base.txt", L"..\\Resources\\Texture\\Stage3\\Support_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Sup_Floor_O_4x2m_Block_2m, ObjectType::GeneralObjects, "../Resources/Sup_Floor_O_4x2m_Block_2m.txt", L"..\\Resources\\Texture\\Stage3\\Support_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Sup_Wall_O_2m, ObjectType::GeneralObjects, "../Resources/Sup_Wall_O_2m.txt", L"..\\Resources\\Texture\\Stage3\\Support_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Trap_4x2m, ObjectType::GeneralObjects, "../Resources/Trap_4x2m.txt", L"..\\Resources\\Texture\\Stage3\\Trap_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Trap_4x4m, ObjectType::GeneralObjects, "../Resources/Trap_4x4m.txt", L"..\\Resources\\Texture\\Stage3\\Trap_A.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_I_4m, ObjectType::GeneralObjects, "../Resources/Wall_I_4m.txt", L"..\\Resources\\Texture\\Stage3\\Wall_I_4m.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_I_4m_Door, ObjectType::GeneralObjects, "../Resources/Wall_I_4m_Door.txt", L"..\\Resources\\Texture\\Stage3\\Wall_I_Door.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_I_4m_TurnIn, ObjectType::GeneralObjects, "../Resources/Wall_I_4m_TurnIn.txt", L"..\\Resources\\Texture\\Stage3\\Wall_I_4m_TurnIn.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_O_4m, ObjectType::GeneralObjects, "../Resources/Wall_O_4m.txt", L"..\\Resources\\Texture\\Stage3\\Wall_O.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Wall_O_4m_Door, ObjectType::GeneralObjects, "../Resources/Wall_O_4m_Door.txt", L"..\\Resources\\Texture\\Stage3\\Wall_O.png", L"..\\Bricks.hlsl");
		InitMeshAsset(Plane, ObjectType::Stage3Room, "../Resources/Wall.txt", L"..\\Resources\\Texture\\bricks.dds", L"..\\Bricks.hlsl");

		ImportMapdata("../Resources/MapData3.txt", _map_data3);
		ImportCollisionObjectsData("../Resources/CollisionMapData3_1.txt", bounding_boxes3);

		boss_obj._final_transforms.resize(boss3_Body._animationPtr->mBoneHierarchy.size());
		cout << "boss3_Body._animationPtr->mBoneHierarchy.size() : " << boss3_Body._animationPtr->mBoneHierarchy.size() << endl;
		boss_obj._transform.y += 1.f;

		int count = 0;
		for (MapData& data : _map_data3)
		{
			if (data.mesh_type.compare("Grid_Metal_door") == 0)
			{
				doorPos0[Scene_num - 1 + count] = data.pos;
				count++;
			}
		}

		break;
	}
	default:
		break;
	}

	for (int i = 0; i < PLAYERMAX; ++i)
	{
		if (playerArr[i]._character_num == 0)
		{
			playerArr[i]._final_transforms.resize(player_AKI_Body_asset._animationPtr->mBoneHierarchy.size());
			playerArr[i]._weapon_final_transforms.resize(player_AKI_Sword_asset._animationPtr->mBoneHierarchy.size());
		}
		else
		{
			playerArr[i]._final_transforms.resize(player_Mika_Body_Astro_asset._animationPtr->mBoneHierarchy.size());
			playerArr[i]._weapon_final_transforms.resize(player_Mika_Sword_asset._animationPtr->mBoneHierarchy.size());
		}
	}
	
	for (int i = 0; i < NPCMAX; ++i) {
		
			npcArr[i]._final_transforms.resize(npc_asset._animationPtr->mBoneHierarchy.size());
			npcArr[i]._transform.y += 0.2f;
	}

	/*for (int i = 0; i < KEYMAX; ++i) {
		key_data[i]._transform = XMFLOAT4(180.f, 0.f, -240.f, 1.f);
		key_data[i]._key = 0;
		key_data[i]._on = true;
	}*/
	
	test.Center = XMFLOAT3(170.f, 1.f, -240.f);
	test.Extents = XMFLOAT3(1.f, 1.f, 1.f);

	XMVECTOR v{ 0, 1, 0, 0 };
	playerArr[networkPtr->myClientId]._attack.Center = XMFLOAT3(0.f, -100.f, 0.f);
	playerArr[networkPtr->myClientId]._attack.Extents = XMFLOAT3(0.1f, 0.1f, 0.1f);
	XMStoreFloat4(&playerArr[networkPtr->myClientId]._skill.Orientation, XMQuaternionRotationNormal(v, playerArr[0]._degree * XM_PI / 180.f));

	playerArr[networkPtr->myClientId]._skill.Center = XMFLOAT3(0.f, -100.f, 0.f);
	playerArr[networkPtr->myClientId]._skill.Extents = XMFLOAT3(0.1f, 0.1f, 0.1f);
	XMStoreFloat4(&playerArr[networkPtr->myClientId]._skill.Orientation, XMQuaternionRotationNormal(v, playerArr[0]._degree * XM_PI / 180.f));

	boss_collision.Center = XMFLOAT3(boss_obj._transform.x,
		boss_obj._transform.y,
		boss_obj._transform.z);
	boss_collision.Extents = XMFLOAT3(3.f, 1.5f, 3.f);

	boss_obj.boss2_skill_vec.resize(100);

	//d11Ptr->addResource(L"..\\Resources\\UserInterface\\test.png");

	ID2D1Bitmap* _i_tmp;
	_i_tmp = d11Ptr->addResource(L"..\\Resources\\UserInterface\\test.png");
	D2D1_RECT_F _tmp = D2D1::RectF(0, 0.0f, 100.f, 100.f);
	_test_ui_vector.emplace_back(_i_tmp,_tmp);

	
	d11Ptr->Late_load();

	cout << "complite late init" << endl;
	_is_loading = true;
}

void DxEngine::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
	networkPtr->ReceiveServer(playerArr, npcArr, pillars_data, boss_obj, open_door_count);
	
	if (playerArr[0]._stage3_boss_on) {
		if (playerArr[0]._stage3_boss_con == 0) {
			inputPtr->move_speed = playerArr[0]._move_speed / 2;
		}
		if (playerArr[0]._stage3_boss_con == 2) {
			inputPtr->move_speed = 0.f;
		}
	}
	else {
		inputPtr->move_speed = playerArr[0]._move_speed;
	}

	if (inputPtr->_god_mod_on) {
		inputPtr->move_speed = 50.f;
	}


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
			if (playerArr[i]._animation_state == AnimationOrder::Idle || playerArr[i]._animation_state == AnimationOrder::Walk) {
				playerArr[i]._can_attack = true;
				playerArr[i]._can_attack2 = true;
				playerArr[i]._can_attack3 = true;
			}
		}
	}
	for (int i = 0; i < NPCMAX; ++i) {
		if (npcArr[i]._on == true) {
			if (npcArr[i]._animation_state == AnimationOrder::Idle || npcArr[i]._animation_state == AnimationOrder::Walk) {
				npcArr[i]._can_attack = true;
			}
		}
	}

	if (boss_obj._on == true) {
		if (boss_obj._animation_state == AnimationOrder::Idle || boss_obj._animation_state == AnimationOrder::Walk) {
			boss_obj._can_attack = true;
		}
	}

	if (isActive)
	{
		switch (Scene_num)
		{
		case 0:
		case 1:
			if(playerArr[0]._character_num == 0)
				inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes, playerArr[0]._character_num, player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state));
			else
				inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes, playerArr[0]._character_num, player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state));
			break;
		case 2:
			if (playerArr[0]._character_num == 0)
				inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes2, playerArr[0]._character_num, player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state));
			else
				inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes2, playerArr[0]._character_num, player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state));
			break;
		case 3:
			if (playerArr[0]._character_num == 0)
				inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes3, playerArr[0]._character_num, player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state));
			else
				inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes3, playerArr[0]._character_num, player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state));
			break;
		default:
			break;
		}
		inputPtr->inputMouse(playerArr, networkPtr);
	}

	// npc bounding box
	for (OBJECT& obj : npcArr)
		obj._bounding_box.Center = XMFLOAT3(obj._transform.x, obj._transform.y, obj._transform.z);

	// boss bounding box
	boss_collision.Center = XMFLOAT3(boss_obj._transform.x,
		boss_obj._transform.y + 2.5f,
		boss_obj._transform.z);
	XMVECTOR v{ 0, 1, 0, 0 };
	XMStoreFloat4(&boss_collision.Orientation, XMQuaternionRotationNormal(v, boss_obj._degree * XM_PI / 180.f));

	// ∞¯∞› ºº∆√ π◊ ∆ƒ∆º≈¨ µø±‚»≠
	for (OBJECT& player : playerArr)
	{
		//if (player._animation_state != player._animation_state0)
		//	player._can_attack = true;

		// «√∑π¿ÃæÓ ±‚∫ª ∞¯∞› ƒ›∂Û¿Ã¥ı on off
		if (player._animation_state == AnimationOrder::Attack1
			|| player._animation_state == AnimationOrder::Attack2
			|| player._animation_state == AnimationOrder::Attack3
			|| player._animation_state == AnimationOrder::Attack4)
		{
			if ((player._character_num == 0 && player._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(player._animation_state) * 0.5f))
			{
				player._attack.Center = XMFLOAT3(player._transform.x + 1.f * cosf((-player._degree + 90.f) * XM_PI / 180.f),
					player._transform.y + 0.5f,
					player._transform.z + 1.f * sinf((-player._degree + 90.f) * XM_PI / 180.f));
				player._attack.Extents = XMFLOAT3(0.7f, 1.f, 0.7f);
				XMVECTOR v{ 0, 1, 0, 0 };
				XMStoreFloat4(&player._attack.Orientation, XMQuaternionRotationNormal(v, player._degree * XM_PI / 180.f));
			}
			else if ((player._character_num == 1 && player._animation_time_pos >= player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(player._animation_state) * 0.5f))
			{
				player._attack.Center = XMFLOAT3(player._transform.x + 1.f * cosf((-player._degree + 90.f) * XM_PI / 180.f),
					player._transform.y + 0.5f,
					player._transform.z + 1.f * sinf((-player._degree + 90.f) * XM_PI / 180.f));
				player._attack.Extents = XMFLOAT3(1.f, 1.f, 1.f);
				XMVECTOR v{ 0, 1, 0, 0 };
				XMStoreFloat4(&player._attack.Orientation, XMQuaternionRotationNormal(v, player._degree * XM_PI / 180.f));
			}
			
		}
		else
		{
			player._attack.Center.y = -100.f;
		}

		// «√∑π¿ÃæÓ Ω∫≈≥ ƒ›∂Û¿Ã¥ı on off
		if (player._animation_state == AnimationOrder::Skill)
		{
			if ((player._character_num == 0 && player._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(player._animation_state) * 0.5f))
			{
				player._skill.Center = XMFLOAT3(player._transform.x,
					player._transform.y + 0.5f,
					player._transform.z);
				player._skill.Extents = XMFLOAT3(3.f, 1.f, 3.f);
				XMVECTOR v{ 0, 1, 0, 0 };
				XMStoreFloat4(&player._skill.Orientation, XMQuaternionRotationNormal(v, player._degree* XM_PI / 180.f));
			}
			else if ((player._character_num == 1 && player._animation_time_pos >= player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(player._animation_state) * 0.5f))
			{
				player._skill.Center = XMFLOAT3(player._transform.x + 1.f * cosf((-player._degree + 90.f) * XM_PI / 180.f),
					player._transform.y + 0.5f,
					player._transform.z + 1.f * sinf((-player._degree + 90.f) * XM_PI / 180.f));
				player._skill.Extents = XMFLOAT3(1.f, 1.f, 1.f);
				XMVECTOR v{ 0, 1, 0, 0 };
				XMStoreFloat4(&player._skill.Orientation, XMQuaternionRotationNormal(v, player._degree* XM_PI / 180.f));
			}
			
		}
		else
			player._skill.Center.y = -100.f;
	}

	// ∆ƒ∆º≈¨ µø±‚»≠
	for (OBJECT& player : playerArr)
	{
		if ((player._animation_state == AnimationOrder::Attack1 ||
			playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack2 ||
			playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack3 ||
			playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack4 || 
			player._animation_state == AnimationOrder::Skill)
			&& player._can_attack2
			&& ((player._character_num == 0 && player._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(player._animation_state) * 0.5f)
				|| (player._character_num == 1 && player._animation_time_pos >= player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(player._animation_state) * 0.5f)))
		{
			for (int i = 0; i < NPCMAX; ++i)
			{
				if (npcArr[i]._on == true && player._attack.Intersects(npcArr[i]._bounding_box) && npcArr[i]._animation_state != AnimationOrder::Death)
				{
					npcArr[i]._particle_count += 100;
				}
				if (npcArr[i]._on == true && player._skill.Intersects(npcArr[i]._bounding_box) && npcArr[i]._animation_state != AnimationOrder::Death)
				{
					npcArr[i]._particle_count += 100;
				}
			}

			if (player._attack.Intersects(boss_collision) && boss_obj._animation_state != AnimationOrder::Death)
				boss_obj._particle_count += 100;
			if (Scene_num == 1)
				for (int i = 0; i < 8; ++i)
					if (player._attack.Intersects(boss_col[i]) && boss_obj._animation_state != AnimationOrder::Death)
						boss_leg[i] += 100;

			if (player._skill.Intersects(boss_collision) && boss_obj._animation_state != AnimationOrder::Death)
				boss_obj._particle_count += 100;
			if (Scene_num == 1)
				for (int i = 0; i < 8; ++i)
					if (player._skill.Intersects(boss_col[i]) && boss_obj._animation_state != AnimationOrder::Death)
						boss_leg[i] += 100;

			player._can_attack2 = false;
		}
	}

	// «√∑π¿ÃæÓ ∞¯∞› √Êµπ ∞®¡ˆ
	{
		if ((playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack1 ||
			playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack2 ||
			playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack3 || 
			playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack4 || 
			playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Skill)
			&& ((playerArr[networkPtr->myClientId]._character_num == 0 && playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f)
				|| (playerArr[networkPtr->myClientId]._character_num == 1 && playerArr[networkPtr->myClientId]._animation_time_pos >= player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f))
			&& playerArr[networkPtr->myClientId]._can_attack)
		{
			for (int i = 0; i < NPCMAX; ++i)
			{
				if (npcArr[i]._on == true && playerArr[networkPtr->myClientId]._attack.Intersects(npcArr[i]._bounding_box) && npcArr[i]._animation_state != AnimationOrder::Death)
				{
					CS_COLLISION_PACKET p;
					p.size = sizeof(p);
					p.type = CS_COLLISION;
					p.attack_type = AnimationOrder::Attack1 - 2;
					p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
					p.target_id = npcArr[i]._my_server_id;
					networkPtr->send_packet(&p);
				}
				if (npcArr[i]._on == true && playerArr[networkPtr->myClientId]._skill.Intersects(npcArr[i]._bounding_box) && npcArr[i]._animation_state != AnimationOrder::Death)
				{
					CS_COLLISION_PACKET p;
					p.size = sizeof(p);
					p.type = CS_COLLISION;
					p.attack_type = playerArr[networkPtr->myClientId]._animation_state - 2;
					p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
					p.target_id = npcArr[i]._my_server_id;
					networkPtr->send_packet(&p);
				}
			}

			if (playerArr[networkPtr->myClientId]._attack.Intersects(boss_collision) && boss_obj._animation_state != AnimationOrder::Death)
			{
				CS_COLLISION_PACKET p;
					p.size = sizeof(p);
					p.type = CS_COLLISION;
					p.attack_type = AnimationOrder::Attack1 - 2;
					p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
					p.target_id = boss_obj._my_server_id;
					networkPtr->send_packet(&p);
			}
			if (playerArr[networkPtr->myClientId]._skill.Intersects(boss_collision) && boss_obj._animation_state != AnimationOrder::Death)
			{
				CS_COLLISION_PACKET p;
				p.size = sizeof(p);
				p.type = CS_COLLISION;
				p.attack_type = playerArr[networkPtr->myClientId]._animation_state - 2;
				p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
				p.target_id = boss_obj._my_server_id;
				networkPtr->send_packet(&p);
			}
			if (Scene_num == 1)
				for (int i = 0; i < 8; ++i)
				{
					if (playerArr[networkPtr->myClientId]._attack.Intersects(boss_col[i]) && boss_obj._animation_state != AnimationOrder::Death)
					{
						CS_COLLISION_PACKET p;
						p.size = sizeof(p);
						p.type = CS_COLLISION;
						p.attack_type = AnimationOrder::Attack1 - 2;
						p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
						p.target_id = boss_obj._my_server_id;
						networkPtr->send_packet(&p);
					}
					if (playerArr[networkPtr->myClientId]._skill.Intersects(boss_col[i]) && boss_obj._animation_state != AnimationOrder::Death)
					{
						CS_COLLISION_PACKET p;
						p.size = sizeof(p);
						p.type = CS_COLLISION;
						p.attack_type = playerArr[networkPtr->myClientId]._animation_state - 2;
						p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
						p.target_id = boss_obj._my_server_id;
						networkPtr->send_packet(&p);
					}
				}
					

			playerArr[networkPtr->myClientId]._can_attack = false;
		}
	}
	
	// npc ∞¯∞› √Êµπ ∞®¡ˆ
	for (int j = 0; j < NPCMAX; ++j)
	{
		if (pow(playerArr[0]._transform.x - npcArr[j]._transform.x, 2) + pow(playerArr[0]._transform.z - npcArr[j]._transform.z, 2) <= 4.f
			&& npcArr[j]._animation_state == AnimationOrder::Attack1
			&& npcArr[j]._animation_time_pos >= npc_asset._animationPtr->GetClipEndTime(npcArr[j]._animation_state) * 0.5f
			&& npcArr[j]._can_attack)
		{
			npcArr[j]._can_attack = false;

			CS_COLLISION_PACKET p;
			p.size = sizeof(p);
			p.type = CS_COLLISION;
			p.attack_type = 0;
			p.attacker_id = npcArr[j]._my_server_id;
			p.target_id = playerArr[0]._my_server_id;
			networkPtr->send_packet(&p);

			//cout << "player" << 0 << " hp : " << playerArr[0]._hp << endl;
			//cout << "npc" << j << " hp : " << npcArr[j]._hp << endl;
		}
	}
	
	// ∫∏Ω∫ ∞¯∞› √Êµπ ∞®¡ˆ
	if (boss_obj._on == true) {
		int i = networkPtr->myClientId;
		if (pow(playerArr[i]._transform.x - boss_obj._transform.x, 2) + pow(playerArr[i]._transform.z - boss_obj._transform.z, 2) <= 26.f) {
			if (boss_obj._animation_state == AnimationOrder::Attack1
				&& boss_obj._animation_time_pos >= npc_asset._animationPtr->GetClipEndTime(boss_obj._animation_state) * 0.5f
				&& boss_obj._can_attack) {

				boss_obj._can_attack = false;

				CS_COLLISION_PACKET p;
				p.size = sizeof(p);
				p.type = CS_COLLISION;
				p.attack_type = 1;
				p.attacker_id = boss_obj._my_server_id;
				p.target_id = playerArr[i]._my_server_id;
				networkPtr->send_packet(&p);

				//cout << "player" << i << " hp : " << playerArr[i]._hp << endl;
				//cout << "BOSS hp : " << boss_obj._hp << endl;
			}
		}
	}

	/*// key √Êµπ√≥∏Æ
	for (int i = 0; i < PLAYERMAX; ++i)
	{
		for (int j = 0; j < KEYMAX; ++j) {
			if (key_data[j]._on == true) {
				if (pow(playerArr[i]._transform.x - key_data[j]._transform.x, 2) + pow(playerArr[i]._transform.z - key_data[j]._transform.z, 2) <= 1.f)
				{
					playerArr[i]._player_color = key_data[j]._key;
					key_data[j]._on = false;


					CS_KEY_PACKET p;
					p.size = sizeof(p);
					p.type = CS_KEY;
					p.color = key_data[j]._key;
					p.key_id = key_data[j]._my_server_id;

					cout << "GET KEY : " << j << "[" << key_data[j]._my_server_id << "]" << endl;

					networkPtr->send_packet(&p);
				}
			}			
		}		
	}*/

	switch (Scene_num)
	{
	case 0:
		break;
	case 1:
	{
		// ∫∏Ω∫ ¥Ÿ∏Æ √Êµπ√≥∏Æ
		float r = 6.f;
		boss_col[0].Center = XMFLOAT3(boss_obj._transform.x + (r + 0.8f) * cosf((-boss_obj._degree - 60.f) * XM_PI / 180.f)
			, boss_obj._transform.y + 1.f
			, boss_obj._transform.z + (r + 0.8f) * sinf((-boss_obj._degree - 60.f) * XM_PI / 180.f));

		boss_col[1].Center = XMFLOAT3(boss_obj._transform.x + r * cosf((-boss_obj._degree - 35.f) * XM_PI / 180.f)
			, boss_obj._transform.y + 1.f
			, boss_obj._transform.z + r * sinf((-boss_obj._degree - 35.f) * XM_PI / 180.f));

		boss_col[2].Center = XMFLOAT3(boss_obj._transform.x + r * cosf((-boss_obj._degree - 15.f) * XM_PI / 180.f)
			, boss_obj._transform.y + 1.f
			, boss_obj._transform.z + r * sinf((-boss_obj._degree - 15.f) * XM_PI / 180.f));

		boss_col[3].Center = XMFLOAT3(boss_obj._transform.x + r * cosf((-boss_obj._degree + 15.f) * XM_PI / 180.f)
			, boss_obj._transform.y + 1.f
			, boss_obj._transform.z + r * sinf((-boss_obj._degree + 15.f) * XM_PI / 180.f));

		boss_col[4].Center = XMFLOAT3(boss_obj._transform.x + (r + 0.8f) * cosf((-boss_obj._degree - 120.f) * XM_PI / 180.f)
			, boss_obj._transform.y + 1.f
			, boss_obj._transform.z + (r + 0.8f) * sinf((-boss_obj._degree - 120.f) * XM_PI / 180.f));

		boss_col[5].Center = XMFLOAT3(boss_obj._transform.x + r * cosf((-boss_obj._degree - 145.f) * XM_PI / 180.f)
			, boss_obj._transform.y + 1.f
			, boss_obj._transform.z + r * sinf((-boss_obj._degree - 145.f) * XM_PI / 180.f));

		boss_col[6].Center = XMFLOAT3(boss_obj._transform.x + r * cosf((-boss_obj._degree - 165.f) * XM_PI / 180.f)
			, boss_obj._transform.y + 1.f
			, boss_obj._transform.z + r * sinf((-boss_obj._degree - 165.f) * XM_PI / 180.f));

		boss_col[7].Center = XMFLOAT3(boss_obj._transform.x + r * cosf((-boss_obj._degree - 195.f) * XM_PI / 180.f)
			, boss_obj._transform.y + 1.f
			, boss_obj._transform.z + r * sinf((-boss_obj._degree - 195.f) * XM_PI / 180.f));

		// ±‚µ’ √Êµπ√≥∏Æ
		for (int i = 0; i < CubeMax; ++i)
		{
			if (playerArr[networkPtr->myClientId]._on == true && pillars_data[i]._on == true)
			{
				if (pow(playerArr[networkPtr->myClientId]._transform.x - pillars_data[i]._transform.x, 2) + pow(playerArr[networkPtr->myClientId]._transform.z - pillars_data[i]._transform.z, 2) <= 9.f) //&& pillars_data[i]._pillar_color == playerArr[networkPtr->myClientId]._player_color
				{
					if ((playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack1)
						&& ((playerArr[networkPtr->myClientId]._character_num == 0 && playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f)
							|| (playerArr[networkPtr->myClientId]._character_num == 1 && playerArr[networkPtr->myClientId]._animation_time_pos >= player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f))
						&& playerArr[networkPtr->myClientId]._can_attack3)
					{
						playerArr[networkPtr->myClientId]._can_attack3 = false;

						CS_OBJECT_COLLISION_PACKET p;
						p.size = sizeof(p);
						p.type = CS_OBJECT_COLLISION;
						p.target_id = i;
						p.object_type = 0;
						networkPtr->send_packet(&p);
					}
				}
			}
		}

		if ((inputPtr->_states[VK_F8] == 2 || open_door_count > 0) && inputPtr->_open_door[Scene_num - 1] == false)
			inputPtr->_open_door[Scene_num - 1] = true;
		break;
	}
	case 2:
		//Ω√∞£∏∏ æ˜µ•¿Ã∆Æ «œ∞Ì ≥™ø¬¥Ÿ≥◊
		boss2_skill_logical_update(boss_obj, logicTimerPtr);
		//¿Â∆«∞˙ √Êµπ √º≈©«œ∞Ì, ∆–≈∂±Ó¡ˆ ∫∏≥ª¥¬ ∂»∂»«— æ∆¿Ã∂Û≥◊
		_atked = boss2_skill_checker(playerArr[0], boss_obj,networkPtr);



		if ((inputPtr->_states[VK_F8] == 2 || open_door_count > 0) && inputPtr->_open_door[Scene_num - 1] == false)
			inputPtr->_open_door[Scene_num - 1] = true;
		break;
	case 3:

		if ((inputPtr->_states[VK_F8] == 2 || open_door_count == 1) && inputPtr->_open_door[Scene_num - 1] == false)
			inputPtr->_open_door[Scene_num - 1] = true;
		else if ((inputPtr->_states[VK_F8] == 2 || open_door_count == 2) && inputPtr->_open_door[Scene_num - 1] == true && inputPtr->_open_door[Scene_num] == false)
			inputPtr->_open_door[Scene_num] = true;
		else if ((inputPtr->_states[VK_F8] == 2 || open_door_count == 3) && inputPtr->_open_door[Scene_num] == true && inputPtr->_open_door[Scene_num + 1] == false)
			inputPtr->_open_door[Scene_num + 1] = true;
		break;
	default:
		break;
	}
	
	if (playerArr[networkPtr->myClientId]._hp <= 0.f)
	{
		playerArr[networkPtr->myClientId]._animation_state = AnimationOrder::Death;

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
	float zoom = 3.f * _scale / 100.f;
	cameraPtr->pos = XMVectorSet(playerArr[networkPtr->myClientId]._transform.x - zoom * cosf(inputPtr->angle.x*XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId]._transform.y + 1.35f * _scale / 100.f + zoom * cosf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f),
		playerArr[networkPtr->myClientId]._transform.z - zoom * sinf(inputPtr->angle.x * XM_PI / 180.f) * sinf(XM_PI / 2.0f - inputPtr->angle.y * XM_PI / 180.f), 0.0f);
	XMVECTOR target = XMVectorSet(playerArr[networkPtr->myClientId]._transform.x, playerArr[networkPtr->myClientId]._transform.y + 1.35f * _scale / 100.f,
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
	if (_is_loading) {

	
	//∫∏∞£¿ª ¿ß«ÿº≠ ≈∏¿Ã∏”∂˚ ∞¢¡æ ∞™µÈ ºº∆√«œ¥¬ øµø™
	//ø©±‚º≠ ¿ß«ÿº≠ ∫∞µµ∑Œ ø¿∆€∑π¿Ã≈Õ ø¿πˆ∑Œµ˘ ¡ﬂ¿Ã¥œ ¬¸∞Ì«œºÕ
	for (int i = 1; i < PLAYERMAX; ++i) {
		if (playerArr[i]._on) {
			float dt = timerPtr->_deltaTime;
			playerArr[i]._delta_percent = dt / playerArr[i]._prev_delta_time;
			
			if (playerArr[i]._prev_delta_time > 0) {
				playerArr[i]._prev_delta_time -= dt;
			}
			else {
				playerArr[i]._prev_delta_time = 0;
				playerArr[i]._delta_percent = 0;
			}
			
			
			playerArr[i]._delta_transform = playerArr[i]._transform - playerArr[i]._prev_transform;
			
			playerArr[i]._delta_transform = playerArr[i]._delta_transform * playerArr[i]._delta_percent;
			
			playerArr[i]._prev_transform = playerArr[i]._prev_transform + playerArr[i]._delta_transform;
			//√ﬂ»ƒ ¿˚øÎ « ø‰
			//playerArr[i]._prev_degree = playerArr[i]._prev_degree + (playerArr[i]._prev_degree - playerArr[i]._degree) * playerArr[i]._delta_percent;
			playerArr[i]._prev_degree = playerArr[i]._degree;
		}

	}
	for (OBJECT& p : npcArr) {
		if (p._on) {
			float dt = timerPtr->_deltaTime;
			p._delta_percent = dt / p._prev_delta_time;

			if (p._prev_delta_time > 0) {
				p._prev_delta_time -= dt;
			}
			else {
				p._prev_delta_time = 0;
				p._delta_percent = 0;
			}


			p._delta_transform = p._transform - p._prev_transform;

			p._delta_transform = p._delta_transform * p._delta_percent;

			p._prev_transform = p._prev_transform + p._delta_transform;
			//√ﬂ»ƒ ¿˚øÎ « ø‰
			//p._prev_degree = p._prev_degree + (p._prev_degree - p._degree) * p._delta_percent;
			p._prev_degree = p._degree;
		}
	}
	{
		float dt = timerPtr->_deltaTime;
		boss_obj._delta_percent = dt / boss_obj._prev_delta_time;

		if (boss_obj._prev_delta_time > 0) {
			boss_obj._prev_delta_time -= dt;
		}
		else {
			boss_obj._prev_delta_time = 0;
			boss_obj._delta_percent = 0;
		}


		boss_obj._delta_transform = boss_obj._transform - boss_obj._prev_transform;

		boss_obj._delta_transform = boss_obj._delta_transform * boss_obj._delta_percent;

		boss_obj._prev_transform = boss_obj._prev_transform + boss_obj._delta_transform;
		//√ﬂ»ƒ ¿˚øÎ « ø‰
		//boss_obj._prev_degree = boss_obj._prev_degree + (boss_obj._prev_degree - boss_obj._degree) * boss_obj._delta_percent;
		boss_obj._prev_degree = boss_obj._degree;
	}
	}
}



void DxEngine::Draw_multi(WindowInfo windowInfo, int i_now_render_index)
{
	ComPtr<ID3D12CommandAllocator>		cmdAlloc = cmdQueuePtr->_arr_cmdAlloc[i_now_render_index];
	ComPtr<ID3D12GraphicsCommandList>	cmdList = cmdQueuePtr->_arr_cmdList[i_now_render_index];

	if (_is_loading) {
		//ÔøΩ÷¥œ∏ÔøΩÔøΩÃºÔøΩ
		for (int i = 0; i < PLAYERMAX; ++i)
		{
			if (playerArr[i]._character_num == 0 && playerArr[i]._on == true)
			{
				player_AKI_Body_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i], 0, playerArr[i]._character_num);
				player_AKI_Sword_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i], 1, playerArr[i]._character_num);
			}
			else if (playerArr[i]._character_num == 1 && playerArr[i]._on == true)
			{
				player_Mika_Body_Astro_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i], 0, playerArr[i]._character_num);
				player_Mika_Sword_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, playerArr[i], 1, playerArr[i]._character_num);
			}
		}
		for (int i = 0; i < NPCMAX; ++i) {
			if (npcArr[i]._on == true) {
				npc_asset.UpdateSkinnedAnimation(timerPtr->_deltaTime, npcArr[i], 0, 0);
			}
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
	if (_is_loading) {
	
#pragma region Player
		for (int i = 0; i < PLAYERMAX; ++i) //ÔøΩ√∑ÔøΩÔøΩÃæÔøΩ ÔøΩÔøΩÔøΩÔøΩ
		{
			if (playerArr[i]._on == true)
			{
				if (playerArr[i]._character_num == 0)
					DrawCharacter(cmdList, player_AKI_Body_asset, i_now_render_index, playerArr[i], i);
				else
				{
					cmdList->SetPipelineState(player_Mika_Body_Astro_asset._pipelineState.Get());
					cmdList->IASetVertexBuffers(0, 1, &player_Mika_Body_Astro_asset._vertexBufferView);
					cmdList->IASetIndexBuffer(&player_Mika_Body_Astro_asset._indexBufferView);

					{
						if (i == 0)
						{
							XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
								* XMMatrixRotationX(-XM_PI / 2.f)
								* XMMatrixRotationY(playerArr[i]._degree * XM_PI / 180.f)
								* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z));
							XMMATRIX world = XMLoadFloat4x4(&_transform.world);
							XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
						}
						else
						{
							XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
								* XMMatrixRotationX(-XM_PI / 2.f)
								* XMMatrixRotationY(playerArr[i]._prev_degree * XM_PI / 180.f)
								* XMMatrixTranslation(playerArr[i]._prev_transform.x, playerArr[i]._prev_transform.y, playerArr[i]._prev_transform.z));
							XMMATRIX world = XMLoadFloat4x4(&_transform.world);
							XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
						}

						copy(begin(playerArr[i]._final_transforms), end(playerArr[i]._final_transforms), &_transform.BoneTransforms[0]);

						int sum = 0;
						int count = 0;
						for (Subset i : player_Mika_Body_Astro_asset._animationPtr->mSubsets)
						{
							if (count == 4) {
								player_Mika_Body_Astro_asset._tex._srvHandle = player_Mika_Body_Astro_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
								player_Mika_Body_Astro_asset._tex._srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
							}
							else
								player_Mika_Body_Astro_asset._tex._srvHandle = player_Mika_Body_Astro_asset._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

							D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
							descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
							descHeapPtr->CopyDescriptor(player_Mika_Body_Astro_asset._tex._srvHandle, 5, devicePtr);
							descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
							cmdList->DrawIndexedInstanced(i.FaceCount * 3, 1, sum, 0, 0);
							sum += i.FaceCount * 3;

							count++;
						}
					}
				}
			}
		}

		// 1
		for (int i = 0; i < PLAYERMAX; ++i) //ÔøΩ√∑ÔøΩÔøΩÃæÔøΩ ÔøΩÔøΩÔøΩÔøΩ
		{
			if (playerArr[i]._on == true)
			{
				if (playerArr[i]._character_num == 0)
					DrawCharacter(cmdList, player_AKI_Astro_A_asset, i_now_render_index, playerArr[i], i);
				else
					DrawCharacter(cmdList, player_Mika_Astro_B_asset, i_now_render_index, playerArr[i], i);
			}
		}

		// 2
		for (int i = 0; i < PLAYERMAX; ++i) //ÔøΩ√∑ÔøΩÔøΩÃæÔøΩ ÔøΩÔøΩÔøΩÔøΩ
		{
			if (playerArr[i]._on == true)
			{
				if (playerArr[i]._character_num == 0)
					DrawCharacter(cmdList, player_AKI_Hair_A_asset, i_now_render_index, playerArr[i], i);
				else
					DrawCharacter(cmdList, player_Mika_Hair_B_asset, i_now_render_index, playerArr[i], i);
			}
		}

		// 3
		for (int i = 0; i < PLAYERMAX; ++i) //ÔøΩ√∑ÔøΩÔøΩÃæÔøΩ ÔøΩÔøΩÔøΩÔøΩ
		{
			if (playerArr[i]._on == true)
			{
				if (playerArr[i]._character_num == 0)
					DrawCharacter(cmdList, player_AKI_HeadPhone_asset, i_now_render_index, playerArr[i], i);
			}
		}

		// 4
		for (int i = 0; i < PLAYERMAX; ++i) //ÔøΩ√∑ÔøΩÔøΩÃæÔøΩ ÔøΩÔøΩÔøΩÔøΩ
		{
			if (playerArr[i]._on == true)
			{
				if (playerArr[i]._character_num == 0)
					DrawCharacter(cmdList, player_AKI_Sword_asset, i_now_render_index, playerArr[i], i, true);
				else
					DrawCharacter(cmdList, player_Mika_Sword_asset, i_now_render_index, playerArr[i], i, true);
			}
		}
#pragma endregion

		/*// ƒ≥∏Ø≈Õ ∞¯∞› ƒ›∂Û¿Ã¥ı
		if (playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack1
			|| playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack2
			|| playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack3
			|| playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack4)
		{
			if (playerArr[networkPtr->myClientId]._character_num == 0 && playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f)
			{
				cmdList->SetPipelineState(testCube._pipelineState.Get());
				cmdList->IASetVertexBuffers(0, 1, &testCube._vertexBufferView);
				cmdList->IASetIndexBuffer(&testCube._indexBufferView);

				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(playerArr[networkPtr->myClientId]._attack.Extents.x, playerArr[networkPtr->myClientId]._attack.Extents.y, playerArr[networkPtr->myClientId]._attack.Extents.z)
					* XMMatrixRotationQuaternion(XMLoadFloat4(&playerArr[networkPtr->myClientId]._attack.Orientation))
					* XMMatrixTranslation(playerArr[networkPtr->myClientId]._attack.Center.x, playerArr[networkPtr->myClientId]._attack.Center.y, playerArr[networkPtr->myClientId]._attack.Center.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				testCube._tex._srvHandle = testCube._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(testCube._tex._srvHandle, 5, devicePtr);

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(testCube._indexCount, 1, 0, 0, 0);
			}
			else if(playerArr[networkPtr->myClientId]._character_num == 1 && playerArr[networkPtr->myClientId]._animation_time_pos >= player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f)
			{
				cmdList->SetPipelineState(testCube._pipelineState.Get());
				cmdList->IASetVertexBuffers(0, 1, &testCube._vertexBufferView);
				cmdList->IASetIndexBuffer(&testCube._indexBufferView);

				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(playerArr[networkPtr->myClientId]._attack.Extents.x, playerArr[networkPtr->myClientId]._attack.Extents.y, playerArr[networkPtr->myClientId]._attack.Extents.z)
					* XMMatrixRotationQuaternion(XMLoadFloat4(&playerArr[networkPtr->myClientId]._attack.Orientation))
					* XMMatrixTranslation(playerArr[networkPtr->myClientId]._attack.Center.x, playerArr[networkPtr->myClientId]._attack.Center.y, playerArr[networkPtr->myClientId]._attack.Center.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				testCube._tex._srvHandle = testCube._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(testCube._tex._srvHandle, 5, devicePtr);

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(testCube._indexCount, 1, 0, 0, 0);
			}
			
		}

		// ƒ≥∏Ø≈Õ Ω∫≈≥ ƒ›∂Û¿Ã¥ı
		if (playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Skill)
		{
			if (playerArr[networkPtr->myClientId]._character_num == 0 && playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f)
			{
				cmdList->SetPipelineState(testCube._pipelineState.Get());
				cmdList->IASetVertexBuffers(0, 1, &testCube._vertexBufferView);
				cmdList->IASetIndexBuffer(&testCube._indexBufferView);

				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(playerArr[networkPtr->myClientId]._skill.Extents.x, playerArr[networkPtr->myClientId]._skill.Extents.y, playerArr[networkPtr->myClientId]._skill.Extents.z)
					* XMMatrixRotationQuaternion(XMLoadFloat4(&playerArr[networkPtr->myClientId]._skill.Orientation))
					* XMMatrixTranslation(playerArr[networkPtr->myClientId]._skill.Center.x, playerArr[networkPtr->myClientId]._skill.Center.y, playerArr[networkPtr->myClientId]._skill.Center.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				testCube._tex._srvHandle = testCube._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(testCube._tex._srvHandle, 5, devicePtr);

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(testCube._indexCount, 1, 0, 0, 0);
			}
			else if (playerArr[networkPtr->myClientId]._character_num == 1 && playerArr[networkPtr->myClientId]._animation_time_pos >= player_Mika_Body_Astro_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f)
			{
				cmdList->SetPipelineState(testCube._pipelineState.Get());
				cmdList->IASetVertexBuffers(0, 1, &testCube._vertexBufferView);
				cmdList->IASetIndexBuffer(&testCube._indexBufferView);

				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(playerArr[networkPtr->myClientId]._skill.Extents.x, playerArr[networkPtr->myClientId]._skill.Extents.y, playerArr[networkPtr->myClientId]._skill.Extents.z)
					* XMMatrixRotationQuaternion(XMLoadFloat4(&playerArr[networkPtr->myClientId]._skill.Orientation))
					* XMMatrixTranslation(playerArr[networkPtr->myClientId]._skill.Center.x, playerArr[networkPtr->myClientId]._skill.Center.y, playerArr[networkPtr->myClientId]._skill.Center.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

				D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
				descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
				testCube._tex._srvHandle = testCube._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
				descHeapPtr->CopyDescriptor(testCube._tex._srvHandle, 5, devicePtr);

				descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
				cmdList->DrawIndexedInstanced(testCube._indexCount, 1, 0, 0, 0);
			}
			
		}

		// ∫∏Ω∫ ¥Ÿ∏Æ ƒ›∂Û¿Ã¥ı
		for (int i = 0; i<8; ++i)
		{
			cmdList->SetPipelineState(testCube._pipelineState.Get());
			cmdList->IASetVertexBuffers(0, 1, &testCube._vertexBufferView);
			cmdList->IASetIndexBuffer(&testCube._indexBufferView);

			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(boss_col[i].Extents.x, boss_col[i].Extents.y, boss_col[i].Extents.z)
				* XMMatrixTranslation(boss_col[i].Center.x, boss_col[i].Center.y, boss_col[i].Center.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			testCube._tex._srvHandle = testCube._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
			descHeapPtr->CopyDescriptor(testCube._tex._srvHandle, 5, devicePtr);

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(testCube._indexCount, 1, 0, 0, 0);
		}

		// ∫∏Ω∫ ∏ˆ≈Î ƒ›∂Û¿Ã¥ı
		{
			cmdList->SetPipelineState(testCube._pipelineState.Get());
			cmdList->IASetVertexBuffers(0, 1, &testCube._vertexBufferView);
			cmdList->IASetIndexBuffer(&testCube._indexBufferView);

			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(boss_collision.Extents.x, boss_collision.Extents.y, boss_collision.Extents.z)
				* XMMatrixRotationQuaternion(XMLoadFloat4(&boss_collision.Orientation))
				* XMMatrixTranslation(boss_collision.Center.x, boss_collision.Center.y, boss_collision.Center.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			testCube._tex._srvHandle = testCube._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
			descHeapPtr->CopyDescriptor(testCube._tex._srvHandle, 5, devicePtr);

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(testCube._indexCount, 1, 0, 0, 0);
		}*/

		switch (Scene_num)
		{
		case 0:
			Map(cmdList, stage0_map, map_asset, i_now_render_index, Scene_num);

			if (boss_obj._on == true)
			{
				XMFLOAT3 boss_scale = XMFLOAT3(800.f, 800.f, 800.f);
				float boss_default_rot_x = -XM_PI * 0.5f;
				Boss(cmdList, boss, i_now_render_index, boss_scale, boss_default_rot_x, Scene_num);
			}
			break;
		case 1: {
			Map(cmdList, floor, map_asset, i_now_render_index, Scene_num);

			if (boss_obj._on == true)
			{
				XMFLOAT3 boss_scale = XMFLOAT3(800.f, 800.f, 800.f);
				float boss_default_rot_x = -XM_PI * 0.5f;
				Boss(cmdList, boss, i_now_render_index, boss_scale, boss_default_rot_x, Scene_num);
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
						XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale * 0.001f, _scale * 0.001f, _scale * 0.001f)
							* XMMatrixRotationX(-atan2f(cameraPtr->pos.m128_f32[1] - (playerArr[i]._transform.y + 1.75f * _scale / 100.f),
								sqrt(pow(cameraPtr->pos.m128_f32[0] - playerArr[i]._transform.x, 2) + pow(cameraPtr->pos.m128_f32[2] - playerArr[i]._transform.z, 2))))
							* XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - playerArr[i]._transform.x, cameraPtr->pos.m128_f32[2] - playerArr[i]._transform.z))
							* XMMatrixTranslation(playerArr[i]._transform.x, playerArr[i]._transform.y + 1.75f * _scale / 100.f, playerArr[i]._transform.z));
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

			// pillar
			for (int i = 0; i < CubeMax; ++i)
			{
				if (pillars_data[i]._on == true)
				{
					XMVECTOR P = XMVectorSet(0.f, 0.f, 0.f, 1.f);
					XMVECTOR Q = XMVectorSet(0.f, 0.f, 0.f, 1.f);
					/*switch (pillars_data[i]._pillar_count)
					{
					case 5:
						pillars_data[i]._animation_time_pos = 0.f;
						break;
					case 4:
						pillars_data[i]._animation_time_pos = 0.025f;
						break;
					case 3:
						pillars_data[i]._animation_time_pos = 0.050f;
						break;
					case 2:
						pillars_data[i]._animation_time_pos = 0.075f;
						break;
					case 1:
						pillars_data[i]._animation_time_pos = 0.100f;
						break;
					default:
						pillars_data[i]._animation_time_pos += timerPtr->_deltaTime;
						break;
					}*/

					if (pillars_data[i]._pillar_count <= 0) {
						pillars_data[i]._animation_time_pos += timerPtr->_deltaTime;
					}
					else {
						pillars_data[i]._animation_time_pos = (10 - pillars_data[i]._pillar_count) * 0.01f;
					}
					for (MESH_ASSET& piece_of_pillar : pillar)
					{
						float scale = 3.f;

						piece_of_pillar.UpdateVertexAnimation(timerPtr->_deltaTime, pillars_data[i], P, Q);

						cmdList->SetPipelineState(piece_of_pillar._pipelineState.Get());
						cmdList->IASetVertexBuffers(0, 1, &piece_of_pillar._vertexBufferView);
						cmdList->IASetIndexBuffer(&piece_of_pillar._indexBufferView);

						XMStoreFloat4x4(&_transform.world, XMMatrixScaling(scale, scale, scale)
							* XMMatrixRotationQuaternion(Q)
							* XMMatrixTranslation(pillars_data[i]._transform.x + P.m128_f32[0] * scale, pillars_data[i]._transform.y + P.m128_f32[1] * scale, pillars_data[i]._transform.z + P.m128_f32[2] * scale));
						XMMATRIX world = XMLoadFloat4x4(&_transform.world);
						XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
						//»Ú
						if (pillars_data[i]._pillar_color == 0)
							_transform.color = XMVectorSet(1.f, 1.f, 1.f, 1.f);
						else if (pillars_data[i]._pillar_color == 1) // ∆ƒ
							_transform.color = XMVectorSet(0.f, 0.f, 1.f, 1.f);
						else if (pillars_data[i]._pillar_color == 2) // √ 
							_transform.color = XMVectorSet(0.f, 1.f, 0.f, 1.f);
						else if (pillars_data[i]._pillar_color == 3) // ª°
							_transform.color = XMVectorSet(1.f, 0.f, 0.f, 1.f);
						else
							_transform.color = XMVectorSet(1.f, 0.f, 0.f, 1.f);

						D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
						descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
						piece_of_pillar._tex._srvHandle = piece_of_pillar._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
						descHeapPtr->CopyDescriptor(piece_of_pillar._tex._srvHandle, 5, devicePtr);

						descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
						cmdList->DrawIndexedInstanced(piece_of_pillar._indexCount, 1, 0, 0, 0);
					}
				}
			}
		}
			break;
		case 2: {
			Map(cmdList, floor, map_asset, i_now_render_index, Scene_num);

			if (boss_obj._on == true)
			{
				XMFLOAT3 boss2_scale = XMFLOAT3(1.f, 1.f, 1.f);
				float boss2_default_rot_x = 0.f;
				Boss(cmdList, boss2, i_now_render_index, boss2_scale, boss2_default_rot_x, Scene_num);
			}

			// boss2Skill
			for (Boss2SkillData& boss2_skill_data : boss_obj.boss2_skill_vec)
			{
				if (boss2_skill_data.isOn)
				{
					if (boss2_skill_data.type == 0) // ø¯
						Boss2Skill(cmdList, boss2_skill_circle, i_now_render_index, boss2_skill_data);
					else // ªÁ∞¢«¸
						Boss2Skill(cmdList, boss2Skill, i_now_render_index, boss2_skill_data);
				}
			}

			//XMFLOAT3 boss2_skill_pos2 = XMFLOAT3(167.f, 0.01f, -240.f);
			//XMFLOAT3 boss2_skill_scale2 = XMFLOAT3(1.f, 1.f, 1.f);
			//Boss2Skill(cmdList, boss2_skill_circle, i_now_render_index, boss2_skill_time[1], boss2_skill_pos2, boss2_skill_scale2);
		}
			break;

		case 3:
			Map(cmdList, floor, map_asset, i_now_render_index, Scene_num);
			if (boss_obj._on == true)
			{
				XMFLOAT3 boss3_scale = XMFLOAT3(100.f, 100.f, 100.f);
				float boss2_default_rot_x = -90.f;
				Boss(cmdList, boss3_Body, i_now_render_index, boss3_scale, boss2_default_rot_x, Scene_num);
				Boss(cmdList, boss3_Cloth_B, i_now_render_index, boss3_scale, boss2_default_rot_x, Scene_num);
				Boss(cmdList, boss3_Cloth_C2, i_now_render_index, boss3_scale, boss2_default_rot_x, Scene_num);
				Boss(cmdList, boss3_Hair_A, i_now_render_index, boss3_scale, boss2_default_rot_x, Scene_num);
				Boss(cmdList, boss3_Mask, i_now_render_index, boss3_scale, boss2_default_rot_x, Scene_num);
			}
			break;
		default:
			break;
		}

		for (int i = 0; i < NPCMAX; ++i)
		{
			if (npcArr[i]._hp <= 0)
			{
				npcArr[i]._animation_state = AnimationOrder::Death; // ∆–≈∂ √º≈©

				if (npcArr[i]._animation_time_pos >= npc_asset._animationPtr->GetClipEndTime(npcArr[i]._animation_state))
				{
					npcArr[i]._on = false;
				}
			}
		}

		//
		cmdList->SetPipelineState(npc_asset._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &npc_asset._vertexBufferView);
		cmdList->IASetIndexBuffer(&npc_asset._indexBufferView);
		for (int i = 0; i < NPCMAX; i++) //npc ÔøΩÔøΩÔøΩÔøΩ
		{
			if (npcArr[i]._on == true)
			{
				{
					//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(2 * _scale, 2 * _scale, 2 * _scale) * XMMatrixRotationX(-XM_PI / 2.f)
						* XMMatrixRotationY(npcArr[i]._prev_degree * XM_PI / 180.f - XM_PI / 2.f)
						* XMMatrixTranslation(npcArr[i]._prev_transform.x, npcArr[i]._prev_transform.y, npcArr[i]._prev_transform.z));
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
			if (npcArr[i]._on == true) {
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale * 0.5 * 0.01f, _scale * 0.001f, _scale * 0.001f)
					* XMMatrixRotationX(-atan2f(cameraPtr->pos.m128_f32[1] - (npcArr[i]._prev_transform.y + 0.01f),
						sqrt(pow(cameraPtr->pos.m128_f32[0] - npcArr[i]._prev_transform.x, 2) + pow(cameraPtr->pos.m128_f32[2] - npcArr[i]._prev_transform.z, 2))))
					* XMMatrixRotationY(atan2f(cameraPtr->pos.m128_f32[0] - npcArr[i]._prev_transform.x, cameraPtr->pos.m128_f32[2] - npcArr[i]._prev_transform.z))
					* XMMatrixTranslation(npcArr[i]._prev_transform.x, npcArr[i]._prev_transform.y + _scale / 100.f, npcArr[i]._prev_transform.z));
				XMMATRIX world = XMLoadFloat4x4(&_transform.world);
				XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));
				_transform.hp_bar_size = 2.f;
				_transform.hp_bar_start_pos = npcArr[i]._transform;
				_transform.hp_bar_start_pos.x -= _transform.hp_bar_size / 2.f;
				_transform.current_hp = npcArr[i]._hp;
				_transform.max_hp = npcArr[i]._max_hp;

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

		/*// key
		for (int i = 0; i < KEYMAX; ++i) {
			if (key_data[i]._on == true)
			{
				cmdList->SetPipelineState(key._pipelineState.Get());
				cmdList->IASetVertexBuffers(0, 1, &key._vertexBufferView);
				cmdList->IASetIndexBuffer(&key._indexBufferView);

				_key_rotation_time += timerPtr->_deltaTime;
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale * 0.02f, _scale * 0.02f, _scale * 0.02f) * XMMatrixRotationX(XM_PI * 0.5f)
					* XMMatrixRotationY(_key_rotation_time * 60.f * XM_PI / 180.f) * XMMatrixTranslation(key_data[i]._transform.x, key_data[i]._transform.y + _scale / 100.f, key_data[i]._transform.z));
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
		}*/

		// skybox
		{
			cmdList->SetPipelineState(skybox._pipelineState.Get());
			cmdList->IASetVertexBuffers(0, 1, &skybox._vertexBufferView);
			cmdList->IASetIndexBuffer(&skybox._indexBufferView);
			//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale * 0.01f, _scale * 0.01f, _scale * 0.01f) * XMMatrixTranslation(0.f, _scale / 50.f, 0.f));
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
			if (npcArr[i]._on == true) {
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
						particles[index].moveSpeed = (float)(rand() % 101) / 100 + 2.f; // 2~3
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
			else if (npcArr[i]._on == true && index <= PARTICLE_NUM) {
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
						particles[index].moveSpeed = (float)(rand() % 101) / 100 + 2.f; // 2~3
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
					particles[index].moveSpeed = (float)(rand() % 101) / 100 + 2.f; // 2~3
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
					particles[index].moveSpeed = (float)(rand() % 101) / 100 + 2.f; // 2~3
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

		if (boss_obj._on == true && Scene_num == 1) // ∫∏Ω∫1 ∞≥∫∞ ¥Ÿ∏Æ √≥∏Æ
		{
			for (int i = 0; i < 8; ++i)
			{
				while (boss_leg[i] > 0)
				{
					if (index >= PARTICLE_NUM) // ∆ƒ∆º≈¨ ∞≥ºˆø° ¥Î«— øπø‹√≥∏Æ
						break;

					if (particles[index].alive == 0)
					{
						particles[index].lifeTime = (float)(rand() % 101) / 1000.f + 0.3f; // 0.3~0.4
						particles[index].curTime = 0.0f;
						particles[index].pos = XMVectorSet(boss_col[i].Center.x, boss_col[i].Center.y + 1.f, boss_col[i].Center.z, 1.f);
						particles[index].moveSpeed = (float)(rand() % 101) / 100 + 2.f; // 2~3
						particles[index].dir = XMVectorSet(((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, ((float)(rand() % 101) / 100 - 0.5f) * 2, 1.0f);
						XMVector3Normalize(particles[index].dir);
						particles[index].velocity = XMVectorSet(particles[index].dir.m128_f32[0] * particles[index].moveSpeed,
							particles[index].dir.m128_f32[1] * particles[index].moveSpeed, particles[index].dir.m128_f32[2] * particles[index].moveSpeed, 1.f);
						particles[index].alive = 1;
						--boss_leg[i];
					}
					else
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
	}
	::WaitForSingleObject(_excuteEvent, INFINITE);
	SetEvent(_renderEvent);

	

	//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(swapChainPtr->_renderTargets[i_now_render_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // »≠ÔøΩÔøΩ ÔøΩÔøΩÔø?

	cmdList->ResourceBarrier(1, &barrier2);
	cmdList->Close();

	

	ID3D12CommandList* cmdListArr[] = { cmdList.Get() };
	cmdQueuePtr->_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);
	d11Ptr->RenderUI(i_now_render_index);
	
	
	
	if (_is_loading) {
		d11Ptr->LateRenderUI(_test_ui_vector);

		D2D1_RECT_F _tmp = D2D1::RectF(0, 620.0f, 1280.f, 720.f);
		WCHAR text[128];
		wsprintf(text, L"HP : %d", playerArr[0]._hp);
		d11Ptr->draw_text(text, _tmp);
		_tmp.left = 1100;
		_tmp.right = 1280;
		for (int i = 1; i < PLAYERMAX; ++i) {
			_tmp.bottom = 720 / 2 - 50 + 25 * i;
			_tmp.top = _tmp.bottom - 25;
			if (playerArr[i]._on == true)
			{
				wsprintf(text, L"HP : %d", playerArr[i]._hp);
				d11Ptr->draw_text(text, _tmp);
			}
			else {
				wsprintf(text, L"WAIT PLAYER");
				
				d11Ptr->draw_text(text, _tmp);
			}
			
			
		}

		//2D ƒ⁄µÂ∂Û≥◊
		//∞´ ∏µÂ ø¬
		wsprintf(text, L"BOSS HP : %d", boss_obj._hp);
		float height = 0.f;
		if (inputPtr->_god_mod_on) {
			_tmp = D2D1::RectF(500.f, 0.f, 1280.f, 15.f);
			_swprintf(text, L"[GOD(DEV) MODE ON]  ");
			d11Ptr->draw_infotext(text, _tmp);
			height = 15.f;
		}
		




		
		if (inputPtr->_show_info) {
			for (int i = 0; i < PLAYERMAX; ++i) {
				if (playerArr[i]._on) {
					_tmp = D2D1::RectF(500.f, height, 1280.f, height + 15.f);
					_swprintf(text, L"PLAYER[%d] X : %f  Y : %f Z : %f", i, playerArr[i]._transform.x, playerArr[i]._transform.y, playerArr[i]._transform.z);
					d11Ptr->draw_infotext(text, _tmp);
					height += 15.f;
				}
			}

			if (boss_obj._on) {
				_tmp = D2D1::RectF(500.f, height, 1280.f, height + 15.f);
				_swprintf(text, L"BOSS X : %f  Y : %f Z : %f", boss_obj._transform.x, boss_obj._transform.y, boss_obj._transform.z);
				d11Ptr->draw_infotext(text, _tmp);
				height += 15.f;
			}

			for (int i = 0; i < NPCMAX; ++i) {
				if (npcArr[i]._on) {
					_tmp = D2D1::RectF(500.f, height, 1280.f, height + 15.f);
					_swprintf(text, L"NPC[%d] X : %f  Y : %f Z : %f", i, npcArr[i]._transform.x, npcArr[i]._transform.y, npcArr[i]._transform.z);
					d11Ptr->draw_infotext(text, _tmp);
					height += 15.f;
				}
			}
			
		}
		
		if (_atked) {
			d11Ptr->draw_stage2_atk();
		}

		d11Ptr->draw_player_info(L"AKI", 100, playerArr[0]._hp, 0);
		
		d11Ptr->draw_bossUI(boss_obj._hp, Scene_num,boss_obj);

		if (boss_obj._game_clear) {
			d11Ptr->draw_game_clear(boss_obj._clear_type);
		}

	}
	else {
		d11Ptr->Loading_draw(timerPtr->_deltaTime);
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
