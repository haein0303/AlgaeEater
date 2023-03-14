#pragma once
#include "Util.h"
#include "AnimationClip.h"

class AnimationObjectLoader
{
public:
	// ������Ʈ ������ ���Ϸκ��� �����͸� �Ľ� �� ó���ϰ� �ε�
	void LoadAnimationObject(const string& filename, vector<SkinnedVertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets, AnimationClip& skinInfo);
};