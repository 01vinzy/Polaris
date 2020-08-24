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

	Window::~Window()
	{
		//TODO: unload Ui
	}

	void Window::Draw()
	{
	}

	void Window::Resize()
	{
	}
}
