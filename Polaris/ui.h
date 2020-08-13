#ifndef UI_H
#define UI_H

#include <string>

namespace polaris
{
	class Ui
	{
	public:
		Ui();
		~Ui();
		virtual void Draw();
		virtual void Resize();
	};
}

#endif // UI_H
