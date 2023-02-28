#pragma once
#include "Util.h"
#include "Device.h"
#include "CmdQueue.h"
#include "SwapChain.h"
#include "RTV.h"
#include "VertexBuffer.h"
#include "RootSignature.h"
#include "DSV.h"
#include "PSO.h"
#include "ConstantBuffer.h"
#include "DescHeap.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Camera.h"
#include "Timer.h"
#include "Animation.h"

class DxEngine {
public:
	//DX엔진 초기화
	void Init(WindowInfo windowInfo);
	
	//매 프레임마다 그리기
	void Draw();

	//요소별 객체 포인터
	shared_ptr<Device> devicePtr = make_shared<Device>();
	shared_ptr<CmdQueue> cmdQueuePtr = make_shared<CmdQueue>();
	shared_ptr<SwapChain> swapChainPtr = make_shared<SwapChain>();
	shared_ptr<RTV> rtvPtr = make_shared<RTV>();
	shared_ptr<VertexBuffer> vertexBufferPtr = make_shared<VertexBuffer>();
	shared_ptr<RootSignature> rootSignaturePtr = make_shared<RootSignature>();
	shared_ptr<PSO> psoPtr = make_shared<PSO>();
	shared_ptr<ConstantBuffer> constantBufferPtr = make_shared<ConstantBuffer>();
	shared_ptr<DescHeap> descHeapPtr = make_shared<DescHeap>();
	shared_ptr<IndexBuffer> indexBufferPtr = make_shared<IndexBuffer>();
	shared_ptr<Texture> texturePtr = make_shared<Texture>();
	shared_ptr<DSV> dsvPtr = make_shared<DSV>();
	shared_ptr<Camera> cameraPtr = make_shared<Camera>();
	shared_ptr<Timer> timerPtr = make_shared<Timer>();
	shared_ptr<SkinningAnimation> animationPtr = make_shared<SkinningAnimation>();

	ParticleData particle[PARTICLE_NUM];

	string skinnedModelFilename = "..\\SkinningAnimation\\resources\\Animationtest.txt";
	AnimationClip skinnedInfo;
	vector<SkinningAnimation::Subset> skinnedSubsets;
	vector<SkinningAnimation::Material> skinnedMats;
	unique_ptr<SkinnedModelInstance> skinnedModelInst;
	void LoadObj(vector<SkinnedVertex>& vertices, std::vector<UINT>& indices) {
		SkinningAnimation m3dLoader;
		m3dLoader.LoadM3d(skinnedModelFilename, vertices, indices, skinnedSubsets, skinnedMats, skinnedInfo);

		skinnedModelInst = make_unique<SkinnedModelInstance>();
		skinnedModelInst->SkinnedInfo = &skinnedInfo;
		skinnedModelInst->FinalTransforms.resize(skinnedInfo.mBoneHierarchy.size());
		skinnedModelInst->ClipName = "Action";
		skinnedModelInst->TimePos = 0.0f;
	}

private:
	//화면 크기 관련
	D3D12_VIEWPORT	_viewport;
	D3D12_RECT		_scissorRect;
};