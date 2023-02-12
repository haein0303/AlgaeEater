#include "ObjectLoader.h"

void ObjectLoader::LoadObject(vector<Vertex>& vertices, vector<UINT>& indices, string path) {
	ifstream fin(path);

	if (!fin)
	{
		MessageBox(0, L"Data File not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;
	string str = "\n";

	while (!fin.eof())
	{
		fin >> str;

		if (str.compare("<Positions>:") == 0) //器瘤记 贸府
		{
			fin >> vcount;
			vertices.resize(vcount);

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> vertices[i].pos.x >> vertices[i].pos.y >> vertices[i].pos.z;
			}
		}
		else if (str.compare("<TextureCoords0>:") == 0) //uv贸府
		{
			fin >> ignore;

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> vertices[i].uv.x >> vertices[i].uv.y;
				//vertices[i].uv.y = 1.0f - vertices[i].uv.y;
			}

		}
		else if (str.compare("<Normals>:") == 0) //畴富 贸府
		{
			fin >> ignore;

			for (UINT i = 0; i < vcount; ++i)
				fin >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
		}
		else if (str.compare("<SubMeshes>:") == 0) //牢郸胶 贸府
		{
			UINT submeshs = 0;
			fin >> submeshs;

			vector<UINT> submesh;
			submesh.resize(submeshs);
			for (int i = 0; i < submeshs; i++)
			{
				fin >> ignore >> submesh[i];
				tcount += submesh[i];
			}
			submeshCount.push_back(submesh);

			indices.resize(tcount);
			for (int i = 0; i < tcount; i++)
			{
				fin >> indices[i];
			}
		}
		else if (fin.eof())
		{
			break;
		}
	}

	fin.close();
}