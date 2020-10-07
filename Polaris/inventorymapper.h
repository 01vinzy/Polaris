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
	private:
		int m_iPickingNewWIDFor;
	public:
		std::list<std::string> m_lInventoryItems;
		std::list<SDK::UFortWeaponRangedItemDefinition*> m_lItemsInMemory;

		InventoryMapper();
		void Draw() override;
	};
}

extern polaris::InventoryMapper* gpInventoryMapper;

#endif // INVENTORYMAPPER_H