#include "ItemEditFrame.h"
#include "ui_ItemEditFrame.h"
#include "LsbObject.h"
#include "LsbReader.h"
#include <QMessageBox>
#include <cstring>

class BaseStatsViewSelectCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
public:
	BaseStatsViewSelectCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		DataContainerTreeItem *current = (DataContainerTreeItem *)statsTree->currentItem();
		if (current->parent() != 0) {
			if (selectedItem != 0) {
				selectedItem->setTextColor(0, QColor(0, 0, 0));
			}
			selectedItem = current;
			selectedItem->setTextColor(0, QColor(255, 0, 0));
			
			LsbObject *statsObject = LsbReader::lookupByUniquePathEntity(itemEditFrame->item->getObject(), "Stats");
			std::string statsText = current->getData();
			statsObject->setData(statsText.c_str(), statsText.length() + 1);
			
			StatsContainer *container = GenStatsReader::getContainer(itemEditFrame->allItemStats, statsText);
			if (container != 0) {
				if (container->getData("InventoryTab") == "Equipment") {
					if (itemEditFrame->item->getStatsDirectory() == 0) {
						itemEditFrame->item->createStatsDirectory();
					}
				} else {
					if (itemEditFrame->item->getStatsDirectory() != 0) {
						itemEditFrame->item->removeStatsDirectory();
					}
				}
			}
			
			LsbObject *currentTemplateObject = LsbReader::lookupByUniquePathEntity(itemEditFrame->item->getObject(), "CurrentTemplate");
			StatsContainer *linkContainer = GenStatsReader::getContainer(itemEditFrame->itemLinks, statsText);
			if (linkContainer != 0) {
				std::string templateText = linkContainer->getArg(1);
				currentTemplateObject->setData(templateText.c_str(), templateText.length() + 1);
			}
			
			LsbObject *generationObject = LsbReader::lookupByUniquePathEntity(itemEditFrame->item->getObject(), "Generation");
			if (generationObject != 0) {
				LsbObject *baseObject = LsbReader::lookupByUniquePathEntity(generationObject, "Base");
				if (baseObject != 0) {
					baseObject->setData(statsText.c_str(), statsText.length() + 1);
				}
			}
			itemEditFrame->redraw();
		}
	}
};

class ModsViewAddCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
public:
	ModsViewAddCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		itemEditFrame->modsView->hide();
		itemEditFrame->modsPicker->show();
	}
};

class ModsViewRemoveCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
public:
	ModsViewRemoveCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		std::string modName = selectedItem->getData();
		long childIndex = statsTree->invisibleRootItem()->indexOfChild(selectedItem);
		delete selectedItem;
		LsbObject *itemObject = itemEditFrame->item->getObject();
		LsbObject *generationObject = LsbReader::lookupByUniquePathEntity(itemObject, "Generation");
		if (generationObject != 0) {
			std::vector<LsbObject *> boostObjects = LsbReader::lookupAllEntitiesWithName(generationObject, "Boost");
			if (childIndex < boostObjects.size()) {
				LsbObject *targetBoost = boostObjects[childIndex];
				//TODO: verify this is the correct node
				generationObject->removeChild(targetBoost);
				delete targetBoost;
			}
		}
		itemEditFrame->redraw();
	}
};

class PermBoostViewAddCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
public:
	PermBoostViewAddCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		itemEditFrame->permBoostView->hide();
		itemEditFrame->permBoostPicker->show();
	}
};

class PermBoostViewRemoveCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
public:
	PermBoostViewRemoveCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		long childIndex = statsTree->invisibleRootItem()->indexOfChild(selectedItem);
		delete selectedItem;
		LsbObject *itemObject = itemEditFrame->item->getObject();
		LsbObject *statsDirectory = 0;
		std::vector<LsbObject *> statsObjects = LsbReader::lookupAllEntitiesWithName(itemObject, "Stats");
		for (int i=0; i<statsObjects.size(); ++i) {
			LsbObject *object = statsObjects[i];
			if (object->isDirectory()) {
				statsDirectory = object;
				break;
			}
		}
		
		if (statsDirectory != 0) {
			LsbObject *permBoostObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "PermanentBoost");
			if (childIndex < permBoostObject->getChildren().size()) {
				LsbObject *targetPermBoost = permBoostObject->getChildren()[childIndex];
				//TODO: verify this is the correct node
				permBoostObject->removeChild(targetPermBoost);
				delete targetPermBoost;
			}
		}
		itemEditFrame->redraw();
	}
};

class PermBoostPickerSelectCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
public:
	PermBoostPickerSelectCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		if (selectedItem->getData().size() == 0) {
			QMessageBox messageBox;
			messageBox.setText("Please enter a valid value in the table");
			messageBox.exec();
		}
		else {
			std::string name = selectedItem->text(0).toStdString();
			long value = 0;
			try {
				value = boost::lexical_cast<long>(selectedItem->getData());
			} catch (const boost::bad_lexical_cast& e) {
				
			}
			
			LsbObject *itemObject = itemEditFrame->item->getObject();
			LsbObject *statsDirectory = 0;
			std::vector<LsbObject *> statsObjects = LsbReader::lookupAllEntitiesWithName(itemObject, "Stats");
			for (int i=0; i<statsObjects.size(); ++i) {
				LsbObject *object = statsObjects[i];
				if (object->isDirectory()) {
					statsDirectory = object;
					break;
				}
			}
			
			LsbObject *newPermBoost = 0;
			std::vector<LsbObject *> newPermBoostObjects;
			if (statsDirectory != 0) {
				TAG_LSB *newBoostTag = LsbReader::getTagByName(name.c_str(), itemEditFrame->tagList);
				if (newBoostTag == 0) {
					TAG_LSB *newTag = new TAG_LSB();
					newTag->index = LsbObject::getNextFreeTagIndex(itemEditFrame->tagList);
					long tagLength = name.length() + 1;
					char *tagAlloc = new char[tagLength];
					strcpy(tagAlloc, name.c_str());
					newTag->tag = tagAlloc;
					newTag->tagLength = tagLength;
					itemEditFrame->tagList->push_back(newTag);
					
					newBoostTag = newTag;
				}
				if (newBoostTag != 0) {
					newPermBoost = LsbReader::lookupByUniquePathEntity(statsDirectory, "PermanentBoost");
					if (newPermBoost == 0) {
						TAG_LSB *permBoostTag = LsbReader::getTagByName("PermanentBoost", itemEditFrame->tagList);
						newPermBoost = new LsbObject(true, permBoostTag->index, permBoostTag->tag, 0, statsDirectory, itemEditFrame->tagList);
						
						TAG_LSB *abilitiesTag = LsbReader::getTagByName("Abilities", itemEditFrame->tagList);
						LsbObject *newAbilitiesObject = new LsbObject(true, abilitiesTag->index, abilitiesTag->tag, 0, newPermBoost, itemEditFrame->tagList);
						
						newPermBoost->addChild(newAbilitiesObject);
						statsDirectory->addChild(newPermBoost);
					}
					LsbObject *newBoostObject = new LsbObject(false, newBoostTag->index, newBoostTag->tag, 0x04, newPermBoost, itemEditFrame->tagList);
					//LsbObject *newBoostObject = new LsbObject(false, newBoostTag->index, newBoostTag->tag, 0x16, newPermBoost, itemEditFrame->tagList);
					//newBoostObject->setData(value.c_str(), value.length() + 1);
					newBoostObject->setData((char *)&value, sizeof(long));
					newPermBoostObjects.push_back(newBoostObject);
					
					newPermBoost->addChild(newBoostObject);
				}
			}
			if (newPermBoostObjects.size() > 0) {
				itemEditFrame->permBoostView->addToTree(newPermBoostObjects);
			}
			itemEditFrame->permBoostPicker->hide();
			itemEditFrame->permBoostView->show();
			itemEditFrame->permBoostView->resizeTree();
			itemEditFrame->redraw();
		}
	}
};

class PermBoostPickerCancelCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
public:
	PermBoostPickerCancelCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		itemEditFrame->permBoostPicker->hide();
		itemEditFrame->permBoostView->show();
	}
};

class ModsPickerSelectCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
	void addBoostToGenerationObject(LsbObject *generationObject, std::string &modName) {
		TAG_LSB *boostTag = LsbReader::getTagByName("Boost", itemEditFrame->tagList);
		LsbObject *newBoost = new LsbObject(true, boostTag->index, boostTag->tag, 0, generationObject, itemEditFrame->tagList);
		TAG_LSB *objectTag = LsbReader::getTagByName("Object", itemEditFrame->tagList);
		LsbObject *newBoostObject = new LsbObject(false, objectTag->index, objectTag->tag, 0x16, newBoost, itemEditFrame->tagList);
		newBoost->addChild(newBoostObject);
		newBoostObject->setData(modName.c_str(), modName.length() + 1);
		generationObject->addChild(newBoost);
	}
		
	
public:
	ModsPickerSelectCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		std::string text = selectedItem->getData();
		StatsContainer *container = GenStatsReader::getContainer(itemEditFrame->allItemStats, text);
		std::string modName = container->getArg(0);
		std::vector<StatsContainer *> containers;
		containers.push_back(container);
		itemEditFrame->modsView->addToTree(containers);
		LsbObject *itemObject = itemEditFrame->item->getObject();
		LsbObject *generationObject = LsbReader::lookupByUniquePathEntity(itemObject, "Generation");
		if (generationObject != 0) {
			addBoostToGenerationObject(generationObject, modName);
		} else {
			TAG_LSB *generationTag = LsbReader::getTagByName("Generation", itemEditFrame->tagList);
			if (generationTag != 0) {
				generationObject = new LsbObject(true, generationTag->index, generationTag->tag, 0, itemObject, itemEditFrame->tagList);
				itemObject->addChild(generationObject);
				
				LsbObject *statsObject = LsbReader::lookupByUniquePathEntity(itemEditFrame->item->getObject(), "Stats");
				
				LsbObject *statsDirectory = 0;
				std::vector<LsbObject *> statsObjects = LsbReader::lookupAllEntitiesWithName(itemEditFrame->item->getObject(), "Stats");
				for (int i=0; i<statsObjects.size(); ++i) {
					LsbObject *object = statsObjects[i];
					if (object->isDirectory()) {
						statsDirectory = object;
						break;
					}
				}
				
				LsbObject *isGeneratedObject = LsbReader::lookupByUniquePathEntity(itemEditFrame->item->getObject(), "IsGenerated");
				bool isGen = true;
				isGeneratedObject->setData((char *)&isGen, sizeof(bool));
				
				if (statsDirectory != 0) {
					LsbObject *isIdentObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "IsIdentified");
					long isIdent = 1;
					isIdentObject->setData((char *)&isIdent, sizeof(long));
				}
				
				TAG_LSB *baseTag = LsbReader::getTagByName("Base", itemEditFrame->tagList);
				LsbObject *newBase = new LsbObject(false, baseTag->index, baseTag->tag, 0x16, generationObject, itemEditFrame->tagList);
				newBase->setData(statsObject->getData(), statsObject->getDataSize());
				
				TAG_LSB *itemTypeTag = LsbReader::getTagByName("ItemType", itemEditFrame->tagList);
				LsbObject *newItemType = new LsbObject(false, itemTypeTag->index, itemTypeTag->tag, 0x16, generationObject, itemEditFrame->tagList);
				std::string itemTypeDefault = "Magic";
				newItemType->setData(itemTypeDefault.c_str(), itemTypeDefault.length() + 1);
				if (statsDirectory != 0) {
					LsbObject *itemTypeObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "ItemType");
					itemTypeObject->setData(itemTypeDefault.c_str(), itemTypeDefault.length() + 1);
				}
				
				TAG_LSB *levelTag = LsbReader::getTagByName("Level", itemEditFrame->tagList);
				LsbObject *newLevel = new LsbObject(false, levelTag->index, levelTag->tag, 0x04, generationObject, itemEditFrame->tagList);
				long statsLevel = itemEditFrame->item->getItemLevel();
				newLevel->setData((char *)&statsLevel, sizeof(long));
				
				TAG_LSB *randomTag = LsbReader::getTagByName("Random", itemEditFrame->tagList);
				LsbObject *newRandom = new LsbObject(false, randomTag->index, randomTag->tag, 0x04, generationObject, itemEditFrame->tagList);
				long random = 1;
				newRandom->setData((char *)&random, sizeof(long));
				
				generationObject->addChild(newBase);
				generationObject->addChild(newItemType);
				generationObject->addChild(newLevel);
				generationObject->addChild(newRandom);
				
				addBoostToGenerationObject(generationObject, modName);
			}
		}
		itemEditFrame->modsView->resizeTree();
		itemEditFrame->modsPicker->hide();
		itemEditFrame->modsView->show();
		itemEditFrame->redraw();
	}
};

class ModsPickerCancelCallback : public StatsButtonCallback {
	ItemEditFrame *itemEditFrame;
public:
	ModsPickerCancelCallback(ItemEditFrame *itemEditFrame) {
		this->itemEditFrame = itemEditFrame;
	}

	void onClick(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree) {
		itemEditFrame->modsPicker->hide();
		itemEditFrame->modsView->show();
	}
};

void ItemEditFrame::redraw() {
	QFrame *itemFrame = this->findChild<QFrame *>("itemFrame");
	itemEditHandler->setIconsPerRow(1);
	itemEditHandler->setMinSlots(1);
	itemEditHandler->setMaxSlots(1);
	long itemSlot = item->getSlot(); //TODO: remove this hack
	item->setSlot(0);
	this->itemEditHandler->getItems()->getItems().clear();
	this->itemEditHandler->getItems()->addItem(item);
	item->setSlot(itemSlot);
	this->itemEditHandler->draw(itemFrame, this);
	this->show();
	
}

ItemEditFrame::ItemEditFrame(std::vector<StatsContainer *> &allItemStats, std::vector<StatsContainer *> &itemLinks, GameItem *originalItem, InventoryHandler *itemEditHandler, 
							 ItemEditCallback *itemEditCallback, std::vector<TAG_LSB *> *tagList, std::map<std::string, std::string> &nameMappings,
							 QWidget *parent) :
	QFrame(parent), allItemStats(allItemStats), itemLinks(itemLinks), oldItem(originalItem), itemEditHandler(itemEditHandler), itemEditCallback(itemEditCallback),
	tagList(tagList), nameMappings(nameMappings),
	ui(new Ui::ItemEditFrame)
{
	this->item = new GameItem(*originalItem); //make a copy of the item
	ui->setupUi(this);
	redraw();
	
	QGroupBox *dataGroupBox = this->findChild<QGroupBox *>("dataGroupBox");
	generalView = new ItemGeneral(item, tagList, dataGroupBox);
	generalView->registerCallback(this);
	dataGroupBox->layout()->addWidget(generalView);
	
	baseStatsView = new StatsView(allItemStats, nameMappings, dataGroupBox);
	DataContainerTreeItem *weaponFolder = baseStatsView->addFolder("Weapons");
	baseStatsView->addStatsDirectory("_Daggers", "Daggers", weaponFolder);
	baseStatsView->addStatsDirectory("_Swords", "Swords", weaponFolder);
	baseStatsView->addStatsDirectory("_Axes", "Axes", weaponFolder);
	baseStatsView->addStatsDirectory("_Clubs", "Clubs", weaponFolder);
	baseStatsView->addStatsDirectory("_TwoHandedAxes", "Two-Handed Axes", weaponFolder);
	baseStatsView->addStatsDirectory("_TwoHandedSwords", "Two-Handed Swords", weaponFolder);
	baseStatsView->addStatsDirectory("_Spears", "Spears", weaponFolder);
	baseStatsView->addStatsDirectory("_Staffs", "Staves", weaponFolder);
	baseStatsView->addStatsDirectory("_Bows", "Bows", weaponFolder);
	baseStatsView->addStatsDirectory("_Crossbows", "Crossbows", weaponFolder);
	
	DataContainerTreeItem *armorFolder = baseStatsView->addFolder("Armor");
	baseStatsView->addStatsDirectory("_ClothArmor", "Cloth Armor", armorFolder);
	baseStatsView->addStatsDirectory("_LeatherArmor", "Leather Armor", armorFolder);
	baseStatsView->addStatsDirectory("_ChainArmor", "Chain Armor", armorFolder);
	baseStatsView->addStatsDirectory("_MetalArmor", "Metal Armor", armorFolder);
	baseStatsView->addStatsDirectory("_RobeArmor", "Robe Armor", armorFolder);
	baseStatsView->addStatsDirectory("_Garments", "Garments", armorFolder);
	baseStatsView->addStatsDirectory("_ShoesBase", "Shoes", armorFolder);
	baseStatsView->addStatsDirectory("_Belts", "Belts", armorFolder);
	baseStatsView->addStatsDirectory("_Gloves", "Gloves", armorFolder);
	baseStatsView->addStatsDirectory("_HelmetsLeather", "Leather Helmets", armorFolder);
	baseStatsView->addStatsDirectory("_HelmetsMetal", "Metal Helmets", armorFolder);
	baseStatsView->addStatsDirectory("_Rings", "Rings", armorFolder);
	baseStatsView->addStatsDirectory("_Amulets", "Amulets", armorFolder);

	baseStatsView->addStatsDirectory("_Shields", "Shields");
	
	DataContainerTreeItem *potionFolder = baseStatsView->addFolder("Consumables");
	baseStatsView->addStatsDirectory("Potion", "Potions", potionFolder);
	baseStatsView->addStatsDirectory("_ComboFood", "Combo Food", potionFolder);
	baseStatsView->addStatsDirectory("_Food", "Food", potionFolder);
	baseStatsView->addStatsDirectory("_Drink", "Drink", potionFolder);
	baseStatsView->addStatsDirectory("_SkillBoost", "Skill Boost", potionFolder);
	baseStatsView->addStatsDirectory("_Totem", "Totem", potionFolder);
	
	DataContainerTreeItem *objectFolder = baseStatsView->addFolder("Objects");
	baseStatsView->addStatsDirectory("Gold", "Gold", objectFolder);
	baseStatsView->addStatsDirectory("_Generics", "Generic", objectFolder);
	//baseStatsView->addStatsDirectory("_TreasureMap", "Treasure Maps", objectFolder);
	baseStatsView->addStatsDirectory("_Arrows", "Arrows", objectFolder);
	baseStatsView->addStatsDirectory("_OrbCharms", "Orb Charms", objectFolder);
	baseStatsView->addStatsDirectory("_TrapObjects", "Trap Objects", objectFolder);
	baseStatsView->addStatsDirectory("_Skillbooks", "Skillbooks", objectFolder);
	baseStatsView->addStatsDirectory("_Unique", "Unique", objectFolder);
	baseStatsView->addStatsDirectory("_Scrolls", "Scrolls", objectFolder);
	//baseStatsView->addStatsDirectory("_RecipeBooks", "Recipe Books", objectFolder);
	
	baseStatsView->resizeTree();
	if (item->getStatsText().size() > 0) {
		baseStatsView->selectNodeByName(item->getStatsText().c_str());
	}
	baseStatsViewSelectCallback = new BaseStatsViewSelectCallback(this);
	baseStatsView->enableSelectButton(baseStatsViewSelectCallback);
	dataGroupBox->layout()->addWidget(baseStatsView);
	
	modsPicker = new StatsView(allItemStats, nameMappings, dataGroupBox);
	modsPicker->addBoostDirectory("Weapon", "Weapon Mods");
	modsPicker->addBoostDirectory("Armor", "Armor Mods");
	modsPicker->addBoostDirectory("Shield", "Shield Mods");
	modsPicker->resizeTree();
	modsPickerSelectCallback = new ModsPickerSelectCallback(this);
	modsPicker->enableSelectButton(modsPickerSelectCallback);
	modsPickerCancelCallback = new ModsPickerCancelCallback(this);
	modsPicker->enableCancelButton(modsPickerCancelCallback);
	dataGroupBox->layout()->addWidget(modsPicker);
	
	modsView = new StatsView(allItemStats, nameMappings, dataGroupBox);
	if (item->getItemStats() != 0) {
		std::vector<StatsContainer *> boosts = item->getBoosts();
		modsView->addToTree(boosts);
	}
	modsView->resizeTree();
	modsViewAddCallback = new ModsViewAddCallback(this);
	modsView->enableAddButton(modsViewAddCallback);
	modsViewRemoveCallback = new ModsViewRemoveCallback(this);
	modsView->enableRemoveButton(modsViewRemoveCallback);
	dataGroupBox->layout()->addWidget(modsView);
	
	permBoostView = new StatsView(allItemStats, nameMappings, dataGroupBox);
	if (item->getPermBoosts().size() > 0) {
		std::vector<LsbObject *> &permBoosts = item->getPermBoosts();
		permBoostView->addToTree(permBoosts);
	}
	permBoostView->resizeTree();
	permBoostViewAddCallback = new PermBoostViewAddCallback(this);
	permBoostView->enableAddButton(permBoostViewAddCallback);
	permBoostViewRemoveCallback = new PermBoostViewRemoveCallback(this);
	permBoostView->enableRemoveButton(permBoostViewRemoveCallback);
	permBoostView->registerTableEditCallback(this);
	dataGroupBox->layout()->addWidget(permBoostView);
	
	permBoostPicker = new StatsView(allItemStats, nameMappings, dataGroupBox);
	{
		std::vector<StatsContainer *> modifierGroups = GenStatsReader::getContainersByContainerType(allItemStats, "type"); //ghetto way to get modifiers
		StatsContainer *weaponContainer = GenStatsReader::getContainer(modifierGroups, "Weapon");
		DataContainerTreeItem *weaponFolder = permBoostPicker->addFolder("Weapon");
		permBoostPicker->addDataToTree(weaponContainer, weaponFolder);
		
		StatsContainer *armorContainer = GenStatsReader::getContainer(modifierGroups, "Armor");
		DataContainerTreeItem *armorFolder = permBoostPicker->addFolder("Armor");
		permBoostPicker->addDataToTree(armorContainer, armorFolder);
		
		StatsContainer *shieldContainer = GenStatsReader::getContainer(modifierGroups, "Shield");
		DataContainerTreeItem *shieldFolder = permBoostPicker->addFolder("Shield");
		permBoostPicker->addDataToTree(shieldContainer, shieldFolder);
		
		StatsContainer *potionContainer = GenStatsReader::getContainer(modifierGroups, "Potion");
		DataContainerTreeItem *potionFolder = permBoostPicker->addFolder("Potion");
		permBoostPicker->addDataToTree(potionContainer, potionFolder);
		
		StatsContainer *objectContainer = GenStatsReader::getContainer(modifierGroups, "Object");
		DataContainerTreeItem *objectFolder = permBoostPicker->addFolder("Object");
		permBoostPicker->addDataToTree(objectContainer, objectFolder);
		
		StatsContainer *characterContainer = GenStatsReader::getContainer(modifierGroups, "Character");
		DataContainerTreeItem *characterFolder = permBoostPicker->addFolder("Character");
		permBoostPicker->addDataToTree(characterContainer, characterFolder);
		
		StatsContainer *skillDataContainer = GenStatsReader::getContainer(modifierGroups, "SkillData");
		DataContainerTreeItem *skillDataFolder = permBoostPicker->addFolder("SkillData");
		permBoostPicker->addDataToTree(skillDataContainer, skillDataFolder);
	}
	
	permBoostPicker->resizeTree();
	permBoostPickerSelectCallback = new PermBoostPickerSelectCallback(this);
	permBoostPicker->enableSelectButton(permBoostPickerSelectCallback);
	permBoostPickerCancelCallback = new PermBoostPickerCancelCallback(this);
	permBoostPicker->enableCancelButton(permBoostPickerCancelCallback);
	dataGroupBox->layout()->addWidget(permBoostPicker);
	
	hideAllViews();
}

void ItemEditFrame::onEdit()
{
	this->redraw();
}

void ItemEditFrame::onEdit(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree, QTableWidgetItem *editedItem, QTableWidget *table)
{
	long childIndex = statsTree->invisibleRootItem()->indexOfChild(selectedItem);
	
	LsbObject *statsDirectory = 0;
	std::vector<LsbObject *> statsObjects = LsbReader::lookupAllEntitiesWithName(this->item->getObject(), "Stats");
	for (int i=0; i<statsObjects.size(); ++i) {
		LsbObject *object = statsObjects[i];
		if (object->isDirectory()) {
			statsDirectory = object;
			break;
		}
	}
	
	if (statsDirectory != 0) {
		LsbObject *permBoostObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "PermanentBoost");
		if (childIndex < permBoostObject->getChildren().size()) {
			LsbObject *targetPermBoost = permBoostObject->getChildren()[childIndex];
			//TODO: verify this is the correct node
			long value = 0;
			try {
				value = boost::lexical_cast<long>(selectedItem->getData());
			}
			catch (const boost::bad_lexical_cast& e) {
				
			}

			targetPermBoost->setData((char *)&value, sizeof(long));
		}
	}
	this->redraw();
}

ItemEditFrame::~ItemEditFrame()
{
	delete modsPickerSelectCallback;
	delete modsPickerCancelCallback;
	delete baseStatsViewSelectCallback;
	delete modsViewAddCallback;
	delete permBoostViewAddCallback;
	delete permBoostPickerSelectCallback;
	delete permBoostPickerCancelCallback;
	
	delete generalView;
	delete baseStatsView;
	delete modsPicker;
	delete modsView;
	delete permBoostPicker;
	delete permBoostView;
	
	delete item;
	delete ui;
}

void ItemEditFrame::hideAllViews() {
	generalView->hide();
	baseStatsView->hide();
	modsPicker->hide();
	modsView->hide();
	permBoostView->hide();
	permBoostPicker->hide();
}

void ItemEditFrame::on_baseStatsButton_released()
{
    hideAllViews();
	baseStatsView->show();
}

void ItemEditFrame::on_modsButton_released()
{
	hideAllViews();
	modsView->show();
}

void ItemEditFrame::on_generalButton_released()
{
    hideAllViews();
	generalView->refreshGeneralData();
	generalView->show();
}

void ItemEditFrame::on_cancelButton_released()
{
    this->close();
}

void ItemEditFrame::on_acceptButton_released()
{
	itemEditCallback->onItemEdited(item, oldItem);
    this->close();
}

void ItemEditFrame::on_permBoostButton_released()
{
	hideAllViews();
    permBoostView->show();
}
