#pragma once
#include "Util.h"
#include "AnimationObjectLoader.h"

class AnimationObject
{
public:
	string mSkinnedModelFilename;
	vector<Subset> mSubsets;
	vector<int> mBoneHierarchy;
	vector<XMFLOAT4X4> mBoneOffsets;
	vector<vector<vector<Keyframe>>> animations;

	vector<XMFLOAT4X4> FinalTransforms;
	string ClipName;
	float TimePos = 0.0f;

	int state = 0;
	int state0 = 0;

public:
	void CreateAnimationObject(vector<SkinnedVertex>& vertices, vector<UINT>& indices, const string& filePath);

	// �� �����Ӹ��� �ִϸ��̼��� ����
	void UpdateSkinnedAnimation(float dt);

	template<typename T>
	static T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}

	// �ִϸ��̼� Ŭ���� ������ �ð��� ��´�.
	float GetClipEndTime();

	// ���� �����ӿ� ���� �ִϸ��̼� ����
	void Interpolate(vector<Keyframe> keyframeVec, float t, XMFLOAT4X4& M);

	// ������ �ִϸ��̼� ��ĵ鿡 ���ؼ� ���� ��� ����
	void GetFinalTransforms(const string& clipName, float timePos, vector<XMFLOAT4X4>& finalTransforms);
};