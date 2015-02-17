#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(EditorSettings *settings, QWidget *parent) :
	QDialog(parent), originalSettings(settings),
	ui(new Ui::SettingsDialog)
{
	if (originalSettings == 0) {
		newSettings = 0;
	} else {
		newSettings = new EditorSettings(*originalSettings);
	}
	ui->setupUi(this);
	initializeValues();
}

SettingsDialog::~SettingsDialog()
{
	if (newSettings != 0) {
		delete newSettings;
	}
	delete ui;
}

void SettingsDialog::initializeValues() {
	if (newSettings != 0) {
		bool useCompression = (newSettings->getProperty(L"useCompression") == L"1"? true : false);
		bool highCompressionMode = (newSettings->getProperty(L"highCompressionMode") == L"1"? true : false);
		QCheckBox *compressionCheckBox = this->findChild<QCheckBox *>("useCompressionCheckBox");
		if (compressionCheckBox != 0) {
			compressionCheckBox->setChecked(useCompression);
		}
		QCheckBox *highCompressionCheckBox = this->findChild<QCheckBox *>("highCompressionCheckBox");
		if (highCompressionCheckBox != 0) {
			highCompressionCheckBox->setChecked(highCompressionMode);
		}
		applyPostProcessing();
	}
}

void SettingsDialog::applyPostProcessing() {
	bool useCompression = (newSettings->getProperty(L"useCompression") == L"1"? true : false);
	QCheckBox *highCompressionCheckBox = this->findChild<QCheckBox *>("highCompressionCheckBox");
	if (highCompressionCheckBox != 0) {
		if (!useCompression) {
			highCompressionCheckBox->setEnabled(false);
		} else {
			highCompressionCheckBox->setEnabled(true);
		}
	}
}

void SettingsDialog::on_useCompressionCheckBox_clicked()
{
	if (newSettings != 0) {
		QCheckBox *compressionCheckBox = this->findChild<QCheckBox *>("useCompressionCheckBox");
		if (compressionCheckBox != 0) {
			if (compressionCheckBox->isChecked()) {
				newSettings->setProperty(L"useCompression", L"1");
			} else {
				newSettings->setProperty(L"useCompression", L"0");
			}
		}
		applyPostProcessing();
	}
}

void SettingsDialog::on_highCompressionCheckBox_clicked()
{
	if (newSettings != 0) {
		QCheckBox *highCompressionCheckBox = this->findChild<QCheckBox *>("highCompressionCheckBox");
		if (highCompressionCheckBox != 0) {
			if (highCompressionCheckBox->isChecked()) {
				newSettings->setProperty(L"highCompressionMode", L"1");
			} else {
				newSettings->setProperty(L"highCompressionMode", L"0");
			}
		}
	}
}

void SettingsDialog::on_buttonBox_accepted()
{
	if (originalSettings != 0 && newSettings != 0) {
		*originalSettings = *newSettings;
	}
	this->setResult(1);
}

void SettingsDialog::on_buttonBox_rejected()
{
	this->setResult(0);
}
