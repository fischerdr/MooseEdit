
#include "mainwindow.h"
#include <QApplication>
#include <QTreeWidget>
#include <QTreeView>
#include <QMessageBox>
#include <QString>
#include <fstream>
#include <QFileDialog>

#include <windows.h>
#include <QGLFormat>

#include "LsbReader.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	QGLFormat glf = QGLFormat::defaultFormat();
	glf.setSampleBuffers(true);
	glf.setSamples(16);
	QGLFormat::setDefaultFormat(glf);
	
	a.setStyle("windowsxp");
	MainWindow *w = new MainWindow();
	w->show();
	
	int exec = a.exec();
	delete w;
	return exec;
}
