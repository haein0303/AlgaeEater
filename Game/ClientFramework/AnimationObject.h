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

	// 매 프레임마다 애니메이션을 진행
	void UpdateSkinnedAnimation(float dt);

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