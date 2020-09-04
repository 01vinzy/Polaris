#include "window.h"
#include "renderer.h"

#include <imgui.h>
#include <Windows.h>

namespace polaris
{
	Window::Window()
	{
		polaris::Renderer::SubscribeUIToEvents(this);
	}

	void Window::Draw()
	{
	}

	void Window::Resize()
	{
	}
}
