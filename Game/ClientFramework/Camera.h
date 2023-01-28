#pragma once
#include "Util.h"

class Camera
{
public:
	XMFLOAT4X4 mProj = Identity4x4(); //���� ��ȯ ���
	XMVECTOR pos;

	//���� ��ȯ
	void TransformProjection(WindowInfo windowInfo);
};