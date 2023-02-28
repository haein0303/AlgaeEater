#pragma once
//라이브러리
#include <Windows.h>
#include <tchar.h>

#include <dxgi1_4.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include "../SkinningAnimation/lib/d3dx12.h"
#include <DirectXColors.h>
#include <vector>

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../SkinningAnimation/lib/DirectXTex.h"
#include "../SkinningAnimation/lib/DirectXTex.inl"

#ifdef _DEBUG
#pragma comment(lib, "../SkinningAnimation/lib/DirectXTex_debug.lib")
#else
#pragma comment(lib, "../SkinningAnimation/lib/DirectXTex.lib")
#endif

#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "D3D12.lib")
#pragma comment (lib, "dxgi.lib")
#pragma warning(disable:4996)

//네임스페이스
using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;
namespace fs = std::filesystem;

#define SWAP_CHAIN_BUFFER_COUNT 2
#define CBV_REGISTER_COUNT 5
#define SRV_REGISTER_COUNT 5
#define REGISTER_COUNT 10
#define PARTICLE_NUM 256

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
	XMFLOAT4X4 TexTransform = Identity4x4();
	XMFLOAT4X4 MatTransform = Identity4x4();
	XMFLOAT4X4 BoneTransforms[96];
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

struct SkinnedVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	XMFLOAT3 TangentU;
	XMFLOAT3 BoneWeights;
	BYTE BoneIndices[4];
};

struct Keyframe  //bone하나 기준
{
	float TimePos = 0.0f;
	XMFLOAT3 Translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 RotationQuat = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

struct AnimationClip
{
	template<typename T>
	static T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}

	float GetClipEndTime()const {
		// Find largest end time over all bones in this clip.
		float t = 0.0f;
		for (UINT i = 0; i < animationVec.size(); ++i)
		{
			t = Max(t, animationVec[i].back().TimePos);
		}

		return t;
	}

	void Interpolate(vector<Keyframe> keyframeVec, float t, XMFLOAT4X4& M)const {
		if (t <= keyframeVec.front().TimePos)
		{
			XMVECTOR S = XMLoadFloat3(&keyframeVec.front().Scale);
			XMVECTOR P = XMLoadFloat3(&keyframeVec.front().Translation);
			XMVECTOR Q = XMLoadFloat4(&keyframeVec.front().RotationQuat);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
		}
		else if (t >= keyframeVec.back().TimePos)
		{
			XMVECTOR S = XMLoadFloat3(&keyframeVec.back().Scale);
			XMVECTOR P = XMLoadFloat3(&keyframeVec.back().Translation);
			XMVECTOR Q = XMLoadFloat4(&keyframeVec.back().RotationQuat);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
		}
		else
		{
			for (UINT i = 0; i < keyframeVec.size() - 1; ++i)
			{
				if (t >= keyframeVec[i].TimePos && t <= keyframeVec[i + 1].TimePos)
				{
					float lerpPercent = (t - keyframeVec[i].TimePos) / (keyframeVec[i + 1].TimePos - keyframeVec[i].TimePos);

					XMVECTOR s0 = XMLoadFloat3(&keyframeVec[i].Scale);
					XMVECTOR s1 = XMLoadFloat3(&keyframeVec[i + 1].Scale);

					XMVECTOR p0 = XMLoadFloat3(&keyframeVec[i].Translation);
					XMVECTOR p1 = XMLoadFloat3(&keyframeVec[i + 1].Translation);

					XMVECTOR q0 = XMLoadFloat4(&keyframeVec[i].RotationQuat);
					XMVECTOR q1 = XMLoadFloat4(&keyframeVec[i + 1].RotationQuat);

					XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
					XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
					XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

					XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
					XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

					break;
				}
			}
		}
	}

	void GetFinalTransforms(const std::string& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms)const {
		UINT numBones = mBoneOffsets.size();

		std::vector<XMFLOAT4X4> toParentTransforms(numBones);

		for (UINT i = 0; i < animationVec.size(); ++i)
		{
			Interpolate(animationVec[i], timePos, toParentTransforms[i]);
		}

		std::vector<XMFLOAT4X4> toRootTransforms(numBones);

		toRootTransforms[0] = toParentTransforms[0];

		for (UINT i = 1; i < numBones; ++i)
		{
			XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

			int parentIndex = mBoneHierarchy[i];
			XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

			XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

			XMStoreFloat4x4(&toRootTransforms[i], toRoot);
		}

		for (UINT i = 0; i < numBones; ++i)
		{
			XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
			XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
			XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);
			XMStoreFloat4x4(&finalTransforms[i], XMMatrixTranspose(finalTransform));
		}
	}

	vector<int> mBoneHierarchy;
	vector<DirectX::XMFLOAT4X4> mBoneOffsets;
	vector<vector<Keyframe>> animationVec; //animationVec = animationClip / vector<Keyframe>, BoneAnimation, animation : 전체본의 한프레임 애니메이션
};