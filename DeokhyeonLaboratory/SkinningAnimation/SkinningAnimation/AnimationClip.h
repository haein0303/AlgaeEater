#pragma once
#include "Util.h"

class AnimationClip
{
public:
	vector<int> mBoneHierarchy;
	vector<XMFLOAT4X4> mBoneOffsets;
	vector<vector<Keyframe>> animationVec;

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
