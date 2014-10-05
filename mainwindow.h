#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include "LsbReader.h"
#include "LsbWriter.h"
#include "LsbObject.h"
#include "CharacterGroup.h"
#include "ItemGroup.h"
#include "TextureAtlas.h"
#include "GenStatsReader.h"
#include "PakReader.h"
#include "GamePakData.h"
#include <QProgressDialog>
#include <QTimer>
#include "EditorSettings.h"
#include "CharacterLoader.h"

#define PRG_VERSION	"alpha29"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, ExtractQueueCallback, ReaderProgressCallback, WriterProgressCallback
{
	Q_OBJECT
	
public:
	explicit MainWindow(std::wstring argument, QWidget *parent = 0);
	~MainWindow();
	std::vector<LsbObject *>& getGlobals() {
		return globals;
	}
	
	QTreeWidgetItem *findInTree(QTreeWidgetItem *item, QString text, int column, QTreeWidgetItem *position, bool wrapAround);
	void resizeEvent(QResizeEvent* event);
	void onExtractBegin(std::queue<GameDataQueueObject>& extractQueue);
	void onExtractUpdate(std::queue<GameDataQueueObject>& extractQueue);
	void onExtractEnd();
	void onLoadBegin(int dirCount);
	void onLoadUpdate(int dirsLeft);
	void onLoadEnd();
	void onSaveBegin(int dirCount);
	void onSaveUpdate(int dirsLeft);
	void onSaveEnd();
	void closeEvent(QCloseEvent *);
	
private slots:
	void refreshCurrentCharacterTab();
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
	
	void on_unloadButton_released();
	
private:
	CharacterLoader *characterLoader = 0;
	QTimer characterTabRefreshTimer;
	EditorSettings settings;
	long initialLsbSaveCount;
	QProgressDialog *lsbSaveProgress = 0;
	long initialLsbLoadCount;
	QProgressDialog *lsbLoadProgress = 0;
	long initialGameDataCount;
	QProgressDialog *gameDataProgressDialog = 0;
	float aspect;
	QTreeWidgetItem *findInTreeHelper(QTreeWidgetItem *item, QString text, int column, QTreeWidgetItem *position, bool& valid, QTreeWidgetItem *& firstItem);
	void recursiveExpandAll(QTreeWidgetItem *item);
	void unload();
	bool lsbOpenFileToTree(std::wstring &resultPath);
	std::vector<TAG_LSB *> globalTagList;
	std::vector<LsbObject *> globals;
	Ui::MainWindow *ui;
	std::vector<LsbObject *> playerProfiles;
	InventoryHandler *editItemHandler = 0;
	std::wstring getSteamPathFromRegistry();
	std::wstring getSaveLocation();
	std::wstring getGameDataLocation();
	PakReader userPakReader;
	std::wstring userPakFileName;
	bool openPakFileToList(std::wstring &fileName);
	GamePakData *gamePakData = 0;
	std::vector<TAG_LSB *> openFileButtonTagList;
	
	long randSeed;
	long randSize;
	std::vector<short> randTable;
	void populateRandTable(long randSeed);
};

#endif // MAINWINDOW_H
