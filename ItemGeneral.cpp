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
	
	LsbObject *statsDirectory = getStatsDirectory();
	
	//stats
	if (statsDirectory != 0) {
		QLineEdit *levelEdit = this->findChild<QLineEdit *>("levelEdit");
		levelEdit->setText(LsbReader::lookupByUniquePathEntity(statsDirectory, "Level")->toString().c_str());
		
		QLineEdit *duraEdit = this->findChild<QLineEdit *>("duraEdit");
		duraEdit->setText(LsbReader::lookupByUniquePathEntity(statsDirectory, "Durability")->toString().c_str());
		
		QCheckBox *identCheck = this->findChild<QCheckBox *>("identCheck");
		std::string identText = LsbReader::lookupByUniquePathEntity(statsDirectory, "IsIdentified")->toString();
		identCheck->setChecked((identText == "false" ? false : true));
		
		QComboBox *itemTypeCombo = this->findChild<QComboBox *>("itemTypeCombo");
		itemTypeCombo->setCurrentText(LsbReader::lookupByUniquePathEntity(statsDirectory, "ItemType")->toString().c_str());
		
		QLineEdit *repairDuraEdit = this->findChild<QLineEdit *>("repairDuraEdit");
		repairDuraEdit->setText(LsbReader::lookupByUniquePathEntity(statsDirectory, "RepairDurabilityPenalty")->toString().c_str());
	}
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

LsbObject *ItemGeneral::getStatsDirectory() {
	LsbObject *statsDirectory = 0;
	std::vector<LsbObject *> statsObjects = LsbReader::lookupAllEntitiesWithName(item->getObject(), "Stats");
	for (int i=0; i<statsObjects.size(); ++i) {
		LsbObject *object = statsObjects[i];
		if (object->isDirectory()) {
			statsDirectory = object;
			break;
		}
	}
	
	return statsDirectory;
}

LsbObject *ItemGeneral::createStatsDirectory() {
	TAG_LSB *statsTag = LsbReader::getTagByName("Stats", tagList);
	LsbObject *newStatsDir = new LsbObject(true, statsTag->index, statsTag->tag, 0, item->getObject(), tagList);
	
	TAG_LSB *customRequirementsTag = LsbReader::getTagByName("CustomRequirements", tagList);
	LsbObject *newCustomRequirements = new LsbObject(false, customRequirementsTag->index, customRequirementsTag->tag, 0x13, newStatsDir, tagList);
	bool custRequirements = false;
	newCustomRequirements->setData((char *)&custRequirements, sizeof(bool));
	
	TAG_LSB *durabilityTag = LsbReader::getTagByName("Durability", tagList);
	LsbObject *newDurability = new LsbObject(false, durabilityTag->index, durabilityTag->tag, 0x04, newStatsDir, tagList);
	long durability = 100;
	newDurability->setData((char *)&durability, sizeof(long));
	
	TAG_LSB *durabilityCounterTag = LsbReader::getTagByName("DurabilityCounter", tagList);
	LsbObject *newDurabilityCounter = new LsbObject(false, durabilityCounterTag->index, durabilityCounterTag->tag, 0x04, newStatsDir, tagList);
	long durabilityCounter = 8;
	newDurabilityCounter->setData((char *)&durabilityCounter, sizeof(long));
	
	TAG_LSB *isIdentifiedTag = LsbReader::getTagByName("IsIdentified", tagList);
	LsbObject *newIsIdentified = new LsbObject(false, isIdentifiedTag->index, isIdentifiedTag->tag, 0x04, newStatsDir, tagList);
	long isIdentified = 1;
	newIsIdentified->setData((char *)&isIdentified, sizeof(long));
	
	TAG_LSB *itemTypeTag = LsbReader::getTagByName("ItemType", tagList);
	LsbObject *newItemType = new LsbObject(false, itemTypeTag->index, itemTypeTag->tag, 0x16, newStatsDir, tagList);
	std::string itemType = "Magic";
	newItemType->setData(itemType.c_str(), itemType.length() + 1);
	
	TAG_LSB *levelTag = LsbReader::getTagByName("Level", tagList);
	LsbObject *newLevel = new LsbObject(false, levelTag->index, levelTag->tag, 0x04, newStatsDir, tagList);
	long level = 1;
	newLevel->setData((char *)&level, sizeof(long));
	
	TAG_LSB *repairDurabilityPenaltyTag = LsbReader::getTagByName("RepairDurabilityPenalty", tagList);
	LsbObject *newRepairDurabilityPenalty = new LsbObject(false, repairDurabilityPenaltyTag->index, repairDurabilityPenaltyTag->tag, 0x04, newStatsDir, tagList);
	long repairDurabilityPenalty = 0;
	newRepairDurabilityPenalty->setData((char *)&repairDurabilityPenalty, sizeof(long));
	
	newStatsDir->addChild(newCustomRequirements);
	newStatsDir->addChild(newDurability);
	newStatsDir->addChild(newDurabilityCounter);
	newStatsDir->addChild(newIsIdentified);
	newStatsDir->addChild(newItemType);
	newStatsDir->addChild(newLevel);
	newStatsDir->addChild(newRepairDurabilityPenalty);
	
	item->getObject()->addChild(newStatsDir);
	return newStatsDir;
}

void ItemGeneral::on_levelEdit_textEdited(const QString &text)
{
	LsbObject *statsDirectory = getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = createStatsDirectory();
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
	LsbObject *statsDirectory = getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = createStatsDirectory();
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
	LsbObject *statsDirectory = getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = createStatsDirectory();
	}
	std::string value = text.toStdString();
	if (statsDirectory != 0) {
		LsbObject *itemTypeObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "ItemType");
		itemTypeObject->setData(value.c_str(), value.length() + 1);
	}
	LsbObject *generationObject = LsbReader::lookupByUniquePathEntity(item->getObject(), "Generation");
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
	LsbObject *statsDirectory = getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = createStatsDirectory();
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
	LsbObject *statsDirectory = getStatsDirectory();
	if (statsDirectory == 0) {
		statsDirectory = createStatsDirectory();
	}
	if (statsDirectory != 0) {
		LsbObject *levelObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "IsIdentified");
		long value = checked;
		levelObject->setData((char *)&value, sizeof(long));
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
