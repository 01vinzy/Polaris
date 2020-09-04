#ifndef UI_H
#define UI_H

#include <string>

namespace polaris
{
	class Window
	{
	public:
		bool bShowWindow = false;

		Window();
		virtual void Draw();
		virtual void Resize();
	};
}

#endif // UI_H
