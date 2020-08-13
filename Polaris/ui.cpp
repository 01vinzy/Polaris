#include "ui.h"
#include "renderer.h";

#include <imgui.h>
#include <Windows.h>

namespace polaris
{
	Ui::Ui()
	{
		polaris::Renderer::SubscribeUIToEvents(this);
	}

	Ui::~Ui()
	{
		//TODO: unload Ui
	}

	void Ui::Draw()
	{
		ImGui::Begin("Test");
	}

	void Ui::Resize()
	{
	}
}