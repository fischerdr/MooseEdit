#include "ItemTemplateWidget.h"
#include "ui_ItemTemplateWidget.h"
#include <QTreeWidgetItem>
#include <QRgb>

#define NAME_RGB_R	63
#define NAME_RGB_G	72
#define NAME_RGB_B	204

ItemTemplateWidget::ItemTemplateWidget(GameItem *item, StatTemplateMap &statToTemplateMap, QWidget *parent) :
	QWidget(parent), statToTemplateMap(statToTemplateMap), item(item),
	ui(new Ui::ItemTemplateWidget)
{
	ui->setupUi(this);
}

ItemTemplateWidget::~ItemTemplateWidget()
{
	QTreeWidget *templateTree = this->findChild<QTreeWidget *>("templateTree");
	QTreeWidget *templateInfoTree = this->findChild<QTreeWidget *>("templateInfoTree");
	for (int i=0; i<templateTree->topLevelItemCount(); ++i) {
		delete templateTree->topLevelItem(i);
		--i;
	}
	for (int i=0; i<templateInfoTree->topLevelItemCount(); ++i) {
		delete templateInfoTree->topLevelItem(i);
		--i;
	}
	delete ui;
}

void ItemTemplateWidget::registerCallback(TemplateEditCallback *templateEditCallback) {
	this->templateEditCallback = templateEditCallback;
}

void ItemTemplateWidget::updateItemTemplate(std::string& newTemplate) {
	QLineEdit *templateIdEdit = this->findChild<QLineEdit *>("templateIdEdit");
	templateIdEdit->setText(newTemplate.c_str());
	if (this->templateEditCallback != 0) {
		templateEditCallback->onTemplateEdit(newTemplate);
	}
}

void ItemTemplateWidget::displayItemsForObject(QTreeWidgetItem *node, LsbObject *object) {
	for (int i=0; i<object->getChildren().size(); ++i) {
		LsbObject *child = object->getChildren()[i];
		
		QTreeWidgetItem *treeItem = new QTreeWidgetItem();
		std::string output = child->getName();
		treeItem->setFlags(treeItem->flags() | Qt::ItemIsEditable);
		if (output == "DisplayName") {
			treeItem->setTextColor(0, QColor::fromRgb(NAME_RGB_R, NAME_RGB_G, NAME_RGB_B));
			treeItem->setTextColor(1, QColor::fromRgb(NAME_RGB_R, NAME_RGB_G, NAME_RGB_B));
		}
		treeItem->setText(0, QString(output.c_str()));
		node->addChild(treeItem);
		if (!child->isDirectory()) {
			output = child->toString();
			treeItem->setText(1, QString(output.c_str()));
		}
		else {
			displayItemsForObject(treeItem, child);
		}
	}
}

QTreeWidgetItem *ItemTemplateWidget::createFolderIfNeeded(QTreeWidgetItem *parent, const char *folderName) {
	for (int i=0; i<parent->childCount(); ++i) {
		QTreeWidgetItem *item = (QTreeWidgetItem *)parent->child(i);
		if (item->text(0) == folderName) {
			return item;
		}
	}
	QTreeWidgetItem *item = new QTreeWidgetItem(parent);
	item->setText(0, folderName);
	return item;
}
GameItem *ItemTemplateWidget::getItem() const
{
	return item;
}

void ItemTemplateWidget::setItem(GameItem *value)
{
	item = value;
}

void ItemTemplateWidget::showEvent(QShowEvent *)
{
	QTreeWidget *templateTree = this->findChild<QTreeWidget *>("templateTree");
	templateTree->setFocus();
}


void ItemTemplateWidget::addToTree(ItemTemplateData &templateData, QTreeWidget *templateTree) {
	TemplateTreeWidgetItem *treeItem = 0;
	if (templateData.isRootTemplate) {
		QTreeWidgetItem *rootFolder = createFolderIfNeeded(templateTree->invisibleRootItem(), "Root Templates");
		treeItem = new TemplateTreeWidgetItem(templateData, rootFolder);
	} else {
		QTreeWidgetItem *modFolder = createFolderIfNeeded(templateTree->invisibleRootItem(), "Mod Templates");
		treeItem = new TemplateTreeWidgetItem(templateData, modFolder);
	}
	std::string outputText = templateData.templateId;
	LsbObject *gameObject = templateData.gameObject;
	if (gameObject != 0) {
		LsbObject *displayNameObject = gameObject->lookupByUniquePath("DisplayName");
		if (displayNameObject != 0) {
			outputText = displayNameObject->getData();
			outputText += " (";
			outputText += templateData.templateId;
			outputText += ")";
			treeItem->setTextColor(0, QColor::fromRgb(NAME_RGB_R, NAME_RGB_G, NAME_RGB_B));
		}
	}
	treeItem->setText(0, outputText.c_str());
}

void ItemTemplateWidget::populate() {
	QLineEdit *templateIdEdit = this->findChild<QLineEdit *>("templateIdEdit");
	QLabel *statNameLabel = this->findChild<QLabel *>("statNameLabel");
	QTreeWidget *templateTree = this->findChild<QTreeWidget *>("templateTree");
	QTreeWidget *templateInfoTree = this->findChild<QTreeWidget *>("templateInfoTree");
	
	selectedItem = 0;
	
	for (int i=0; i<templateTree->topLevelItemCount(); ++i) {
		delete templateTree->topLevelItem(i);
		--i;
	}
	for (int i=0; i<templateInfoTree->topLevelItemCount(); ++i) {
		delete templateInfoTree->topLevelItem(i);
		--i;
	}
	
	if (item != 0) {
		LsbObject *itemObject = item->getObject();
		if (itemObject != 0) {
			LsbObject *currentTemplateObject = itemObject->lookupByUniquePath("CurrentTemplate");
			std::string templateId = "";
			if (currentTemplateObject != 0) {
				templateId = currentTemplateObject->getData();
				templateIdEdit->setText(templateId.c_str());
			}
			LsbObject *statsObject = itemObject->lookupByUniquePath("Stats");
			if (statsObject != 0) {
				std::string statId = statsObject->getData();
				statNameLabel->setText(statsObject->getData());
				if (statToTemplateMap.find(statId) != statToTemplateMap.end()) {
					std::vector<ItemTemplateData> &templateDataList = statToTemplateMap[statId];
					for (int i=0; i<templateDataList.size(); ++i) {
						ItemTemplateData &templateData = templateDataList[i];
						addToTree(templateData, templateTree);
					}
				}
			}
			if (templateId.length() > 0) {
				QList<QTreeWidgetItem *> matches = templateTree->findItems(templateId.c_str(), Qt::MatchContains | Qt::MatchRecursive);
				if (matches.size() == 1) {
					QTreeWidgetItem *item = matches[0];
					templateTree->setCurrentItem(item);
					item->setTextColor(0, QColor::fromRgb(255, 0, 0));
					selectedItem = item;
				}
			}
		}
	}
	templateTree->resizeColumnToContents(0);
}

void ItemTemplateWidget::on_templateIdEdit_textEdited(const QString &text) {
	std::string itemTemplate = text.toStdString();
    updateItemTemplate(itemTemplate);
}

void ItemTemplateWidget::on_templateTree_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item->childCount() != 0) {
		item->setExpanded(!item->isExpanded());
	}
}

void ItemTemplateWidget::on_templateTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current == 0) {
		return;
	}
	QTreeWidget *templateInfoTree = this->findChild<QTreeWidget *>("templateInfoTree");
	if (current->childCount() == 0) {
		for (int i=0; i<templateInfoTree->topLevelItemCount(); ++i) {
			delete templateInfoTree->topLevelItem(i);
			--i;
		}
		
		TemplateTreeWidgetItem *item = (TemplateTreeWidgetItem *)current;
		displayItemsForObject(templateInfoTree->invisibleRootItem(), item->getTemplateData().gameObject);
		templateInfoTree->resizeColumnToContents(0);
		templateInfoTree->resizeColumnToContents(1);
	}
}

void ItemTemplateWidget::selectItem(QTreeWidgetItem *item) {
	QTreeWidget *templateTree = this->findChild<QTreeWidget *>("templateTree");
	if (item != 0 && item->childCount() == 0) {
		TemplateTreeWidgetItem *templateItem = (TemplateTreeWidgetItem *)item;
		if (selectedItem != 0) {
			selectedItem->setTextColor(0, QColor::fromRgb(0, 0, 0));
		}
		templateItem->setTextColor(0, QColor::fromRgb(255, 0, 0));
		updateItemTemplate(templateItem->getTemplateData().templateId);
		selectedItem = templateItem;
		templateTree->setFocus();
	}
}

void ItemTemplateWidget::on_selectButton_released()
{
    QTreeWidget *templateTree = this->findChild<QTreeWidget *>("templateTree");
	selectItem(templateTree->currentItem());
}

void ItemTemplateWidget::on_templateTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	selectItem(item);
}
