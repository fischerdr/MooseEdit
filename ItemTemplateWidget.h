#ifndef ITEMTEMPLATEWIDGET_H
#define ITEMTEMPLATEWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include "GameItem.h"
#include "LsbObject.h"
#include "LsbReader.h"
#include "GamePakData.h"

namespace Ui {
class ItemTemplateWidget;
}

class TemplateEditCallback {
public:
	virtual void onTemplateEdit(std::string& newTemplate) = 0;
};

class TemplateTreeWidgetItem : public QTreeWidgetItem {
	ItemTemplateData &templateData;
	
public:
	TemplateTreeWidgetItem(ItemTemplateData &templateData, QTreeWidgetItem *parent = 0) : templateData(templateData), QTreeWidgetItem(parent) {
		;
	}
	ItemTemplateData &getTemplateData() const {
		return templateData;
	}
	void setTemplateData(const ItemTemplateData &value) {
		this->templateData = templateData;
	}
};

class ItemTemplateWidget : public QWidget
{
	Q_OBJECT
	
public:
	void populate();
	void registerCallback(TemplateEditCallback *templateEditCallback);
	explicit ItemTemplateWidget(GameItem *item, StatTemplateMap &statToTemplateMap, QWidget *parent = 0);
	~ItemTemplateWidget();
	
	GameItem *getItem() const;
	void setItem(GameItem *value);
	void showEvent(QShowEvent *);
	
private slots:
	void on_templateIdEdit_textEdited(const QString &text);
	
	void on_templateTree_itemClicked(QTreeWidgetItem *item, int column);
	
	void on_templateTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	
	void on_selectButton_released();
	
	void on_templateTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
	
private:
	QTreeWidgetItem *selectedItem = 0;
	void selectItem(QTreeWidgetItem *item);
	void displayItemsForObject(QTreeWidgetItem *node, LsbObject *object);
	StatTemplateMap &statToTemplateMap;
	Ui::ItemTemplateWidget *ui;
	TemplateEditCallback *templateEditCallback = 0;
	void updateItemTemplate(std::string& newTemplate);
	void addToTree(ItemTemplateData &templateData, QTreeWidget *templateTree);
	QTreeWidgetItem *createFolderIfNeeded(QTreeWidgetItem *parent, const char *folderName);
	GameItem *item = 0;
};

#endif // ITEMTEMPLATEWIDGET_H
