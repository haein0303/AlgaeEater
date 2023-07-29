#pragma once
#include "Util.h"
#include "AnimationObjectLoader.h"

class OBJECT;

class AnimationObject
{
public:
	string mSkinnedModelFilename;
	vector<Subset> mSubsets;
	vector<int> mBoneHierarchy;
	vector<XMFLOAT4X4> mBoneOffsets;
	vector<vector<vector<Keyframe>>> animations;

	//vector<XMFLOAT4X4> FinalTransforms;
	string ClipName;
	//float TimePos = 0.0f;

public:
	void CreateAnimationObject(vector<SkinnedVertex>& vertices, vector<UINT>& indices, const string& filePath);

	void CreateVertexAnimationObject(vector<Vertex>& vertices, vector<UINT>& indices, const string& filePath);

	void CreateVertexAnimationObjectForPillar(vector<Vertex>& vertices, vector<UINT>& indices, const string& filePath);

#pragma region ��Ű�� �ִϸ��̼�
	// �� �����Ӹ��� ��Ű�� �ִϸ��̼��� ����
	void UpdateSkinnedAnimation(float dt, OBJECT& player, int i, int character_num);

	// ������ �ִϸ��̼� ��ĵ鿡 ���ؼ� ���� ��� ����
	void GetFinalTransforms(const string& clipName, float timePos, vector<XMFLOAT4X4>& finalTransforms, int state);

	// ���� �����ӿ� ���� �ִϸ��̼� ����
	void Interpolate(vector<Keyframe> keyframeVec, float t, XMFLOAT4X4& M);
#pragma endregion

#pragma region ���� �ִϸ��̼�
	// �� �����Ӹ��� ���� �ִϸ��̼��� ����
	void UpdateVertexAnimation(OBJECT& player, XMVECTOR& P, XMVECTOR& Q, ObjectType obj_type);

	// ������ �ִϸ��̼� ��ĵ鿡 ���ؼ� ���� ��� ����
	void GetFinalTransforms_VertexAnimation(const string& clipName, float timePos, int state, XMVECTOR& P, XMVECTOR& Q);

	void Interpolate_VertexAnimation(vector<Keyframe> keyframeVec, float t, XMVECTOR& P, XMVECTOR& Q);
#pragma endregion

	template<typename T>
	static T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}

	// �ִϸ��̼� Ŭ���� ������ �ð��� ��´�.
	float GetClipEndTime(int state);
};