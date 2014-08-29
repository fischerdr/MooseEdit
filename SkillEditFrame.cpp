#include "SkillEditFrame.h"
#include "ui_SkillEditFrame.h"

SkillEditFrame::SkillEditFrame(GameCharacter *character, std::vector<StatsContainer *> *skillStats, QWidget *parent) :
	QFrame(parent), skillStats(skillStats), character(character),
	ui(new Ui::SkillEditFrame)
{
	ui->setupUi(this);
	
	populateSkillTree();
	populateSkillList();
}

DataTreeItem *SkillEditFrame::createFolderIfNeeded(QTreeWidgetItem *parent, const char *folderName, const char *data) {
	for (int i=0; i<parent->childCount(); ++i) {
		QTreeWidgetItem *item = parent->child(i);
		if (item->text(0) == folderName) {
			return (DataTreeItem *)item;
		}
	}
	DataTreeItem *item = new DataTreeItem(parent);
	item->setText(0, folderName);
	if (data != 0) {
		item->setStringData(data);
	}
	return item;
}

void SkillEditFrame::populateSkillList() {
	QListWidget *skillList = this->findChild<QListWidget *>("skillList");
	std::vector<LsbObject *> skillObjects = character->getSkillList();
	for (int i=0; i<skillObjects.size(); ++i) {
		LsbObject *skillObject = skillObjects[i];
		LsbObject *mapKeyObject = skillObject->lookupByUniquePath("MapKey");
		if (mapKeyObject != 0) {
			DataListItem *item = new DataListItem(skillList);
			StatsContainer *skill = GenStatsReader::getContainer(*skillStats, mapKeyObject->getData());
			if (skill != 0) {
				std::string skillText = skill->getData("DisplayNameRef");
				item->setStringData(mapKeyObject->getData());
				item->setText(skillText.c_str());
			}
		}
	}
}

void SkillEditFrame::populateSkillTree() {
	QTreeWidget *skillTree = this->findChild<QTreeWidget *>("skillTree");
	for (int i=0; i<skillStats->size(); ++i) {
		StatsContainer *skill = skillStats->at(i);
		std::string abilityText = skill->getData("Ability");
		std::string elementText = skill->getData("Element");
		if (elementText.size() == 0) {
			elementText = "Unknown";
		}
		std::string skillText = skill->getData("DisplayNameRef");
		std::string skillInternalText = skill->getArg(0);
		QTreeWidgetItem *abilityFolder = createFolderIfNeeded(skillTree->invisibleRootItem(), abilityText.c_str(), 0);
		QTreeWidgetItem *elementFolder = createFolderIfNeeded(abilityFolder, elementText.c_str(), 0);
		createFolderIfNeeded(elementFolder, skillText.c_str(), skillInternalText.c_str());
	}
	skillTree->setUpdatesEnabled(false);
	skillTree->expandAll();
	skillTree->resizeColumnToContents(0);
	skillTree->collapseAll();
	skillTree->setUpdatesEnabled(true);
}

SkillEditFrame::~SkillEditFrame()
{
	delete ui;
}

void SkillEditFrame::onCurrentItemChanged(DataContainerItem *current) {
	if (current == 0)
		return;
	QTableWidget *skillTable = this->findChild<QTableWidget *>("skillTable");
	DataContainerItem *item = current;
	if (item->getStringData().size() > 0) {
		StatsContainer *skill = GenStatsReader::getContainer(*skillStats, item->getStringData());
		if (skill != 0) {
			typedef std::map<std::string, std::string> StringMap;
			StringMap dataMap = skill->getBaseDataMap();
			for (int i=0; i<skillTable->rowCount(); ++i) {
				for (int j=0; j<skillTable->columnCount(); ++j) {
					delete skillTable->item(i, j);
				}
			}
			
			skillTable->setRowCount(0);
			int row = 0;
			for (StringMap::iterator it = dataMap.begin(); it != dataMap.end(); ++it) {
				QTableWidgetItem *nameItem = new QTableWidgetItem();
				nameItem->setText(it->first.c_str());
				QTableWidgetItem *valueItem = new QTableWidgetItem();
				valueItem->setText(it->second.c_str());
				skillTable->insertRow(row);
				skillTable->setItem(row, 0, nameItem);
				skillTable->setItem(row, 1, valueItem);
				++row;
			}

			skillTable->resizeRowsToContents();
			skillTable->resizeColumnsToContents();
		}
	}
}

void SkillEditFrame::on_skillTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	DataTreeItem *item = (DataTreeItem *) current;
	onCurrentItemChanged((DataContainerItem *)item);
}

void SkillEditFrame::on_skillList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	DataListItem *item = (DataListItem *) current;
    onCurrentItemChanged((DataContainerItem *)item);
}

void SkillEditFrame::on_addButton_released()
{
	QTreeWidget *skillTree = this->findChild<QTreeWidget *>("skillTree");
	QList<QTreeWidgetItem *> items = skillTree->selectedItems();
	for (int i=0; i<items.size(); ++i) {
		DataTreeItem *item = (DataTreeItem *)items[i];
		if (item == 0)
			return;
		if (item->getStringData().size() == 0)
			return;
		character->addSkill(item->getStringData().c_str());
		
		QListWidget *skillList = this->findChild<QListWidget *>("skillList");
		DataListItem *newItem = new DataListItem();
		StatsContainer *skill = GenStatsReader::getContainer(*skillStats, item->getStringData());
		std::string skillText = skill->getData("DisplayNameRef");
		newItem->setStringData(item->getStringData());
		newItem->setText(skillText.c_str());
		skillList->addItem(newItem);
	}
}

void SkillEditFrame::on_removeButton_released()
{
	QListWidget *skillList = this->findChild<QListWidget *>("skillList");
	QList<QListWidgetItem *> items = skillList->selectedItems();
	for (int i=0; i<items.size(); ++i) {
		DataListItem *item = (DataListItem *)items[i];
		if (item == 0)
			return;
		if (item->getStringData().size() == 0)
			return;
		character->removeSkill(item->getStringData().c_str());
		
		delete item;
	}
}

void SkillEditFrame::on_skillList_itemClicked(QListWidgetItem *current)
{
	DataListItem *item = (DataListItem *) current;
    onCurrentItemChanged((DataContainerItem *)item);
}
