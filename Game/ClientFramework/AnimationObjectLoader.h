#pragma once
#include "Util.h"

class AnimationObjectLoader
{
public:
	// 스키닝 애니메이션 오브젝트 데이터 파일로부터 데이터를 파싱 및 처리하고 로드
	void LoadAnimationObject(const string& filename, vector<SkinnedVertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets,
		vector<int>& mBoneHierarchy, vector<XMFLOAT4X4>& mBoneOffsets, vector<vector<vector<Keyframe>>>& animations);

	// 정점 애니메이션 오브젝트 데이터 파일로부터 데이터를 파싱 및 처리하고 로드
	void LoadVertexAnimationObject(const string& filename, vector<Vertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets,
		vector<int>& mBoneHierarchy, vector<XMFLOAT4X4>& mBoneOffsets, vector<vector<vector<Keyframe>>>& animations);
};