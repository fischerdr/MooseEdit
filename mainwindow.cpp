#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "charactertab.h"
#include "InventoryHandler.h"
#include "LsbReader.h"
#include "LsbWriter.h"
#include "LsxReader.h"
#include "PakReader.h"
#include "TextureAtlas.h"
#include "finddialog.h"
#include "EquipmentHandler.h"
#include "PakWriter.h"
#include "SanityHash.h"
#include "SettingsDialog.h"
#include <windows.h>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <QListWidget>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <QLineEdit>
#include <unistd.h>
#include <QClipboard>
#include <QDir>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <tinyxml/tinyxml.h>
#include <boost/lexical_cast.hpp>
#include <QScrollBar>
#include <QShortcut>
#include <Shlobj.h>
#include <QResizeEvent>
#include <QDesktopWidget>
#include <QProgressDialog>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>

#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)

std::vector<std::wstring> getProfileNamesFromPath(std::wstring fullPath) {
	std::vector<std::wstring> profileNames;
	wchar_t cwd[MAX_PATH + 1];
	_wgetcwd(cwd, MAX_PATH + 1);
	if (_wchdir(fullPath.c_str()) == 0) {
		boost::filesystem::path dirPath(fullPath);
		boost::filesystem::directory_iterator it(dirPath);
		boost::filesystem::directory_iterator end;
		for (it; it != end; ++it) {
			boost::filesystem::path folder = (*it).path();
			if (boost::filesystem::is_directory(folder)) {
				std::wstring entName = folder.filename().wstring();
				if (entName != L"." && entName != L"..") {
					profileNames.push_back(entName);
				}
			}
		}
		_wchdir(cwd);
	}
	return profileNames;
}

std::vector<std::wstring> getSaveGameList(std::wstring fullPath, std::wstring profileName) {
	std::vector<std::wstring> saveList;
	fullPath += L"\\";
	fullPath += profileName;
	fullPath += L"\\";
	fullPath += L"SaveGames";
	wchar_t cwd[MAX_PATH + 1];
	_wgetcwd(cwd, MAX_PATH + 1);
	if (_wchdir(fullPath.c_str()) == 0) {
		boost::filesystem::path dirPath(fullPath);
		boost::filesystem::directory_iterator it(dirPath);
		boost::filesystem::directory_iterator end;
		for (it; it != end; ++it) {
			boost::filesystem::path folder = (*it).path();
			if (boost::filesystem::is_directory(folder)) {
				std::wstring entName = folder.filename().wstring();
				if (entName != L"." && entName != L"..") {
					std::wstring saveText = profileName;
					saveText += L"/";
					saveText += entName;
					saveList.push_back(saveText);
				}
			}
		}
		_wchdir(cwd);
	}
	return saveList;
}

MainWindow::MainWindow(std::wstring argument, QWidget *parent) :
	QMainWindow(parent), settings(L"moose_settings.ini"),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
	aspect = (float)this->width() / this->height();
	
	QDesktopWidget widget;
	QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());
	if (mainScreenSize.height() <= 800) {
		this->resize(this->width(), this->height() * 0.78f);
	}
	
	QPushButton *openFileButton = this->findChild<QPushButton *>("devOpenFileButton");
	this->connect(openFileButton, SIGNAL(released()), SLOT(handleOpenFileButton()));
	QPushButton *loadButton = this->findChild<QPushButton *>("loadButton");
	this->connect(loadButton, SIGNAL(released()), SLOT(handleLoadButton()));
	QPushButton *expandCollapseButton = this->findChild<QPushButton *>("devExpandCollapseButton");
	this->connect(expandCollapseButton, SIGNAL(released()), SLOT(handleExpandCollapseButton()));
	QListWidget *loadFileWidget = this->findChild<QListWidget *>("loadFileWidget");
	loadFileWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	QTreeWidget *treeWidget = this->findChild<QTreeWidget *>("treeWidget");
	treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	QShortcut *findShortcut = new QShortcut(QKeySequence::Find, treeWidget);
	findShortcut->setContext(Qt::ApplicationShortcut);
	treeWidget->connect(findShortcut, SIGNAL(activated()), this, SLOT(treeFindAction()));
	//this->connect(loadFileWidget, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(on_loadFileWidget_itemChanged(QListWidgetItem*)));
	QListWidget *pakListWidget = this->findChild<QListWidget *>("pakListWidget");
	pakListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	
	QStatusBar *statusBar = this->findChild<QStatusBar *>("statusBar");
	std::string versionString = "Version ";
	versionString += PRG_VERSION;
	statusBar->showMessage(QString(versionString.c_str()), 0);
	
	std::wstring path = getSteamPathFromRegistry();
	
	QLineEdit *gameDataEdit = this->findChild<QLineEdit *>("gameDataEdit");
	std::wstring savedDataPath = settings.getProperty(L"dataPath");
	if (savedDataPath.length() == 0) {
		if (path.length() > 0) {
			std::wstring gameDataPath = path;
			if (!boost::ends_with(gameDataPath, "\\")) {
				gameDataPath += L"\\";
			}
			gameDataPath += L"Data\\";
			gameDataEdit->setText(QString::fromStdWString(gameDataPath));
		}
	} else {
		gameDataEdit->setText(QString::fromStdWString(savedDataPath));
	}
	
	QLineEdit *savesFolderEdit = this->findChild<QLineEdit *>("savesFolderEdit");
	std::wstring savedSavePath = settings.getProperty(L"savePath");
	if (savedSavePath.length() == 0) {
		wchar_t my_documents[MAX_PATH + 1];
		HRESULT result = SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
		if (result == S_OK) {
			std::wstring saveFolderPath = my_documents;
			if (!boost::ends_with(saveFolderPath, L"\\")) {
				saveFolderPath += L"\\";
			}
			saveFolderPath += L"Larian Studios\\Divinity Original Sin\\PlayerProfiles\\";
			savesFolderEdit->setText(QString::fromStdWString(saveFolderPath));
		} else {
			std::vector<std::wstring> splitVector;
			boost::split(splitVector, path, boost::is_any_of(L"\\"));
			if (splitVector.size() > 0) {
				std::wstring &first = splitVector[0];
				if (first.length() != 0) {
					std::wstring saveFolderPath = first;
					saveFolderPath += L"\\Users\\";
					const char *username = std::getenv("USER");
					if (username == 0) {
						username = std::getenv("USERNAME");
					}
					if (username != 0) {
						std::wstring wUsername = L"";
						{
							long usernameSize = strlen(username) + 1;
							wchar_t alloc[usernameSize];
							mbstowcs(alloc, username, usernameSize);
							wUsername = alloc;
						}
						saveFolderPath += wUsername;
						saveFolderPath += L"\\Documents\\Larian Studios\\Divinity Original Sin\\PlayerProfiles\\";
						savesFolderEdit->setText(QString::fromStdWString(saveFolderPath));
					}
				}
			}
		}
	} else {
		savesFolderEdit->setText(QString::fromStdWString(savedSavePath));
	}
	
	characterTabRefreshTimer.setSingleShot(false);
	this->connect(&characterTabRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshCurrentCharacterTab()));
	characterTabRefreshTimer.start(200);
	
	if (argument.length() > 0) {
		QTabWidget *tabWidget = this->findChild<QTabWidget *>("tabWidget");
		if (boost::ends_with(argument, L".pak")) {
			if (openPakFileToList(argument)) {
				for (int i=0; i<tabWidget->count(); ++i) {
					if (boost::contains(tabWidget->tabText(i).toStdString(), "PAK")) {
						tabWidget->setCurrentIndex(i);
						break;
					}
				}
			}
		} else {
			if (lsbOpenFileToTree(argument)) {
				for (int i=0; i<tabWidget->count(); ++i) {
					if (boost::contains(tabWidget->tabText(i).toStdString(), "Dev")) {
						tabWidget->setCurrentIndex(i);
						break;
					}
				}
			}
		}
	}
	
	if (settings.getProperty(L"highCompressionMode").length() == 0) {
		settings.setProperty(L"highCompressionMode", L"1");
	}
	if (settings.getProperty(L"useCompression").length() == 0) {
		settings.setProperty(L"useCompression", L"1");
	}
	settings.saveFile(PRG_VERSION);
}

std::wstring MainWindow::getSteamPathFromRegistry() {
	std::wstring text = L"";
	#ifdef _WIN32
	HKEY hKey;
	unsigned long returnVal;
	if ((returnVal = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 230230",
				  0, KEY_READ, &hKey)) == ERROR_SUCCESS) {
		unsigned long dataSize = MAX_PATH + 1;
		wchar_t buf[dataSize];
		dataSize *= 2;
		if ((returnVal = RegQueryValueExW(hKey, L"InstallLocation", 0, 0, (LPBYTE) buf, &dataSize)) == ERROR_SUCCESS) {
			dataSize /= 2;
			buf[dataSize] = 0;
			text = buf;
		}
		else {
			//MessageBoxA(0, (boost::format("Failed to read registry key: %s") % returnVal).str().c_str(), 0, 0);
		}
		RegCloseKey(hKey);
	}
	else {
		//MessageBoxA(0, (boost::format("Failed to open registry key: %s") % returnVal).str().c_str(), 0, 0);
	}
	#endif
	return text;
}

class EditableTreeWidgetItem : public QTreeWidgetItem
{
public:
	LsbObject *object = 0;
	EditableTreeWidgetItem() {
		
	}
};

void displayItemsForObject2(QTreeWidgetItem *node, LsbObject *object, int level) {
	for (int i=0; i<object->getChildren().size(); ++i) {
		LsbObject *child = object->getChildren()[i];
		
		EditableTreeWidgetItem *treeItem = new EditableTreeWidgetItem();
		std::string output = child->getName();
		treeItem->setFlags(treeItem->flags() | Qt::ItemIsEditable);
		treeItem->object = child;
		treeItem->setText(0, QString(output.c_str()));
		node->addChild(treeItem);
		if (!child->isDirectory()) {
			output = child->toString();
			treeItem->setText(1, QString(output.c_str()));
		}
		else {
			displayItemsForObject2(treeItem, child, level + 1);
		}
	}
}

void displayAllItems2(QTreeWidget *tree, std::vector<LsbObject *>& objects) {
	tree->blockSignals(true);
	tree->setUpdatesEnabled(false);
	for (int i=0; i<objects.size(); ++i) {
		LsbObject *object = objects[i];
		EditableTreeWidgetItem *treeItem = new EditableTreeWidgetItem();
		treeItem->setFlags(treeItem->flags() | Qt::ItemIsEditable);
		treeItem->object = object;
		treeItem->setText(0, QString(object->getName().c_str()));
		tree->addTopLevelItem(treeItem);
		displayItemsForObject2(treeItem, object, 0);
	}
	tree->expandAll();
	for (int i=0; i<tree->columnCount(); ++i) {
		tree->resizeColumnToContents(i);
	}
	tree->collapseAll();
	tree->setUpdatesEnabled(true);
	tree->blockSignals(false);
}

bool expanded = false;
void MainWindow::handleExpandCollapseButton() {
	QTreeWidget *tree = this->findChild<QTreeWidget *>("treeWidget");
	if (!expanded) {
		tree->expandAll();
	}
	else {
		tree->collapseAll();
	}
	expanded = !expanded;
}

void MainWindow::unload() {
	if (editItemHandler != 0) {
		delete editItemHandler;
		editItemHandler = 0;
	}
	if (gamePakData != 0) {
		delete gamePakData;
		gamePakData = 0;
	}
	if (characterLoader != 0) {
		for (int i=0; i<characterLoader->getCharacterGroup().getCharacters().size(); ++i) {
			delete characterLoader->getCharacterGroup().getCharacters()[i];
		}
		characterLoader->getCharacterGroup().getCharacters().clear();
		
		delete characterLoader;
		characterLoader = 0;
	}
	for (int i=0; i<globals.size(); ++i) {
		delete globals[i];
	}
	globals.clear();
	for (int i=0; i<globalTagList.size(); ++i) {
		delete globalTagList[i];
	}
	globalTagList.clear();
}

void MainWindow::populateRandTable(long randSeed) {
	randTable.clear();
	
	long seed = randSeed;
	for (int i=0; i<randSize; ++i) {
		seed = seed * 0x343FD + 0x269EC3;
		short value = (short)((seed >> 0x10) & 0x7FFF);
		randTable.push_back(value);
	}
}

void MainWindow::handleLoadButton() {
	unload();
	
	QPushButton *loadButton = this->findChild<QPushButton *>("loadButton");
	QPushButton *unloadButton = this->findChild<QPushButton *>("unloadButton");
	QListWidget *loadFileWidget = this->findChild<QListWidget *>("loadFileWidget");
	QListWidgetItem *item = loadFileWidget->currentItem();
	if (item != 0) {
		loadButton->setEnabled(false);
		unloadButton->setEnabled(true);
		
		settings.setProperty(L"savePath", this->getSaveLocation());
		settings.setProperty(L"dataPath", this->getGameDataLocation());
		settings.saveFile(PRG_VERSION);
		
		QString text = item->text();
		QLineEdit *currentlyLoadedEdit = this->findChild<QLineEdit *>("currentlyLoadedEdit");
		currentlyLoadedEdit->setText(text);
		
		std::wstringstream stream;
		stream<<this->getSaveLocation();
		std::wstring profilesPath = stream.str();
		std::wstring lsvPath = stream.str();
		std::wstring listText = text.toStdWString();
		std::vector<std::wstring> tokens;
		boost::split(tokens, listText, boost::is_any_of(L"/"));
		if (tokens.size() == 2) {
			profilesPath += tokens[0];
			profilesPath += L"\\";
			profilesPath += L"SaveGames\\";
			profilesPath += tokens[1];
			profilesPath += L"\\Globals.lsb";
			
			lsvPath += tokens[0];
			lsvPath += L"\\";
			lsvPath += L"SaveGames\\";
			lsvPath += tokens[1];
			lsvPath += L"\\";
			lsvPath += tokens[1];
			lsvPath += L".lsv";
			
			boost::filesystem::ifstream fin(profilesPath,
				std::ios_base::binary);
			if (fin) {
				LsbReader reader;
				reader.registerProgressCallback(this);
				globals = reader.loadFile(fin);
				globalTagList = reader.getTagList();
				fin.close();
			} else {
				PakReader lsvReader;
				bool loadedLsv = lsvReader.loadFile(lsvPath);
				if (loadedLsv) {
					std::string globalsPath = "globals.lsb";
					std::wstring destPath = L".";
					unsigned long fileSize;
					char *globalsAlloc = lsvReader.extractFileIntoMemory(lsvPath, globalsPath, destPath, true, &fileSize);
					if (globalsAlloc != 0) {
						LsbReader reader;
						reader.registerProgressCallback(this);
						std::istringstream ss;
						ss.rdbuf()->pubsetbuf(globalsAlloc, fileSize);
						globals = reader.loadFile(ss);
						globalTagList = reader.getTagList();
						delete[] globalsAlloc;
					} else {
						QMessageBox msgBox;
						msgBox.setText("Could not find globals.lsb inside LSV archive");
						msgBox.exec();
						return;
					}
				} else {
					QMessageBox msgBox;
					msgBox.setText("Could not locate save game file (missing LSB and LSV)");
					msgBox.exec();
					return;
				}
			}
			
			populateRandTable(randSeed);
			
			characterLoader = new CharacterLoader();
			characterLoader->load(globals, this->getGameDataLocation(), &globalTagList, this);
			
			//load pak resources for textures
			std::wstring gameDataPath = this->getGameDataLocation();
			std::wstring mainPak = L"Main.pak";
			boost::filesystem::path mainPakPath(gameDataPath);
			mainPakPath /= mainPak;
			if (!boost::filesystem::exists(mainPakPath)) {
				QMessageBox msgBox;
				msgBox.setText("Unable to locate Main.pak: check Game Data path");
				msgBox.exec();
			}
			gamePakData = new GamePakData();
			gamePakData->registerExtractQueueCallback(this);
			gamePakData->load(gameDataPath);
			
			if (gamePakData->getInventoryCellImg() != 0) {
				editItemHandler = new InventoryHandler(*gamePakData->getInventoryCellImg(), gamePakData->getStats(), gamePakData->getRootTemplates(), 
													   gamePakData->getModTemplates(), gamePakData->getIconAtlas(), gamePakData->getAbilityAtlas(), gamePakData->getItemStats(), gamePakData->getNameMappings(),
													   gamePakData->getRootTemplateMap(), gamePakData->getModTemplateMap(), randTable);
				for (int i=0; i<characterLoader->getCharacterGroup().getCharacters().size(); ++i) {
					GameCharacter *character = characterLoader->getCharacterGroup().getCharacters()[i];
					//if (i != 0)
						//continue;
					InventoryHandler *handlerPtr = new InventoryHandler(*gamePakData->getInventoryCellImg(), gamePakData->getStats(), gamePakData->getRootTemplates(), 
																		gamePakData->getModTemplates(), gamePakData->getIconAtlas(), gamePakData->getAbilityAtlas(), gamePakData->getItemStats(), gamePakData->getNameMappings(),
																		gamePakData->getRootTemplateMap(), gamePakData->getModTemplateMap(), randTable);
					InventoryHandler& inventoryHandler = *handlerPtr;
					for (int j=0; j<character->getInventory().getItems().size(); ++j) {
						inventoryHandler.getItems()->addItem(character->getInventory().getItems()[j]);
					}
					std::ostringstream ss;
					ss<<"charTab"<<i;
					//inventoryHandler.draw(this->findChild<QWidget *>(ss.str().c_str())->findChild<QWidget *>("inventoryContents"), this);
					
					character->setInventoryHandler(handlerPtr);
					
					characterTab *charTab = (characterTab *)character->getWidget();
					charTab->setRandTable(randTable);
					charTab->setNameMappings(&gamePakData->getNameMappings());
					charTab->setItemLinks(gamePakData->getItemLinks());
					charTab->setAllItemStats(gamePakData->getItemStats());
					charTab->setItemEditHandler(editItemHandler);
					charTab->setSkillStats(&gamePakData->getSkillStats());
					charTab->setStatToTemplateMap(&gamePakData->getStatToTemplateMap());
					
					QWidget *equipmentWidget = charTab->findChild<QWidget *>("equipmentWidget");
					LsbObject *itemsObject = LsbObject::lookupByUniquePath(globals, "Items/root/ItemFactory/Items");
					EquipmentHandler *equipHandler = 
							new EquipmentHandler(*gamePakData->getInventoryCellImg(), gamePakData->getStats(), gamePakData->getRootTemplates(), 
												gamePakData->getModTemplates(), gamePakData->getIconAtlas(), gamePakData->getAbilityAtlas(), gamePakData->getItemStats(), gamePakData->getNameMappings(),
												equipmentWidget, this, gamePakData->getItemLinks(), globalTagList, itemsObject, character,
												 gamePakData->getRootTemplateMap(), gamePakData->getModTemplateMap(), gamePakData->getStatToTemplateMap(), randTable);
					
					std::vector<GameItem *> &equipmentSet = characterLoader->getEquipmentSets()[i];
					for (int j=0; j<equipmentSet.size(); ++j) {
						GameItem *item = equipmentSet[j];
						equipHandler->addItem(item);
					}
					charTab->setEquipmentHandler(equipHandler);
					charTab->setGamePakData(gamePakData);
					charTab->updateToCurrentPortrait();
					
					QTreeWidget *tree = this->findChild<QTreeWidget *>("treeWidget");
					std::vector<LsbObject *> one;
					one.push_back(character->getObject());
					displayAllItems2(tree, one);
				}
			}
			else {
				MessageBoxA(0, "failed to load inventory image", 0, 0);
			}
		}
	}
}

bool MainWindow::lsbOpenFileToTree(std::wstring &resultPath) {
	boost::filesystem::ifstream fin(resultPath,
					  std::ios_base::binary);
	if (boost::ends_with(resultPath, L".lsx")) {
		LsxReader reader;
		std::vector<LsbObject *> directoryList = reader.loadFile(fin);
		openFileButtonTagList = reader.getTagList();
		fin.close();
		QTreeWidget *tree = this->findChild<QTreeWidget *>("treeWidget");
		tree->clear();
		if (directoryList.size() != 0) {
			displayAllItems2(tree, directoryList);
		}
		else {
			QMessageBox msgBox;
			msgBox.setText("Bad data");
			msgBox.exec();
			return false;
		}
	} else {
		LsbReader reader;
		std::vector<LsbObject *> directoryList = reader.loadFile(fin);
		openFileButtonTagList = reader.getTagList();
		fin.close();
		QTreeWidget *tree = this->findChild<QTreeWidget *>("treeWidget");
		tree->clear();
		if (directoryList.size() != 0) {
			displayAllItems2(tree, directoryList);
		}
		else {
			QMessageBox msgBox;
			msgBox.setText("Bad data");
			msgBox.exec();
			return false;
		}
	}
	return true;
}

void MainWindow::handleOpenFileButton() {
	std::wstringstream stream;
	stream<<this->getSaveLocation();
	QString result = QFileDialog::getOpenFileName(this,
												  QString("Open LSB"), QString::fromStdWString(stream.str()), QString("LSB Files (*.lsb);;LSX Files (*.lsx)"));
	if (result.size() != 0) {
		std::wstring resultPath = result.toStdWString();
		lsbOpenFileToTree(resultPath);
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_loadFileWidget_customContextMenuRequested(const QPoint &pos)
{
	QListWidget *loadFileWidget = this->findChild<QListWidget *>("loadFileWidget");
	QListWidgetItem *item;
	if ((item = loadFileWidget->itemAt(pos)) != 0) {
		QMenu contextMenu(tr("Context menu"), this);
		contextMenu.addAction("Open File &Location");
		QAction *result = contextMenu.exec(loadFileWidget->mapToGlobal(pos));
		if (result) {
			std::wstringstream stream;
			stream<<this->getSaveLocation();
			std::wstring listText = item->text().toStdWString();
			std::vector<std::wstring> tokens;
			boost::split(tokens, listText, boost::is_any_of(L"/"));
			if (tokens.size() == 2) {
				std::wstring profilesPath = stream.str();
				profilesPath += tokens[0];
				profilesPath += L"\\";
				profilesPath += L"SaveGames\\";
				profilesPath += tokens[1];
				ShellExecuteW(NULL, L"open", profilesPath.c_str(), NULL, NULL, SW_SHOW);
			}
		}
	}
}

void MainWindow::readMetaInputStream(std::istream& metaInputStream, std::wstring bitmapPath, std::wstring pngPath, std::vector<std::wstring>& tokens) {
	if (metaInputStream) {
		LsbReader reader;
		std::vector<LsbObject *> metaDataList = reader.loadFile(metaInputStream);
		LsbObject *metaData = LsbObject::lookupByUniquePath(metaDataList, "MetaData");
		LsbObject *saveTime = metaData->lookupByUniquePath("root/MetaData/SaveTime");
		LsbObject *module = metaData->lookupByUniquePath("root/MetaData/ModuleSettings/Mods/ModuleShortDesc");
		LsbObject *seedObject = metaData->lookupByUniquePath("root/MetaData/Seed");
		LsbObject *sizeObject = metaData->lookupByUniquePath("root/MetaData/Size");
		if (seedObject != 0 && sizeObject != 0) {
			randSeed = *((long *)seedObject->getData());
			randSize = *((long *)sizeObject->getData());
		}
		QLabel *previewPicture = this->findChild<QLabel *>("previewPicture");
		QImage img = QImage(QString::fromStdWString(bitmapPath));
		if (img.isNull()) {
			img = QImage(QString::fromStdWString(pngPath));
		}
		img = img.scaled(previewPicture->size());
		previewPicture->setPixmap(QPixmap::fromImage(img));
		QLineEdit *previewName = this->findChild<QLineEdit *>("previewName");
		QLineEdit *previewDate = this->findChild<QLineEdit *>("previewDate");
		long month = saveTime->lookupByUniquePath("Month")->getByteData();
		long day = saveTime->lookupByUniquePath("Day")->getByteData();
		long year = 1900 + saveTime->lookupByUniquePath("Year")->getByteData();
		std::ostringstream ssDate;
		ssDate<<month<<"/"<<day<<"/"<<year;
		std::string dateText = ssDate.str();
		QLineEdit *previewTime = this->findChild<QLineEdit *>("previewTime");
		std::ostringstream ssTime;
		long hours24 = saveTime->lookupByUniquePath("Hours")->getByteData();
		std::string midday = (hours24 >= 12 ? "PM" : "AM");
		long hours = hours24 % 12;
		if (hours == 0)
			hours = 12;
		long minutes = saveTime->lookupByUniquePath("Minutes")->getByteData();
		ssTime<<boost::format("%i:%02i %s") % hours % minutes % midday;
		std::string timeText = ssTime.str();
		QLineEdit *previewModule = this->findChild<QLineEdit *>("previewModule");
		std::string moduleText = module->lookupByUniquePath("Name")->getData();
		previewName->setText(QString::fromStdWString(tokens[1]));
		previewDate->setText(QString(dateText.c_str()));
		previewTime->setText(QString(timeText.c_str()));
		previewModule->setText(QString(moduleText.c_str()));
	}
}

void MainWindow::on_loadFileWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	QPushButton *loadButton = this->findChild<QPushButton *>("loadButton");
	loadButton->setEnabled(true);
	std::wstringstream stream;
	stream<<this->getSaveLocation();
	std::wstring profilesPath = stream.str();
	if (current != 0) {
		std::wstring listText = current->text().toStdWString();
		std::vector<std::wstring> tokens;
		boost::split(tokens, listText, boost::is_any_of(L"/"));
		if (tokens.size() == 2) {
			profilesPath += tokens[0];
			profilesPath += L"\\";
			profilesPath += L"SaveGames\\";
			profilesPath += tokens[1];
			profilesPath += L"\\";
			std::wstring bitmapPath = profilesPath;
			bitmapPath += tokens[1];
			bitmapPath += L".bmp";
			std::wstring pngPath = profilesPath;
			pngPath += tokens[1];
			pngPath += L".png";
			std::wstring configPath = profilesPath;
			configPath += tokens[1];
			configPath += L".lsb";
			std::wstring lsvPath = profilesPath;
			lsvPath += tokens[1];
			lsvPath += L".lsv";
			std::wstring lsvBakPath = profilesPath;
			lsvBakPath += tokens[1];
			lsvBakPath += L".lsv.bak";
			std::wstring metaPath = profilesPath;
			metaPath += L"meta.lsb";
			boost::filesystem::ifstream fin(configPath,
				std::ios_base::binary);
			if (!fin) {
				fin.open(metaPath,
						std::ios_base::binary);
			}
			if (fin) {
				readMetaInputStream(fin, bitmapPath, pngPath, tokens);
				fin.close();
			}
			else {
				PakReader reader;
			    bool lsvLoaded = reader.loadFile(lsvPath);
				if (lsvLoaded) {
					unsigned long fileSize;
					std::string metaPath = "meta.lsb";
					std::wstring fileDest = L".";
					char *metaAlloc = reader.extractFileIntoMemory(lsvPath, metaPath, fileDest, true, &fileSize);
					if (metaAlloc != 0) {
						std::istringstream fileByteStream;
						fileByteStream.rdbuf()->pubsetbuf(metaAlloc, fileSize);
						readMetaInputStream(fileByteStream, bitmapPath, pngPath, tokens);
						delete[] metaAlloc;
					} else {
						QMessageBox msg;
						msg.setText("Could not find meta information file");
						msg.exec();
					}
				}
			}
		}
	}
}

QTreeWidgetItem *MainWindow::findInTreeHelper(QTreeWidgetItem *item, QString text, int column, QTreeWidgetItem *position, bool& valid, QTreeWidgetItem *& firstItem) {
	if (position == 0) {
		valid = true;
	}
	QTreeWidgetItem *found = 0;
	long childCount = item->childCount();
	for (int i=0; i<childCount; ++i) {
		QTreeWidgetItem *child = item->child(i);
		if (child->text(column).contains(text)) {
			if (firstItem == 0) {
				firstItem = child;
			}
			if (valid) {
				found = child;
				break;
			}
		}
		if (child == position) {
			valid = true;
		}
		if (child->childCount() > 0) {
			QTreeWidgetItem *treeResult = findInTreeHelper(child, text, column, position, valid, firstItem);
			if (treeResult != 0) {
				return treeResult;
			}
		}
	}
	return found;
}

QTreeWidgetItem *MainWindow::findInTree(QTreeWidgetItem *item, QString text, int column, QTreeWidgetItem *position, bool wrapAround) {
	bool valid = false;
	QTreeWidgetItem *firstItem = 0;
	QTreeWidgetItem *findResult = findInTreeHelper(item, text, column, position, valid, firstItem);
	if (findResult == 0 && wrapAround) {
		return firstItem;
	}
	return findResult;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	if (event->oldSize().height() != event->size().height()) {
		int h = event->size().height();
		int w = h * aspect;
		this->setMinimumWidth(w);
		this->setMaximumWidth(w);
	}
}

void MainWindow::onExtractBegin(std::queue<GameDataQueueObject> &extractQueue)
{
	if (gameDataProgressDialog != 0) {
		delete gameDataProgressDialog;
	}
	initialGameDataCount = extractQueue.size();
	gameDataProgressDialog = new QProgressDialog("Loading game data...", QString(), 0, initialGameDataCount, this);
	gameDataProgressDialog->setWindowFlags(gameDataProgressDialog->windowFlags() & ~(Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint));
	gameDataProgressDialog->setWindowModality(Qt::WindowModal);
	gameDataProgressDialog->show();
	QApplication::processEvents();
}

void MainWindow::onExtractUpdate(std::queue<GameDataQueueObject> &extractQueue)
{
	gameDataProgressDialog->setValue(initialGameDataCount - extractQueue.size());
	QApplication::processEvents();
}

void MainWindow::onExtractEnd()
{
	
}

void MainWindow::onLoadBegin(int dirCount)
{
	if (lsbLoadProgress != 0) {
		delete lsbLoadProgress;
	}
	initialLsbLoadCount = dirCount;
	lsbLoadProgress = new QProgressDialog("Loading save data...", QString(), 0, initialLsbLoadCount, this);
	lsbLoadProgress->setWindowFlags(lsbLoadProgress->windowFlags() & ~(Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint));
	lsbLoadProgress->setWindowModality(Qt::WindowModal);
	lsbLoadProgress->show();
	QApplication::processEvents();
}

void MainWindow::onLoadUpdate(int dirsLeft)
{
	lsbLoadProgress->setValue(initialLsbLoadCount - dirsLeft);
	QApplication::processEvents();
}

void MainWindow::onLoadEnd()
{
	
}

void MainWindow::onSaveBegin(int dirCount)
{
	if (lsbSaveProgress != 0) {
		delete lsbSaveProgress;
	}
	initialLsbSaveCount = dirCount;
	lsbSaveProgress = new QProgressDialog("Saving game data...", QString(), 0, initialLsbSaveCount, this);
	lsbSaveProgress->setWindowFlags(lsbSaveProgress->windowFlags() & ~(Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint));
	lsbSaveProgress->setWindowModality(Qt::WindowModal);
	lsbSaveProgress->show();
	QApplication::processEvents();
}

void MainWindow::onSaveUpdate(int dirsLeft)
{
	lsbSaveProgress->setValue(initialLsbSaveCount - dirsLeft);
	QApplication::processEvents();
}

void MainWindow::onSaveEnd()
{
	
}

void MainWindow::closeEvent(QCloseEvent *)
{
	exit(0);
}

void MainWindow::refreshCurrentCharacterTab() {
	QTabWidget *tabWidget = this->findChild<QTabWidget *>("tabWidget");
	if (boost::starts_with(tabWidget->currentWidget()->objectName().toStdString(), "charTab")) {
		characterTab *charTab = ((characterTab *)tabWidget->currentWidget());
		charTab->refreshIconSizes();
	}
}

void MainWindow::treeFindAction() {
	long column = 1;
	QDialog *dialog = new FindDialog();
	QLineEdit *findEdit = dialog->findChild<QLineEdit *>("findEdit");
	QTreeWidget *treeWidget = this->findChild<QTreeWidget *>("treeWidget");
	QTreeWidgetItem *item = treeWidget->currentItem();
	findEdit->setText(item->text(column));
	int result = dialog->exec();
	if (result == 1) {
		QTreeWidgetItem *findResult = findInTree(treeWidget->invisibleRootItem(), findEdit->text(), 1, item, true);
		if (findResult != 0) {
			treeWidget->setCurrentItem(findResult);
		}
	}
}

void MainWindow::recursiveExpandAll(QTreeWidgetItem *item) {
	item->setExpanded(true);
	for (int i=0; i<item->childCount(); ++i) {
		QTreeWidgetItem *child = item->child(i);
		recursiveExpandAll(child);
	}
}

void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
	QTreeWidget *treeWidget = this->findChild<QTreeWidget *>("treeWidget");
	QTreeWidgetItem *item;
	if ((item = treeWidget->itemAt(pos)) != 0) {
		QMenu contextMenu(this);
		int column = treeWidget->columnAt(pos.x());
		if (column != 0) {
			contextMenu.addAction("&Edit");
		}
		contextMenu.addAction("&Copy Text");
		contextMenu.addAction("Copy &Path");
		contextMenu.addAction("Copy &Type");
		contextMenu.addAction("Copy &Child Number");
		contextMenu.addAction("&Expand All");
		QAction *findAction = contextMenu.addAction("&Find");
		findAction->setShortcut(QKeySequence::Find);
		QAction *result = contextMenu.exec(treeWidget->viewport()->mapToGlobal(pos));
		if (result) {
			if (result->text() == "&Edit") {
				treeWidget->editItem(item, 1);
			}
			if (result->text() == "&Copy Text") {
				QClipboard *clipboard = QApplication::clipboard();
				clipboard->setText(item->text(column));
			}
			if (result->text() == "Copy &Path") {
				QTreeWidgetItem *current = item;
				std::stack<std::string> stringStack;
				while (current != 0) {
					stringStack.push(current->text(0).toStdString());
					current = current->parent();
				}
				std::ostringstream ss;
				boolean first = true;
				while (!stringStack.empty()) {
					std::string& text = stringStack.top();
					if (!first) {
						ss<<"/";
					}
					first = false;
					ss<<text;
					stringStack.pop();
				}
				QClipboard *clipboard = QApplication::clipboard();
				clipboard->setText(QString(ss.str().c_str()));
			} 
			if (result->text() == "Copy &Type") {
				QClipboard *clipboard = QApplication::clipboard();
				EditableTreeWidgetItem *editable = (EditableTreeWidgetItem *)item;
				long type = editable->object->getType();
				clipboard->setText((boost::format("0x%02X") % type).str().c_str());
			}
			if (result->text() == "Copy &Child Number") {
				QClipboard *clipboard = QApplication::clipboard();
				EditableTreeWidgetItem *editable = (EditableTreeWidgetItem *)item;
				std::vector<LsbObject *> objects = editable->object->getParent()->getChildren();
				bool found = false;
				for (int i=0; i<objects.size(); ++i) {
					if (objects[i] == editable->object) {
						clipboard->setText((boost::format("%i") % i).str().c_str());
						found = true;
						break;
					}
				}
				if (!found)
					clipboard->setText("?");
			}
			if (result->text() == "&Expand All") {
				EditableTreeWidgetItem *editable = (EditableTreeWidgetItem *)item;
				recursiveExpandAll(editable);
			}
			if (result->text() == "&Find") {
				treeFindAction();
			}
		}
	}
}

void MainWindow::on_saveAction_triggered()
{
	std::wstringstream stream;
	stream<<this->getSaveLocation();
	std::wstring profilesPath = stream.str();
	std::wstring lsvPath = stream.str();
	QLineEdit *currentlyLoadedEdit = this->findChild<QLineEdit *>("currentlyLoadedEdit");
	std::wstring listText = currentlyLoadedEdit->text().toStdWString();
	std::vector<std::wstring> tokens;
	boost::split(tokens, listText, boost::is_any_of(L"/"));
	if (tokens.size() == 2) {
		profilesPath += tokens[0];
		profilesPath += L"\\";
		profilesPath += L"SaveGames\\";
		profilesPath += tokens[1];
		profilesPath += L"\\Globals.lsb";
	
		lsvPath += tokens[0];
		lsvPath += L"\\";
		lsvPath += L"SaveGames\\";
		lsvPath += tokens[1];
		lsvPath += L"\\";
		lsvPath += tokens[1];
		lsvPath += L".lsv";
		
		std::wstring bakPath = profilesPath;
		bakPath += L".bak";
		
		std::wstring lsvBakPath = lsvPath;
		lsvBakPath += L".bak";
		
		bool isLsb = false;
		
		//create LSB backup
		{
			boost::filesystem::ifstream src(profilesPath, std::ios::binary);
			if (src) {
				isLsb = true;
			}
			boost::filesystem::ifstream dstExists(bakPath, std::ios::binary);
			if (src && !dstExists) {
				boost::filesystem::ofstream dst(bakPath, std::ios::binary);
				dst << src.rdbuf();
				dst.close();
			}
			dstExists.close();
			src.close();
		}
		
		//create LSV backup
		{
			boost::filesystem::ifstream src(lsvPath, std::ios::binary);
			boost::filesystem::ifstream dstExists(lsvBakPath, std::ios::binary);
			if (src && !dstExists) {
				boost::filesystem::ofstream dst(lsvBakPath, std::ios::binary);
				dst << src.rdbuf();
				dst.close();
			}
			dstExists.close();
			src.close();
		}
		
		bool result = false;
		if (isLsb) {
			LsbWriter writer;
			writer.registerProgressCallback(this);
			boost::filesystem::ofstream fout(profilesPath, std::ios::binary);
			result = writer.writeFile(globals, globalTagList, fout);
			fout.close();
		} else {
			if (boost::filesystem::exists(lsvBakPath)) {
				PakReader reader;
				reader.loadFile(lsvBakPath);
				std::vector<std::string> fileList = reader.getFileList();
				
				std::vector<std::string> fileNames;
				std::vector<char *> fileData;
				std::vector<unsigned long> fileDataSize;
				std::wstring dest = L".";
				bool failure = false;
				if (fileList.size() == 0) {
					failure = true;
				}
				
				SanityHash sanity;
				
				LsbWriter lsbWriter;
				lsbWriter.registerProgressCallback(this);
				std::stringstream globalsStream;
				result = lsbWriter.writeFile(globals, globalTagList, globalsStream);
				fileNames.push_back("globals.lsb");
				const std::string& globalBytes = globalsStream.rdbuf()->str();
				char *alloc = new char[globalBytes.size()];
				memcpy(alloc, globalBytes.c_str(), globalBytes.size());
				globalsStream.str("");
				fileData.push_back(alloc);
				fileDataSize.push_back(globalBytes.size());
				sanity.addFile((unsigned char *)fileData.back(), fileDataSize.back());
				
				for (int i=0; i<fileList.size(); ++i) {
					std::string fileName = fileList[i];
					if (boost::algorithm::to_lower_copy(fileName) == "globals.lsb")
						continue;
					if (boost::algorithm::to_lower_copy(fileName) == "sanity.lsb")
						continue;
					unsigned long fileSize;
					fileNames.push_back(fileName);
					fileData.push_back(reader.extractFileIntoMemory(lsvBakPath, fileName, dest, false, &fileSize));
					if (fileData.back() == 0) {
						failure = true;
						break;
					}
					fileDataSize.push_back(fileSize);
					sanity.addFile((unsigned char *)fileData[i], fileDataSize[i]);
				}
				
				//create sanity.lsb
				std::vector<TAG_LSB *> sanityTags;
				std::vector<LsbObject *> sanityObjects;
				sanity.generateLsb(sanityTags, sanityObjects);
				LsbWriter sanityWriter;
				std::stringstream sanityStream;
				sanityWriter.writeFile(sanityObjects, sanityTags, sanityStream);
				fileNames.push_back("sanity.lsb");
				const std::string& sanityBytes = sanityStream.rdbuf()->str();
				char *alloc2 = new char[sanityBytes.size()];
				memcpy(alloc2, sanityBytes.c_str(), sanityBytes.size());
				sanityStream.str("");
				fileData.push_back(alloc2);
				fileDataSize.push_back(sanityBytes.size());
				
				if (!failure) {
					bool useCompression = (settings.getProperty(L"useCompression") == L"1" ? true : false);
					bool highCompressionMode = (settings.getProperty(L"highCompressionMode") == L"1" ? true : false);
					QProgressDialog compressProgress("Compressing...", QString(), 0, fileData.size(), this);
					compressProgress.setWindowFlags(compressProgress.windowFlags() & ~(Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint));
					compressProgress.setWindowModality(Qt::WindowModal);
					if (useCompression) {
						compressProgress.show();
					}
					QApplication::processEvents();
					
					PakWriter writer;
					writer.setShouldCompress(useCompression);
					writer.setHighCompressionMode(highCompressionMode);
					for (int i=0; i<fileData.size(); ++i) {
						writer.addFile(fileNames[i], fileData[i], fileDataSize[i]);
						compressProgress.setValue(i + 1);
						QApplication::processEvents();
					}
					if (useCompression) {
						compressProgress.hide();
					}
					boost::filesystem::ofstream fout(lsvPath, std::ios::binary);
					writer.writeFile(fout);
					fout.close();
					result = true;
				}
				for (int i=0; i<fileData.size(); ++i) {
					delete[] fileData[i];
				}
			} else {
				QMessageBox msg;
				msg.setText("Could not find lsv .BAK file");
				msg.exec();
				result = false;
			}
		}
		
		QMessageBox msg;
		if (result) {
			msg.setText("Save Success");
		}
		else {
			msg.setText("Save Failure");
		}
		msg.exec();
	}
}

void MainWindow::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    EditableTreeWidgetItem *editable = (EditableTreeWidgetItem *)item;
	std::string text = editable->text(column).toStdString();;
	if (editable->object != 0) {
		long type = editable->object->getType();
		if (type >= 0x14 && type <= 0x19) {
			editable->object->setData(text.c_str(), text.length() + 1);
		} else if (type == 0x04) {
			long value = 0;
			try {
				value = boost::lexical_cast<long>(text);
			} catch (const boost::bad_lexical_cast& e) {
				
			}

			editable->object->setData((char *)&value, sizeof(long));
		} else if (type == 0x05) {
			unsigned long value = 0;
			try {
				value = boost::lexical_cast<unsigned long>(text);
			}
			catch (const boost::bad_lexical_cast& e) {
				;
			}

			editable->object->setData((char *)&value, sizeof(unsigned long));
		} else if (type == 0x06) {
			float value = 0;
			try {
				value = boost::lexical_cast<float>(text);
			} catch (const boost::bad_lexical_cast& e) {
				
			}

			editable->object->setData((char *)&value, sizeof(float));
		} else if (type == 0x07) {
			double value = 0;
			try {
				value = boost::lexical_cast<double>(text);
			} catch (const boost::bad_lexical_cast& e) {
				
			}

			editable->object->setData((char *)&value, sizeof(double));
		} else if (type == 0x13) {
			bool value = false;
			if (boost::to_lower_copy(text) == "true") {
				value = true;
			}
			editable->object->setData((char *)&value, sizeof(bool));
		} else if (type == 0x0C) {
			std::vector<float> floatValues;
			std::vector<std::string> values;
			boost::split(values, text, boost::is_any_of(","));
			for (int i=0; i<values.size(); ++i) {
				std::string value = values[i];
				boost::trim(value);
				bool success = true;
				float floatValue;
				try {
					floatValue = boost::lexical_cast<float>(value);
				} catch (const boost::bad_lexical_cast& e) {
					success = false;
				}
				if (success) {
					floatValues.push_back(floatValue);
				}
			}
			if (floatValues.size() == 3) {
				long newDataLen = sizeof(float) * 3;
				char *newData = new char[newDataLen];
				memcpy(newData + sizeof(float) * 0, &floatValues[0], sizeof(float));
				memcpy(newData + sizeof(float) * 1, &floatValues[1], sizeof(float));
				memcpy(newData + sizeof(float) * 2, &floatValues[2], sizeof(float));
				editable->object->setData(newData, newDataLen);
				delete[] newData;
			}
		} else if (type == 0x01) {
			unsigned char value = 0;
			try {
				value = (unsigned char)boost::lexical_cast<unsigned short>(text);
			} catch (const boost::bad_lexical_cast& e) {
				
			}

			editable->object->setData((char *)&value, sizeof(unsigned char));
		}
	}
}

void MainWindow::on_actionE_xit_triggered()
{
    exit(0);
}

void MainWindow::on_savesFolderButton_released()
{
    QFileDialog fileDialog;
	fileDialog.setFileMode(QFileDialog::Directory);
	fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
	fileDialog.setNameFilter("PlayerProfiles");
	if (fileDialog.exec()) {
		if (fileDialog.selectedFiles().size() == 1) {
			QString fileName = fileDialog.selectedFiles()[0];
			QLineEdit *savesFolderEdit = this->findChild<QLineEdit *>("savesFolderEdit");
			savesFolderEdit->setText(fileName);
		}
	}
}

void MainWindow::on_gameDataButton_released()
{
	QFileDialog fileDialog;
	fileDialog.setFileMode(QFileDialog::Directory);
	fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
	fileDialog.setNameFilter("Data");
	if (fileDialog.exec()) {
		if (fileDialog.selectedFiles().size() == 1) {
			QString fileName = fileDialog.selectedFiles()[0];
			QLineEdit *gameDataEdit = this->findChild<QLineEdit *>("gameDataEdit");
			gameDataEdit->setText(fileName);
		}
	}
}

std::wstring MainWindow::getSaveLocation() {
	QLineEdit *savesFolderEdit = this->findChild<QLineEdit *>("savesFolderEdit");
	std::wstring result = savesFolderEdit->text().toStdWString();
	if (boost::contains(result, L"\\") && !boost::ends_with(result, L"\\")) {
		result += L"\\";
	}
	if (boost::contains(result, L"/") && !boost::ends_with(result, L"/")) {
		result += L"/";
	}
	return result;
}

std::wstring MainWindow::getGameDataLocation() {
	QLineEdit *gameDataEdit = this->findChild<QLineEdit *>("gameDataEdit");
	std::wstring result = gameDataEdit->text().toStdWString();
	if (boost::contains(result, L"\\") && !boost::ends_with(result, L"\\")) {
		result += L"\\";
	}
	if (boost::contains(result, L"/") && !boost::ends_with(result, L"/")) {
		result += L"/";
	}
	return result;
}

void MainWindow::on_savesFolderEdit_textChanged(const QString &text)
{
	QListWidget *loadFileWidget = this->findChild<QListWidget *>("loadFileWidget");
	
	loadFileWidget->clear();
	std::wstringstream stream;
	stream<<text.toStdWString();
	if (boost::contains(stream.str(), L"\\") && !boost::ends_with(stream.str(), L"\\")) {
		stream<<L"\\";
	}
	if (boost::contains(stream.str(), L"/") && !boost::ends_with(stream.str(), L"/")) {
		stream<<L"/";
	}
	std::wstring profilesPath = stream.str();
	std::vector<std::wstring> profileNames = getProfileNamesFromPath(profilesPath);
	for (int i=0; i<profileNames.size(); ++i) {
		std::wstring &data = profileNames[i];
		std::vector<std::wstring> saveList = getSaveGameList(profilesPath, data);
		for (int j=0; j<saveList.size(); ++j) {
			loadFileWidget->addItem(QString::fromStdWString(saveList[j]));
		}
	}
}

bool MainWindow::openPakFileToList(std::wstring &fileName) {
	userPakFileName = fileName;
	userPakReader.loadFile(userPakFileName);
	std::vector<std::string> fileList = userPakReader.getFileList();
	QListWidget *pakListWidget = this->findChild<QListWidget *>("pakListWidget");
	pakListWidget->clear();
	if (fileList.size() != 0) {
		for (int i=0; i<fileList.size(); ++i) {
			pakListWidget->addItem(fileList[i].c_str());
		}
	}
	else {
		QMessageBox msgBox;
		msgBox.setText("Bad data");
		msgBox.exec();
		return false;
	}
	return true;
}

void MainWindow::on_pakOpenButton_released()
{
	std::wstringstream stream;
	stream<<this->getGameDataLocation();
	QString result = QFileDialog::getOpenFileName(this,
												  QString("Open PAK"), QString::fromStdWString(stream.str()), QString("PAK Files (*.pak);;LSV Files (*.lsv)"));
	if (result.size() != 0) {
		std::wstring wresult = result.toStdWString();
		openPakFileToList(wresult);
	}
}

void MainWindow::on_pakListWidget_customContextMenuRequested(const QPoint &pos)
{
	QListWidget *pakListWidget = this->findChild<QListWidget *>("pakListWidget");
	
    QMenu menu;
	menu.addAction("&Extract to...");
	QAction *result = menu.exec(pakListWidget->mapToGlobal(pos));
	if (result != 0) {
		if (result->text() == "&Extract to...") {
			QFileDialog fileDialog;
			fileDialog.setFileMode(QFileDialog::Directory);
			fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
			if (fileDialog.exec()) {
				if (fileDialog.selectedFiles().size() == 1) {
					std::wstring folderName = fileDialog.selectedFiles()[0].toStdWString();
					QProgressDialog progress("Extracting...", "Cancel", 0, pakListWidget->selectedItems().size(), this);
					progress.setWindowModality(Qt::WindowModal);
					progress.show();
					QApplication::processEvents();
					for (int i=0; i<pakListWidget->selectedItems().size(); ++i) {
						if (progress.wasCanceled()) {
							progress.close();
							break;
						}
						QListWidgetItem *item = pakListWidget->selectedItems()[i];
						std::string text = item->text().toStdString();
						userPakReader.extractFile(userPakFileName, text, folderName, true);
						progress.setValue(i + 1);
						QApplication::processEvents();
					}
					if (!progress.wasCanceled()) {
						QMessageBox qmsg;
						qmsg.setText("Done");
						qmsg.exec();
					}
				}
			}
		}
	}
}

void MainWindow::on_devSaveFileButton_released()
{
	std::wstringstream stream;
	stream<<this->getSaveLocation();
	QString result = QFileDialog::getSaveFileName(this,
												  QString("Save File"), QString::fromStdWString(stream.str()), QString("LSB Files (*.lsb)"));
	if (result.size() != 0) {
		boost::filesystem::ofstream fout(result.toStdWString(),
						  std::ios_base::binary);
		if (fout) {
			QTreeWidget *tree = this->findChild<QTreeWidget *>("treeWidget");
			std::vector<LsbObject *> directoryList;
			for (int i=0; i<tree->topLevelItemCount(); ++i) {
				QTreeWidgetItem *item = tree->topLevelItem(i);
				EditableTreeWidgetItem *editable = (EditableTreeWidgetItem *)item;
				directoryList.push_back(editable->object);
			}
			
			LsbWriter writer;
			writer.writeFile(directoryList, openFileButtonTagList, fout);
			QMessageBox msgBox;
			msgBox.setText("Success");
			msgBox.exec();
		}
	}
}

void MainWindow::on_unloadButton_released()
{
	QPushButton *loadButton = this->findChild<QPushButton *>("loadButton");
	QPushButton *unloadButton = this->findChild<QPushButton *>("unloadButton");
    this->unload();
	unloadButton->setEnabled(false);
	loadButton->setEnabled(true);
}

void MainWindow::on_actionExtract_triggered()
{
	std::wstringstream stream;
	stream<<this->getSaveLocation();
	std::wstring folderPath = stream.str();
	QLineEdit *currentlyLoadedEdit = this->findChild<QLineEdit *>("currentlyLoadedEdit");
	std::wstring listText = currentlyLoadedEdit->text().toStdWString();
	std::vector<std::wstring> tokens;
	boost::split(tokens, listText, boost::is_any_of(L"/"));
	if (tokens.size() == 2) {
		folderPath += tokens[0];
		folderPath += L"\\";
		folderPath += L"SaveGames\\";
		folderPath += tokens[1];
		folderPath += L"\\";
		
		std::wstring lsvPath = folderPath;
		lsvPath += tokens[1];
		lsvPath += L".lsv";
		
		if (boost::filesystem::exists(boost::filesystem::path(lsvPath))) {
			QMessageBox::StandardButton reply;
			  reply = QMessageBox::question(this, "Extract", "Are you sure you want to extract all files?",
			                                QMessageBox::Yes|QMessageBox::No);
			  if (reply == QMessageBox::Yes) {
				  PakReader reader;
				  bool lsvLoaded = reader.loadFile(lsvPath);
				  if (lsvLoaded) {
					  std::vector<std::string> fileList = reader.getFileList();
					  if (fileList.size() > 0) {
						  QProgressDialog decompressProgress("Decompressing...", QString(), 0, fileList.size(), this);
						  decompressProgress.setWindowFlags(decompressProgress.windowFlags() & ~(Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint));
						  decompressProgress.setWindowModality(Qt::WindowModal);
						  decompressProgress.show();
						  QApplication::processEvents();
						  
						  bool success = true;
						  for (int i=0; i<fileList.size(); ++i) {
							  if (!reader.extractFile(lsvPath, fileList[i], folderPath, true)) {
								  success = false;
								  if (!success) {
									  QMessageBox qmsg;
									  std::ostringstream stream;
									  stream<<"Failed to extract file: "<<fileList[i];
									  qmsg.setText(QString::fromStdString(stream.str()));
									  qmsg.exec();
								  }
								  break;
							  } else {
								  decompressProgress.setValue(i + 1);
								  QApplication::processEvents();
							  }
						  }
						  decompressProgress.hide();
						  if (success) {
							  QMessageBox qmsg;
							  std::ostringstream stream;
							  stream<<"Success";
							  qmsg.setText(QString::fromStdString(stream.str()));
							  qmsg.exec();
							  
//							  boost::system::error_code ec;
//							  boost::filesystem::rename(boost::filesystem::path(lsvPath), boost::filesystem::path(lsvBakPath), ec);
//							  std::string errorMessage = ec.message();
//							  if (ec != ec.default_error_condition()) {
//								  QMessageBox qmsg;
//								  std::ostringstream stream;
//								  stream<<"Could not backup LSV archive: "<<errorMessage;
//								  qmsg.setText(QString::fromStdString(stream.str()));
//								  qmsg.exec();
//							  }
						  }
					  }
				  }
			}
		}
	}
}

void MainWindow::on_action_About_triggered()
{
	const std::string aboutName = "About";
	const std::string aboutText = "MooseEdit version " + std::string(PRG_VERSION);
    QMessageBox::about(this, QString::fromStdString(aboutName), QString::fromStdString(aboutText));
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog *settingsDialog = new SettingsDialog(&settings, this);
	if (settingsDialog->exec() == 1) {
		settings.saveFile(PRG_VERSION);
	}
	delete settingsDialog;
}
