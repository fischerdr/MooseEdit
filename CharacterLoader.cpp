#include <string>
#include "CharacterLoader.h"
#include "LsbReader.h"
#include "LsbObject.h"

#include <QApplication>
#include <QTreeWidget>
#include <QProgressDialog>
#include <QLineEdit>
#include "charactertab.h"

std::vector<std::vector<GameItem *> > &CharacterLoader::getEquipmentSets()
{
	return equipmentSets;
}

void CharacterLoader::setEquipmentSets(const std::vector<std::vector<GameItem *> > &value)
{
	equipmentSets = value;
}
CharacterLoader::CharacterLoader()
{
}

void CharacterLoader::load(std::vector<LsbObject *> &globals, std::wstring gameDataPath, std::vector<TAG_LSB *> *globalTagList, QWidget *mainWindow) {
	LsbObject *characters = LsbObject::lookupByUniquePath(globals, "Characters/root/CharacterFactory/Party/Characters");
	QTreeWidget *tree = mainWindow->findChild<QTreeWidget *>("treeWidget");
	tree->clear();
	//displayAllItems2(tree, characters->getChildren());
	std::vector<LsbObject *> characterCreatorHandles = LsbObject::extractPropertyForEachListItem(characters->getChildren(), "Handle");
	LsbObject *creators = LsbObject::lookupByUniquePath(globals, "Characters/root/CharacterFactory/Creators");
	std::vector<LsbObject *> matchingCharacterCreators;
	int characterLoadCounter = characterCreatorHandles.size() * 5 - 1;
	QProgressDialog characterProgress("Processing character data...", QString(), 0, characterLoadCounter, mainWindow);
	characterProgress.setWindowFlags(characterProgress.windowFlags() & ~(Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint));
	characterProgress.setWindowModality(Qt::WindowModal);
	characterProgress.show();
	QApplication::processEvents();
	for (int i=0; i<characterCreatorHandles.size(); ++i) {
		long handleId = characterCreatorHandles[i]->getIntData();
		std::vector<LsbObject *> matches = LsbObject::findItemsByAttribute(creators->getChildren(), "Handle", (const char *)&handleId, sizeof(handleId));
		if (matches.size() == 1) {
			matchingCharacterCreators.push_back(matches[0]);
		}
		else {
			std::cout<<"Found zero or multiple matches for character handle!\n";
		}
		characterProgress.setValue(characterProgress.value() + 1);
		QApplication::processEvents();
	}
	std::vector<LsbObject *> templateIds = LsbObject::extractPropertyForEachListItem(matchingCharacterCreators, "TemplateID");
	//displayAllItems2(tree, matchingCharacterCreators);
	LsbObject *allCharacters = LsbObject::lookupByUniquePath(globals, "Characters/root/CharacterFactory/Characters");
	std::vector<LsbObject *> matchingCharacters;
	for (int i=0; i<templateIds.size(); ++i) {
		std::string templateId = templateIds[i]->getData();
		std::vector<LsbObject *> matches = LsbObject::findItemsByAttribute(allCharacters->getChildren(), "OriginalTemplate", templateId.c_str(), templateId.length() + 1);
		if (matches.size() == 1) {
			matchingCharacters.push_back(matches[0]);
		}
		else {
			std::cout<<"Found zero or multiple matches for character TemplateID!\n";
		}
		characterProgress.setValue(characterProgress.value() + 1);
		QApplication::processEvents();
	}
	//displayAllItems2(tree, matchingCharacters);
	
	QTabWidget *tabWidget = mainWindow->findChild<QTabWidget *>("tabWidget");
	this->getCharacterGroup().setTabWidget(tabWidget);
	for (int i=matchingCharacters.size() - 1; i>=0; --i) {
		LsbObject *character = matchingCharacters[i];
		std::ostringstream ss;
		ss<<"charTab"<<i;
		LsbObject *itemsObject = LsbObject::lookupByUniquePath(globals, "Items/root/ItemFactory/Items");
		QWidget *widget = new characterTab(globalTagList, gameDataPath, itemsObject, this, tabWidget, mainWindow);
		widget->setObjectName(QString(ss.str().c_str()));
		
		LsbObject *origTemplate = LsbObject::lookupByUniquePathEntity(character, "OriginalTemplate");
		LsbObject *playerName = LsbObject::lookupByUniquePathEntity(character, "PlayerData/PlayerCustomData/Name");
		std::string origTemplateId = origTemplate->getData();
		std::wstring charName;
		if (origTemplateId == "5c5447e5-c1cf-4677-b84b-006d9be3f075") {
			charName = L"Madora";
		}
		else if (origTemplateId == "80240f83-778e-4753-850b-48b05729589c") {
			charName = L"Jahan";
		}
		else {
			charName = (wchar_t *)playerName->getData();
		}
		this->getCharacterGroup().getCharacters().insert(this->getCharacterGroup().getCharacters().begin(), new GameCharacter(globals, *globalTagList));
		GameCharacter *gameCharacter = this->getCharacterGroup().getCharacters()[0];
		gameCharacter->setName(charName);
		gameCharacter->setObject(character);
		gameCharacter->setWidget(widget);
		((characterTab *)widget)->setCharacter(gameCharacter);
		QLineEdit *nameEdit = widget->findChild<QLineEdit *>(QString("nameEdit"));
		nameEdit->setText(QString::fromStdWString(charName));
		tabWidget->insertTab(1, widget, QString::fromStdWString(charName));
		characterProgress.setValue(characterProgress.value() + 1);
		QApplication::processEvents();
	}
	
	//compile item list
	LsbObject *allItems = LsbObject::lookupByUniquePath(globals, "Items/root/ItemFactory/Items");
	
	for (int i=0; i<characterCreatorHandles.size(); ++i) {
		long handleId = characterCreatorHandles[i]->getIntData();
		std::vector<LsbObject *> matches = LsbObject::findItemsByAttribute(allItems->getChildren(), "owner", (const char *)&handleId, sizeof(handleId));
		long parentId = 0;
		std::vector<GameItem *> equipmentSet;
		for (int j=0; j<matches.size(); ++j) {
			LsbObject *match = matches[j];
			LsbObject *slotObject = LsbObject::lookupByUniquePathEntity(match, "Slot");
			std::vector<LsbObject *> deleteMe;
			if (slotObject != 0) {
				unsigned short slot = *((unsigned short *)slotObject->getData());
				if (slot >= 0 && slot <= EQUIP_SLOTS) {
					GameItem *equipmentItem = new GameItem(globalTagList);
					equipmentItem->setObject(match);
					equipmentSet.push_back(equipmentItem);
					deleteMe.push_back(match);
				}
			}
			if (i == 0) {
				//displayAllItems2(tree, deleteMe);
			}
		}
		equipmentSets.push_back(equipmentSet);
		characterProgress.setValue(characterProgress.value() + 1);
		QApplication::processEvents();
	}
	
	
	typedef std::map<unsigned long, ItemHandleData> ItemHandleMap;
	ItemHandleMap itemHandleMap;
	for (int i=0; i<this->getCharacterGroup().getCharacters().size(); ++i) {
		//if (i != 0)
			//continue;
		LsbObject *character = this->getCharacterGroup().getCharacters()[i]->getObject();
		long inventoryId = *((long *)LsbObject::lookupByUniquePathEntity(character, "Inventory")->getData());
		LsbObject *inventoryCreators = LsbObject::lookupByUniquePath(globals, "Inventories/root/InventoryFactory/Creators");
		std::vector<LsbObject *> creatorMatches = LsbObject::findItemsByAttribute(inventoryCreators->getChildren(), "Object", (const char *)&inventoryId, sizeof(long));
		if (creatorMatches.size() == 1) {
			LsbObject *creator = creatorMatches[0];
			LsbObject *inventory = LsbObject::getObjectFromCreator(creator, "Inventories");
			std::vector<LsbObject *> inventoryHolder; //TODO: remove me
			inventoryHolder.push_back(inventory); //TODO: remove me
			//displayAllItems2(tree, inventoryHolder); //TODO: remove me
			std::vector<LsbObject *> views = LsbObject::lookupAllEntitiesWithName(inventory, "Views");
			if (views.size() > 0) {
				for (int k=0; k<views.size(); ++k) {
					LsbObject *viewMapKey = LsbObject::lookupByUniquePathEntity(views[k], "MapKey");
					unsigned long viewId = *((unsigned long*)viewMapKey->getData());
					LsbObject *viewMapValue = LsbObject::lookupByUniquePathEntity(views[k], "MapValue");
					if (viewMapValue != 0) {
						std::vector<LsbObject *> indicesList = LsbObject::lookupAllEntitiesWithName(viewMapValue, "Indices");
						for (int j=0; j<indicesList.size(); ++j) {
							LsbObject *index = indicesList[j];
							unsigned long itemCreatorHandle = *((unsigned long *)LsbObject::lookupByUniquePathEntity(index, "MapKey")->getData());
							unsigned long slot = *((unsigned long *)LsbObject::lookupByUniquePathEntity(index, "MapValue")->getData());
							if (i == 0){
								std::cout<<"slot = "<<slot<<'\n';
							}
							if (itemHandleMap.find(itemCreatorHandle) == itemHandleMap.end()) {
								itemHandleMap[itemCreatorHandle] = ItemHandleData();
							}
							ItemHandleData &itemHandleData = itemHandleMap[itemCreatorHandle];
							itemHandleData.characterId = i;
							ViewSlotMap &viewSlotMap = itemHandleData.viewSlotMap;
							viewSlotMap[viewId] = slot;
						}
					}
				}
			}
		}
		characterProgress.setValue(characterProgress.value() + 1);
	}
	
	std::vector<LsbObject *> itemList;
	for (ItemHandleMap::iterator it = itemHandleMap.begin(); it != itemHandleMap.end(); ++it) {
		ItemHandleData& itemHandleData = it->second;
		ViewSlotMap& viewSlotMap = itemHandleData.viewSlotMap;
		unsigned long itemCreatorHandle = it->first;
		
		LsbObject *itemCreators = LsbObject::lookupByUniquePath(globals, "Items/root/ItemFactory/Creators");
		std::vector<LsbObject *> itemCreatorMatches = LsbObject::findItemsByAttribute(itemCreators->getChildren(), "Handle", (char *)&itemCreatorHandle, sizeof(long));
		if (itemCreatorMatches.size() == 1) {
			LsbObject *itemCreator = itemCreatorMatches[0];
			LsbObject *item = LsbObject::getObjectFromCreator(itemCreator, "Items");
			GameItem *newItem = new GameItem(globalTagList);
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
	//displayAllItems2(tree, itemList);
}
