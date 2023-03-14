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

	// �� �����Ӹ��� �ִϸ��̼��� ����
	void UpdateSkinnedAnimation(float dt);
};