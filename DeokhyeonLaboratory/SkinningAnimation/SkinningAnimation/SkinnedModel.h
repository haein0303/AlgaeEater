#pragma once
#include "Util.h"
#include "AnimationClip.h"

class SkinnedModel
{
public:
	AnimationClip* AnimationInfo = nullptr;
	vector<XMFLOAT4X4> FinalTransforms;
	string ClipName;
	float TimePos = 0.0f;

	// 매 프레임마다 애니메이션을 진행
	void UpdateSkinnedAnimation(float dt);
};