#pragma once
//라이브러리
#pragma warning(disable:4996)

#include <Windows.h>
#include <tchar.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXColors.h>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "DirectXTex.h"
#include "DirectXTex.inl"

#pragma region sfml
#define SFML_STATIC 1

#include <SFML/System.hpp>
#include <SFML/Network/Ftp.hpp>
#include <SFML/Network/Http.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/SocketHandle.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>

#ifdef _DEBUG
#pragma comment (lib, "lib/sfml-system-s-d.lib")
#pragma comment (lib, "lib/sfml-network-s-d.lib")
#else
#pragma comment (lib, "lib/sfml-system-s.lib")
#pragma comment (lib, "lib/sfml-network-s.lib")
#endif
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

#define CHARACTERINDEX 0
#define PLAYERMAX 4
#define NPCMAX 10
#define CubeMax 4
#pragma endregion

#ifdef _DEBUG
#pragma comment(lib, "..\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "..\\DirectXTex.lib")
#endif

#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "D3D12.lib")
#pragma comment (lib, "dxgi.lib")

#include <dxgidebug.h>
#include <comutil.h>

#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxguid.lib")

//네임스페이스
using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;
namespace fs = std::filesystem;

#define SWAP_CHAIN_BUFFER_COUNT 2
#define CBV_REGISTER_COUNT 5
#define SRV_REGISTER_COUNT 5
#define REGISTER_COUNT 10

//조명정보
#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

namespace COMUtil
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		explicit com_exception(HRESULT hr) noexcept
			: result(hr) {}

		const char* what() const noexcept override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X", result);
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}

	inline void Init()
	{
		ThrowIfFailed(CoInitializeEx(nullptr, 0));
	}
} // namespace COMUtil

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};

struct LightInfo
{
	//float4인 이유는 float4로 선언하면 패딩값을 일일이 채울 필요가 없어서
	XMFLOAT4 diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.f);
	XMFLOAT4 ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.f);
	XMFLOAT4 specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.f);
	XMFLOAT4 direction = XMFLOAT4(-1.f, -1.f, 1.f, 0.f);
};

//정점 구조체
struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 normal;
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
	XMFLOAT4X4 world = Identity4x4();
	XMFLOAT4X4 view = Identity4x4();
	XMFLOAT4X4 proj = Identity4x4();
	LightInfo lnghtInfo;
	XMFLOAT4X4 TexTransform = Identity4x4();
	XMFLOAT4X4 MatTransform = Identity4x4();
	XMFLOAT4X4 BoneTransforms[96];
};

struct Obj
{
	bool on = false;
	float	degree;
	XMFLOAT4 transform = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	bool isCollision = false;
	bool isFirstCollision = false;
	chrono::milliseconds time;
};

struct Point
{
	XMFLOAT3 pos;
	XMFLOAT2 size;
};

// 윈도우와 관련된 정보
struct WindowInfo {
	HWND hwnd;
	int ClientWidth = 1280;
	int ClientHeight = 720;
};

// 파티클
struct ParticleData {
	int alive = 0;
	XMVECTOR dir;
	float moveSpeed;
	XMVECTOR pos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	float lifeTime = 0.f;
	float curTime = 0.f;
};

// 애니메이션
struct AniamtionConstants
{
	XMFLOAT4X4 world = Identity4x4();
	XMFLOAT4X4 view = Identity4x4();
	XMFLOAT4X4 proj = Identity4x4();
	XMFLOAT4X4 TexTransform = Identity4x4();
	XMFLOAT4X4 MatTransform = Identity4x4();
	XMFLOAT4X4 BoneTransforms[96];
};

struct Keyframe  //bone하나 기준
{
	float TimePos = 0.0f;
	XMFLOAT3 Translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 RotationQuat = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

struct SkinnedVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Normal;
	XMFLOAT2 TexC;
	XMFLOAT3 TangentU;
	XMFLOAT3 BoneWeights;
	BYTE BoneIndices[4];
};

struct Subset
{
	UINT Id = -1;
	UINT VertexStart = 0;
	UINT VertexCount = 0;
	UINT FaceStart = 0;
	UINT FaceCount = 0;
};

struct FrameHierarchy
{
	int index, parentindex;
	string myname, parentname;
	vector<XMFLOAT4X4> boneOffsets;
	int boneIndexToParentIndex;
};


#pragma once
constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

constexpr int MAX_USER = PLAYERMAX;
constexpr int NPC_NUM = NPCMAX;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CONSOLE = 2;

constexpr char SC_LOGIN_OK = 11;
constexpr char SC_ADD_OBJECT = 12;
constexpr char SC_REMOVE_OBJECT = 13;
constexpr char SC_MOVE_OBJECT = 14;
constexpr char SC_ADD_CUBE = 15;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	float	x;
	float	y;
	float	z;
	float	degree;
	int		char_state;
	unsigned int client_time;
};

struct CS_CONSOLE_PACKET {
	unsigned char size;
	char	type;
	int console;
};

struct SC_LOGIN_OK_PACKET {
	unsigned char size;
	char	type;
	int	id;
	float	x, y, z;
	float	degree;
	int		hp;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	char	name[NAME_SIZE];
	int		hp;
	int		char_state;
};

struct SC_ADD_CUBE_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	char	name[NAME_SIZE];
	int		hp;
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int	id;
};

struct SC_MOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int	id;
	float	x, y, z;
	float	degree;
	char	name[NAME_SIZE];
	int		hp;
	int		char_state;
	std::chrono::milliseconds time;
};

#pragma pack (pop)