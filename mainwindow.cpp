#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "charactertab.h"
#include "InventoryHandler.h"
#include "LsbReader.h"
#include "LsbWriter.h"
#include "PakReader.h"
#include "TextureAtlas.h"
#include "finddialog.h"
#include "EquipmentHandler.h"
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
#include <QGLWidget>
#include <QClipboard>
#include <QDir>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <tinyxml/tinyxml.h>
#include <boost/lexical_cast.hpp>
#include <QGLContext>
#include <QGLFramebufferObject>
#include <QScrollBar>
#include <QShortcut>
#include <Shlobj.h>
#include <QResizeEvent>
#include <QDesktopWidget>
#include <QProgressDialog>

#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)

std::vector<std::string> getSaveGameList(const char *path, const char *profileName) {
	const char *saveDirName = "Savegames";
	std::vector<std::string> saveList;
	DIR *dir;
	dirent *ent;
	std::string fullPath = path;
	fullPath += "\\";
	fullPath += profileName;
	fullPath += "\\";
	fullPath += saveDirName;
	chdir(fullPath.c_str());
	if ((dir = opendir (fullPath.c_str())) != 0) {
		while ((ent = readdir (dir)) != 0) {
			struct stat st;
			stat(ent->d_name, &st);
			if (S_ISDIR(st.st_mode)) {
				std::string entName = ent->d_name;
				if (entName != "." && entName != "..") {
					std::string saveText = profileName;
					saveText += "/";
					saveText += entName;
					saveList.push_back(saveText);
				}
			}
		}
		closedir (dir);
	}
	return saveList;
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
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
	
	std::string path = getSteamPathFromRegistry();
	
	QLineEdit *gameDataEdit = this->findChild<QLineEdit *>("gameDataEdit");
	if (path.length() > 0) {
		std::string gameDataPath = path;
		if (!boost::ends_with(gameDataPath, "\\")) {
			gameDataPath += "\\";
		}
		gameDataPath += "Data\\";
		gameDataEdit->setText(gameDataPath.c_str());
	}
	
	std::vector<std::string> splitVector;
	boost::split(splitVector, path, boost::is_any_of("\\"));
	if (splitVector.size() > 0) {
		std::string &first = splitVector[0];
		if (first.length() != 0) {
			QLineEdit *savesFolderEdit = this->findChild<QLineEdit *>("savesFolderEdit");
			
			CHAR my_documents[MAX_PATH + 1];
			HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
			if (result == S_OK) {
				std::string saveFolderPath = my_documents;
				if (!boost::ends_with(saveFolderPath, "\\")) {
					saveFolderPath += "\\";
				}
				saveFolderPath += "Larian Studios\\Divinity Original Sin\\PlayerProfiles\\";
				savesFolderEdit->setText(saveFolderPath.c_str());
			}
			else {
				std::string saveFolderPath = first;
				saveFolderPath += "\\Users\\";
				const char *username = std::getenv("USER");
				if (username == 0) {
					username = std::getenv("USERNAME");
				}
				if (username != 0) {
					saveFolderPath += username;
					saveFolderPath += "\\Documents\\Larian Studios\\Divinity Original Sin\\PlayerProfiles\\";
					savesFolderEdit->setText(saveFolderPath.c_str());
				}
			}
		}
	}
}

std::string MainWindow::getSteamPathFromRegistry() {
	std::string text = "";
	#ifdef _WIN32
	HKEY hKey;
	unsigned long returnVal;
	if ((returnVal = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 230230",
				  0, KEY_READ, &hKey)) == ERROR_SUCCESS) {
		unsigned long dataSize = MAX_PATH + 1;
		char buf[dataSize];
		if ((returnVal = RegQueryValueExA(hKey, "InstallLocation", 0, 0, (LPBYTE) buf, &dataSize)) == ERROR_SUCCESS) {
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

typedef std::map<unsigned long, unsigned long> ViewSlotMap;
struct ItemHandleData {
	long characterId;
	ViewSlotMap viewSlotMap;
};

void MainWindow::handleLoadButton() {
	for (int i=0; i<this->getCharacterGroup().getCharacters().size(); ++i) {
		delete this->getCharacterGroup().getCharacters()[i];
	}
	this->getCharacterGroup().getCharacters().clear();
	for (int i=0; i<globals.size(); ++i) {
		delete globals[i];
	}
	globals.clear();
	
	QPushButton *loadButton = this->findChild<QPushButton *>("loadButton");
	loadButton->setEnabled(false);
	QListWidget *loadFileWidget = this->findChild<QListWidget *>("loadFileWidget");
	QListWidgetItem *item = loadFileWidget->currentItem();
	if (item != 0) {
		QString text = item->text();
		QLineEdit *currentlyLoadedEdit = this->findChild<QLineEdit *>("currentlyLoadedEdit");
		currentlyLoadedEdit->setText(text);
		
		std::ostringstream stream;
		//stream<<"C:\\Users\\"<<username<<"\\Documents\\Larian Studios\\Divinity Original Sin\\PlayerProfiles\\";
		stream<<this->getSaveLocation();
		std::string profilesPath = stream.str();
		std::string listText = text.toStdString();
		std::vector<std::string> tokens;
		boost::split(tokens, listText, boost::is_any_of("/"));
		if (tokens.size() == 2) {
			profilesPath += tokens[0];
			profilesPath += "\\";
			profilesPath += "SaveGames\\";
			profilesPath += tokens[1];
			profilesPath += "\\Globals.lsb";
			std::ifstream fin(profilesPath.c_str(),
				std::ios_base::binary);
			LsbReader reader;
			globals = reader.loadFile(fin);
			globalTagList = reader.getTagList();
			fin.close();
			LsbObject *characters = LsbReader::lookupByUniquePath(globals, "Characters/root/CharacterFactory/Party/Characters");
			QTreeWidget *tree = this->findChild<QTreeWidget *>("treeWidget");
			tree->clear();
			displayAllItems2(tree, characters->getChildren());
			std::vector<LsbObject *> characterCreatorHandles = LsbReader::extractPropertyForEachListItem(characters->getChildren(), "Handle");
			LsbObject *creators = LsbReader::lookupByUniquePath(globals, "Characters/root/CharacterFactory/Creators");
			std::vector<LsbObject *> matchingCharacterCreators;
			for (int i=0; i<characterCreatorHandles.size(); ++i) {
				long handleId = characterCreatorHandles[i]->getIntData();
				std::vector<LsbObject *> matches = LsbReader::findItemsByAttribute(creators->getChildren(), "Handle", (const char *)&handleId, sizeof(handleId));
				if (matches.size() == 1) {
					matchingCharacterCreators.push_back(matches[0]);
				}
				else {
					std::cout<<"Found zero or multiple matches for character handle!\n";
				}
			}
			std::vector<LsbObject *> templateIds = LsbReader::extractPropertyForEachListItem(matchingCharacterCreators, "TemplateID");
			displayAllItems2(tree, matchingCharacterCreators);
			LsbObject *allCharacters = LsbReader::lookupByUniquePath(globals, "Characters/root/CharacterFactory/Characters");
			std::vector<LsbObject *> matchingCharacters;
			for (int i=0; i<templateIds.size(); ++i) {
				std::string templateId = templateIds[i]->getData();
				std::vector<LsbObject *> matches = LsbReader::findItemsByAttribute(allCharacters->getChildren(), "OriginalTemplate", templateId.c_str(), templateId.length() + 1);
				if (matches.size() == 1) {
					matchingCharacters.push_back(matches[0]);
				}
				else {
					std::cout<<"Found zero or multiple matches for character TemplateID!\n";
				}
			}
			displayAllItems2(tree, matchingCharacters);
			
			QTabWidget *tabWidget = this->findChild<QTabWidget *>("tabWidget");
			this->getCharacterGroup().setTabWidget(tabWidget);
			for (int i=matchingCharacters.size() - 1; i>=0; --i) {
				LsbObject *character = matchingCharacters[i];
				std::ostringstream ss;
				ss<<"charTab"<<i;
				LsbObject *itemsObject = LsbReader::lookupByUniquePath(globals, "Items/root/ItemFactory/Items");
				QWidget *widget = new characterTab(&globalTagList, itemsObject, this);
				widget->setObjectName(QString(ss.str().c_str()));
				
				LsbObject *origTemplate = LsbReader::lookupByUniquePathEntity(character, "OriginalTemplate");
				LsbObject *playerName = LsbReader::lookupByUniquePathEntity(character, "PlayerData/PlayerCustomData/Name");
				std::string origTemplateId = origTemplate->getData();
				std::string charName;
				if (origTemplateId == "5c5447e5-c1cf-4677-b84b-006d9be3f075") {
					charName = "Madora";
				}
				else if (origTemplateId == "80240f83-778e-4753-850b-48b05729589c") {
					charName = "Jahan";
				}
				else {
					char *data = playerName->getData();
					long dataSize = playerName->getDataSize();
					char *alloc = new char[dataSize / 2];
					wcstombs(alloc, (const wchar_t*)data, dataSize);
					charName = alloc;
					delete []alloc;
				}
				//tabWidget->addTab(widget, "CharTab");
				std::string charNameStr = charName;
				this->getCharacterGroup().getCharacters().insert(this->getCharacterGroup().getCharacters().begin(), new GameCharacter(globals, globalTagList));
				GameCharacter *gameCharacter = this->getCharacterGroup().getCharacters()[0];
				gameCharacter->setName(charNameStr);
				gameCharacter->setObject(character);
				gameCharacter->setWidget(widget);
				((characterTab *)widget)->setCharacter(gameCharacter);
				QLineEdit *nameEdit = widget->findChild<QLineEdit *>(QString("nameEdit"));
				nameEdit->setText(QString(charName.c_str()));
				tabWidget->insertTab(1, widget, QString(charName.c_str()));
			}
			
			//compile item list
			LsbObject *allItems = LsbReader::lookupByUniquePath(globals, "Items/root/ItemFactory/Items");
			
			std::vector<std::vector<GameItem *> > equipmentSets;
			for (int i=0; i<characterCreatorHandles.size(); ++i) {
				long handleId = characterCreatorHandles[i]->getIntData();
				std::vector<LsbObject *> matches = LsbReader::findItemsByAttribute(allItems->getChildren(), "owner", (const char *)&handleId, sizeof(handleId));
				long parentId = 0;
				//matches = LsbReader::findItemsByAttribute(matches, "Parent", (const char *)&parentId, sizeof(parentId));
				std::vector<GameItem *> equipmentSet;
				for (int j=0; j<matches.size(); ++j) {
					LsbObject *match = matches[j];
					LsbObject *slotObject = LsbReader::lookupByUniquePathEntity(match, "Slot");
					std::vector<LsbObject *> deleteMe;
					if (slotObject != 0) {
						unsigned short slot = *((unsigned short *)slotObject->getData());
						if (slot >= 0 && slot <= EQUIP_SLOTS) {
							GameItem *equipmentItem = new GameItem(&this->globalTagList);
							equipmentItem->setObject(match);
							equipmentSet.push_back(equipmentItem);
							deleteMe.push_back(match);
						}
					}
					if (i == 0) {
						displayAllItems2(tree, deleteMe);
					}
					//GameItem *newItem = new GameItem;
					//newItem->setObject(matches[j]);
					//this->getCharacterGroup().getCharacters()[i]->getInventory().addItem(newItem);
				}
				equipmentSets.push_back(equipmentSet);
//				if (i == 0) {
//					displayAllItems2(tree, equipmentSet);
//				}
			}
			
			
			typedef std::map<unsigned long, ItemHandleData> ItemHandleMap;
			ItemHandleMap itemHandleMap;
			for (int i=0; i<this->getCharacterGroup().getCharacters().size(); ++i) {
				//if (i != 0)
					//continue;
				LsbObject *character = this->getCharacterGroup().getCharacters()[i]->getObject();
				long inventoryId = *((long *)LsbReader::lookupByUniquePathEntity(character, "Inventory")->getData());
				LsbObject *inventoryCreators = LsbReader::lookupByUniquePath(globals, "Inventories/root/InventoryFactory/Creators");
				std::vector<LsbObject *> creatorMatches = LsbReader::findItemsByAttribute(inventoryCreators->getChildren(), "Object", (const char *)&inventoryId, sizeof(long));
				if (creatorMatches.size() == 1) {
					LsbObject *creator = creatorMatches[0];
					LsbObject *inventory = LsbReader::getObjectFromCreator(creator, "Inventories");
					std::vector<LsbObject *> inventoryHolder; //TODO: remove me
					inventoryHolder.push_back(inventory); //TODO: remove me
					displayAllItems2(tree, inventoryHolder); //TODO: remove me
					std::vector<LsbObject *> views = LsbReader::lookupAllEntitiesWithName(inventory, "Views");
					if (views.size() > 0) {
						for (int k=0; k<views.size(); ++k) {
							LsbObject *viewMapKey = LsbReader::lookupByUniquePathEntity(views[k], "MapKey");
							unsigned long viewId = *((unsigned long*)viewMapKey->getData());
							LsbObject *viewMapValue = LsbReader::lookupByUniquePathEntity(views[k], "MapValue");
							if (viewMapValue != 0) {
								std::vector<LsbObject *> indicesList = LsbReader::lookupAllEntitiesWithName(viewMapValue, "Indices");
								//std::vector<LsbObject *> itemList;
								for (int j=0; j<indicesList.size(); ++j) {
									LsbObject *index = indicesList[j];
									unsigned long itemCreatorHandle = *((unsigned long *)LsbReader::lookupByUniquePathEntity(index, "MapKey")->getData());
									unsigned long slot = *((unsigned long *)LsbReader::lookupByUniquePathEntity(index, "MapValue")->getData());
									if (i == 0){
										std::cout<<"slot = "<<slot<<'\n';
									}
									if (itemHandleMap.find(itemCreatorHandle) == itemHandleMap.end()) {
										//itemHandleMap[itemCreatorHandle] = std::map<unsigned long, unsigned long>();
										itemHandleMap[itemCreatorHandle] = ItemHandleData();
									}
									ItemHandleData &itemHandleData = itemHandleMap[itemCreatorHandle];
									itemHandleData.characterId = i;
									ViewSlotMap &viewSlotMap = itemHandleData.viewSlotMap;
									viewSlotMap[viewId] = slot;
									
//									if (viewId == 0 && itemCreatorHandle != 0) {
//										LsbObject *itemCreators = LsbReader::lookupByUniquePath(globals, "Items/root/ItemFactory/Creators");
//										std::vector<LsbObject *> itemCreatorMatches = LsbReader::findItemsByAttribute(itemCreators->getChildren(), "Handle", (char *)&itemCreatorHandle, sizeof(long));
//										if (itemCreatorMatches.size() == 1) {
//											LsbObject *itemCreator = itemCreatorMatches[0];
//											LsbObject *item = LsbReader::getObjectFromCreator(itemCreator, "Items");
//											GameItem *newItem = new GameItem(&globalTagList);
//											itemHandleMap[itemCreatorHandle] = newItem;
//											newItem->setObject(item);
//											newItem->setRenderSlot(slot);
//											this->getCharacterGroup().getCharacters()[i]->getInventory().addItem(newItem);
											
//											itemList.push_back(item); //TODO: remove me
//										}
//									} else {
//										if (itemHandleMap.find(itemCreatorHandle) != itemHandleMap.end()) {
////											GameItem *item = itemHandleMap[itemCreatorHandle];
////											std::string itemStatName = LsbReader::lookupByUniquePathEntity(item->getObject(), "Stats")->getData();
											
//										}
//									}
								}
//								if (i == 0) {
//									displayAllItems2(tree, itemList);
//								}
							}
						}
					}
				}
			}
			
			std::vector<LsbObject *> itemList;
			for (ItemHandleMap::iterator it = itemHandleMap.begin(); it != itemHandleMap.end(); ++it) {
				ItemHandleData& itemHandleData = it->second;
				ViewSlotMap& viewSlotMap = itemHandleData.viewSlotMap;
				unsigned long itemCreatorHandle = it->first;
				
				LsbObject *itemCreators = LsbReader::lookupByUniquePath(globals, "Items/root/ItemFactory/Creators");
				std::vector<LsbObject *> itemCreatorMatches = LsbReader::findItemsByAttribute(itemCreators->getChildren(), "Handle", (char *)&itemCreatorHandle, sizeof(long));
				if (itemCreatorMatches.size() == 1) {
					LsbObject *itemCreator = itemCreatorMatches[0];
					LsbObject *item = LsbReader::getObjectFromCreator(itemCreator, "Items");
					GameItem *newItem = new GameItem(&globalTagList);
					newItem->setObject(item);
					
					for (ViewSlotMap::iterator it2 = viewSlotMap.begin(); it2 != viewSlotMap.end(); ++it2) {
						switch (it2->first) {
						case 0:
							newItem->setRenderSlot(it2->second);
							break;
						case 2:
							newItem->setEquipmentSlot(it2->second);
							break;
						case 3:
							newItem->setConsumableSlot(it2->second);
							break;
						case 4:
							newItem->setMagicalSlot(it2->second);
							break;
						case 5:
							newItem->setIngredientSlot(it2->second);
							break;
						case 6:
							newItem->setKeysSlot(it2->second);
							break;
						case 7:
							newItem->setMiscSlot(it2->second);
							break;
						}
					}
					
					this->getCharacterGroup().getCharacters()[itemHandleData.characterId]->getInventory().addItem(newItem);
					
					if (itemHandleData.characterId == 0) {
						itemList.push_back(item); //TODO: remove me
					}
				}
			}
			displayAllItems2(tree, itemList);
			
			//load pak resources for textures
			std::string gameDataPath = this->getGameDataLocation();
			gamePakData.load(gameDataPath);
			
			if (gamePakData.getInventoryCellImg() != 0) {
				editItemHandler = new InventoryHandler(*gamePakData.getInventoryCellImg(), gamePakData.getStats(), gamePakData.getRootTemplates(), 
													   gamePakData.getModTemplates(), gamePakData.getIconAtlas(), gamePakData.getItemStats(), gamePakData.getNameMappings());
				for (int i=0; i<this->getCharacterGroup().getCharacters().size(); ++i) {
					GameCharacter *character = this->getCharacterGroup().getCharacters()[i];
					//if (i != 0)
						//continue;
					InventoryHandler *handlerPtr = new InventoryHandler(*gamePakData.getInventoryCellImg(), gamePakData.getStats(), gamePakData.getRootTemplates(), 
																		gamePakData.getModTemplates(), gamePakData.getIconAtlas(), gamePakData.getItemStats(), gamePakData.getNameMappings());
					InventoryHandler& inventoryHandler = *handlerPtr;
					for (int j=0; j<character->getInventory().getItems().size(); ++j) {
						inventoryHandler.getItems()->addItem(character->getInventory().getItems()[j]);
					}
					std::ostringstream ss;
					ss<<"charTab"<<i;
					//inventoryHandler.draw(this->findChild<QWidget *>(ss.str().c_str())->findChild<QWidget *>("inventoryContents"), this);
					
					character->setInventoryHandler(handlerPtr);
					
					characterTab *charTab = (characterTab *)character->getWidget();
					charTab->setNameMappings(&gamePakData.getNameMappings());
					charTab->setItemLinks(gamePakData.getItemLinks());
					charTab->setAllItemStats(gamePakData.getItemStats());
					charTab->setItemEditHandler(editItemHandler);
					charTab->setSkillStats(&gamePakData.getSkillStats());
					
					QWidget *equipmentWidget = charTab->findChild<QWidget *>("equipmentWidget");
					LsbObject *itemsObject = LsbReader::lookupByUniquePath(globals, "Items/root/ItemFactory/Items");
					EquipmentHandler *equipHandler = 
							new EquipmentHandler(*gamePakData.getInventoryCellImg(), gamePakData.getStats(), gamePakData.getRootTemplates(), 
												gamePakData.getModTemplates(), gamePakData.getIconAtlas(), gamePakData.getItemStats(), gamePakData.getNameMappings(),
												equipmentWidget, this, gamePakData.getItemLinks(), globalTagList, itemsObject, character);
					
					std::vector<GameItem *> &equipmentSet = equipmentSets[i];
					for (int j=0; j<equipmentSet.size(); ++j) {
						GameItem *item = equipmentSet[j];
						equipHandler->addItem(item);
					}
					charTab->setEquipmentHandler(equipHandler);
				}
			}
			else {
				MessageBoxA(0, "failed to load inventory image", 0, 0);
			}
		}
	}
}

void MainWindow::handleOpenFileButton() {
	std::ostringstream stream;
	stream<<this->getSaveLocation();
	QString result = QFileDialog::getOpenFileName(this,
												  QString("Open LSB"), stream.str().c_str(), QString("LSB Files (*.lsb)"));
	if (result.size() != 0) {
		std::ifstream fin(result.toStdString().c_str(),
						  std::ios_base::binary);
		LsbReader reader;
		std::vector<LsbObject *> directoryList = reader.loadFile(fin);
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
		}
	}
}

MainWindow::~MainWindow()
{
	delete ui;
	QApplication::quit();
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
			std::ostringstream stream;
			//stream<<"C:\\Users\\"<<username<<"\\Documents\\Larian Studios\\Divinity Original Sin\\PlayerProfiles\\";
			stream<<this->getSaveLocation();
			std::string listText = item->text().toStdString();
			std::vector<std::string> tokens;
			boost::split(tokens, listText, boost::is_any_of("/"));
			if (tokens.size() == 2) {
				std::string profilesPath = stream.str();
				profilesPath += tokens[0];
				profilesPath += "\\";
				profilesPath += "SaveGames\\";
				profilesPath += tokens[1];
				ShellExecuteA(NULL, "open", profilesPath.c_str(), NULL, NULL, SW_SHOW);
			}
		}
	}
}

void MainWindow::on_loadFileWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	QPushButton *loadButton = this->findChild<QPushButton *>("loadButton");
	loadButton->setEnabled(true);
	std::ostringstream stream;
	//stream<<"C:\\Users\\"<<username<<"\\Documents\\Larian Studios\\Divinity Original Sin\\PlayerProfiles\\";
	stream<<this->getSaveLocation();
	std::string profilesPath = stream.str();
	if (current != 0) {
		std::string listText = current->text().toStdString();
		std::vector<std::string> tokens;
		boost::split(tokens, listText, boost::is_any_of("/"));
		if (tokens.size() == 2) {
			profilesPath += tokens[0];
			profilesPath += "\\";
			profilesPath += "SaveGames\\";
			profilesPath += tokens[1];
			profilesPath += "\\";
			std::string bitmapPath = profilesPath;
			bitmapPath += tokens[1];
			bitmapPath += ".bmp";
			std::string configPath = profilesPath;
			configPath += tokens[1];
			configPath += ".lsb";
			std::string metaPath = profilesPath;
			metaPath += "meta.lsb";
			std::ifstream fin(configPath.c_str(),
				std::ios_base::binary);
			if (!fin) {
				fin.open(metaPath.c_str(),
						std::ios_base::binary);
			}
			if (fin) {
				LsbReader reader;
				std::vector<LsbObject *> metaDataList = reader.loadFile(fin);
				fin.close();
				LsbObject *metaData = LsbReader::lookupByUniquePath(metaDataList, "MetaData");
				LsbObject *saveTime = LsbReader::lookupByUniquePathEntity(metaData, "root/MetaData/SaveTime");
				LsbObject *module = LsbReader::lookupByUniquePathEntity(metaData, "root/MetaData/ModuleSettings/Mods/ModuleShortDesc");
				QLabel *previewPicture = this->findChild<QLabel *>("previewPicture");
				QImage img = QImage(bitmapPath.c_str());
				img = img.scaled(previewPicture->size());
				previewPicture->setPixmap(QPixmap::fromImage(img));
				QLineEdit *previewName = this->findChild<QLineEdit *>("previewName");
				QLineEdit *previewDate = this->findChild<QLineEdit *>("previewDate");
				long month = LsbReader::lookupByUniquePathEntity(saveTime, "Month")->getByteData();
				long day = LsbReader::lookupByUniquePathEntity(saveTime, "Day")->getByteData();
				long year = 1900 + LsbReader::lookupByUniquePathEntity(saveTime, "Year")->getByteData();
				std::ostringstream ssDate;
				ssDate<<month<<"/"<<day<<"/"<<year;
				std::string dateText = ssDate.str();
				QLineEdit *previewTime = this->findChild<QLineEdit *>("previewTime");
				std::ostringstream ssTime;
				long hours24 = LsbReader::lookupByUniquePathEntity(saveTime, "Hours")->getByteData();
				std::string midday = (hours24 >= 12 ? "PM" : "AM");
				long hours = hours24 % 12;
				if (hours == 0)
					hours = 12;
				long minutes = LsbReader::lookupByUniquePathEntity(saveTime, "Minutes")->getByteData();
				ssTime<<boost::format("%i:%02i %s") % hours % minutes % midday;
				std::string timeText = ssTime.str();
				QLineEdit *previewModule = this->findChild<QLineEdit *>("previewModule");
				std::string moduleText = LsbReader::lookupByUniquePathEntity(module, "Name")->getData();
				previewName->setText(QString(tokens[1].c_str()));
				previewDate->setText(QString(dateText.c_str()));
				previewTime->setText(QString(timeText.c_str()));
				previewModule->setText(QString(moduleText.c_str()));
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
			if (result->text() == "&Find") {
				treeFindAction();
			}
		}
	}
}

void MainWindow::on_saveAction_triggered()
{
	std::ostringstream stream;
	//stream<<"C:\\Users\\"<<username<<"\\Documents\\Larian Studios\\Divinity Original Sin\\PlayerProfiles\\";
	stream<<this->getSaveLocation();
	std::string profilesPath = stream.str();
	QLineEdit *currentlyLoadedEdit = this->findChild<QLineEdit *>("currentlyLoadedEdit");
	std::string listText = currentlyLoadedEdit->text().toStdString();;
	std::vector<std::string> tokens;
	boost::split(tokens, listText, boost::is_any_of("/"));
	if (tokens.size() == 2) {
		profilesPath += tokens[0];
		profilesPath += "\\";
		profilesPath += "SaveGames\\";
		profilesPath += tokens[1];
		profilesPath += "\\Globals.lsb";
	
		std::string bakPath = profilesPath;
		bakPath += ".bak";
		
		//create backup
		std::ifstream src(profilesPath.c_str(), std::ios::binary);
		std::ifstream dstExists(bakPath.c_str(), std::ios::binary);
		if (!dstExists) {
			std::ofstream dst(bakPath.c_str(),   std::ios::binary);
			dst << src.rdbuf();
			dst.close();
		}
		dstExists.close();
		src.close();
		
		LsbWriter writer;
		std::ofstream fout(profilesPath.c_str(), std::ios::binary);
		bool result = writer.writeFile(globals, globalTagList, fout);
		fout.close();
		
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
		} else if (type == 0x07) {
			double value = 0;
			try {
				value = boost::lexical_cast<double>(text);
			} catch (const boost::bad_lexical_cast& e) {
				
			}

			editable->object->setData((char *)&value, sizeof(double));
		}
	}
}

void MainWindow::on_actionE_xit_triggered()
{
    QApplication::quit();
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

std::string MainWindow::getSaveLocation() {
	QLineEdit *savesFolderEdit = this->findChild<QLineEdit *>("savesFolderEdit");
	std::string result = savesFolderEdit->text().toStdString();
	if (!boost::ends_with(result, "\\")) {
		result += "\\";
	}
	return result;
}

std::string MainWindow::getGameDataLocation() {
	QLineEdit *gameDataEdit = this->findChild<QLineEdit *>("gameDataEdit");
	std::string result = gameDataEdit->text().toStdString();
	if (!boost::ends_with(result, "\\")) {
		result += "\\";
	}
	return result;
}

void MainWindow::on_savesFolderEdit_textChanged(const QString &text)
{
	QListWidget *loadFileWidget = this->findChild<QListWidget *>("loadFileWidget");
	
	loadFileWidget->clear();
	std::ostringstream stream;
	stream<<text.toStdString();
	if (!boost::ends_with(stream.str(), "\\")) {
		stream<<"\\";
	}
	std::string profilesPath = stream.str();
	stream<<"playerprofiles.lsb";
	std::string profileLsbPath = stream.str();
	std::ifstream fin(profileLsbPath.c_str(),
		std::ios_base::binary);
	if (fin) {
		LsbReader reader;
		playerProfiles = reader.loadFile(fin);
		if (playerProfiles.size() > 0) {
			LsbObject *profilesDir = LsbReader::lookupByUniquePath(playerProfiles, "UserProfiles/root");
			std::vector<LsbObject *> profiles = LsbReader::lookupAllEntitiesWithName(profilesDir, "PlayerProfile");
			std::vector<LsbObject *> profileNames = LsbReader::extractPropertyForEachListItem(profiles, "PlayerProfileName");
			for (int i=0; i<profileNames.size(); ++i) {
				char *data = profileNames[i]->getData();
				long dataSize = profileNames[i]->getDataSize();
				char *alloc = new char[dataSize / 2];
				wcstombs(alloc, (const wchar_t*)data, dataSize);
				std::vector<std::string> saveList = getSaveGameList(profilesPath.c_str(), alloc);
				for (int j=0; j<saveList.size(); ++j) {
					loadFileWidget->addItem(QString(saveList[j].c_str()));
				}
				delete []alloc;
			}
		}
	}
	fin.close();
}

void MainWindow::on_pakOpenButton_released()
{
	std::ostringstream stream;
	stream<<this->getGameDataLocation();
	QString result = QFileDialog::getOpenFileName(this,
												  QString("Open PAK"), stream.str().c_str(), QString("PAK Files (*.pak)"));
	if (result.size() != 0) {
		userPakFileName = result.toStdString();
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
		}
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
					std::string folderName = fileDialog.selectedFiles()[0].toStdString();
					QProgressDialog progress("Extracting...", "Cancel", 0, pakListWidget->selectedItems().size(), this);
					progress.setWindowModality(Qt::WindowModal);
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
