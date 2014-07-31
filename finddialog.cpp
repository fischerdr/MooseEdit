#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FindDialog)
{
	ui->setupUi(this);
}

#include <windows.h>
void FindDialog::showEvent(QShowEvent *show) {
	QWidget::showEvent(show);
	QLineEdit *findEdit = this->findChild<QLineEdit *>("findEdit");
	findEdit->setFocus();
	findEdit->selectAll();
}

FindDialog::~FindDialog()
{
	delete ui;
}
