#ifndef STATSVIEW_H
#define STATSVIEW_H

#include <QWidget>
#include <vector>
#include "GenStatsReader.h"
#include "LsbObject.h"
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <string>

class DataContainerTreeItem : public QTreeWidgetItem {
	std::string data;
public:
	DataContainerTreeItem(QTreeWidgetItem *parent) : QTreeWidgetItem(parent) {
		
	}
	
	DataContainerTreeItem(QTreeWidget *parent = 0) : QTreeWidgetItem(parent) {
		
	}
	
	std::string getData() const {
		return data;
	}
	
	void setData(const std::string &value) {
		data = value;
	}
};

class StatsButtonCallback {
public:
	virtual void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) = 0;
};

class TableEditCallback {
public:
	virtual void onEdit(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree, QTableWidgetItem *editedItem, QTableWidget *table) = 0;
};

namespace Ui {
class StatsView;
}

class StatsView : public QWidget
{
	Q_OBJECT
	
public:
	explicit StatsView(std::vector<StatsContainer *> &allItemStats, std::map<std::string, std::string> &nameMappings, QWidget *parent = 0);
	void addStatsDirectory(std::string name, std::string text, QTreeWidgetItem *parent = 0);
	void addBoostDirectory(std::string modifierType, std::string text);
	void addToTree(std::vector<StatsContainer *>& boosts, QTreeWidgetItem *parent = 0);
	void addToTree(std::vector<LsbObject *>& objects, QTreeWidgetItem *parent = 0);
	void addIndividualDataItemToTree(std::string dataName, QTreeWidgetItem *parent);
	void addDataToTree(StatsContainer *stat, QTreeWidgetItem *parent);
	void selectNodeByName(const char *name);
	void enableSelectButton(StatsButtonCallback *callback);
	void enableAddButton(StatsButtonCallback *callback);
	void enableCancelButton(StatsButtonCallback *callback);
	void enableRemoveButton(StatsButtonCallback *callback);
	void registerTableEditCallback(TableEditCallback *tableEditCallback);
	DataContainerTreeItem *addFolder(std::string folderName);
	void resizeTree();
	void clearTree();
	~StatsView();
	
protected:
	void showEvent(QShowEvent *event);
	
private slots:
	void on_statsTree_itemClicked(QTreeWidgetItem *item, int column);
	void on_statsTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void on_selectButton_released();
	
	void on_addButton_released();
	
	void on_statsTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
	
	void on_cancelButton_released();
	
	void on_removeButton_released();
	
	void on_modTable_itemChanged(QTableWidgetItem *tableItem);
	
private:
	DataContainerTreeItem *findNodeByData(QTreeWidgetItem *node, std::string &data);
	Ui::StatsView *ui;
	std::vector<StatsContainer *> &allItemStats;
	DataContainerTreeItem *selectedItem = 0;
	StatsButtonCallback *addCallback = 0;
	StatsButtonCallback *selectCallback = 0;
	StatsButtonCallback *cancelCallback = 0;
	StatsButtonCallback *removeCallback = 0;
	TableEditCallback *tableEditCallback = 0;
	std::map<std::string, std::string> &nameMappings;
};

#endif // STATSVIEW_H
