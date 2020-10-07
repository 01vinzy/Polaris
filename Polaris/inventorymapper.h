#ifndef INVENTORYMAPPER_H
#define INVENTORYMAPPER_H

#include "common.h"
#include "window.h"
#include "athena.h"

#include <imgui.h>
#include <list>

namespace polaris
{
	class InventoryMapper : public Window
	{
	public:
		std::list<std::string> inventoryItems;

		InventoryMapper();
		void Draw() override;
	};
}

#endif // INVENTORYMAPPER_H