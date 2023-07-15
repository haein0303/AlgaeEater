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

	boss2Skill.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	boss2Skill.Init("../Resources/Boss2Skill.txt", ObjectType::GeneralObjects);
	boss2Skill.Add_texture(L"..\\Resources\\Texture\\spider_paint_Red_BaseColor_Eye.jpg");
	boss2Skill.Add_texture(L"..\\Resources\\Texture\\spider_paint_Blue_Color_Eye.jpg");
	boss2Skill.Add_texture(L"..\\Resources\\Texture\\spider_paint_Green_Color_Eye.png");
	boss2Skill.Add_texture(L"..\\Resources\\Texture\\spider_paint_White_Color_Eye.png");
	boss2Skill.Make_SRV();
	boss2Skill.CreatePSO(L"..\\Bricks.hlsl");

	boss2_skill_circle.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	boss2_skill_circle.Init("../Resources/Boss2Skill.txt", ObjectType::GeneralObjects);
	boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\spider_paint_Red_BaseColor_Eye.jpg");
	boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\spider_paint_Blue_Color_Eye.jpg");
	boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\spider_paint_Green_Color_Eye.png");
	boss2_skill_circle.Add_texture(L"..\\Resources\\Texture\\spider_paint_White_Color_Eye.png");
	boss2_skill_circle.Make_SRV();
	boss2_skill_circle.CreatePSO(L"..\\Circle.hlsl");

	stage0_map.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	stage0_map.Init("../Resources/stage0_map.txt", ObjectType::GeneralObjects);
	stage0_map.Add_texture(L"..\\Resources\\Texture\\sample.png");
	stage0_map.Add_texture(L"..\\Resources\\Texture\\Back_wall.png");
	stage0_map.Add_texture(L"..\\Resources\\Texture\\Front_wall.png");
	stage0_map.Add_texture(L"..\\Resources\\Texture\\Right_wall.png");
	stage0_map.Add_texture(L"..\\Resources\\Texture\\Left_wall.png");
	stage0_map.Make_SRV();
	stage0_map.CreatePSO(L"..\\Bricks.hlsl");

	Tube.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube.Init("../Resources/Tube.txt", ObjectType::GeneralObjects);
	Tube.Add_texture(L"..\\Resources\\Texture\\Atlass_albedo.tga");
	Tube.Make_SRV();
	Tube.CreatePSO(L"..\\Bricks.hlsl");

	barrel.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	barrel.Init("../Resources/barrel.txt", ObjectType::GeneralObjects);
	barrel.Add_texture(L"..\\Resources\\Texture\\barrel.png");
	barrel.Make_SRV();
	barrel.CreatePSO(L"..\\Bricks.hlsl");

	Box.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Box.Init("../Resources/Box.txt", ObjectType::GeneralObjects);
	Box.Add_texture(L"..\\Resources\\Texture\\Box_Sci-fi_box_mat_AlbedoTransparency.tga");
	Box.Make_SRV();
	Box.CreatePSO(L"..\\Bricks.hlsl");

	NeonCrate_0.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	NeonCrate_0.Init("../Resources/NeonCrate_0.txt", ObjectType::GeneralObjects);
	NeonCrate_0.Add_texture(L"..\\Resources\\Texture\\Box_Sci-fi_box_mat_AlbedoTransparency.tga");
	NeonCrate_0.Make_SRV();
	NeonCrate_0.CreatePSO(L"..\\Bricks.hlsl");

	Clotch.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Clotch.Init("../Resources/Clotch.txt", ObjectType::GeneralObjects);
	Clotch.Add_texture(L"..\\Resources\\Texture\\diff.png");
	Clotch.Make_SRV();
	Clotch.CreatePSO(L"..\\Bricks.hlsl");

	tank.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	tank.Init("../Resources/tank.txt", ObjectType::GeneralObjects);
	tank.Add_texture(L"..\\Resources\\Texture\\Tank_Color.png");
	tank.Make_SRV();
	tank.CreatePSO(L"..\\Bricks.hlsl");

	Plane002.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Plane002.Init("../Resources/Plane002.txt", ObjectType::GeneralObjects);
	Plane002.Add_texture(L"..\\Resources\\Texture\\mettile1_diff.png");
	Plane002.Make_SRV();
	Plane002.CreatePSO(L"..\\Bricks.hlsl");

	Grid_Metal_tile.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Grid_Metal_tile.Init("../Resources/Grid_Metal_tile.txt", ObjectType::GeneralObjects);
	Grid_Metal_tile.Add_texture(L"..\\Resources\\Texture\\Atlass_albedo.tga");
	Grid_Metal_tile.Make_SRV();
	Grid_Metal_tile.CreatePSO(L"..\\Wall.hlsl");

	Plate_mettal_wall_HQ__2_.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Plate_mettal_wall_HQ__2_.Init("../Resources/Plate_mettal_wall_HQ__2_.txt", ObjectType::GeneralObjects);
	Plate_mettal_wall_HQ__2_.Add_texture(L"..\\Resources\\Texture\\Atlass_albedo.tga");
	Plate_mettal_wall_HQ__2_.Make_SRV();
	Plate_mettal_wall_HQ__2_.CreatePSO(L"..\\Wall.hlsl");

	Cube.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Cube.Init("../Resources/Cube.txt", ObjectType::GeneralObjects);
	Cube.Add_texture(L"..\\Resources\\Texture\\mettile1_diff.png");
	Cube.Make_SRV();
	Cube.CreatePSO(L"..\\Bricks.hlsl");

	Grid_Metal_door.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Grid_Metal_door.Init("../Resources/Grid_Metal_door.txt", ObjectType::GeneralObjects);
	Grid_Metal_door.Add_texture(L"..\\Resources\\Texture\\Atlass_albedo.tga");
	Grid_Metal_door.Make_SRV();
	Grid_Metal_door.CreatePSO(L"..\\Wall.hlsl");

	Wall_Door_4m_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Door_4m_A.Init("../Resources/Wall_Door_4m_A.txt", ObjectType::GeneralObjects);
	Wall_Door_4m_A.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_A.png");
	Wall_Door_4m_A.Make_SRV();
	Wall_Door_4m_A.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_1m_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_1m_A.Init("../Resources/Wall_Line_1m_A.txt", ObjectType::GeneralObjects);
	Wall_Line_1m_A.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_A.png");
	Wall_Line_1m_A.Make_SRV();
	Wall_Line_1m_A.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_2m_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_2m_A.Init("../Resources/Wall_Line_2m_A.txt", ObjectType::GeneralObjects);
	Wall_Line_2m_A.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_A.png");
	Wall_Line_2m_A.Make_SRV();
	Wall_Line_2m_A.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_4m_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_4m_A.Init("../Resources/Wall_Line_4m_A.txt", ObjectType::GeneralObjects);
	Wall_Line_4m_A.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_A.png");
	Wall_Line_4m_A.Make_SRV();
	Wall_Line_4m_A.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Turn_L_Out_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Turn_L_Out_A.Init("../Resources/Wall_Turn_L_Out_A.txt", ObjectType::GeneralObjects);
	Wall_Turn_L_Out_A.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_A.png");
	Wall_Turn_L_Out_A.Make_SRV();
	Wall_Turn_L_Out_A.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Win_4m_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Win_4m_A.Init("../Resources/Wall_Win_4m_A.txt", ObjectType::GeneralObjects);
	Wall_Win_4m_A.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_A.png");
	Wall_Win_4m_A.Make_SRV();
	Wall_Win_4m_A.CreatePSO(L"..\\Bricks.hlsl");

	Trap_A_2x2m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Trap_A_2x2m.Init("../Resources/Trap_A_2x2m.txt", ObjectType::GeneralObjects);
	Trap_A_2x2m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Trap.png");
	Trap_A_2x2m.Make_SRV();
	Trap_A_2x2m.CreatePSO(L"..\\Bricks.hlsl");

	Trap_A_4x2m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Trap_A_4x2m.Init("../Resources/Trap_A_4x2m.txt", ObjectType::GeneralObjects);
	Trap_A_4x2m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Trap.png");
	Trap_A_4x2m.Make_SRV();
	Trap_A_4x2m.CreatePSO(L"..\\Bricks.hlsl");

	Trap_A_Stairs.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Trap_A_Stairs.Init("../Resources/Trap_A_Stairs.txt", ObjectType::GeneralObjects);
	Trap_A_Stairs.Add_texture(L"..\\Resources\\Texture\\Stage2\\Trap.png");
	Trap_A_Stairs.Make_SRV();
	Trap_A_Stairs.CreatePSO(L"..\\Bricks.hlsl");

	Column_BB_6m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Column_BB_6m.Init("../Resources/Column_BB_6m.txt", ObjectType::GeneralObjects);
	Column_BB_6m.Add_texture(L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png");
	Column_BB_6m.Make_SRV();
	Column_BB_6m.CreatePSO(L"..\\Bricks.hlsl");

	Railings_A_1m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Railings_A_1m.Init("../Resources/Railings_A_1m.txt", ObjectType::GeneralObjects);
	Railings_A_1m.Add_texture(L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png");
	Railings_A_1m.Make_SRV();
	Railings_A_1m.CreatePSO(L"..\\Bricks.hlsl");

	Railings_A_2m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Railings_A_2m.Init("../Resources/Railings_A_2m.txt", ObjectType::GeneralObjects);
	Railings_A_2m.Add_texture(L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png");
	Railings_A_2m.Make_SRV();
	Railings_A_2m.CreatePSO(L"..\\Bricks.hlsl");

	Railings_A_4m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Railings_A_4m.Init("../Resources/Railings_A_4m.txt", ObjectType::GeneralObjects);
	Railings_A_4m.Add_texture(L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png");
	Railings_A_4m.Make_SRV();
	Railings_A_4m.CreatePSO(L"..\\Bricks.hlsl");

	Railings_A_Turn_L_In.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Railings_A_Turn_L_In.Init("../Resources/Railings_A_Turn_L_In.txt", ObjectType::GeneralObjects);
	Railings_A_Turn_L_In.Add_texture(L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png");
	Railings_A_Turn_L_In.Make_SRV();
	Railings_A_Turn_L_In.CreatePSO(L"..\\Bricks.hlsl");

	Railings_A_Turn_L_Out.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Railings_A_Turn_L_Out.Init("../Resources/Railings_A_Turn_L_Out.txt", ObjectType::GeneralObjects);
	Railings_A_Turn_L_Out.Add_texture(L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png");
	Railings_A_Turn_L_Out.Make_SRV();
	Railings_A_Turn_L_Out.CreatePSO(L"..\\Bricks.hlsl");

	Beam_B_4m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Beam_B_4m.Init("../Resources/Beam_B_4m.txt", ObjectType::GeneralObjects);
	Beam_B_4m.Add_texture(L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png");
	Beam_B_4m.Make_SRV();
	Beam_B_4m.CreatePSO(L"..\\Bricks.hlsl");

	Column_A_2m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Column_A_2m.Init("../Resources/Column_A_2m.txt", ObjectType::GeneralObjects);
	Column_A_2m.Add_texture(L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png");
	Column_A_2m.Make_SRV();
	Column_A_2m.CreatePSO(L"..\\Bricks.hlsl");

	Column_B_6m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Column_B_6m.Init("../Resources/Column_B_6m.txt", ObjectType::GeneralObjects);
	Column_B_6m.Add_texture(L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png");
	Column_B_6m.Make_SRV();
	Column_B_6m.CreatePSO(L"..\\Bricks.hlsl");

	Console_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Console_B.Init("../Resources/Console_B.txt", ObjectType::GeneralObjects);
	Console_B.Add_texture(L"..\\Resources\\Texture\\Stage2\\ColumnAndBeam.png");
	Console_B.Make_SRV();
	Console_B.CreatePSO(L"..\\Bricks.hlsl");

	FireStair_Base.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	FireStair_Base.Init("../Resources/FireStair_Base.txt", ObjectType::GeneralObjects);
	FireStair_Base.Add_texture(L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png");
	FireStair_Base.Make_SRV();
	FireStair_Base.CreatePSO(L"..\\Bricks.hlsl");

	FireStair_Cover.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	FireStair_Cover.Init("../Resources/FireStair_Cover.txt", ObjectType::GeneralObjects);
	FireStair_Cover.Add_texture(L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png");
	FireStair_Cover.Make_SRV();
	FireStair_Cover.CreatePSO(L"..\\Bricks.hlsl");

	FireStair_EndTop.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	FireStair_EndTop.Init("../Resources/FireStair_EndTop.txt", ObjectType::GeneralObjects);
	FireStair_EndTop.Add_texture(L"..\\Resources\\Texture\\Stage2\\RailingsAndFireStair.png");
	FireStair_EndTop.Make_SRV();
	FireStair_EndTop.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Line_A_1m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Line_A_1m.Init("../Resources/Tube_Line_A_1m.txt", ObjectType::GeneralObjects);
	Tube_Line_A_1m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Line_A_1m.Make_SRV();
	Tube_Line_A_1m.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Line_A_2m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Line_A_2m.Init("../Resources/Tube_Line_A_2m.txt", ObjectType::GeneralObjects);
	Tube_Line_A_2m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Line_A_2m.Make_SRV();
	Tube_Line_A_2m.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Line_A_4m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Line_A_4m.Init("../Resources/Tube_Line_A_4m.txt", ObjectType::GeneralObjects);
	Tube_Line_A_4m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Line_A_4m.Make_SRV();
	Tube_Line_A_4m.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Line_A_6m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Line_A_6m.Init("../Resources/Tube_Line_A_6m.txt", ObjectType::GeneralObjects);
	Tube_Line_A_6m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Line_A_6m.Make_SRV();
	Tube_Line_A_6m.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Line_B_1m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Line_B_1m.Init("../Resources/Tube_Line_B_1m.txt", ObjectType::GeneralObjects);
	Tube_Line_B_1m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Line_B_1m.Make_SRV();
	Tube_Line_B_1m.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Line_B_2m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Line_B_2m.Init("../Resources/Tube_Line_B_2m.txt", ObjectType::GeneralObjects);
	Tube_Line_B_2m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Line_B_2m.Make_SRV();
	Tube_Line_B_2m.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Line_B_4m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Line_B_4m.Init("../Resources/Tube_Line_B_4m.txt", ObjectType::GeneralObjects);
	Tube_Line_B_4m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Line_B_4m.Make_SRV();
	Tube_Line_B_4m.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Line_B_6m.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Line_B_6m.Init("../Resources/Tube_Line_B_6m.txt", ObjectType::GeneralObjects);
	Tube_Line_B_6m.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Line_B_6m.Make_SRV();
	Tube_Line_B_6m.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Turn_L_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Turn_L_A.Init("../Resources/Tube_Turn_L_A.txt", ObjectType::GeneralObjects);
	Tube_Turn_L_A.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Turn_L_A.Make_SRV();
	Tube_Turn_L_A.CreatePSO(L"..\\Bricks.hlsl");

	Tube_Turn_L_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Tube_Turn_L_B.Init("../Resources/Tube_Turn_L_B.txt", ObjectType::GeneralObjects);
	Tube_Turn_L_B.Add_texture(L"..\\Resources\\Texture\\Stage2\\Tubes.png");
	Tube_Turn_L_B.Make_SRV();
	Tube_Turn_L_B.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Door_4m_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Door_4m_B.Init("../Resources/Wall_Door_4m_B.txt", ObjectType::GeneralObjects);
	Wall_Door_4m_B.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_B.png");
	Wall_Door_4m_B.Make_SRV();
	Wall_Door_4m_B.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_2m_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_2m_B.Init("../Resources/Wall_Line_2m_B.txt", ObjectType::GeneralObjects);
	Wall_Line_2m_B.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_B.png");
	Wall_Line_2m_B.Make_SRV();
	Wall_Line_2m_B.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_2m_D.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_2m_D.Init("../Resources/Wall_Line_2m_D.txt", ObjectType::GeneralObjects);
	Wall_Line_2m_D.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_C.png");
	Wall_Line_2m_D.Make_SRV();
	Wall_Line_2m_D.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_2m_¨≥.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_2m_¨≥.Init("../Resources/Wall_Line_2m_¨≥.txt", ObjectType::GeneralObjects);
	Wall_Line_2m_¨≥.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_C.png");
	Wall_Line_2m_¨≥.Make_SRV();
	Wall_Line_2m_¨≥.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_4m_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_4m_B.Init("../Resources/Wall_Line_4m_B.txt", ObjectType::GeneralObjects);
	Wall_Line_4m_B.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_B.png");
	Wall_Line_4m_B.Make_SRV();
	Wall_Line_4m_B.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_4m_D.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_4m_D.Init("../Resources/Wall_Line_4m_D.txt", ObjectType::GeneralObjects);
	Wall_Line_4m_D.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_C.png");
	Wall_Line_4m_D.Make_SRV();
	Wall_Line_4m_D.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Line_4m_¨≥.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Line_4m_¨≥.Init("../Resources/Wall_Line_4m_¨≥.txt", ObjectType::GeneralObjects);
	Wall_Line_4m_¨≥.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_C.png");
	Wall_Line_4m_¨≥.Make_SRV();
	Wall_Line_4m_¨≥.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Turn_L_In_A.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Turn_L_In_A.Init("../Resources/Wall_Turn_L_In_A.txt", ObjectType::GeneralObjects);
	Wall_Turn_L_In_A.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_A.png");
	Wall_Turn_L_In_A.Make_SRV();
	Wall_Turn_L_In_A.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Turn_L_In_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Turn_L_In_B.Init("../Resources/Wall_Turn_L_In_B.txt", ObjectType::GeneralObjects);
	Wall_Turn_L_In_B.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_B.png");
	Wall_Turn_L_In_B.Make_SRV();
	Wall_Turn_L_In_B.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Turn_L_In_D.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Turn_L_In_D.Init("../Resources/Wall_Turn_L_In_D.txt", ObjectType::GeneralObjects);
	Wall_Turn_L_In_D.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_C.png");
	Wall_Turn_L_In_D.Make_SRV();
	Wall_Turn_L_In_D.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Turn_L_Out_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Turn_L_Out_B.Init("../Resources/Wall_Turn_L_Out_B.txt", ObjectType::GeneralObjects);
	Wall_Turn_L_Out_B.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_B.png");
	Wall_Turn_L_Out_B.Make_SRV();
	Wall_Turn_L_Out_B.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Turn_L_Out_D.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Turn_L_Out_D.Init("../Resources/Wall_Turn_L_Out_D.txt", ObjectType::GeneralObjects);
	Wall_Turn_L_Out_D.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_C.png");
	Wall_Turn_L_Out_D.Make_SRV();
	Wall_Turn_L_Out_D.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Turn_L_Out_¨≥.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Turn_L_Out_¨≥.Init("../Resources/Wall_Turn_L_Out_¨≥.txt", ObjectType::GeneralObjects);
	Wall_Turn_L_Out_¨≥.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_C.png");
	Wall_Turn_L_Out_¨≥.Make_SRV();
	Wall_Turn_L_Out_¨≥.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Win_4m_B.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Win_4m_B.Init("../Resources/Wall_Win_4m_B.txt", ObjectType::GeneralObjects);
	Wall_Win_4m_B.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_B.png");
	Wall_Win_4m_B.Make_SRV();
	Wall_Win_4m_B.CreatePSO(L"..\\Bricks.hlsl");

	Wall_Win_4m_C.Link_ptr(devicePtr, fbxLoaderPtr, vertexBufferPtr, indexBufferPtr, cmdQueuePtr, rootSignaturePtr, dsvPtr);
	Wall_Win_4m_C.Init("../Resources/Wall_Win_4m_C.txt", ObjectType::GeneralObjects);
	Wall_Win_4m_C.Add_texture(L"..\\Resources\\Texture\\Stage2\\Walls_C.png");
	Wall_Win_4m_C.Make_SRV();
	Wall_Win_4m_C.CreatePSO(L"..\\Bricks.hlsl");

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

	for (int i = 0; i < KEYMAX; ++i) {
		key_data[i]._transform = XMFLOAT4(180.f, 0.f, -240.f, 1.f);
		key_data[i]._key = 0;
		key_data[i]._on = true;
	}
	

	ImportMapdata("../Resources/MapData1.txt", _map_data);
	ImportMapdata("../Resources/MapData2.txt", _map_data2);
	ImportMapdata("../Resources/MapData3.txt", _map_data3);
	ImportCollisionObjectsData("../Resources/CollisionMapData1.txt", bounding_boxes);
	ImportCollisionObjectsData("../Resources/CollisionMapData2.txt", bounding_boxes2);
	ImportCollisionObjectsData("../Resources/CollisionData3.txt", bounding_boxes3);
	
	test.Center = XMFLOAT3(170.f, 1.f, -240.f);
	test.Extents = XMFLOAT3(1.f, 1.f, 1.f);

	XMVECTOR v{ 0, 1, 0, 0 };
	testCharacter.Center = XMFLOAT3(0.f, -100.f, 0.f);
	testCharacter.Extents = XMFLOAT3(0.1f, 0.1f, 0.1f);
	XMStoreFloat4(&testCharacter2.Orientation, XMQuaternionRotationNormal(v, playerArr[0]._degree * XM_PI / 180.f));

	testCharacter2.Center = XMFLOAT3(0.f, -100.f, 0.f);
	testCharacter2.Extents = XMFLOAT3(0.1f, 0.1f, 0.1f);
	XMStoreFloat4(&testCharacter2.Orientation, XMQuaternionRotationNormal(v, playerArr[0]._degree * XM_PI / 180.f));

	boss_obj.boss2_skill_vec.reserve(100);

	// boss1 bounding box
	for (int i = 0; i < 8; ++i)
	{
		boss_col[i].Extents = XMFLOAT3(0.5f, 2.f, 0.5f);
		boss_col[i].Center = XMFLOAT3(boss_obj._transform.x, boss_obj._transform.y, boss_obj._transform.z);
	}

	boss_collision.Center = XMFLOAT3(boss_obj._transform.x,
		boss_obj._transform.y,
		boss_obj._transform.z);
	boss_collision.Extents = XMFLOAT3(2.f, 1.5f, 3.f);

	boss2Skill._tex._srvHandle = boss2Skill._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
	boss2_skill_circle._tex._srvHandle = boss2_skill_circle._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();

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
	networkPtr->ReceiveServer(playerArr, npcArr, pillars_data, boss_obj, key_data);

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
			inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes);
			break;
		case 2:
			inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes2);
			break;
		case 3:
			inputPtr->InputKey(logicTimerPtr, playerArr, networkPtr, bounding_boxes3);
			break;
		default:
			break;
		}
		inputPtr->inputMouse(playerArr, networkPtr);
	}

	// «√∑π¿ÃæÓ ±‚∫ª ∞¯∞› ƒ›∂Û¿Ã¥ı on off
	if (playerArr[0]._animation_state == AnimationOrder::Attack
		&& playerArr[0]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[0]._animation_state) * 0.5f)
	{
		testCharacter.Center = XMFLOAT3(playerArr[0]._transform.x + 0.5f * cosf((-playerArr[0]._degree + 90.f) * XM_PI / 180.f),
			playerArr[0]._transform.y + 0.5f,
			playerArr[0]._transform.z + 0.5f * sinf((-playerArr[0]._degree + 90.f) * XM_PI / 180.f));
		testCharacter.Extents = XMFLOAT3(1.f, 0.5f, 0.3f);
		XMVECTOR v{ 0, 1, 0, 0 };
		XMStoreFloat4(&testCharacter.Orientation, XMQuaternionRotationNormal(v, playerArr[0]._degree * XM_PI / 180.f));
	}
	else
	{
		testCharacter.Center.y = -100.f;
	}

	// «√∑π¿ÃæÓ Ω∫≈≥ ƒ›∂Û¿Ã¥ı on off
	if (playerArr[0]._animation_state == AnimationOrder::Skill
		&& playerArr[0]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[0]._animation_state) * 0.5f)
	{
		testCharacter2.Center = XMFLOAT3(playerArr[0]._transform.x,
			playerArr[0]._transform.y + 0.5f,
			playerArr[0]._transform.z);
		testCharacter2.Extents = XMFLOAT3(10.f, 1.f, 10.f);
		XMVECTOR v{ 0, 1, 0, 0 };
		XMStoreFloat4(&testCharacter2.Orientation, XMQuaternionRotationNormal(v, playerArr[0]._degree * XM_PI / 180.f));
	}
	else
	{
		testCharacter2.Center.y = -100.f;
	}

	// npc bounding box
	for (OBJECT& obj : npcArr)
	{
		obj._bounding_box.Center = XMFLOAT3(obj._transform.x, obj._transform.y, obj._transform.z);
	}

	// boss bounding box
	boss_collision.Center = XMFLOAT3(boss_obj._transform.x,
		boss_obj._transform.y + 2.5f,
		boss_obj._transform.z);
	XMVECTOR v{ 0, 1, 0, 0 };
	XMStoreFloat4(&boss_collision.Orientation, XMQuaternionRotationNormal(v, boss_obj._degree * XM_PI / 180.f));

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

	// ∆ƒ∆º≈¨ µø±‚»≠
	for (OBJECT& player : playerArr)
	{
		if ((player._animation_state == AnimationOrder::Attack || player._animation_state == AnimationOrder::Skill)
			&& player._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(player._animation_state) * 0.5f
			&& player._can_attack2)
		{
			for (int i = 0; i < NPCMAX; ++i)
			{
				if (npcArr[i]._on == true && testCharacter.Intersects(npcArr[i]._bounding_box))
				{
					npcArr[i]._particle_count += 100;
				}
				if (npcArr[i]._on == true && testCharacter2.Intersects(npcArr[i]._bounding_box))
				{
					npcArr[i]._particle_count += 100;
				}
			}

			if (testCharacter.Intersects(boss_collision))
				boss_obj._particle_count += 100;
			if (Scene_num == 1)
				for (int i = 0; i < 8; ++i)
					if (testCharacter.Intersects(boss_col[i]))
						boss_leg[i] += 100;

			if (testCharacter2.Intersects(boss_collision))
				boss_obj._particle_count += 100;
			if (Scene_num == 1)
				for (int i = 0; i < 8; ++i)
					if (testCharacter2.Intersects(boss_col[i]))
						boss_leg[i] += 100;

			playerArr[networkPtr->myClientId]._can_attack2 = false;
		}
	}

	// «√∑π¿ÃæÓ ∞¯∞› √Êµπ ∞®¡ˆ
	if ((playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack || playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Skill)
		&& playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f
		&& playerArr[networkPtr->myClientId]._can_attack)
	{
		for (int i = 0; i < NPCMAX; ++i)
		{
			// npc ∆Ú≈∏ ∞®¡ˆ
			if (npcArr[i]._on == true && testCharacter.Intersects(npcArr[i]._bounding_box))
			{
				CS_COLLISION_PACKET p;
				p.size = sizeof(p);
				p.type = CS_COLLISION;
				p.attack_type = playerArr[networkPtr->myClientId]._animation_state - 2;
				p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
				p.target_id = npcArr[i]._my_server_id;
				networkPtr->send_packet(&p);

				cout << "player" << playerArr[networkPtr->myClientId]._my_server_id << endl;
				cout << "npc" << i << " hp : " << npcArr[i]._hp << endl;
				cout << "particle " << i << " : " << npcArr[i]._particle_count << endl;
			}

			// npc Ω∫≈≥ ∞®¡ˆ
			if (npcArr[i]._on == true && testCharacter2.Intersects(npcArr[i]._bounding_box))
			{
				CS_COLLISION_PACKET p;
				p.size = sizeof(p);
				p.type = CS_COLLISION;
				p.attack_type = playerArr[networkPtr->myClientId]._animation_state - 2;
				p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
				p.target_id = npcArr[i]._my_server_id;
				networkPtr->send_packet(&p);

				cout << "player" << playerArr[networkPtr->myClientId]._my_server_id << endl;
				cout << "npc" << i << " hp : " << npcArr[i]._hp << endl;
				cout << "particle " << i << " : " << npcArr[i]._particle_count << endl;
			}
		}

		// ∫∏Ω∫ ¥ÎªÛ ∆Ú≈∏ √Êµπ ∞®¡ˆ
		if (testCharacter.Intersects(boss_collision))
		{
			CS_COLLISION_PACKET p;
			p.size = sizeof(p);
			p.type = CS_COLLISION;
			p.attack_type = playerArr[networkPtr->myClientId]._animation_state - 2;
			p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
			p.target_id = boss_obj._my_server_id;
			networkPtr->send_packet(&p);
		}
		for (int i = 0; i < 8; ++i)
		{
			if (testCharacter.Intersects(boss_col[i]))
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

		// ∫∏Ω∫ ¥ÎªÛ Ω∫≈≥ √Êµπ ∞®¡ˆ
		if (testCharacter2.Intersects(boss_collision))
		{
			CS_COLLISION_PACKET p;
			p.size = sizeof(p);
			p.type = CS_COLLISION;
			p.attack_type = playerArr[networkPtr->myClientId]._animation_state - 2;
			p.attacker_id = playerArr[networkPtr->myClientId]._my_server_id;
			p.target_id = boss_obj._my_server_id;
			networkPtr->send_packet(&p);
		}
		for (int i = 0; i < 8; ++i)
		{
			if (testCharacter2.Intersects(boss_col[i]))
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
	
	// npc ∞¯∞› √Êµπ ∞®¡ˆ
	for (int j = 0; j < NPCMAX; ++j)
	{
		if (pow(playerArr[0]._transform.x - npcArr[j]._transform.x, 2) + pow(playerArr[0]._transform.z - npcArr[j]._transform.z, 2) <= 9.f
			&& npcArr[j]._animation_state == AnimationOrder::Attack
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

			cout << "player" << 0 << " hp : " << playerArr[0]._hp << endl;
			cout << "npc" << j << " hp : " << npcArr[j]._hp << endl;
		}
	}
	
	// ∫∏Ω∫ ∞¯∞› √Êµπ ∞®¡ˆ
	if (boss_obj._on == true) {
		int i = networkPtr->myClientId;
		if (pow(playerArr[i]._transform.x - boss_obj._transform.x, 2) + pow(playerArr[i]._transform.z - boss_obj._transform.z, 2) <= 9.f) {
			/*if ((playerArr[i]._animation_state == AnimationOrder::Attack || playerArr[i]._animation_state == AnimationOrder::Skill)
				&& playerArr[i]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[i]._animation_state) * 0.5f
				&& playerArr[i]._can_attack) {

				playerArr[i]._can_attack = false;

				CS_COLLISION_PACKET p;
				p.size = sizeof(p);
				p.type = CS_COLLISION;
				p.attack_type = playerArr[i]._animation_state-2;
				p.attacker_id = playerArr[i]._my_server_id;
				p.target_id = boss_obj._my_server_id;
				networkPtr->send_packet(&p);

				cout << "player" << i << " hp : " << boss_obj._hp << endl;
				cout << "BOSS hp : " << boss_obj._hp << endl;
				cout << "BOSS particle : " << boss_obj._particle_count << endl;
			}*/
			if (boss_obj._animation_state == AnimationOrder::Attack
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

				cout << "player" << i << " hp : " << playerArr[i]._hp << endl;
				cout << "BOSS hp : " << boss_obj._hp << endl;
			}
		}
	}

	// key √Êµπ√≥∏Æ
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
	}

	// ±‚µ’ √Êµπ√≥∏Æ
	for (int i = 0; i < CubeMax; ++i) {
		if (playerArr[networkPtr->myClientId]._on == true && pillars_data[i]._on == true)
		{
			if (pow(playerArr[networkPtr->myClientId]._transform.x - pillars_data[i]._transform.x, 2) + pow(playerArr[networkPtr->myClientId]._transform.z - pillars_data[i]._transform.z, 2) <= 9.f) //&& pillars_data[i]._pillar_color == playerArr[networkPtr->myClientId]._player_color
			{
				if ((playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack || playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Skill)
					&& playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f
					&& playerArr[networkPtr->myClientId]._can_attack3)
				{
					playerArr[networkPtr->myClientId]._can_attack3 = false;
					--pillars_data[i]._pillar_count;
					cout << "pillar : " << pillars_data[i]._pillar_count << endl;

					if (pillars_data[i]._pillar_count == 0) {
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
	}
	
	if (playerArr[networkPtr->myClientId]._hp <= 0.f) {
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
		cmdList->SetPipelineState(player_AKI_Body_asset._pipelineState.Get());
		cmdList->IASetVertexBuffers(0, 1, &player_AKI_Body_asset._vertexBufferView);
		cmdList->IASetIndexBuffer(&player_AKI_Body_asset._indexBufferView);
	
		//ÔøΩÔøΩÔøΩÔøΩ
		{
			int i = 0;
			{
				//ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»Ø
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
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
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
						* XMMatrixRotationX(-XM_PI / 2.f)
						* XMMatrixRotationY(playerArr[i]._prev_degree * XM_PI / 180.f)
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
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
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
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
						* XMMatrixRotationX(-XM_PI / 2.f)
						* XMMatrixRotationY(playerArr[i]._prev_degree * XM_PI / 180.f)
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
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
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
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
						* XMMatrixRotationX(-XM_PI / 2.f)
						* XMMatrixRotationY(playerArr[i]._prev_degree * XM_PI / 180.f)
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
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
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
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
						* XMMatrixRotationX(-XM_PI / 2.f)
						* XMMatrixRotationY(playerArr[i]._prev_degree * XM_PI / 180.f)
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
				XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
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
					XMStoreFloat4x4(&_transform.world, XMMatrixScaling(_scale, _scale, _scale)
						* XMMatrixRotationX(-XM_PI / 2.f)
						* XMMatrixRotationY(playerArr[i]._prev_degree * XM_PI / 180.f)
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

		/*// ƒ≥∏Ø≈Õ ∞¯∞› ƒ›∂Û¿Ã¥ı
		if(playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Attack
			&& playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f)
		{
			cmdList->SetPipelineState(testCube._pipelineState.Get());
			cmdList->IASetVertexBuffers(0, 1, &testCube._vertexBufferView);
			cmdList->IASetIndexBuffer(&testCube._indexBufferView);
			
			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(testCharacter.Extents.x, testCharacter.Extents.y, testCharacter.Extents.z)
				*XMMatrixRotationQuaternion(XMLoadFloat4(&testCharacter.Orientation))
				* XMMatrixTranslation(testCharacter.Center.x, testCharacter.Center.y, testCharacter.Center.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			testCube._tex._srvHandle = testCube._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
			descHeapPtr->CopyDescriptor(testCube._tex._srvHandle, 5, devicePtr);

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(testCube._indexCount, 1, 0, 0, 0);
		}

		// ƒ≥∏Ø≈Õ Ω∫≈≥ ƒ›∂Û¿Ã¥ı
		if(playerArr[networkPtr->myClientId]._animation_state == AnimationOrder::Skill
			&& playerArr[networkPtr->myClientId]._animation_time_pos >= player_AKI_Body_asset._animationPtr->GetClipEndTime(playerArr[networkPtr->myClientId]._animation_state) * 0.5f)
		{
			cmdList->SetPipelineState(testCube._pipelineState.Get());
			cmdList->IASetVertexBuffers(0, 1, &testCube._vertexBufferView);
			cmdList->IASetIndexBuffer(&testCube._indexBufferView);

			XMStoreFloat4x4(&_transform.world, XMMatrixScaling(testCharacter2.Extents.x, testCharacter2.Extents.y, testCharacter2.Extents.z)
				*XMMatrixRotationQuaternion(XMLoadFloat4(&testCharacter2.Orientation))
				* XMMatrixTranslation(testCharacter2.Center.x, testCharacter2.Center.y, testCharacter2.Center.z));
			XMMATRIX world = XMLoadFloat4x4(&_transform.world);
			XMStoreFloat4x4(&_transform.world, XMMatrixTranspose(world));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = constantBufferPtr->PushData(0, &_transform, sizeof(_transform));
			descHeapPtr->CopyDescriptor(handle, 0, devicePtr);
			testCube._tex._srvHandle = testCube._tex._srvHeap->GetCPUDescriptorHandleForHeapStart();
			descHeapPtr->CopyDescriptor(testCube._tex._srvHandle, 5, devicePtr);

			descHeapPtr->CommitTable_multi(cmdQueuePtr, i_now_render_index);
			cmdList->DrawIndexedInstanced(testCube._indexCount, 1, 0, 0, 0);
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

		// Boss
		switch (Scene_num)
		{
		case 0:
		case 1:
			if (boss_obj._on == true)
			{
				XMFLOAT3 boss_scale = XMFLOAT3(800.f, 800.f, 800.f);
				float boss_default_rot_x = -XM_PI * 0.5f;
				Boss(cmdList, boss, i_now_render_index, boss_scale, boss_default_rot_x);
			}
			break;
		case 2:
			if (boss_obj._on == true)
			{
				XMFLOAT3 boss2_scale = XMFLOAT3(1.f, 1.f, 1.f);
				float boss2_default_rot_x = 0.f;
				Boss(cmdList, boss2, i_now_render_index, boss2_scale, boss2_default_rot_x);
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

		// boss2Skill
		for (const Boss2SkillData& boss2_skill : boss_obj.boss2_skill_vec)
		{
			XMFLOAT3 boss2_skill_scale = XMFLOAT3(boss2_skill.scale, boss2_skill.scale, boss2_skill.scale);
			Boss2Skill(cmdList, boss2Skill, i_now_render_index, boss2_skill_time[0], boss2_skill.pos, boss2_skill_scale);
		}

		//XMFLOAT3 boss2_skill_pos2 = XMFLOAT3(167.f, 0.01f, -240.f);
		//XMFLOAT3 boss2_skill_scale2 = XMFLOAT3(1.f, 1.f, 1.f);
		//Boss2Skill(cmdList, boss2_skill_circle, i_now_render_index, boss2_skill_time[1], boss2_skill_pos2, boss2_skill_scale2);

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
			if (npcArr[i]._on == true && i != 9) {
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
				_transform.max_hp = 100; // ∆–≈∂ √º≈©

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

		// map
		switch (Scene_num) {
		case 0:
			Map(cmdList, stage0_map, map_asset, i_now_render_index, Scene_num);
			break;
		case 1:
			Map(cmdList, floor, map_asset, i_now_render_index, Scene_num);
			break;
		case 2:
			Map(cmdList, floor, map_asset, i_now_render_index, Scene_num);
			break;
		case 3:
			Map(cmdList, floor, map_asset, i_now_render_index, Scene_num);
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
		}

		// pillar
		for (int i = 0; i < CubeMax; ++i)
		{
			if (pillars_data[i]._on == true)
			{
				XMVECTOR P = XMVectorSet(0.f, 0.f, 0.f, 1.f);
				XMVECTOR Q = XMVectorSet(0.f, 0.f, 0.f, 1.f);
				switch (pillars_data[i]._pillar_count)
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
						particles[index].moveSpeed = (float)(rand() % 101) / 50 + 2.f; // 2~4
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
		d11Ptr->draw_player_info(L"AKI", 100, playerArr[0]._hp, 0);
		d11Ptr->draw_bossUI(boss_obj._hp, Scene_num,boss_obj);
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
