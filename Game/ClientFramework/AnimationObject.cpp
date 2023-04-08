#include "AnimationObject.h"

void AnimationObject::CreateAnimationObject(vector<SkinnedVertex>& vertices, vector<UINT>& indices, const string& filePath) {
	AnimationObjectLoader animationObjectLoader;
	animationObjectLoader.LoadAnimationObject(filePath, vertices, indices, mSubsets, mBoneHierarchy, mBoneOffsets, animations);

	FinalTransforms.resize(mBoneHierarchy.size());
	ClipName = "SkinningAnimtion";
	TimePos = 0.0f;
}

void AnimationObject::UpdateSkinnedAnimation(float dt)
{
	TimePos += dt;

	if (state0 != state)
	{
		TimePos = 0.f;
		state0 = state;
	}

	// 애니메이션이 끝나면 애니메이션 루프
	if (TimePos > GetClipEndTime()) {
		TimePos = 0.f;
	}

	// 현재 프레임에 대해 최종행렬 연산
	GetFinalTransforms(ClipName, TimePos, FinalTransforms);
}

float AnimationObject::GetClipEndTime() {
	float t = 0.0f;
	for (UINT i = 0; i < animations[state].size(); ++i)
	{
		t = Max(t, animations[state][i].back().TimePos);
	}

	return t;
}

void AnimationObject::Interpolate(vector<Keyframe> keyframeVec, float t, XMFLOAT4X4& M) {
	if (t <= keyframeVec.front().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&keyframeVec.front().Scale);
		XMVECTOR P = XMLoadFloat3(&keyframeVec.front().Translation);
		XMVECTOR Q = XMLoadFloat4(&keyframeVec.front().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (t >= keyframeVec.back().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&keyframeVec.back().Scale);
		XMVECTOR P = XMLoadFloat3(&keyframeVec.back().Translation);
		XMVECTOR Q = XMLoadFloat4(&keyframeVec.back().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else
	{
		for (UINT i = 0; i < keyframeVec.size() - 1; ++i)
		{
			if (t >= keyframeVec[i].TimePos && t <= keyframeVec[i + 1].TimePos)
			{
				float lerpPercent = (t - keyframeVec[i].TimePos) / (keyframeVec[i + 1].TimePos - keyframeVec[i].TimePos);

				XMVECTOR s0 = XMLoadFloat3(&keyframeVec[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&keyframeVec[i + 1].Scale);

				XMVECTOR p0 = XMLoadFloat3(&keyframeVec[i].Translation);
				XMVECTOR p1 = XMLoadFloat3(&keyframeVec[i + 1].Translation);

				XMVECTOR q0 = XMLoadFloat4(&keyframeVec[i].RotationQuat);
				XMVECTOR q1 = XMLoadFloat4(&keyframeVec[i + 1].RotationQuat);

				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

				break;
			}
		}
	}
}

void AnimationObject::GetFinalTransforms(const string& clipName, float timePos, vector<XMFLOAT4X4>& finalTransforms) {
	UINT numBones = mBoneOffsets.size();

	vector<XMFLOAT4X4> toParentTransforms(numBones);

	for (UINT i = 0; i < animations[state].size(); ++i)
	{
		Interpolate(animations[state][i], timePos, toParentTransforms[i]);
	}

	vector<XMFLOAT4X4> toRootTransforms(numBones);

	toRootTransforms[0] = toParentTransforms[0];

	for (UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		int parentIndex = mBoneHierarchy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixTranspose(finalTransform));
	}
}