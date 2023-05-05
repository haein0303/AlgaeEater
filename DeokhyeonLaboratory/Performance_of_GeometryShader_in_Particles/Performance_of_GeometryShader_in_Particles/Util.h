#pragma once
//라이브러리
#include <Windows.h>
#include <tchar.h>

#include <dxgi1_4.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include "../Performance_of_GeometryShader_in_Particles/lib/d3dx12.h"
#include <DirectXColors.h>
#include <vector>

#include <string>
#include <filesystem>

#include "../Performance_of_GeometryShader_in_Particles/lib/DirectXTex.h"
#include "../Performance_of_GeometryShader_in_Particles/lib/DirectXTex.inl"

#ifdef _DEBUG
#pragma comment(lib, "../Performance_of_GeometryShader_in_Particles/lib/DirectXTex_debug.lib")
#else
#pragma comment(lib, "../Performance_of_GeometryShader_in_Particles/lib/DirectXTex.lib")
#endif

#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "D3D12.lib")
#pragma comment (lib, "dxgi.lib")

//네임스페이스
using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;
namespace fs = std::filesystem;

#define SWAP_CHAIN_BUFFER_COUNT 2
#define CBV_REGISTER_COUNT 5
#define SRV_REGISTER_COUNT 5
#define REGISTER_COUNT 10
#define PARTICLE_NUM 800
#define CONSTANT_COUNT 1000

//정점 구조체
struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
	XMFLOAT2 uv;
};

//행렬을 단위행렬로 초기화
static XMFLOAT4X4 Identity4x4()
{
	static XMFLOAT4X4 I(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return I;
}

//상수버퍼로 넘겨줄 구조체의 형태
struct Constants
{
	XMFLOAT4X4 worldViewProj = Identity4x4();
	XMFLOAT4X4 ViewProj = Identity4x4();
};

//윈도우와 관련된 정보
struct WindowInfo {
	HWND hwnd;
	int ClientWidth = 600;
	int ClientHeight = 600;
};

struct ParticleData
{
	int alive = 0;
	XMVECTOR dir;
	float moveSpeed;
	XMVECTOR pos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	float lifeTime = 0.f;
	float curTime = 0.f;
};

struct GSPoint
{
	XMFLOAT3 pos;
	XMFLOAT2 size;
};