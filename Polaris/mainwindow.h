#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common.h"
#include "ui.h"

namespace polaris
{
	class MainWindow : public Ui
	{
	public:
		MainWindow();
		~MainWindow();
		void Draw() override;
	};
}

#endif // MAINWINDOW_H