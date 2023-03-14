#pragma once
#include "Util.h"
#include "AnimationClip.h"

class AnimationObjectLoader
{
public:
	// 오브젝트 데이터 파일로부터 데이터를 파싱 및 처리하고 로드
	void LoadAnimationObject(const string& filename, vector<SkinnedVertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets, AnimationClip& skinInfo);
};