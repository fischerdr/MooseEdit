#include "AddNodeDialog.h"
#include "ui_AddNodeDialog.h"
#include <boost/lexical_cast.hpp>

AddNodeDialog::AddNodeDialog(LsbObject *parentObject, std::vector<TAG_LSB *> *tagList, QWidget *parent) :
	QDialog(parent), parentObject(parentObject), tagList(tagList),
	ui(new Ui::AddNodeDialog)
{
	ui->setupUi(this);
	QRadioButton *noRadio = this->findChild<QRadioButton *>("noDir");
	if (noRadio != 0) {
		isDirectory = false;
		noRadio->setChecked(true);
	}
}

AddNodeDialog::~AddNodeDialog()
{
	delete ui;
}

LsbObject *AddNodeDialog::getNewLsbObject() {
	return newObject;
}

void AddNodeDialog::on_buttonBox_accepted()
{
	if (parentObject != 0 && newObject == 0 && tagList != 0) {
		TAG_LSB *tag = LsbObject::createTagIfNeeded(tagName.c_str(), tagList);
		if (this->nodeType.length() > 0 || this->isDirectory) {
			long type = 0;
			try {
				type = boost::lexical_cast<long>(nodeType);
			} catch (const boost::bad_lexical_cast& e) {
				
			}

			newObject = new LsbObject(isDirectory, tag->index, tag->tag, type, parentObject, tagList);
			if (this->nodeValue.length() > 0) {
				newObject->fromString(this->nodeValue);
			}
			this->setResult(1);
			return;
		}
	}
	this->setResult(0);
}

void AddNodeDialog::on_buttonBox_rejected()
{
    this->setResult(0);
}

void AddNodeDialog::on_nameEdit_textChanged(const QString &text)
{
    this->tagName = text.toStdString();
}

void AddNodeDialog::on_typeEdit_textChanged(const QString &text)
{
    this->nodeType = text.toStdString();
}

void AddNodeDialog::on_valueEdit_textChanged(const QString &text)
{
    this->nodeValue = text.toStdString();
}

void AddNodeDialog::on_yesDir_clicked()
{
    this->isDirectory = true;
	QLineEdit *typeEdit = this->findChild<QLineEdit *>("typeEdit");
	QLineEdit *valueEdit = this->findChild<QLineEdit *>("valueEdit");
	if (typeEdit != 0 && valueEdit != 0) {
		typeEdit->setEnabled(false);
		valueEdit->setEnabled(false);
	}
}

void AddNodeDialog::on_noDir_clicked()
{
    this->isDirectory = false;
	QLineEdit *typeEdit = this->findChild<QLineEdit *>("typeEdit");
	QLineEdit *valueEdit = this->findChild<QLineEdit *>("valueEdit");
	if (typeEdit != 0 && valueEdit != 0) {
		typeEdit->setEnabled(true);
		valueEdit->setEnabled(true);
	}
}
