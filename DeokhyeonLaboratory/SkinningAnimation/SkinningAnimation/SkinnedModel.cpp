#include "SkinnedModel.h"

void SkinnedModel::UpdateSkinnedAnimation(float dt)
{
	TimePos += dt;

	// 애니메이션이 끝나면 애니메이션 루프
	if (TimePos > AnimationInfo->GetClipEndTime()) {
		TimePos = 0.0f;
	}

	// 현재 프레임에 대해 최종행렬 연산
	AnimationInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
}