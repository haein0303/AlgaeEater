#include "AnimationObjectLoader.h"

void AnimationObjectLoader::LoadAnimationObject(const string& filename, vector<SkinnedVertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets, AnimationClip& skinInfo)
{
	ifstream fin(filename);

	if (!fin)
	{
		exit(0);
	}

	UINT vcount = 0;
	UINT tcount = 0;
	int temp = 0;
	string ignore;
	string str = "\n";
	int boneIndices[4];
	float weights[4];
	int boneNum = 0;

	vector<framehierarchy> tempframehier;
	vector<animation> tempanimations;
	vector<vector<Keyframe>> tempanimation;

	while (true)
	{
		fin >> str;

		// SkinnedVertices 정보 로드
		if (str.compare("<BoneIndices>:") == 0) // 뼈 인덱스 처리
		{
			fin >> vcount;
			vertices.resize(vcount);

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> boneIndices[0] >> boneIndices[1] >> boneIndices[2] >> boneIndices[3];

				vertices[i].BoneIndices[0] = boneIndices[0];
				vertices[i].BoneIndices[1] = boneIndices[1];
				vertices[i].BoneIndices[2] = boneIndices[2];
				vertices[i].BoneIndices[3] = boneIndices[3];
			}
		}
		else if (str.compare("<Positions>:") == 0) // 포지션 처리
		{
			fin >> ignore;

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
			}
		}
		else if (str.compare("<TextureCoords0>:") == 0) // uv처리
		{
			fin >> ignore;

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> vertices[i].TexC.x >> vertices[i].TexC.y;
			}

		}
		else if (str.compare("<Normals>:") == 0) // 노말 처리
		{
			fin >> ignore;

			for (UINT i = 0; i < vcount; ++i)
				fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
		}
		else if (str.compare("<BoneWeights>:") == 0) // 뼈 가중치 처리
		{
			fin >> ignore;

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> weights[0] >> weights[1] >> weights[2] >> weights[3];
				vertices[i].BoneWeights.x = weights[0];
				vertices[i].BoneWeights.y = weights[1];
				vertices[i].BoneWeights.z = weights[2];

			}
		}

		// index정보 로드
		else if (str.compare("<SubMeshes>:") == 0)
		{
			UINT submeshs = 0;
			int indexCount = 0;
			fin >> submeshs;
			subsets.resize(submeshs);
			for (int i = 0; i < submeshs; i++)
			{
				fin >> ignore >> indexCount;
				subsets[i].FaceCount = indexCount / 3;
				subsets[i].FaceStart = tcount;
				tcount += subsets[i].FaceCount;
				subsets[i].Id = 0;
				subsets[i].VertexCount = vertices.size();
				subsets[i].VertexStart = 0;
			}

			indices.resize(tcount * 3);
			for (int i = 0; i < tcount * 3; i++)
			{
				fin >> indices[i];
			}
		}

		// 뼈 계층구조 로드
		else if (str.compare("<BoneNames>:") == 0)
		{
			fin >> boneNum;
			tempframehier.resize(boneNum);
			for (UINT i = 0; i < boneNum; ++i)
			{
				fin >> tempframehier[i].myname;
			}

			fin >> ignore >> ignore;
			int index = 0;
			for (UINT i = 0; i < boneNum; ++i)
			{
				fin >> index;
				skinInfo.mBoneHierarchy.push_back(index);
			}

		}

		// 각 뼈의 offset행렬 로드
		else if (str.compare("<BoneOffsets>:") == 0)
		{
			fin >> ignore;
			skinInfo.mBoneOffsets.resize(boneNum);
			for (int i = 0; i < boneNum; i++)
			{
				fin >> skinInfo.mBoneOffsets[i](0, 0) >> skinInfo.mBoneOffsets[i](0, 1) >> skinInfo.mBoneOffsets[i](0, 2) >> skinInfo.mBoneOffsets[i](0, 3) >>
					skinInfo.mBoneOffsets[i](1, 0) >> skinInfo.mBoneOffsets[i](1, 1) >> skinInfo.mBoneOffsets[i](1, 2) >> skinInfo.mBoneOffsets[i](1, 3) >>
					skinInfo.mBoneOffsets[i](2, 0) >> skinInfo.mBoneOffsets[i](2, 1) >> skinInfo.mBoneOffsets[i](2, 2) >> skinInfo.mBoneOffsets[i](2, 3) >>
					skinInfo.mBoneOffsets[i](3, 0) >> skinInfo.mBoneOffsets[i](3, 1) >> skinInfo.mBoneOffsets[i](3, 2) >> skinInfo.mBoneOffsets[i](3, 3);
			}
		}

		// 애니메이션 정보 로드
		else if (str.compare("<AnimationSets>:") == 0)
		{
			string clipName;
			int numFrames;
			UINT numKeyframes = 0;

			fin >> ignore;
			fin >> ignore >> numFrames;
			tempanimations.resize(numFrames);
			for (int i = 0; i < numFrames; i++)
			{
				fin >> tempanimations[i].name;
			}

			fin >> ignore >> ignore >> clipName >> ignore >> ignore >> numKeyframes;

			for (UINT i = 0; i < numKeyframes; ++i)
			{
				float t = 0.0f;
				fin >> ignore >> ignore >> t;

				for (UINT boneIndex = 0; boneIndex < numFrames; ++boneIndex)
				{
					tempanimations[boneIndex].key.resize(numKeyframes);

					XMFLOAT3 p(0.0f, 0.0f, 0.0f);
					XMFLOAT3 s(1.0f, 1.0f, 1.0f);
					XMFLOAT4 q(0.0f, 0.0f, 0.0f, 1.0f);

					fin >> p.x >> p.y >> p.z;
					fin >> s.x >> s.y >> s.z;
					fin >> q.x >> q.y >> q.z >> q.w;

					tempanimations[boneIndex].key[i].TimePos = t;
					tempanimations[boneIndex].key[i].Translation = p;
					tempanimations[boneIndex].key[i].Scale = s;
					tempanimations[boneIndex].key[i].RotationQuat = q;
				}
			}

		}
		else if (fin.eof())
		{
			for (framehierarchy frame : tempframehier)
			{
				for (animation bone : tempanimations)
				{
					if (frame.myname.compare(bone.name) == 0)
					{
						skinInfo.animationVec.push_back(bone.key);
					}
				}
			}

			printf_s("end\n");
			break;
		}
	}
	fin.close();
}
