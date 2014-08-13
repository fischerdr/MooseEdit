#include "StatsView.h"
#include "ui_StatsView.h"
#include <typeinfo>

StatsView::StatsView(std::vector<StatsContainer *> &allItemStats, std::map<std::string, std::string> &nameMappings, QWidget *parent) :
	QWidget(parent), allItemStats(allItemStats), nameMappings(nameMappings),
	ui(new Ui::StatsView)
{
	ui->setupUi(this);
}

void StatsView::showEvent(QShowEvent *event) {
	QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
	for (int i=0; i<buttons.size(); ++i) {
		if (!buttons[i]->isEnabled())
			buttons[i]->hide();
	}
}

DataContainerTreeItem *StatsView::addFolder(std::string folderName) {
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	DataContainerTreeItem *item = new DataContainerTreeItem();
	statsTree->addTopLevelItem(item);
	item->setText(0, folderName.c_str());
	return item;
}

void StatsView::addToTree(std::vector<LsbObject *>& objects, QTreeWidgetItem *parent) {
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	for (int i=0; i<objects.size(); ++i) {
		LsbObject *object = objects[i];
		DataContainerTreeItem *boostNode;
		if (parent != 0) {
			boostNode = new DataContainerTreeItem(parent);
		}
		else {
			boostNode = new DataContainerTreeItem();
			statsTree->addTopLevelItem(boostNode);
		}
		std::string objectText = object->getName();
		boostNode->setData(object->toString());
		boostNode->setText(0, objectText.c_str());
	}
}

void StatsView::addDataToTree(StatsContainer *stat, QTreeWidgetItem *parent) {
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	std::map<std::string, std::string> baseData = stat->getBaseDataMap();
	for (std::map<std::string, std::string>::iterator it = baseData.begin(); it != baseData.end(); ++it) {
		std::string dataName = it->first;
		DataContainerTreeItem *dataNode;
		if (parent != 0) {
			dataNode = new DataContainerTreeItem(parent);
		}
		else {
			dataNode = new DataContainerTreeItem();
			statsTree->addTopLevelItem(dataNode);
		}
		dataNode->setData("");
		dataNode->setText(0, dataName.c_str());
	}
}

void StatsView::addToTree(std::vector<StatsContainer *>& stats, QTreeWidgetItem *parent) {
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	for (int i=0; i<stats.size(); ++i) {
		StatsContainer *stat = stats[i];
		DataContainerTreeItem *boostNode;
		if (parent != 0) {
			boostNode = new DataContainerTreeItem(parent);
		}
		else {
			boostNode = new DataContainerTreeItem();
			statsTree->addTopLevelItem(boostNode);
		}
		std::string statText = stat->getArg(0);
		boostNode->setData(statText);
		if (nameMappings.find(statText) != nameMappings.end()) {
			statText = nameMappings[statText] + "    (" + statText + ")";
		} else if (stat->getPrefixList().size() > 0 || stat->getSuffixList().size() > 0) {
			if (stat->getPrefixList().size() > 0) {
				statText = stat->getPrefixList()[0] + "    (" + statText + ")";
			} else if (stat->getSuffixList().size() > 0) {
				statText = stat->getSuffixList()[0] + "    (" + statText + ")";
			}
		}
		boostNode->setText(0, statText.c_str());
	}
}

DataContainerTreeItem *StatsView::findNodeByData(QTreeWidgetItem *node, std::string &data) {
	if (node->childCount() == 0) {
		DataContainerTreeItem *item = (DataContainerTreeItem *)node;
		if (item->getData() == data)
			return item;
	}
	for (int i=0; i<node->childCount(); ++i) {
		DataContainerTreeItem *value = 0;
		if ((value = findNodeByData(node->child(i), data)) != 0) {
			return value;
		}
	}
	return 0;
}

void StatsView::selectNodeByName(const char *name)
{
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	std::string nodeName = name;
	DataContainerTreeItem *selectedNode = findNodeByData(statsTree->invisibleRootItem(), nodeName);
	if (selectedNode != 0) {
		statsTree->setCurrentItem(selectedNode);
		selectedNode->setTextColor(0, QColor(255, 0, 0));
		selectedItem = selectedNode;
	}
}

void StatsView::clearTree() {
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	for (int i=0; i<statsTree->topLevelItemCount(); ++i) {
		delete statsTree->topLevelItem(i);
		--i;
	}
}

void StatsView::resizeTree() {
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	statsTree->expandAll();
	statsTree->resizeColumnToContents(0);
	statsTree->collapseAll();
}

void StatsView::addStatsDirectory(std::string name, std::string text, QTreeWidgetItem *parent) {
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	StatsContainer *boostBase = GenStatsReader::getContainer(allItemStats, name);
	std::vector<StatsContainer *> boosts = GenStatsReader::getSubclassContainers(allItemStats, boostBase);
	DataContainerTreeItem *boostFolder;
	if (parent != 0) {
		boostFolder = new DataContainerTreeItem(parent);
	}
	else {
		boostFolder = new DataContainerTreeItem(statsTree);
	}
	boostFolder->setText(0, text.c_str());
	addToTree(boosts, boostFolder);
}

void StatsView::addBoostDirectory(std::string modifierType, std::string text) {
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	std::vector<StatsContainer *> boosts = GenStatsReader::getContainersByContainerType(allItemStats, "deltamod");
	for (int i=0; i<boosts.size(); ++i) {
		StatsContainer *boost = boosts[i];
		if (boost->getData("ModifierType") != modifierType) {
			boosts.erase(boosts.begin() + i);
			--i;
			continue;
		}
	}
	DataContainerTreeItem *boostFolder = new DataContainerTreeItem(statsTree);
	boostFolder->setText(0, text.c_str());
	addToTree(boosts, boostFolder);
}

StatsView::~StatsView()
{
	delete ui;
}

void StatsView::enableSelectButton(StatsButtonCallback *callback) {
	QPushButton *selectButton = this->findChild<QPushButton *>("selectButton");
	selectButton->setEnabled(true);
	selectCallback = callback;
}

void StatsView::enableAddButton(StatsButtonCallback *callback) {
	QPushButton *addButton = this->findChild<QPushButton *>("addButton");
	addButton->setEnabled(true);
	addCallback = callback;
}

void StatsView::enableCancelButton(StatsButtonCallback *callback) {
	QPushButton *cancelButton = this->findChild<QPushButton *>("cancelButton");
	cancelButton->setEnabled(true);
	cancelCallback = callback;
}

void StatsView::enableRemoveButton(StatsButtonCallback *callback) {
	QPushButton *removeButton = this->findChild<QPushButton *>("removeButton");
	removeButton->setEnabled(true);
	removeCallback = callback;
}

void StatsView::registerTableEditCallback(TableEditCallback *tableEditCallback)
{
	this->tableEditCallback = tableEditCallback;
}

void StatsView::on_statsTree_itemClicked(QTreeWidgetItem *item, int column)
{
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	if (item->childCount() != 0) {
		item->setExpanded(!item->isExpanded());
	}
}

void StatsView::on_statsTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current == 0)
		return;
	std::string data = "";
	if (current->childCount() == 0) {
		DataContainerTreeItem *treeData = (DataContainerTreeItem *)current;
		data = treeData->getData();
	}
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	QLabel *modName = this->findChild<QLabel *>("modName");
	QTableWidget *modTable = this->findChild<QTableWidget *>("modTable");
	modTable->blockSignals(true);
	StatsContainer *itemStats = GenStatsReader::getContainer(allItemStats, data.c_str());
	if (itemStats != 0) {
		modName->setText(itemStats->getArg(0).c_str());
		std::map<std::string, std::string> baseData = itemStats->getBaseDataMap();
		if (itemStats->getUsing() != 0) {
			std::map<std::string, std::string> parentData = itemStats->getUsing()->getBaseDataMap();
			for (std::map<std::string, std::string>::iterator it = parentData.begin(); it != parentData.end(); ++it) {
				if (baseData.find(it->first) == baseData.end()) {
					baseData[it->first] = it->second;
				}
			}
		}
		for (int i=0; i<modTable->rowCount(); ++i) {
			for (int j=0; j<modTable->columnCount(); ++j) {
				delete modTable->item(i, j);
			}
		}
		modTable->setRowCount(0);
		int row = 0;
		if (itemStats->getContainerType() == "deltamod") {
			StatsContainer *boost = GenStatsReader::getContainer(allItemStats, itemStats->getBoostName());
			if (boost != 0) {
				std::map<std::string, std::string> boostMap = boost->getBaseDataMap();
				for (std::map<std::string, std::string>::iterator it = boostMap.begin(); it != boostMap.end(); ++it) {
					if (baseData.find(it->first) == baseData.end()) {
						baseData[it->first] = it->second;
					}
				}
			}
			for (int i=0; i<itemStats->getPrefixList().size(); ++i) {
				QTableWidgetItem *nameItem = new QTableWidgetItem();
				nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
				nameItem->setText("Prefix");
				QTableWidgetItem *valueItem = new QTableWidgetItem();
				valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
				valueItem->setText(itemStats->getPrefixList()[i].c_str());
				modTable->insertRow(row);
				modTable->setItem(row, 0, nameItem);
				modTable->setItem(row, 1, valueItem);
				++row;
			}
			for (int i=0; i<itemStats->getSuffixList().size(); ++i) {
				QTableWidgetItem *nameItem = new QTableWidgetItem();
				nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
				nameItem->setText("Suffix");
				QTableWidgetItem *valueItem = new QTableWidgetItem();
				valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
				valueItem->setText(itemStats->getSuffixList()[i].c_str());
				modTable->insertRow(row);
				modTable->setItem(row, 0, nameItem);
				modTable->setItem(row, 1, valueItem);
				++row;
			}
		}
		for (std::map<std::string, std::string>::iterator it = baseData.begin(); it != baseData.end(); ++it) {
			QTableWidgetItem *nameItem = new QTableWidgetItem();
			nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
			nameItem->setText(it->first.c_str());
			QTableWidgetItem *valueItem = new QTableWidgetItem();
			valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
			valueItem->setText(it->second.c_str());
			modTable->insertRow(row);
			modTable->setItem(row, 0, nameItem);
			modTable->setItem(row, 1, valueItem);
			++row;
		}
		
		modTable->resizeRowsToContents();
		modTable->resizeColumnsToContents();
	} else if (current->childCount() == 0) {
		QStringList headerList;
		headerList.push_back("Name");
		headerList.push_back("Editable Value");
		modTable->setHorizontalHeaderLabels(headerList);
		for (int i=0; i<modTable->rowCount(); ++i) {
			for (int j=0; j<modTable->columnCount(); ++j) {
				delete modTable->item(i, j);
			}
		}
		modTable->setRowCount(0);
		int row = 0;
		modTable->insertRow(row);
		QTableWidgetItem *nameItem = new QTableWidgetItem();
		nameItem->setText(current->text(0));
		nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
		modTable->setItem(row, 0, nameItem);
		QTableWidgetItem *valueItem = new QTableWidgetItem();
		valueItem->setText(data.c_str());
		modTable->setItem(row, 1, valueItem);
		
		modTable->resizeRowsToContents();
		modTable->resizeColumnsToContents();
	}
	modTable->blockSignals(false);
}

void StatsView::on_selectButton_released()
{
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	if (statsTree->currentItem() != 0 && statsTree->currentItem()->childCount() == 0) {
		if (selectedItem == 0) {
			DataContainerTreeItem *item = (DataContainerTreeItem *)statsTree->currentItem();
			selectCallback->onClick(item, statsTree);
		}
		else {
			selectCallback->onClick(selectedItem, statsTree);
		}
	}
}

void StatsView::on_addButton_released()
{
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
    addCallback->onClick(selectedItem, statsTree);
}

void StatsView::on_cancelButton_released()
{
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
    cancelCallback->onClick(selectedItem, statsTree);
}

void StatsView::on_removeButton_released()
{
	QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	DataContainerTreeItem *item = (DataContainerTreeItem *)statsTree->currentItem();
	if (item != 0 && item->childCount() == 0) {
		removeCallback->onClick(item, statsTree);
	}
}


void StatsView::on_statsTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (selectCallback != 0) {
		on_selectButton_released();
	}
}

void StatsView::on_modTable_itemChanged(QTableWidgetItem *tableItem)
{
    QTreeWidget *statsTree = this->findChild<QTreeWidget *>("statsTree");
	DataContainerTreeItem *treeItem = (DataContainerTreeItem *)statsTree->currentItem();
	if ((tableItem->flags() & Qt::ItemIsEditable) != 0) {
		treeItem->setData(tableItem->text().toStdString());
		
		DataContainerTreeItem *item = (DataContainerTreeItem *)statsTree->currentItem();
		
		QTableWidget *modTable = this->findChild<QTableWidget *>("modTable");
		if (tableEditCallback != 0) {
			tableEditCallback->onEdit(item, statsTree, tableItem, modTable);
		}
	}
}
