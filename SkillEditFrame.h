#ifndef SKILLEDITFRAME_H
#define SKILLEDITFRAME_H

#include <QFrame>
#include <QTreeWidget>
#include <QListWidget>
#include "GenStatsReader.h"
#include "GameCharacter.h"

namespace Ui {
class SkillEditFrame;
}

class DataContainerItem {
	std::string data = "";
public:
	std::string getStringData() const {
		return this->data;
	}
	void setStringData(const std::string &data) {
		this->data = data;
	}
};

class DataTreeItem : public DataContainerItem, public QTreeWidgetItem {
public:
	DataTreeItem(QTreeWidgetItem *parent = 0) : QTreeWidgetItem(parent) {}
};

class DataListItem : public DataContainerItem, public QListWidgetItem {
public:
	DataListItem(QListWidget *parent = 0) : QListWidgetItem(parent) {}
};

class SkillEditFrame : public QFrame
{
	Q_OBJECT
	
public:
	explicit SkillEditFrame(GameCharacter *character, std::vector<StatsContainer *> *skillStats, QWidget *parent = 0);
	~SkillEditFrame();
	
private slots:
	void on_skillTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	
	void on_skillList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	
	void on_addButton_released();
	
	void on_removeButton_released();
	
	void on_skillList_itemClicked(QListWidgetItem *current);
	
private:
	Ui::SkillEditFrame *ui;
	GameCharacter *character;
	std::vector<StatsContainer *> *skillStats;
	void populateSkillList();
	void populateSkillTree();
	void onCurrentItemChanged(DataContainerItem *current);
	DataTreeItem *createFolderIfNeeded(QTreeWidgetItem *parent, const char *folderName, const char *data);
};

#endif // SKILLEDITFRAME_H
