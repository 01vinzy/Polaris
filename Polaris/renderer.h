#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "ui.h"

#include <list>

namespace polaris
{
	class Renderer
	{
	public:
		static std::list<polaris::Ui*> pUiInstances;

		ID3D11Device* pCurrentDevice = nullptr;
		ID3D11DeviceContext* pCurrentContext = nullptr;
		ID3D11RenderTargetView* pCurrentView = nullptr;

		Renderer();
		~Renderer();

		static void SubscribeUIToEvents(Ui* ui)
		{
			pUiInstances.push_back(ui);
		}
	};
}

static polaris::Renderer* gpRenderer;

#endif // RENDERER_H
