#pragma once
#include "Util.h"
class DescHeap
{
public:
	ComPtr<ID3D12DescriptorHeap> _descHeap;
	UINT _handleSize = 0;
	UINT _groupSize = 0;
	UINT _groupCount = 0;
	UINT _currentGroupIndex = 0;

	void CreateDescTable(UINT count, shared_ptr<Device> devicePtr);

	void CopyDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UINT reg, shared_ptr<Device> devicePtr);

	void CommitTable(shared_ptr<CmdQueue> cmdQueuePtr);
	void CommitTable_multi(shared_ptr<CmdQueue> cmdQueuePtr,int index);
};