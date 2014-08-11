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
#include "GamePakData.h"

#define PRG_VERSION	"alpha12"

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
	
	void on_devSaveFileButton_released();
	
private:
	float aspect;
	QTreeWidgetItem *findInTreeHelper(QTreeWidgetItem *item, QString text, int column, QTreeWidgetItem *position, bool& valid, QTreeWidgetItem *& firstItem);
	void recursiveExpandAll(QTreeWidgetItem *item);
	std::vector<TAG_LSB *> globalTagList;
	std::vector<LsbObject *> globals;
	Ui::MainWindow *ui;
	std::vector<LsbObject *> playerProfiles;
	CharacterGroup characters;
	InventoryHandler *editItemHandler;
	std::string getSteamPathFromRegistry();
	std::string getSaveLocation();
	std::string getGameDataLocation();
	PakReader userPakReader;
	std::string userPakFileName;
	GamePakData gamePakData;
	std::vector<TAG_LSB *> openFileButtonTagList;
};

#endif // MAINWINDOW_H
