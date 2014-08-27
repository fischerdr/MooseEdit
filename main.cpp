
#include "mainwindow.h"
#include <QApplication>
#include <QTreeWidget>
#include <QTreeView>
#include <QMessageBox>
#include <QString>
#include <fstream>
#include <QFileDialog>

#include <windows.h>

#include "LsbReader.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle("windowsxp");
	MainWindow *w = new MainWindow();
	w->show();
	
	int exec = a.exec();
	delete w;
	return exec;
}
