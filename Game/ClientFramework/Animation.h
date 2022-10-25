#pragma once
#include "Util.h"
#include <fbxsdk.h>
#include <fstream>

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

struct Keyframe
{
	float TimePos = 0.0f;
	XMFLOAT3 Translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 RotationQuat = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

class M3DLoader
{
public:
	struct SkinnedVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexC;
		DirectX::XMFLOAT3 TangentU;
		DirectX::XMFLOAT3 BoneWeights;
		BYTE BoneIndices[4];
	};

	struct Subset
	{
		UINT Id = -1;
		UINT VertexStart = 0;
		UINT VertexCount = 0;
		UINT FaceStart = 0;
		UINT FaceCount = 0;
	};

	struct M3dMaterial
	{
		std::string Name;

		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.8f;
		bool AlphaClip = false;

		std::string MaterialTypeName;
		std::string DiffuseMapName;
		std::string NormalMapName;
	};

	bool LoadM3d(const std::string& filename,
		std::vector<SkinnedVertex>& vertices,
		std::vector<UINT>& indices,
		std::vector<Subset>& subsets,
		std::vector<M3dMaterial>& mats,
		AnimationClip& skinInfo)
	{
		std::ifstream fin(filename);

		UINT numMaterials = 0;
		UINT numVertices = 0;
		UINT numTriangles = 0;
		UINT numBones = 0;
		UINT numAnimationClips = 0;

		std::string ignore;

		if (fin)
		{
			fin >> ignore; // file header text
			fin >> ignore >> numMaterials;
			fin >> ignore >> numVertices;
			fin >> ignore >> numTriangles;
			fin >> ignore >> numBones;
			fin >> ignore >> numAnimationClips;

			std::vector<XMFLOAT4X4> boneOffsets;
			std::vector<int> boneIndexToParentIndex;
			vector<vector<Keyframe>> animations;
			//std::unordered_map<std::string, AnimationClip> animations;

			ReadMaterials(fin, numMaterials, mats);
			ReadSubsetTable(fin, numMaterials, subsets);
			ReadSkinnedVertices(fin, numVertices, vertices);
			ReadTriangles(fin, numTriangles, indices);
			ReadBoneOffsets(fin, numBones, boneOffsets);
			ReadBoneHierarchy(fin, numBones, boneIndexToParentIndex);
			ReadAnimationClips(fin, numBones, numAnimationClips, animations);

			skinInfo.mBoneHierarchy = boneIndexToParentIndex;
			skinInfo.mBoneOffsets = boneOffsets;
			skinInfo.animationVec = animations;

			return true;
		}
		return false;
	}

private:



	void ReadMaterials(std::ifstream& fin, UINT numMaterials, std::vector<M3dMaterial>& mats)
	{
		std::string ignore;
		mats.resize(numMaterials);

		std::string diffuseMapName;
		std::string normalMapName;

		fin >> ignore; // materials header text
		for (UINT i = 0; i < numMaterials; ++i)
		{
			fin >> ignore >> mats[i].Name;
			fin >> ignore >> mats[i].DiffuseAlbedo.x >> mats[i].DiffuseAlbedo.y >> mats[i].DiffuseAlbedo.z;
			fin >> ignore >> mats[i].FresnelR0.x >> mats[i].FresnelR0.y >> mats[i].FresnelR0.z;
			fin >> ignore >> mats[i].Roughness;
			fin >> ignore >> mats[i].AlphaClip;
			fin >> ignore >> mats[i].MaterialTypeName;
			fin >> ignore >> mats[i].DiffuseMapName;
			fin >> ignore >> mats[i].NormalMapName;
		}
	}

	void ReadSubsetTable(std::ifstream& fin, UINT numSubsets, std::vector<Subset>& subsets)
	{
		std::string ignore;
		subsets.resize(numSubsets);

		fin >> ignore; // subset header text
		for (UINT i = 0; i < numSubsets; ++i)
		{
			fin >> ignore >> subsets[i].Id;
			fin >> ignore >> subsets[i].VertexStart;
			fin >> ignore >> subsets[i].VertexCount;
			fin >> ignore >> subsets[i].FaceStart;
			fin >> ignore >> subsets[i].FaceCount;
		}
	}

	//스키닝관련데이터로드
	void ReadSkinnedVertices(std::ifstream& fin, UINT numVertices, std::vector<SkinnedVertex>& vertices)
	{
		std::string ignore;
		vertices.resize(numVertices);

		fin >> ignore; // vertices header text
		int boneIndices[4];
		float weights[4];
		for (UINT i = 0; i < numVertices; ++i)
		{
			float blah;
			fin >> ignore >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
			fin >> ignore >> vertices[i].TangentU.x >> vertices[i].TangentU.y >> vertices[i].TangentU.z >> blah /*vertices[i].TangentU.w*/;
			fin >> ignore >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
			fin >> ignore >> vertices[i].TexC.x >> vertices[i].TexC.y;
			fin >> ignore >> weights[0] >> weights[1] >> weights[2] >> weights[3];
			fin >> ignore >> boneIndices[0] >> boneIndices[1] >> boneIndices[2] >> boneIndices[3];

			vertices[i].BoneWeights.x = weights[0];
			vertices[i].BoneWeights.y = weights[1];
			vertices[i].BoneWeights.z = weights[2];

			vertices[i].BoneIndices[0] = (BYTE)boneIndices[0];
			vertices[i].BoneIndices[1] = (BYTE)boneIndices[1];
			vertices[i].BoneIndices[2] = (BYTE)boneIndices[2];
			vertices[i].BoneIndices[3] = (BYTE)boneIndices[3];
		}
	}

	void ReadTriangles(std::ifstream& fin, UINT numTriangles, std::vector<UINT>& indices)
	{
		std::string ignore;
		indices.resize(numTriangles * 3);

		fin >> ignore; // triangles header text
		for (UINT i = 0; i < numTriangles; ++i)
		{
			fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
		}
	}

	//오프셋행렬 구하기 데이터 로드
	void ReadBoneOffsets(std::ifstream& fin, UINT numBones, std::vector<XMFLOAT4X4>& boneOffsets)
	{
		std::string ignore;
		boneOffsets.resize(numBones);

		fin >> ignore; // BoneOffsets header text
		for (UINT i = 0; i < numBones; ++i)
		{
			fin >> ignore >>
				boneOffsets[i](0, 0) >> boneOffsets[i](0, 1) >> boneOffsets[i](0, 2) >> boneOffsets[i](0, 3) >>
				boneOffsets[i](1, 0) >> boneOffsets[i](1, 1) >> boneOffsets[i](1, 2) >> boneOffsets[i](1, 3) >>
				boneOffsets[i](2, 0) >> boneOffsets[i](2, 1) >> boneOffsets[i](2, 2) >> boneOffsets[i](2, 3) >>
				boneOffsets[i](3, 0) >> boneOffsets[i](3, 1) >> boneOffsets[i](3, 2) >> boneOffsets[i](3, 3);
		}
	}

	//뼈 계층구조 데이터로드
	void ReadBoneHierarchy(std::ifstream& fin, UINT numBones, std::vector<int>& boneIndexToParentIndex)
	{
		std::string ignore;
		boneIndexToParentIndex.resize(numBones);

		fin >> ignore; // BoneHierarchy header text
		for (UINT i = 0; i < numBones; ++i)
		{
			fin >> ignore >> boneIndexToParentIndex[i];
		}
	}

	//뼈의 키프레임 애니메이션들을 담음
	void ReadAnimationClips(std::ifstream& fin, UINT numBones, UINT numAnimationClips,
		vector<vector<Keyframe>>& boneAnimations)
	{
		std::string ignore;
		fin >> ignore; // AnimationClips header text
		for (UINT clipIndex = 0; clipIndex < numAnimationClips; ++clipIndex)
		{
			std::string clipName;
			fin >> ignore >> clipName;
			fin >> ignore; // {

			vector<vector<Keyframe>> animations;
			animations.resize(numBones);

			for (UINT boneIndex = 0; boneIndex < numBones; ++boneIndex)
			{
				ReadBoneKeyframes(fin, numBones, animations[boneIndex]);
			}
			fin >> ignore; // }

			boneAnimations = animations;
		}
	}

	void ReadBoneKeyframes(std::ifstream& fin, UINT numBones, vector<Keyframe>& boneAnimation)
	{
		std::string ignore;
		UINT numKeyframes = 0;
		fin >> ignore >> ignore >> numKeyframes;
		fin >> ignore; // {

		boneAnimation.resize(numKeyframes);
		for (UINT i = 0; i < numKeyframes; ++i)
		{
			float t = 0.0f;
			XMFLOAT3 p(0.0f, 0.0f, 0.0f);
			XMFLOAT3 s(1.0f, 1.0f, 1.0f);
			XMFLOAT4 q(0.0f, 0.0f, 0.0f, 1.0f);
			fin >> ignore >> t;
			fin >> ignore >> p.x >> p.y >> p.z;
			fin >> ignore >> s.x >> s.y >> s.z;
			fin >> ignore >> q.x >> q.y >> q.z >> q.w;

			boneAnimation[i].TimePos = t;
			boneAnimation[i].Translation = p;
			boneAnimation[i].Scale = s;
			boneAnimation[i].RotationQuat = q;
		}

		fin >> ignore; // }
	}
};

struct AnimationClip
{
	template<typename T>
	static T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}

	float GetClipEndTime()const {
		// Find largest end time over all bones in this clip.
		float t = 0.0f;
		for (UINT i = 0; i < animationVec.size(); ++i)
		{
			t = Max(t, animationVec[i].back().TimePos);
		}

		return t;
	}

	void Interpolate(vector<Keyframe> keyframeVec, float t, XMFLOAT4X4& M)const {
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

	void GetFinalTransforms(const std::string& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms)const {
		UINT numBones = mBoneOffsets.size();

		std::vector<XMFLOAT4X4> toParentTransforms(numBones);

		for (UINT i = 0; i < animationVec.size(); ++i)
		{
			Interpolate(animationVec[i], timePos, toParentTransforms[i]);
		}

		std::vector<XMFLOAT4X4> toRootTransforms(numBones);

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

	vector<int> mBoneHierarchy;
	vector<DirectX::XMFLOAT4X4> mBoneOffsets;
	vector<vector<Keyframe>> animationVec;
};

struct SkinnedModelInstance
{
	AnimationClip* SkinnedInfo = nullptr;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float dt)
	{
		TimePos += dt;

		// Loop animation
		if (TimePos > SkinnedInfo->GetClipEndTime())
			TimePos = 0.0f;

		// Compute the final transforms for this time position.
		SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
	}
};

class FbxLoader
{
public:
	struct FbxKeyFrameInfo
	{
		FbxAMatrix  matTransform;
		double		time;
	};

	struct FbxAnimClipInfo
	{
		wstring			name;
		FbxTime			startTime;
		FbxTime			endTime;
		FbxTime::EMode	mode;
		vector<vector<FbxKeyFrameInfo>>	keyFrames;
	};

	struct BoneWeight
	{
		using Pair = pair<int, double>;
		vector<Pair> boneWeights;

		void AddWeights(UINT index, double weight)
		{
			if (weight <= 0.f)
				return;

			auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
				[=](const Pair& p) { return p.second < weight; });

			if (findIt != boneWeights.end())
				boneWeights.insert(findIt, Pair(index, weight));
			else
				boneWeights.push_back(Pair(index, weight));

			// 가중치는 최대 4개
			if (boneWeights.size() > 4)
				boneWeights.pop_back();
		}

		void Normalize()
		{
			double sum = 0.f;
			std::for_each(boneWeights.begin(), boneWeights.end(), [&](Pair& p) { sum += p.second; });
			std::for_each(boneWeights.begin(), boneWeights.end(), [=](Pair& p) { p.second = p.second / sum; });
		}
	};

	struct FbxBoneInfo
	{
		wstring					boneName;
		int					  parentIndex;
		FbxAMatrix				matOffset;
	};

	//fbx
	FbxManager* mFbxManager = nullptr;
	//FbxScene* mFbxScene = nullptr;
	FbxScene* lScene;
	FbxNode* mFbxNode = nullptr;
	FbxMesh* mFbxMesh = nullptr;
	FbxIOSettings* mFbxIOSet = nullptr;
	FbxVector4* mPos = nullptr;
	vector<shared_ptr<FbxBoneInfo>>		_bones;
	vector<BoneWeight>					_boneWeights;
	FbxArray<FbxString*>				_animNames;
	vector<shared_ptr<FbxAnimClipInfo>>	_animClips;
	std::unique_ptr<SkinnedModelInstance> mSkinnedModelInst;

	wstring s2ws(const string& s)
	{
		int len;
		int slength = static_cast<int>(s.length()) + 1;
		len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		wstring ret(buf);
		delete[] buf;
		return ret;
	}

	void BuildBoxGeometry(vector<M3DLoader::SkinnedVertex>& vertexVec, vector<UINT>& indexVec, AnimationClip& clip)
	{
		FbxManager* lSdkManager = FbxManager::Create();
		FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);
		FbxString lPath = FbxGetApplicationDirectory();
		lSdkManager->LoadPluginsDirectory(lPath.Buffer());
		//FbxScene* lScene = FbxScene::Create(lSdkManager, myObj.mSceneName);
		lScene = FbxScene::Create(lSdkManager, "My Scene");
		bool lResult;

		lResult = LoadScene(lSdkManager, lScene, "../Resources/Model/Dragon.fbx");
		FbxNode* lNode = lScene->GetRootNode();

		FbxGeometryConverter geoConverter(lSdkManager);
		geoConverter.Triangulate(lScene, true);

		LoadBones(lScene->GetRootNode(), 0, -1, clip);
		LoadAnimationInfo();

		clip.animationVec.resize(clip.mBoneHierarchy.size());

		DisplayContent(lNode, vertexVec, indexVec, clip);
	}

	void LoadBones(FbxNode* node, int idx, int parentIdx, AnimationClip& clip)
	{
		FbxNodeAttribute* attribute = node->GetNodeAttribute();

		if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			shared_ptr<FbxBoneInfo> bone = make_shared<FbxBoneInfo>();
			bone->boneName = s2ws(node->GetName());
			bone->parentIndex = parentIdx;
			//
			//
			//
			//
			//
			clip.mBoneHierarchy.push_back(parentIdx);
			printf_s("%d\n", clip.mBoneHierarchy.back());
			_bones.push_back(bone);
		}

		const int childCount = node->GetChildCount();
		for (int i = 0; i < childCount; i++)
			LoadBones(node->GetChild(i), static_cast<int>(_bones.size()), idx, clip);
	}

	bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor, lSDKMinor, lSDKRevision;
		//int lFileFormat = -1;
		int i, lAnimStackCount;
		bool lStatus;
		char lPassword[1024];

		// Get the file version number generate by the FBX SDK.
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		FbxImporter* lImporter = FbxImporter::Create(pManager, "");

		// Initialize the importer by providing client_main filename.
		const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if (!lImportStatus)
		{
			FbxString error = lImporter->GetStatus().GetErrorString();
			FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
			FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

			if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
				FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
			}

			return false;
		}

		FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

		if (lImporter->IsFBX())
		{
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

			// From this point, it is possible to access animation stack information without
			// the expense of loading the entire file.

			FBXSDK_printf("Animation Stack Information\n");

			lAnimStackCount = lImporter->GetAnimStackCount();

			FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
			FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
			FBXSDK_printf("\n");

			for (i = 0; i < lAnimStackCount; i++)
			{
				FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

				FBXSDK_printf("    Animation Stack %d\n", i);
				FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
				FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

				// Change the value of the import name if the animation stack should be imported 
				// under client_main different name.
				FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

				// Set the value of the import state to false if the animation stack should be not
				// be imported. 
				FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
				FBXSDK_printf("\n");
			}

			// Set the import states. By default, the import states are always set to 
			// true. The code below shows how to change these states.
			IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
			IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
			IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
			IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
			IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
			IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
			IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		// Import the scene.
		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("Please enter password: ");

			lPassword[0] = '\0';

			FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
				scanf("%s", lPassword);
			FBXSDK_CRT_SECURE_NO_WARNING_END

				FbxString lString(lPassword);

			IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
			IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

			lStatus = lImporter->Import(pScene);

			if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
			{
				FBXSDK_printf("\nPassword is wrong, import aborted.\n");
			}
		}

		// Destroy the importer.
		lImporter->Destroy();

		return lStatus;
	}

	void DisplayContent(FbxNode* pNode, vector<M3DLoader::SkinnedVertex>& vertexVec, vector<UINT>& indexVec, AnimationClip& clip)
	{
		FbxNodeAttribute::EType lAttributeType;
		int j;

		if (pNode->GetNodeAttribute() == NULL)
		{
			FBXSDK_printf("NULL Node Attribute\n\n");
		}
		else
		{
			lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

			switch (lAttributeType)
			{
			default:
				break;

			case FbxNodeAttribute::eMesh:
				FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();
				DisplayPolygons(lMesh, vertexVec, indexVec, clip);
				break;
			}
		}

		for (j = 0; j < pNode->GetChildCount(); j++)
		{
			DisplayContent(pNode->GetChild(j), vertexVec, indexVec, clip);
		}
	}

	void DisplayPolygons(FbxMesh* pMesh, vector<M3DLoader::SkinnedVertex>& vertexVec, vector<UINT>& indexVec, AnimationClip& clip)
	{
		int count = pMesh->GetControlPointsCount();
		vertexVec.resize(count);

		_boneWeights.resize(count);

		FbxVector4* controlPoints = pMesh->GetControlPoints();

		for (int i = 0; i < count; i++)
		{
			vertexVec[i].Pos.x = static_cast<float>(controlPoints[i].mData[0]);
			vertexVec[i].Pos.y = static_cast<float>(controlPoints[i].mData[2]);
			vertexVec[i].Pos.z = static_cast<float>(controlPoints[i].mData[1]);
		}

		int triCount = pMesh->GetPolygonCount();

		indexVec.resize(triCount * 3);

		FbxGeometryElementMaterial* geometryElementMaterial = pMesh->GetElementMaterial();

		const int polygonSize = pMesh->GetPolygonSize(0);

		UINT arrIdx[3];
		UINT vertexCounter = 0;

		for (int i = 0; i < triCount; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int controlPointIndex = pMesh->GetPolygonVertex(i, j);
				arrIdx[j] = controlPointIndex;

				vertexVec[controlPointIndex].Normal.x = 0.5f;
				vertexVec[controlPointIndex].Normal.y = 0.5f;
				vertexVec[controlPointIndex].Normal.z = 0.5f;

				FbxVector2 uv = pMesh->GetElementUV()->GetDirectArray().GetAt(pMesh->GetTextureUVIndex(i, j));
				vertexVec[controlPointIndex].TexC.x = static_cast<float>(uv.mData[0]);
				vertexVec[controlPointIndex].TexC.y = 1.f - static_cast<float>(uv.mData[1]);

				vertexCounter++;
			}

			indexVec.push_back(arrIdx[0]);
			indexVec.push_back(arrIdx[2]);
			indexVec.push_back(arrIdx[1]);
		}

		LoadAnimationData(pMesh, vertexVec, indexVec, clip);
	}

	void LoadAnimationData(FbxMesh* mesh, vector<M3DLoader::SkinnedVertex>& vertexVec, vector<UINT>& indexVec, AnimationClip& clip)
	{
		const int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);

		for (int i = 0; i < skinCount; i++)
		{
			FbxSkin* fbxSkin = static_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));

			if (fbxSkin)
			{
				FbxSkin::EType type = fbxSkin->GetSkinningType();
				if (FbxSkin::eRigid == type || FbxSkin::eLinear)
				{
					const int clusterCount = fbxSkin->GetClusterCount();
					for (int j = 0; j < clusterCount; j++)
					{
						FbxCluster* cluster = fbxSkin->GetCluster(j);
						if (cluster->GetLink() == nullptr)
							continue;

						int boneIdx = FindBoneIndex(cluster->GetLink()->GetName());
						assert(boneIdx >= 0);

						FbxAMatrix matNodeTransform = GetTransform(mesh->GetNode());
						LoadBoneWeight(cluster, boneIdx);
						LoadOffsetMatrix(cluster, matNodeTransform, boneIdx, clip);

						const int animCount = _animNames.Size();
						for (int k = 0; k < animCount; k++) {
							LoadKeyframe(k, mesh->GetNode(), cluster, matNodeTransform, boneIdx, clip);
						}
					}
				}
			}
		}

		FillBoneWeight(mesh, vertexVec);
	}

	int FindBoneIndex(string name)
	{
		wstring boneName = wstring(name.begin(), name.end());

		for (UINT i = 0; i < _bones.size(); ++i)
		{
			if (_bones[i]->boneName == boneName)
				return i;
		}

		return -1;
	}

	FbxAMatrix GetTransform(FbxNode* node)
	{
		const FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 scaling = node->GetGeometricScaling(FbxNode::eSourcePivot);
		return FbxAMatrix(translation, rotation, scaling);
	}

	//뼈가중치 데이터 로드
	void LoadBoneWeight(FbxCluster* cluster, int boneIdx)
	{
		const int indicesCount = cluster->GetControlPointIndicesCount();
		for (int i = 0; i < indicesCount; i++)
		{
			double weight = cluster->GetControlPointWeights()[i];
			int vtxIdx = cluster->GetControlPointIndices()[i];
			_boneWeights[vtxIdx].AddWeights(boneIdx, weight);
		}
	}

	//BoneOffset데이터로드
	void LoadOffsetMatrix(FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int boneIdx, AnimationClip& clip)
	{
		FbxAMatrix matClusterTrans;
		FbxAMatrix matClusterLinkTrans;
		// The transformation of the mesh at binding time 
		cluster->GetTransformMatrix(matClusterTrans);
		// The transformation of the cluster(joint) at binding time from joint space to world space 
		cluster->GetTransformLinkMatrix(matClusterLinkTrans);

		FbxVector4 V0 = { 1, 0, 0, 0 };
		FbxVector4 V1 = { 0, 0, 1, 0 };
		FbxVector4 V2 = { 0, 1, 0, 0 };
		FbxVector4 V3 = { 0, 0, 0, 1 };

		FbxAMatrix matReflect;
		matReflect[0] = V0;
		matReflect[1] = V1;
		matReflect[2] = V2;
		matReflect[3] = V3;

		FbxAMatrix matOffset;
		matOffset = matClusterLinkTrans.Inverse() * matClusterTrans;
		matOffset = matReflect * matOffset * matReflect;

		//
		//
		//
		_bones[boneIdx]->matOffset = matOffset.Transpose();
		//
		clip.mBoneOffsets.push_back(GetMatrix(_bones[boneIdx]->matOffset));
	}

	XMFLOAT4X4 GetMatrix(FbxAMatrix& matrix)
	{
		XMFLOAT4X4 mat;

		for (int y = 0; y < 4; ++y)
			for (int x = 0; x < 4; ++x)
				mat.m[y][x] = static_cast<float>(matrix.Get(y, x));

		return mat;
	}

	//animationclip데이터로드
	void LoadKeyframe(int animIndex, FbxNode* node, FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int boneIdx, AnimationClip& clip)
	{
		FbxVector4	v1 = { 1, 0, 0, 0 };
		FbxVector4	v2 = { 0, 0, 1, 0 };
		FbxVector4	v3 = { 0, 1, 0, 0 };
		FbxVector4	v4 = { 0, 0, 0, 1 };
		FbxAMatrix	matReflect;
		matReflect.mData[0] = v1;
		matReflect.mData[1] = v2;
		matReflect.mData[2] = v3;
		matReflect.mData[3] = v4;

		FbxTime::EMode timeMode = lScene->GetGlobalSettings().GetTimeMode();

		// 애니메이션 골라줌
		FbxAnimStack* animStack = lScene->FindMember<FbxAnimStack>(_animNames[animIndex]->Buffer());
		lScene->SetCurrentAnimationStack(OUT animStack);

		FbxLongLong startFrame = _animClips[animIndex]->startTime.GetFrameCount(timeMode);
		FbxLongLong endFrame = _animClips[animIndex]->endTime.GetFrameCount(timeMode);

		for (FbxLongLong frame = startFrame; frame < endFrame; frame++)
		{
			FbxKeyFrameInfo keyFrameInfo = {};
			FbxTime fbxTime = 0;

			fbxTime.SetFrame(frame, timeMode);

			FbxAMatrix matFromNode = node->EvaluateGlobalTransform(fbxTime);
			FbxAMatrix matTransform = matFromNode.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(fbxTime);
			matTransform = matReflect * matTransform * matReflect;

			keyFrameInfo.time = fbxTime.GetSecondDouble();
			keyFrameInfo.matTransform = matTransform;

			if (animIndex == 0) {
				Keyframe keyFrame;

				//kfInfo.frame = static_cast<int>(size);
				keyFrame.TimePos = (float)fbxTime.GetSecondDouble();
				keyFrame.Scale.x = static_cast<float>(matTransform.GetS().mData[0]);
				keyFrame.Scale.y = static_cast<float>(matTransform.GetS().mData[1]);
				keyFrame.Scale.z = static_cast<float>(matTransform.GetS().mData[2]);
				keyFrame.RotationQuat.x = static_cast<float>(matTransform.GetQ().mData[0]);
				keyFrame.RotationQuat.y = static_cast<float>(matTransform.GetQ().mData[1]);
				keyFrame.RotationQuat.z = static_cast<float>(matTransform.GetQ().mData[2]);
				keyFrame.RotationQuat.w = static_cast<float>(matTransform.GetQ().mData[3]);
				keyFrame.Translation.x = static_cast<float>(matTransform.GetT().mData[0]);
				keyFrame.Translation.y = static_cast<float>(matTransform.GetT().mData[1]);
				keyFrame.Translation.z = static_cast<float>(matTransform.GetT().mData[2]);

				clip.animationVec[boneIdx].push_back(keyFrame);
			}

			_animClips[animIndex]->keyFrames[boneIdx].push_back(keyFrameInfo);
		}
	}

	void LoadAnimationInfo()
	{
		lScene->FillAnimStackNameArray(OUT _animNames);

		const int animCount = _animNames.GetCount();
		for (int i = 0; i < animCount; i++)
		{
			FbxAnimStack* animStack = lScene->FindMember<FbxAnimStack>(_animNames[i]->Buffer());
			if (animStack == nullptr)
				continue;

			shared_ptr<FbxAnimClipInfo> animClip = make_shared<FbxAnimClipInfo>();
			animClip->name = s2ws(animStack->GetName());
			animClip->keyFrames.resize(_bones.size()); // 키프레임은 본의 개수만큼

			FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStack->GetName());
			animClip->startTime = takeInfo->mLocalTimeSpan.GetStart();
			animClip->endTime = takeInfo->mLocalTimeSpan.GetStop();
			animClip->mode = lScene->GetGlobalSettings().GetTimeMode();

			_animClips.push_back(animClip);
		}
	}

	void FillBoneWeight(FbxMesh* mesh, vector<M3DLoader::SkinnedVertex>& vertexVec)
	{
		const int size = static_cast<int>(_boneWeights.size());
		for (int v = 0; v < size; v++)
		{
			BoneWeight& boneWeight = _boneWeights[v];
			boneWeight.Normalize();

			float animBoneIndex[4] = {};
			float animBoneWeight[4] = {};

			const int weightCount = static_cast<int>(boneWeight.boneWeights.size());
			for (int w = 0; w < weightCount; w++)
			{
				animBoneIndex[w] = static_cast<float>(boneWeight.boneWeights[w].first);
				animBoneWeight[w] = static_cast<float>(boneWeight.boneWeights[w].second);
			}

			vertexVec[v].BoneIndices[0] = animBoneIndex[0];
			vertexVec[v].BoneIndices[1] = animBoneIndex[1];
			vertexVec[v].BoneIndices[2] = animBoneIndex[2];
			vertexVec[v].BoneIndices[3] = animBoneIndex[3];

			vertexVec[v].BoneWeights.x = animBoneWeight[0];
			vertexVec[v].BoneWeights.y = animBoneWeight[1];
			vertexVec[v].BoneWeights.z = animBoneWeight[2];
		}
	}
};