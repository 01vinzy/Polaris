#include "renderer.h"

#include <windows.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

WNDPROC lpPrevWndFunc;

bool bShowMenu;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

__declspec(dllexport) LRESULT CALLBACK WndProcHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	// Handle application-specific workload.

	if (Msg == WM_KEYUP && (wParam == VK_INSERT || (bShowMenu && wParam == VK_ESCAPE))) {
		bShowMenu = !bShowMenu;

		ImGui::GetIO().MouseDrawCursor = bShowMenu;
	}

	if (bShowMenu)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

		return TRUE;
	}

	return CallWindowProc(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

ImGuiWindow& BeginScene()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

	ImGui::Begin(("##scene"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();

	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}

VOID EndScene(ImGuiWindow& window) {
	window.DrawList->PushClipRectFullScreen();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::Render();
}

HRESULT(*Present)(IDXGISwapChain* pInstance, UINT SyncInterval, UINT Flags) = nullptr;

__declspec(dllexport) HRESULT PresentHook(IDXGISwapChain* pInstance, UINT SyncInterval, UINT Flags)
{
	static float fWidth = 0;
	static float fHeight = 0;

	static HWND hWnd = 0;

	// Jesus fucking christ.
	if (!gpRenderer->pCurrentDevice)
	{
		pInstance->GetDevice(__uuidof(gpRenderer->pCurrentDevice), reinterpret_cast<PVOID*>(&gpRenderer->pCurrentDevice));
		gpRenderer->pCurrentDevice->GetImmediateContext(&gpRenderer->pCurrentContext);

		ID3D11Texture2D* pTarget = nullptr;
		pInstance->GetBuffer(0, __uuidof(pTarget), reinterpret_cast<PVOID*>(&pTarget));

		gpRenderer->pCurrentDevice->CreateRenderTargetView(pTarget, nullptr, &gpRenderer->pCurrentView);

		pTarget->Release();

		ID3D11Texture2D* pBuffer = nullptr;
		pInstance->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<PVOID*>(&pBuffer));

		D3D11_TEXTURE2D_DESC desc = { };
		pBuffer->GetDesc(&desc);

		hWnd = FindWindow(L"UnrealWindow", L"Fortnite  ");

		if (!fWidth)
			lpPrevWndFunc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));

		fWidth = static_cast<float>(desc.Width);
		fHeight = static_cast<float>(desc.Height);

		pBuffer->Release();

		// Handle application-specific workload.

		ImGui_ImplDX11_Init(gpRenderer->pCurrentDevice, gpRenderer->pCurrentContext);
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	gpRenderer->pCurrentContext->OMSetRenderTargets(1, &gpRenderer->pCurrentView, nullptr);

	// Handle application-specific workload.

	auto& window = BeginScene();

	if (bShowMenu)
	{
		ImGui::Begin("My First Tool", reinterpret_cast<bool*>(true), ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
				if (ImGui::MenuItem("Close", "Ctrl+W")) { }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Plot some values
		const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
		ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
		ImGui::BeginChild("Scrolling");
		for (int n = 0; n < 50; n++)
			ImGui::Text("%04d: Some text", n);
		ImGui::EndChild();
		ImGui::End();
	}

	EndScene(window);

	return Present(pInstance, SyncInterval, Flags);
}

HRESULT(*ResizeBuffers)(IDXGISwapChain* pInstance, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = nullptr;

__declspec(dllexport) HRESULT ResizeBuffersHook(IDXGISwapChain* pInstance, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	// Handle application-specific workload.

	ImGui_ImplDX11_Shutdown();

	gpRenderer->pCurrentView->Release();
	gpRenderer->pCurrentContext->Release();
	gpRenderer->pCurrentDevice->Release();

	gpRenderer->pCurrentDevice = nullptr;

	return ResizeBuffers(pInstance, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

namespace polaris
{
	Renderer::Renderer()
	{
		Console::Log("Initializing Renderer");

		if (gpRenderer)
		{
			MessageBox(0, L"Renderer is already initialized.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}

		gpRenderer = this;

		IDXGISwapChain* pSwapChain;
		ID3D11Device* pDevice;
		ID3D11DeviceContext* pContext;

		auto featureLevel = D3D_FEATURE_LEVEL_11_0;

		DXGI_SWAP_CHAIN_DESC desc = { };

		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;

		desc.OutputWindow = FindWindow(L"UnrealWindow", L"Fortnite  ");
		desc.Windowed = TRUE;

		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Initialize and check if we failed to initialize our DirectX 11 device.
		HRESULT hResult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &desc, &pSwapChain, &pDevice, nullptr, &pContext);
		if (FAILED(hResult))
		{
			MessageBox(0, L"Failed to create DirectX 11 device.", L"Error", MB_ICONERROR);
			ExitProcess(EXIT_FAILURE);
		}

		auto pTable = *reinterpret_cast<PVOID**>(pSwapChain);

		auto pPresent = pTable[8];
		auto pResizeBuffers = pTable[13];

		pSwapChain->Release();
		pDevice->Release();
		pContext->Release();

		MH_CreateHook(pPresent, PresentHook, reinterpret_cast<PVOID*>(&Present));
		MH_EnableHook(pPresent);

		MH_CreateHook(pResizeBuffers, ResizeBuffersHook, reinterpret_cast<PVOID*>(&ResizeBuffers));
		MH_EnableHook(pResizeBuffers);
	}

	Renderer::~Renderer()
	{
		gpRenderer = nullptr;

		auto pPresent = reinterpret_cast<PVOID*>(&Present);

		MH_DisableHook(pPresent);
		MH_RemoveHook(pPresent);

		auto pResizeBuffers = reinterpret_cast<PVOID*>(&ResizeBuffers);

		MH_DisableHook(pResizeBuffers);
		MH_RemoveHook(pResizeBuffers);
	}
}
