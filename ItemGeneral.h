#ifndef ITEMGENERAL_H
#define ITEMGENERAL_H

#include <QWidget>
#include <vector>
#include "GameItem.h"
#include "LsbReader.h"
#include "LsbObject.h"

namespace Ui {
class ItemGeneral;
}

class GeneralEditCallback {
public:
	virtual void onEdit() = 0;
};

class ItemGeneral : public QWidget
{
	Q_OBJECT
	
public:
	explicit ItemGeneral(GameItem *item, std::vector<TAG_LSB *> *tagList, QWidget *parent = 0);
	void registerCallback(GeneralEditCallback *generalEditCallback);
	void refreshGeneralData();
	~ItemGeneral();
	
private slots:
	void on_amountEdit_textEdited(const QString &arg1);
	
	void on_scaleEdit_textEdited(const QString &arg1);
	
	void on_levelEdit_textEdited(const QString &arg1);
	
	void on_duraEdit_textEdited(const QString &arg1);
	
	void on_itemTypeCombo_currentIndexChanged(const QString &arg1);
	
	void on_repairDuraEdit_textEdited(const QString &arg1);
	
	void on_identCheck_toggled(bool checked);
	
private:
	Ui::ItemGeneral *ui;
	GameItem *item;
	LsbObject *getStatsDirectory();
	LsbObject *createStatsDirectory();
	std::vector<TAG_LSB *> *tagList;
	GeneralEditCallback *generalEditCallback = 0;
};

#endif // ITEMGENERAL_H
