#pragma once
#include "Util.h"
#include "SkinnedModel.h"
#include "AnimationObjectLoader.h"

class AnimationObject
{
public:
	string mSkinnedModelFilename;
	AnimationClip mSkinnedInfo;
	vector<Subset> mSubsets;
	unique_ptr<SkinnedModel> mSkinnedModel;

	void CreateAnimationObject(vector<SkinnedVertex>& vertices, vector<UINT>& indices, const string& filePath);
};