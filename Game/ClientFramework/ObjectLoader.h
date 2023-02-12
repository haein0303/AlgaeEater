#pragma once
#include "Util.h"
#include <fstream>

class ObjectLoader
{
public:
	vector<vector<UINT>> submeshCount;

	void LoadObject(vector<Vertex>& vertices, vector<UINT>& indices, string path);
};