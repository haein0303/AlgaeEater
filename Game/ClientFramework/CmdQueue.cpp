#include "Device.h"
#include "CmdQueue.h"

void CmdQueue::CreateCmdListAndCmdQueue(shared_ptr<Device> devicePtr)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	devicePtr->_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue)); //Ŀ�ǵ� ť ����

	devicePtr->_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc)); //Ŀ�ǵ� �Ҵ��� ����
	devicePtr->_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList)); //Ŀ�ǵ� ����Ʈ ����
	_cmdList->Close();

	//��Ƽ������� Ŀ��� ����Ʈ �и�
	for (int i = 0; i < 2; ++i) {
		devicePtr->_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_arr_cmdAlloc[i])); //Ŀ�ǵ� �Ҵ��� ����
		devicePtr->_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _arr_cmdAlloc[i].Get(), nullptr, IID_PPV_ARGS(&_arr_cmdList[i])); //Ŀ�ǵ� ����Ʈ ����
		_arr_cmdList[i]->Close();
	}

	devicePtr->_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc)); //���ҽ��� Ŀ�ǵ� �Ҵ��� ����
	devicePtr->_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList)); //���ҽ��� Ŀ�ǵ� ����Ʈ ����

	devicePtr->_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CmdQueue::WaitSync()
{
	_fenceValue++;
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void CmdQueue::FlushResourceCommandQueue()
{
	_resCmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitSync();

	_resCmdAlloc->Reset();
	_resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}

