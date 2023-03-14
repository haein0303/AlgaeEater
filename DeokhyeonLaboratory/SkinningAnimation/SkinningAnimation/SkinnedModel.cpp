#include "SkinnedModel.h"

void SkinnedModel::UpdateSkinnedAnimation(float dt)
{
	TimePos += dt;

	// �ִϸ��̼��� ������ �ִϸ��̼� ����
	if (TimePos > AnimationInfo->GetClipEndTime()) {
		TimePos = 0.0f;
	}

	// ���� �����ӿ� ���� ������� ����
	AnimationInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
}