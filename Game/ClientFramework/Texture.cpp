
#include "Util.h"
#include "Device.h"
#include "CmdQueue.h"
#include "Texture.h"

void Texture::CreateTexture(const wstring& path, shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr, int index)
{
	wstring ext = fs::path(path).extension();

	if (ext == L".dds" || ext == L".DDS") //dds
		LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, _imageArr[index]);
	else if (ext == L".tga" || ext == L".TGA") //tga
		LoadFromTGAFile(path.c_str(), nullptr, _imageArr[index]);
	else //png, jpg, jpeg, bmp
		LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, _imageArr[index]);

	HRESULT hr = ::CreateTexture(devicePtr->_device.Get(), _imageArr[index].GetMetadata(), &_texArr[index]);
	assert(SUCCEEDED(hr));

	vector<D3D12_SUBRESOURCE_DATA> subResources;

	hr = ::PrepareUpload(devicePtr->_device.Get(), _imageArr[index].GetImages(), _imageArr[index].GetImageCount(), _imageArr[index].GetMetadata(), subResources);

	assert(SUCCEEDED(hr));

	const UINT bufferSize = ::GetRequiredIntermediateSize(_texArr[index].Get(), 0, static_cast<UINT>(subResources.size()));

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> textureUploadHeap;
	hr = devicePtr->_device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(textureUploadHeap.GetAddressOf()));

	assert(SUCCEEDED(hr));

	UpdateSubresources(cmdQueuePtr->_resCmdList.Get(), _texArr[index].Get(), textureUploadHeap.Get(), 0, 0, static_cast<unsigned int>(subResources.size()), subResources.data());

	cmdQueuePtr->FlushResourceCommandQueue();
}

void Texture::CreateSRVs(shared_ptr<Device> devicePtr)
{
	//SRVHeap ����
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 9;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	devicePtr->_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

	_srvHandle = _srvHeap->GetCPUDescriptorHandleForHeapStart();

	//ù��° SRV ����
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = _imageArr[0].GetMetadata().format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	devicePtr->_device->CreateShaderResourceView(_texArr[0].Get(), &srvDesc, _srvHandle);

	//�ι�° SRV ����
	_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	srvDesc.Format = _imageArr[1].GetMetadata().format;
	devicePtr->_device->CreateShaderResourceView(_texArr[1].Get(), &srvDesc, _srvHandle);

	//����° SRV ����
	_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	srvDesc.Format = _imageArr[2].GetMetadata().format;
	devicePtr->_device->CreateShaderResourceView(_texArr[2].Get(), &srvDesc, _srvHandle);

	//�׹�° SRV ����
	_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	srvDesc.Format = _imageArr[3].GetMetadata().format;
	devicePtr->_device->CreateShaderResourceView(_texArr[3].Get(), &srvDesc, _srvHandle);

	//�ټ���° SRV ����
	_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	srvDesc.Format = _imageArr[4].GetMetadata().format;
	devicePtr->_device->CreateShaderResourceView(_texArr[4].Get(), &srvDesc, _srvHandle);

	//������° SRV ����
	_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	srvDesc.Format = _imageArr[5].GetMetadata().format;
	devicePtr->_device->CreateShaderResourceView(_texArr[5].Get(), &srvDesc, _srvHandle);

	//�ϰ���° SRV ����
	_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	srvDesc.Format = _imageArr[6].GetMetadata().format;
	devicePtr->_device->CreateShaderResourceView(_texArr[6].Get(), &srvDesc, _srvHandle);

	//������° SRV ����
	_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	srvDesc.Format = _imageArr[7].GetMetadata().format;
	devicePtr->_device->CreateShaderResourceView(_texArr[7].Get(), &srvDesc, _srvHandle);
}

//�ι�°���̵�
void Texture::_CreateTexture(const wstring& path, shared_ptr<Device> devicePtr, shared_ptr<CmdQueue> cmdQueuePtr)
{
	wstring ext = fs::path(path).extension();

	//���� ���� ������
	const int size = _v_image.size();
	_v_image.emplace_back();
	_v_tex.emplace_back();



	if (ext == L".dds" || ext == L".DDS") //dds
		LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, _v_image[size]);
	else if (ext == L".tga" || ext == L".TGA") //tga
		LoadFromTGAFile(path.c_str(), nullptr, _v_image[size]);
	else //png, jpg, jpeg, bmp
		LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, _v_image[size]);

	HRESULT hr = ::CreateTexture(devicePtr->_device.Get(), _v_image[size].GetMetadata(), &_v_tex[size]);
	assert(SUCCEEDED(hr));

	vector<D3D12_SUBRESOURCE_DATA> subResources;

	hr = ::PrepareUpload(devicePtr->_device.Get(), _v_image[size].GetImages(), _v_image[size].GetImageCount(), _v_image[size].GetMetadata(), subResources);

	assert(SUCCEEDED(hr));

	const UINT bufferSize = ::GetRequiredIntermediateSize(_v_tex[size].Get(), 0, static_cast<UINT>(subResources.size()));

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> textureUploadHeap;
	hr = devicePtr->_device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(textureUploadHeap.GetAddressOf()));

	assert(SUCCEEDED(hr));

	UpdateSubresources(cmdQueuePtr->_resCmdList.Get(), _v_tex[size].Get(), textureUploadHeap.Get(), 0, 0, static_cast<unsigned int>(subResources.size()), subResources.data());

	cmdQueuePtr->FlushResourceCommandQueue();
}

void Texture::_CreateSRVs(shared_ptr<Device> devicePtr)
{
	//SRVHeap ����
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = _v_image.size();
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	devicePtr->_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

	_srvHandle = _srvHeap->GetCPUDescriptorHandleForHeapStart();

	//ù��° SRV ����
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//srvDesc.Format = _image.GetMetadata().format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;


	int counter = 0;
	for (int i = 0; i < _v_tex.size(); ++i) {
		srvDesc.Format = _v_image[i].GetMetadata().format;
		devicePtr->_device->CreateShaderResourceView(_v_tex[i].Get(), &srvDesc, _srvHandle);
		if (_v_tex.size() != (i - 1)) {
			_srvHandle.Offset(1, devicePtr->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		}
		
	}
	
}
