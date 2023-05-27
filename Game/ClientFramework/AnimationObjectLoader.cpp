#include "AnimationObjectLoader.h"

void AnimationObjectLoader::LoadAnimationObject(const string& filename, vector<SkinnedVertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets,
	vector<int>& mBoneHierarchy, vector<XMFLOAT4X4>& mBoneOffsets, vector<vector<vector<Keyframe>>>& animations)
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
	int animationNum = 0;

	vector<FrameHierarchy> tempframehier;
	//vector<vector<Animation>> tempanimations;
	vector<string> framename;
	vector<vector<vector<Keyframe>>> tmpanimations;

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
				mBoneHierarchy.push_back(index);
			}

		}

		// 각 뼈의 offset행렬 로드
		else if (str.compare("<BoneOffsets>:") == 0)
		{
			fin >> ignore;
			mBoneOffsets.resize(boneNum);
			for (int i = 0; i < boneNum; i++)
			{
				fin >> mBoneOffsets[i](0, 0) >> mBoneOffsets[i](0, 1) >> mBoneOffsets[i](0, 2) >> mBoneOffsets[i](0, 3) >>
					mBoneOffsets[i](1, 0) >> mBoneOffsets[i](1, 1) >> mBoneOffsets[i](1, 2) >> mBoneOffsets[i](1, 3) >>
					mBoneOffsets[i](2, 0) >> mBoneOffsets[i](2, 1) >> mBoneOffsets[i](2, 2) >> mBoneOffsets[i](2, 3) >>
					mBoneOffsets[i](3, 0) >> mBoneOffsets[i](3, 1) >> mBoneOffsets[i](3, 2) >> mBoneOffsets[i](3, 3);
			}
		}

		// 애니메이션 정보 로드
		else if (str.compare("<AnimationSets>:") == 0)
		{
			string clipName;
			int numFrames;
			UINT numKeyframes = 0;

			fin >> animationNum;
			animations.resize(animationNum);
			tmpanimations.resize(animationNum);
			fin >> ignore >> numFrames;
			framename.resize(numFrames);
			for (int i = 0; i < animationNum; i++) {
				tmpanimations[i].resize(numFrames);
			}
			for (int i = 0; i < numFrames; i++)
			{
				fin >> framename[i];
			}

			// Animation Sets 로드
			for (int i = 0; i < animationNum; i++) {
				fin >> ignore >> ignore >> clipName >> ignore >> ignore >> numKeyframes;

				for (UINT j = 0; j < numKeyframes; ++j)
				{
					float t = 0.0f;
					fin >> ignore >> ignore >> t;

					for (UINT boneIndex = 0; boneIndex < numFrames; ++boneIndex)
					{
						tmpanimations[i][boneIndex].resize(numKeyframes);

						XMFLOAT3 p(0.0f, 0.0f, 0.0f);
						XMFLOAT3 s(1.0f, 1.0f, 1.0f);
						XMFLOAT4 q(0.0f, 0.0f, 0.0f, 1.0f);

						fin >> p.x >> p.y >> p.z;
						fin >> s.x >> s.y >> s.z;
						fin >> q.x >> q.y >> q.z >> q.w;

						tmpanimations[i][boneIndex][j].TimePos = t;
						tmpanimations[i][boneIndex][j].Translation = p;
						tmpanimations[i][boneIndex][j].Scale = s;
						tmpanimations[i][boneIndex][j].RotationQuat = q;
					}
				}
			}

		}
		else if (fin.eof())
		{
			for (FrameHierarchy frame : tempframehier)
			{
				int i = 0;
				for (string name : framename)
				{
					if (frame.myname.compare(name) == 0)
					{
						for (int j = 0; j < animationNum; j++) {
							animations[j].push_back(tmpanimations[j][i]);
						}
					}
					++i;
				}
			}

			printf_s("end\n");
			break;
		}
	}
	fin.close();
}

void AnimationObjectLoader::LoadVertexAnimationObject(const string& filename, vector<Vertex>& vertices, vector<UINT>& indices, vector<Subset>& subsets, vector<int>& mBoneHierarchy, vector<XMFLOAT4X4>& mBoneOffsets, vector<vector<vector<Keyframe>>>& animations)
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
	int animationNum = 0;

	while (true)
	{
		fin >> str;

		// Vertices 정보 로드
		if (str.compare("<Positions>:") == 0) // 포지션 처리
		{
			fin >> vcount;
			vertices.resize(vcount);

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> vertices[i].pos.x >> vertices[i].pos.y >> vertices[i].pos.z;
			}
		}
		else if (str.compare("<TextureCoords0>:") == 0) // uv처리
		{
			fin >> ignore;

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> vertices[i].uv.x >> vertices[i].uv.y;
			}

		}
		else if (str.compare("<Normals>:") == 0) // 노말 처리
		{
			fin >> ignore;

			for (UINT i = 0; i < vcount; ++i)
				fin >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
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

		// 애니메이션 정보 로드
		else if (str.compare("<AnimationSets>:") == 0)
		{
			string clipName;
			int numFrames;
			UINT numKeyframes = 0;

			fin >> animationNum;
			animations.resize(animationNum);
			fin >> ignore >> numFrames;
			for (int i = 0; i < animationNum; ++i) {
				animations[i].resize(1);
			}
			for (int i = 0; i < numFrames; ++i)
			{
				fin >> ignore;
			}

			// Animation Sets 로드
			for (int i = 0; i < animationNum; i++) {
				fin >> ignore >> ignore >> clipName >> ignore >> ignore >> numKeyframes;

				for (UINT j = 0; j < numKeyframes; ++j)
				{
					float t = 0.0f;
					fin >> ignore >> ignore >> t;

					for (UINT boneIndex = 0; boneIndex < 1; ++boneIndex)
					{
						fin >> ignore >> ignore >> ignore >>
							ignore >> ignore >> ignore >>
							ignore >> ignore >> ignore >> ignore;

						animations[i][boneIndex].resize(numKeyframes);

						XMFLOAT3 p(0.0f, 0.0f, 0.0f);
						XMFLOAT3 s(1.0f, 1.0f, 1.0f);
						XMFLOAT4 q(0.0f, 0.0f, 0.0f, 1.0f);

						fin >> p.x >> p.y >> p.z;
						fin >> s.x >> s.y >> s.z;
						fin >> q.x >> q.y >> q.z >> q.w;

						animations[i][boneIndex][j].TimePos = t;
						animations[i][boneIndex][j].Translation = p;
						animations[i][boneIndex][j].Scale = s;
						animations[i][boneIndex][j].RotationQuat = q;
					}
				}
			}

		}
		else if (fin.eof())
		{
			printf_s("end\n");
			break;
		}
	}
	fin.close();
}
