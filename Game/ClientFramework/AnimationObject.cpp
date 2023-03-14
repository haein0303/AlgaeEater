#include "AnimationObject.h"

void AnimationObject::CreateAnimationObject(vector<SkinnedVertex>& vertices, vector<UINT>& indices, const string& filePath) {
	AnimationObjectLoader animationObjectLoader;
	animationObjectLoader.LoadAnimationObject(filePath, vertices, indices, mSubsets, mSkinnedInfo);

	mSkinnedModel = make_unique<SkinnedModel>();
	mSkinnedModel->AnimationInfo = &mSkinnedInfo;
	mSkinnedModel->FinalTransforms.resize(mSkinnedInfo.mBoneHierarchy.size());
	mSkinnedModel->ClipName = "SkinningAnimtion";
	mSkinnedModel->TimePos = 0.0f;
}