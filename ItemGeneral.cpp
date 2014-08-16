#include "ItemGeneral.h"
#include "ui_ItemGeneral.h"
#include <QCheckBox>
#include <QComboBox>
#include <boost/lexical_cast.hpp>

ItemGeneral::ItemGeneral(GameItem *item, std::vector<TAG_LSB *> *tagList, QWidget *parent) :
	QWidget(parent), item(item), tagList(tagList),
	ui(new Ui::ItemGeneral)
{
	ui->setupUi(this);
	refreshGeneralData();
}

void ItemGeneral::registerCallback(GeneralEditCallback *generalEditCallback)
{
	this->generalEditCallback = generalEditCallback;
}

void ItemGeneral::refreshGeneralData()
{
	QLineEdit *amountEdit = this->findChild<QLineEdit *>("amountEdit");
	amountEdit->setText(LsbReader::lookupByUniquePathEntity(item->getObject(), "Amount")->toString().c_str());
	
	QLineEdit *scaleEdit = this->findChild<QLineEdit *>("scaleEdit");
	scaleEdit->setText(LsbReader::lookupByUniquePathEntity(item->getObject(), "Scale")->toString().c_str());
	
	LsbObject *statsDirectory = item->getStatsDirectory();
	
	//stats
	QLineEdit *levelEdit = this->findChild<QLineEdit *>("levelEdit");
	QLineEdit *duraEdit = this->findChild<QLineEdit *>("duraEdit");
	QCheckBox *identCheck = this->findChild<QCheckBox *>("identCheck");
	QComboBox *itemTypeCombo = this->findChild<QComboBox *>("itemTypeCombo");
	QLineEdit *repairDuraEdit = this->findChild<QLineEdit *>("repairDuraEdit");
	levelEdit->blockSignals(true);
	duraEdit->blockSignals(true);
	identCheck->blockSignals(true);
	itemTypeCombo->blockSignals(true);
	repairDuraEdit->blockSignals(true);
	if (statsDirectory != 0) {
		levelEdit->setText(LsbReader::lookupByUniquePathEntity(statsDirectory, "Level")->toString().c_str());
		duraEdit->setText(LsbReader::lookupByUniquePathEntity(statsDirectory, "Durability")->toString().c_str());
		long identValue = *((long*)LsbReader::lookupByUniquePathEntity(statsDirectory, "IsIdentified")->getData());
		bool checked = identValue != 0;
		identCheck->setChecked(checked);
		identCheck->setText((checked ? "True" : "False"));
		itemTypeCombo->setCurrentText(LsbReader::lookupByUniquePathEntity(statsDirectory, "ItemType")->toString().c_str());
		repairDuraEdit->setText(LsbReader::lookupByUniquePathEntity(statsDirectory, "RepairDurabilityPenalty")->toString().c_str());
	} else {
		levelEdit->clear();
		duraEdit->clear();
		identCheck->setChecked(false);
		itemTypeCombo->setCurrentIndex(0);
		repairDuraEdit->clear();
	}
	levelEdit->blockSignals(false);
	duraEdit->blockSignals(false);
	identCheck->blockSignals(false);
	itemTypeCombo->blockSignals(false);
	repairDuraEdit->blockSignals(false);
}

ItemGeneral::~ItemGeneral()
{
	delete ui;
}

void ItemGeneral::on_amountEdit_textEdited(const QString &text)
{
    LsbObject *amountObject = LsbReader::lookupByUniquePathEntity(item->getObject(), "Amount");
	long value = 0;
	try {
		value = boost::lexical_cast<long>(text.toStdString());
	} catch (const boost::bad_lexical_cast &e) {
		;
	}

	amountObject->setData((char *)&value, sizeof(long));
	
	if (generalEditCallback != 0) {
		generalEditCallback->onEdit();
	}
}

void ItemGeneral::on_scaleEdit_textEdited(const QString &text)
{
	LsbObject *scaleObject = LsbReader::lookupByUniquePathEntity(item->getObject(), "Scale");
	float value = 0;
	try {
		value = boost::lexical_cast<float>(text.toStdString());
	} catch (const boost::bad_lexical_cast &e) {
		;
	}
	scaleObject->setData((char *)&value, sizeof(float));
	
	if (generalEditCallback != 0) {
		generalEditCallback->onEdit();
	}
}

void ItemGeneral::on_levelEdit_textEdited(const QString &text)
{
	LsbObject *statsDirectory = item->getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = item->createStatsDirectory();
	}
	long value = 0;
	try {
		value = boost::lexical_cast<long>(text.toStdString());
	} catch (const boost::bad_lexical_cast &e) {
		;
	}
	if (statsDirectory != 0) {
		LsbObject *levelObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "Level");
		levelObject->setData((char *)&value, sizeof(long));
	}
//	LsbObject *generationObject = LsbReader::lookupByUniquePathEntity(item->getObject(), "Generation");
//	if (generationObject != 0) {
//		LsbObject *generationLevelObject = LsbReader::lookupByUniquePathEntity(generationObject, "Level");
//		//WARNING: level is short here
//		generationLevelObject->setData((char *)&value, sizeof(long));
//	}
	
	refreshGeneralData();
	
	if (generalEditCallback != 0) {
		generalEditCallback->onEdit();
	}
}

void ItemGeneral::on_duraEdit_textEdited(const QString &text)
{
	LsbObject *statsDirectory = item->getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = item->createStatsDirectory();
	}
	if (statsDirectory != 0) {
		LsbObject *durabilityObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "Durability");
		long value = 0;
		try {
			value = boost::lexical_cast<long>(text.toStdString());
		} catch (const boost::bad_lexical_cast &e) {
			;
		}
		durabilityObject->setData((char *)&value, sizeof(long));
	}
	
	refreshGeneralData();
	
	if (generalEditCallback != 0) {
		generalEditCallback->onEdit();
	}
}

void ItemGeneral::on_itemTypeCombo_currentIndexChanged(const QString &text)
{
	LsbObject *statsDirectory = item->getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = item->createStatsDirectory();
	}
	std::string value = text.toStdString();
	if (statsDirectory != 0) {
		LsbObject *itemTypeObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "ItemType");
		itemTypeObject->setData(value.c_str(), value.length() + 1);
	}
	LsbObject *generationObject = LsbReader::lookupByUniquePathEntity(item->getObject(), "Generation");
	if (generationObject == 0) {
		if (value != "Common") {
			generationObject = item->createGenerationDirectory();
		}
	}
	if (generationObject != 0) {
		LsbObject *generationItemTypeObject = LsbReader::lookupByUniquePathEntity(generationObject, "ItemType");
		generationItemTypeObject->setData(value.c_str(), value.length() + 1);
	}
	
	refreshGeneralData();
	
	if (generalEditCallback != 0) {
		generalEditCallback->onEdit();
	}
}

void ItemGeneral::on_repairDuraEdit_textEdited(const QString &text)
{
	LsbObject *statsDirectory = item->getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = item->createStatsDirectory();
	}
	if (statsDirectory != 0) {
		LsbObject *repairDuraObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "RepairDurabilityPenalty");
		long value = 0;
		try {
			value = boost::lexical_cast<long>(text.toStdString());
		} catch (const boost::bad_lexical_cast &e) {
			;
		}
		repairDuraObject->setData((char *)&value, sizeof(long));
	}
	
	refreshGeneralData();
	
	if (generalEditCallback != 0) {
		generalEditCallback->onEdit();
	}
}

void ItemGeneral::on_identCheck_toggled(bool checked)
{
	LsbObject *statsDirectory = item->getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = item->createStatsDirectory();
	}
	if (statsDirectory != 0) {
		LsbObject *identObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "IsIdentified");
		long value = checked;
		identObject->setData((char *)&value, sizeof(long));
		QCheckBox *identCheck = this->findChild<QCheckBox *>("identCheck");
		if (checked) {
			identCheck->setText("True");
		}
		else {
			identCheck->setText("False");
		}
	}
	
	refreshGeneralData();
	
	if (generalEditCallback != 0) {
		generalEditCallback->onEdit();
	}
}
GameItem *ItemGeneral::getItem() const
{
	return item;
}

void ItemGeneral::setItem(GameItem *value)
{
	item = value;
}

