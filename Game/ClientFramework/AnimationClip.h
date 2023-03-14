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

	// 애니메이션 클립이 끝나는 시간을 얻는다.
	float GetClipEndTime();

	// 현재 프레임에 대한 애니메이션 보간
	void Interpolate(vector<Keyframe> keyframeVec, float t, XMFLOAT4X4& M);

	// 보간된 애니메이션 행렬들에 대해서 최종 행렬 연산
	void GetFinalTransforms(const string& clipName, float timePos, vector<XMFLOAT4X4>& finalTransforms);
};
