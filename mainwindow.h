#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include "LsbReader.h"
#include "LsbObject.h"
#include "CharacterGroup.h"
#include "ItemGroup.h"
#include "TextureAtlas.h"
#include "GenStatsReader.h"
#include "PakReader.h"

#define PRG_VERSION	"alpha8"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	std::vector<LsbObject *>& getGlobals() {
		return globals;
	}
	CharacterGroup& getCharacterGroup() {
		return characters;
	}
	void addRootTemplates(std::vector<LsbObject *>& rootTemplates) {
		for (int i=0; i<rootTemplates.size(); ++i) {
			this->rootTemplates.push_back(rootTemplates[i]);
		}
	}
	void addModTemplates(std::vector<LsbObject *>& modTemplates) {
		for (int i=0; i<modTemplates.size(); ++i) {
			this->modTemplates.push_back(modTemplates[i]);
		}
	}
	void addItemStats(std::vector<StatsContainer *>& itemStats) {
		for (int i=0; i<itemStats.size(); ++i) {
			this->itemStats.push_back(itemStats[i]);
		}
	}
	void addItemLinks(std::vector<StatsContainer *>& itemLinks) {
		for (int i=0; i<itemLinks.size(); ++i) {
			this->itemLinks.push_back(itemLinks[i]);
		}
	}
	QTreeWidgetItem *findInTree(QTreeWidgetItem *item, QString text, int column, QTreeWidgetItem *position, bool wrapAround);
	void resizeEvent(QResizeEvent* event);
	
private slots:
	void handleOpenFileButton();
	void handleLoadButton();
	void handleExpandCollapseButton();
	void on_loadFileWidget_customContextMenuRequested(const QPoint &pos);
	void on_loadFileWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	
	void on_treeWidget_customContextMenuRequested(const QPoint &pos);
	
	void on_saveAction_triggered();
	
	void treeFindAction();
	
	void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);
	
	void on_actionE_xit_triggered();
	
	void on_savesFolderButton_released();
	
	void on_gameDataButton_released();
	
	void on_savesFolderEdit_textChanged(const QString &arg1);
	
	void on_pakOpenButton_released();
	
	void on_pakListWidget_customContextMenuRequested(const QPoint &pos);
	
private:
	float aspect;
	QTreeWidgetItem *findInTreeHelper(QTreeWidgetItem *item, QString text, int column, QTreeWidgetItem *position, bool& valid, QTreeWidgetItem *& firstItem);
	std::vector<TAG_LSB *> globalTagList;
	std::vector<LsbObject *> globals;
	Ui::MainWindow *ui;
	std::vector<LsbObject *> playerProfiles;
	CharacterGroup characters;
	std::vector<LsbObject *> stats;
	std::vector<LsbObject *> rootTemplates;
	std::vector<LsbObject *> modTemplates;
	std::vector<StatsContainer *> itemStats;
	std::vector<StatsContainer *> itemLinks;
	TextureAtlas iconAtlas;
	InventoryHandler *editItemHandler;
	std::map<std::string, std::string> nameMappings;
	void buildNameMappings();
	std::string getSteamPathFromRegistry();
	std::string getSaveLocation();
	std::string getGameDataLocation();
	PakReader userPakReader;
	std::string userPakFileName;
};

#endif // MAINWINDOW_H
