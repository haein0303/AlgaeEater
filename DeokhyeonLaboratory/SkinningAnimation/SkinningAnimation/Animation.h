#pragma once
#include "Util.h"

class SkinningAnimation
{
public:

	struct Subset
	{
		UINT Id = -1;
		UINT VertexStart = 0;
		UINT VertexCount = 0;
		UINT FaceStart = 0;
		UINT FaceCount = 0;
	};

	struct Material
	{
		string Name;

		XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.8f;
		bool AlphaClip = false;

		string MaterialTypeName;
		string DiffuseMapName;
		string NormalMapName;
	};

	bool LoadM3d(const string& filename, vector<SkinnedVertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets, vector<Material>& mats, AnimationClip& skinInfo);

private:
	void ReadMaterials(ifstream& fin, UINT numMaterials, vector<Material>& mats);

	void ReadSubsetTable(ifstream& fin, UINT numSubsets, vector<Subset>& subsets);

	//��Ű�׹��ؽ� ������ �ε�
	void ReadSkinnedVertices(ifstream& fin, UINT numVertices, vector<SkinnedVertex>& vertices);

	void ReadTriangles(ifstream& fin, UINT numTriangles, vector<UINT>& indices);

	//bone offset��� �ε�
	void ReadBoneOffsets(ifstream& fin, UINT numBones, vector<XMFLOAT4X4>& boneOffsets);

	//bone �������� �ε�
	void ReadBoneHierarchy(ifstream& fin, UINT numBones, vector<int>& boneIndexToParentIndex);

	//�ִϸ��̼� ���� �ε�
	void ReadAnimationClips(ifstream& fin, UINT numBones, UINT numAnimationClips, vector<vector<Keyframe>>& boneAnimations);

	//Ű������ �ִϸ��̼� �ε�
	void ReadBoneKeyframes(ifstream& fin, UINT numBones, vector<Keyframe>& boneAnimation);
};

struct SkinnedModelInstance
{
	AnimationClip* SkinnedInfo = nullptr;
	vector<XMFLOAT4X4> FinalTransforms;
	string ClipName;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		if (TimePos > SkinnedInfo->GetClipEndTime())
			TimePos = 0.0f;

		SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
	}
};

