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
	MainWindow w;
	w.show();
	QTreeWidget *tree = w.findChild<QTreeWidget *>("treeWidget");
	//std::cout.rdbuf(0);
	
	return a.exec();
}
