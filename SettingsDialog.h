#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "EditorSettings.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT
	
	EditorSettings *originalSettings;
	EditorSettings *newSettings;
	void initializeValues();
	void applyPostProcessing();
	
public:
	explicit SettingsDialog(EditorSettings *originalSettings, QWidget *parent = 0);
	~SettingsDialog();
	
private slots:
	void on_useCompressionCheckBox_clicked();
	
	void on_highCompressionCheckBox_clicked();
	
	void on_buttonBox_accepted();
	
	void on_buttonBox_rejected();
	
private:
	Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
