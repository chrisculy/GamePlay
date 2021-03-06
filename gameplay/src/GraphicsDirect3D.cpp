#ifdef _WINDOWS

#include "Base.h"
#include "GraphicsDirect3D.h"
#include "Game.h"

namespace gameplay
{

#define SAFE_RELEASE(x)			\
   if(x != nullptr)				\
   {							\
      x->Release();				\
      x = nullptr;				\
   }

#define D3D_CHECK_RESULT(f)		\
{								\
	HRESULT hr = (f);			\
	if (FAILED(hr))				\
	{							\
		std::cout << "Fatal: HRESULT is \"" << std::to_string(hr).c_str() << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl << std::flush; \
		assert(SUCCEEDED(hr));	\
	}							\
}

GraphicsDirect3D::GraphicsDirect3D() :
    _initialized(false),
	_prepared(false),
	_width(0),
	_height(0),
	_fullscreen(false),
	_vsync(false),
	_multisampling(0),
	_hwnd(nullptr),
	_displayMode(0),
	_device(nullptr),
	_commandQueue(nullptr),
	_swapchain(nullptr),
	_backBufferIndex(0),
	_renderTargetViewHeap(nullptr),
	_commandList(nullptr),
	_fence(nullptr)
{
}

GraphicsDirect3D::~GraphicsDirect3D()
{
	if(_swapchain)
		_swapchain->SetFullscreenState(false, nullptr);
	SAFE_RELEASE(_fence);
	SAFE_RELEASE(_commandList);
	for (uint32_t i = 0; i < GP_GRAPHICS_BACK_BUFFERS; i++)
		SAFE_RELEASE(_commandAllocators[i]);
	for (uint32_t i = 0; i < GP_GRAPHICS_BACK_BUFFERS; i++)
		SAFE_RELEASE(_renderTargets[i]);
	SAFE_RELEASE(_renderTargetViewHeap);
	SAFE_RELEASE(_swapchain);
	SAFE_RELEASE(_commandQueue);
	SAFE_RELEASE(_device);
}

void GraphicsDirect3D::initialize(unsigned long window, unsigned long connection)
{
    if (_initialized)
        return;

    _hwnd = (HWND)window;

	// Get the game config
	std::shared_ptr<Game::Config> config = Game::getInstance()->getConfig();
	_width = config->width;
	_height = config->height;
	_fullscreen = config->fullscreen;
	_vsync = config->vsync;
	_multisampling = config->multisampling;
	_validation = config->validation;

	// Graphics validation
	uint32_t dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	if (_validation)
	{
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&debugController)))
			debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	// Create the graphics interface factory
	IDXGIFactory4* factory;
	if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory4), (void**)&factory)))
		GP_ERROR("Failed to create the graphics interface factory.");

	// Create the device
	IDXGIAdapter1* hardwareAdapter;
	getHardwareAdapter(factory, &hardwareAdapter);
	if (FAILED(D3D12CreateDevice(hardwareAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&_device)))
		GP_ERROR("Failed to create a Direct3D 12 device.");

	//Create command queue
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;
	D3D_CHECK_RESULT(_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&_commandQueue));

	// Pick the first adapter and ouput
	IDXGIAdapter* adapter;
	D3D_CHECK_RESULT(factory->EnumAdapters(0, &adapter));
	IDXGIOutput* output;
	D3D_CHECK_RESULT(adapter->EnumOutputs(0, &output));

	// Get the number of display modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM
	uint32_t displayModeCount;
	if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &displayModeCount, nullptr)) ||  displayModeCount < 1)
		GP_ERROR("Failed to find compatible display mode for adapter outputs(monitor).");

	// Get the display modes
	_displayModes.resize(displayModeCount);
	if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &displayModeCount, &_displayModes[0])))
		GP_ERROR("Failed to get display mode list for adapter outputs(monitor).");
	
	// Find a display mode that matches our config
	for (size_t i = 0; i<displayModeCount; i++)
	{
		if (_displayModes[i].Height == _height && _displayModes[i].Width == _width)
		{
			_width = _displayModes[i].Width;
			_height = _displayModes[i].Height;
			_displayMode = i;
			break;
		}
	}
	SAFE_RELEASE(output);
	SAFE_RELEASE(adapter);
	
	// Create the swapchain
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.BufferCount = GP_GRAPHICS_BACK_BUFFERS;
	swapchainDesc.Width = _width;
	swapchainDesc.Height = _height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.SampleDesc.Count = 1 + _multisampling;
	IDXGISwapChain1* swapchain;
	D3D_CHECK_RESULT(factory->CreateSwapChainForHwnd(_commandQueue, _hwnd, &swapchainDesc, nullptr, nullptr, &swapchain));
	D3D_CHECK_RESULT(swapchain->QueryInterface(__uuidof(IDXGISwapChain4), (void**)&_swapchain));

	SAFE_RELEASE(factory);

	// Create a render target description heap for the back buffers
	_backBufferIndex = _swapchain->GetCurrentBackBufferIndex();
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	// One extra for the intermediate render target
	rtvHeapDesc.NumDescriptors = GP_GRAPHICS_BACK_BUFFERS;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	D3D_CHECK_RESULT(_device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&_renderTargetViewHeap));
	
	// Create a command allocators
	for (uint32_t i = 0; i < GP_GRAPHICS_BACK_BUFFERS; i++)
		D3D_CHECK_RESULT(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&_commandAllocators[i]));

	// Create the command list
	D3D_CHECK_RESULT(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocators[_backBufferIndex], nullptr, __uuidof(ID3D12CommandList), (void**)&_commandList));
	D3D_CHECK_RESULT(_commandList->Close());

	// Create the render targets
	createRenderTargets();

	// Create a fence and event for synchronization.
	D3D_CHECK_RESULT(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&_fence));
	_fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);

    _initialized = true;
	_prepared = true;
}

void GraphicsDirect3D::resize(int width, int height)
{
	if (!_prepared || (width == _width && height == _height))
		return;
	
	// Wait for the gpu to finish processing before resizing
	waitForGpu();

	_prepared = false;
	
	// Release the resources holding references to the swapchain
	for (uint32_t i = 0; i < GP_GRAPHICS_BACK_BUFFERS; i++)
	{
		SAFE_RELEASE(_renderTargets[i]);
		_fenceValues[i] = _fenceValues[_backBufferIndex];
	}

	// Resize the swap chain to the desired dimensions.
	DXGI_SWAP_CHAIN_DESC desc = {};
	_swapchain->GetDesc(&desc);
	D3D_CHECK_RESULT(_swapchain->ResizeBuffers(GP_GRAPHICS_BACK_BUFFERS, width, height, desc.BufferDesc.Format, desc.Flags));

	// Reset the frame index to the current back buffer index.
	_backBufferIndex = _swapchain->GetCurrentBackBufferIndex();

	createRenderTargets();

	_width = width;
	_height = height;

	_prepared = true;
}

void GraphicsDirect3D::render(float elapsedTime)
{
	if (!_prepared)
		return;

	buildCommands();

	// Load the command list array (only one command list for now).
	ID3D12CommandList* commandLists[] = { _commandList };

	// Execute the list of commands.
	_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	// Present the backbuffer to the screen
	if (FAILED(_swapchain->Present(_vsync ? 1 : 0, 0)))
		return;

	// Signal and increment the fence value.
	const uint64_t fenceToWaitFor = _fenceValues[_backBufferIndex];
	if (FAILED(_commandQueue->Signal(_fence, fenceToWaitFor)))
		return;
	
	_backBufferIndex = _swapchain->GetCurrentBackBufferIndex();

	// Wait until the GPU is done rendering.
	if (_fence->GetCompletedValue() < _fenceValues[_backBufferIndex])
	{
		if(FAILED(_fence->SetEventOnCompletion(_fenceValues[_backBufferIndex], _fenceEvent)))
			return;
		WaitForSingleObject(_fenceEvent, INFINITE);
	}
	_fenceValues[_backBufferIndex] = fenceToWaitFor + 1;
}

bool GraphicsDirect3D::isInitialized()
{
    return _initialized;
}

bool GraphicsDirect3D::isPrepared()
{
	return _prepared;
}

void GraphicsDirect3D::getHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
	IDXGIAdapter1* adapter;
	*ppAdapter = nullptr;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;
		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			break;
	}
}

void GraphicsDirect3D::createRenderTargets()
{
	uint32_t renderTargetDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = _renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
	for (uint32_t i = 0; i < GP_GRAPHICS_BACK_BUFFERS; i++)
	{
		D3D_CHECK_RESULT(_swapchain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&_renderTargets[i]));
		_device->CreateRenderTargetView(_renderTargets[i], NULL, renderTargetViewHandle);
		renderTargetViewHandle.ptr += renderTargetDescriptorSize;
	}
	_backBufferIndex = _swapchain->GetCurrentBackBufferIndex();
}


void GraphicsDirect3D::waitForGpu()
{
	D3D_CHECK_RESULT(_commandQueue->Signal(_fence, _fenceValues[_backBufferIndex]));
	D3D_CHECK_RESULT(_fence->SetEventOnCompletion(_fenceValues[_backBufferIndex], _fenceEvent));
	WaitForSingleObjectEx(_fenceEvent, INFINITE, FALSE);
	_fenceValues[_backBufferIndex]++;
}

void GraphicsDirect3D::buildCommands()
{
	// Reset (re-use) the memory associated command allocator.
	D3D_CHECK_RESULT(_commandAllocators[_backBufferIndex]->Reset());

	// Reset the command list, use empty pipeline state for now since there are no shaders and we are just clearing the screen.
	if (FAILED(_commandList->Reset(_commandAllocators[_backBufferIndex], nullptr)))
		return;

	// Record commands in the command list starting by setting the resource barrier.
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = _renderTargets[_backBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	_commandList->ResourceBarrier(1, &barrier);

	// Get the render target view handle for the current back buffer.
	uint32_t renderTargetDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	
	_backBufferIndex = _swapchain->GetCurrentBackBufferIndex();
	if (_backBufferIndex == 0)
		_renderTargetViewHandle = _renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
	else
		_renderTargetViewHandle.ptr += renderTargetDescriptorSize;

	// Set the back buffer as the render target.
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	_commandList->OMSetRenderTargets(1, &_renderTargetViewHandle, FALSE, NULL);
	_commandList->ClearRenderTargetView(_renderTargetViewHandle, clearColor, 0, NULL);

	// Indicate that the back buffer will now be used to present.
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	_commandList->ResourceBarrier(1, &barrier);

	D3D_CHECK_RESULT(_commandList->Close());

	waitForGpu();
}

int GraphicsDirect3D::getWidth()
{
    return _width;
}

int GraphicsDirect3D::getHeight()
{
    return _height;

}

}

#endif
