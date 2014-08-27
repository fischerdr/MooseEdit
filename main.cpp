
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
	std::wstring arg = L"";
	#ifdef _WIN32
		LPWSTR wideArgText = GetCommandLineW();
		int wArgc;
		LPWSTR *wideArgs = CommandLineToArgvW(wideArgText, &wArgc);
		if (wArgc == 2) {
			arg = wideArgs[1];
		}
	#endif
	MainWindow *w = new MainWindow(arg);
	w->show();
	
	int exec = a.exec();
	delete w;
	#ifdef _WIN32
		LocalFree(wideArgs);
	#endif
	return exec;
}
