#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"

namespace polaris
{
	class Renderer
	{
	public:
		ID3D11Device* pCurrentDevice = nullptr;
		ID3D11DeviceContext* pCurrentContext = nullptr;
		ID3D11RenderTargetView* pCurrentView = nullptr;

		Renderer();
		~Renderer();
	};
}

static polaris::Renderer* gpRenderer;

#endif // RENDERER_H
