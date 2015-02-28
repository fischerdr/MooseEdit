#ifndef ADDNODEDIALOG_H
#define ADDNODEDIALOG_H

#include <QDialog>
#include "LsbObject.h"

namespace Ui {
class AddNodeDialog;
}

class AddNodeDialog : public QDialog
{
	Q_OBJECT
	
	LsbObject *parentObject;
	LsbObject *newObject = 0;
	std::vector<TAG_LSB *> *tagList = 0;
	std::string tagName;
	bool isDirectory;
	std::string nodeType;
	std::string nodeValue;
	
public:
	explicit AddNodeDialog(LsbObject *parentObject, std::vector<TAG_LSB *> *tagList, QWidget *parent = 0);
	~AddNodeDialog();
	LsbObject *getNewLsbObject();
	
private slots:
	void on_buttonBox_accepted();
	
	void on_buttonBox_rejected();
	
	void on_nameEdit_textChanged(const QString &text);
	
	void on_typeEdit_textChanged(const QString &text);
	
	void on_valueEdit_textChanged(const QString &text);
	
	void on_yesDir_clicked();
	
	void on_noDir_clicked();
	
private:
	Ui::AddNodeDialog *ui;
};

#endif // ADDNODEDIALOG_H
