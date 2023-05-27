#pragma once
#include "Util.h"

class AnimationObjectLoader
{
public:
	// ��Ű�� �ִϸ��̼� ������Ʈ ������ ���Ϸκ��� �����͸� �Ľ� �� ó���ϰ� �ε�
	void LoadAnimationObject(const string& filename, vector<SkinnedVertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets,
		vector<int>& mBoneHierarchy, vector<XMFLOAT4X4>& mBoneOffsets, vector<vector<vector<Keyframe>>>& animations);

	// ���� �ִϸ��̼� ������Ʈ ������ ���Ϸκ��� �����͸� �Ľ� �� ó���ϰ� �ε�
	void LoadVertexAnimationObject(const string& filename, vector<Vertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets,
		vector<int>& mBoneHierarchy, vector<XMFLOAT4X4>& mBoneOffsets, vector<vector<vector<Keyframe>>>& animations);
};