#include "GameCharacter.h"

std::vector<LsbObject *> GameCharacter::getSkillList() {
	std::vector<LsbObject *> skillObjects;
	LsbObject *skillManagerObject = LsbReader::lookupByUniquePathEntity(this->getObject(), "SkillManager");
	if (skillManagerObject != 0) {
		return LsbReader::lookupAllEntitiesWithName(skillManagerObject, "Skills");
	}
	return skillObjects;
}

void GameCharacter::addSkill(const char *skillName) {
	LsbObject *skillManagerObject = LsbReader::lookupByUniquePathEntity(this->getObject(), "SkillManager");
	if (skillManagerObject != 0) {
		TAG_LSB *skillsTag = LsbReader::createTagIfNeeded("Skills", &tagList);
		LsbObject *skillsObject = new LsbObject(true, skillsTag->index, skillsTag->tag, 0, skillManagerObject, &tagList);
		
		TAG_LSB *activeCooldownTag = LsbReader::createTagIfNeeded("ActiveCooldown", &tagList);
		LsbObject *activeCooldownObject = new LsbObject(false, activeCooldownTag->index, activeCooldownTag->tag, 6, skillsObject, &tagList);
		float activeCooldown = 0.0f;
		activeCooldownObject->setData((char *)&activeCooldown, sizeof(float));
		
		TAG_LSB *isLearnedTag = LsbReader::createTagIfNeeded("IsLearned", &tagList);
		LsbObject *isLearnedObject = new LsbObject(false, isLearnedTag->index, isLearnedTag->tag, 19, skillsObject, &tagList);
		bool isLearned = true;
		isLearnedObject->setData((char *)&isLearned, sizeof(bool));
		
		TAG_LSB *mapKeyTag = LsbReader::createTagIfNeeded("MapKey", &tagList);
		LsbObject *mapKeyObject = new LsbObject(false, mapKeyTag->index, mapKeyTag->tag, 0x16, skillsObject, &tagList);
		mapKeyObject->setData(skillName, strlen(skillName) + 1);
		
		skillsObject->addChild(activeCooldownObject);
		skillsObject->addChild(isLearnedObject);
		skillsObject->addChild(mapKeyObject);
		
		skillManagerObject->addChild(skillsObject);
		
		TAG_LSB *timeItemAddedToSkillManagerTag = LsbReader::createTagIfNeeded("TimeItemAddedToSkillManager", &tagList);
		LsbObject *timeItemAddedToSkillManagerObject = new LsbObject(true, timeItemAddedToSkillManagerTag->index, timeItemAddedToSkillManagerTag->tag, 0, skillManagerObject, &tagList);
		
		TAG_LSB *mapKeyTag2 = LsbReader::createTagIfNeeded("MapKey", &tagList);
		LsbObject *mapKeyObject2 = new LsbObject(false, mapKeyTag2->index, mapKeyTag2->tag, 0x16, timeItemAddedToSkillManagerObject, &tagList);
		mapKeyObject2->setData(skillName, strlen(skillName) + 1);
		
		TAG_LSB *mapValueTag = LsbReader::createTagIfNeeded("MapValue", &tagList);
		LsbObject *mapValueObject = new LsbObject(false, mapValueTag->index, mapValueTag->tag, 0x07, timeItemAddedToSkillManagerObject, &tagList);
		double mapValue = 0;
		mapValueObject->setData((char *)&mapValue, sizeof(mapValue));
		
		timeItemAddedToSkillManagerObject->addChild(mapKeyObject2);
		timeItemAddedToSkillManagerObject->addChild(mapValueObject);
		
		skillManagerObject->addChild(timeItemAddedToSkillManagerObject);
	}
}

unsigned long GameCharacter::getInventoryId() {
	return *((long *)LsbReader::lookupByUniquePathEntity(this->getObject(), "Inventory")->getData());
}

unsigned long GameCharacter::getCreatorId() {
	LsbObject *creatorObject = LsbReader::getObjectCreator(this->getObject());
	if (creatorObject != 0) {
		LsbObject *handleObject = LsbReader::lookupByUniquePathEntity(creatorObject, "Handle");
		if (handleObject != 0) {
			return *((unsigned long *)handleObject->getData());
		}
	}
	return 0;
}

void GameCharacter::ensureInventoryCapacity(LsbObject *viewMapValueObject, unsigned long viewSlot) {
	std::vector<LsbObject *> itemsObjects = LsbReader::lookupAllEntitiesWithName(viewMapValueObject, "Items");
	long additionalSlotsRequired = (viewSlot + 1) - itemsObjects.size();
	for (int i=0; i<additionalSlotsRequired; ++i) {
		TAG_LSB *itemsTag = LsbReader::createTagIfNeeded("Items", &tagList);
		LsbObject *itemsObject = new LsbObject(true, itemsTag->index, itemsTag->tag, 0, viewMapValueObject, &tagList);
		
		TAG_LSB *objectTag = LsbReader::createTagIfNeeded("Object", &tagList);
		LsbObject *objectObject = new LsbObject(false, objectTag->index, objectTag->tag, 0x05, itemsObject, &tagList);
		unsigned long invalidHandle = 0;
		objectObject->setData((char *)&invalidHandle, sizeof(invalidHandle));
		
		itemsObject->addChild(objectObject);
		
		viewMapValueObject->addChild(itemsObject);
	}
}

bool GameCharacter::addItemToInventoryObject(LsbObject *itemCreatorObject, unsigned long viewSlot, unsigned long extraInventoryTab, unsigned long extraViewSlot, bool equippedItem) {
	LsbObject *handleObject = LsbReader::lookupByUniquePathEntity(itemCreatorObject, "Handle");
	if (handleObject != 0) {
		unsigned long creatorHandle = *((unsigned long *)handleObject->getData());
		LsbObject *inventoryObject = this->getInventoryObject();
		
		{
			TAG_LSB *timeItemAddedToInventoryTag = LsbReader::createTagIfNeeded("TimeItemAddedToInventory", &tagList);
			LsbObject *timeAddedToInventoryObject = new LsbObject(true, timeItemAddedToInventoryTag->index, timeItemAddedToInventoryTag->tag, 0, inventoryObject, &tagList);
			
			TAG_LSB *mapKeyTag = LsbReader::createTagIfNeeded("MapKey", &tagList);
			LsbObject *mapKeyObject = new LsbObject(false, mapKeyTag->index, mapKeyTag->tag, 0x05, timeAddedToInventoryObject, &tagList);
			unsigned long mapKey = creatorHandle;
			mapKeyObject->setData((char *)&mapKey, sizeof(mapKey));
			
			TAG_LSB *mapValueTag = LsbReader::createTagIfNeeded("MapValue", &tagList);
			LsbObject *mapValueObject = new LsbObject(false, mapValueTag->index, mapValueTag->tag, 0x07, timeAddedToInventoryObject, &tagList);
			double mapValue = 0.0;
			mapValueObject->setData((char *)&mapValue, sizeof(mapValue));
			
			timeAddedToInventoryObject->addChild(mapKeyObject);
			timeAddedToInventoryObject->addChild(mapValueObject);
			
			inventoryObject->addChild(timeAddedToInventoryObject);
		}
		
		if (!equippedItem) {
			std::vector<LsbObject *> viewsObjects = LsbReader::lookupAllEntitiesWithName(inventoryObject, "Views");
			for (int i=0; i<viewsObjects.size(); ++i) {
				LsbObject *viewObject = viewsObjects[i];
				if (viewObject != 0) {
					LsbObject *mapKeyObject = LsbReader::lookupByUniquePathEntity(viewObject, "MapKey");
					if (mapKeyObject != 0) {
						unsigned long viewId = *((long *)mapKeyObject->getData());
						
						unsigned long currentSlot = viewSlot;
						if (extraInventoryTab != 0 && viewId == extraInventoryTab) {
							currentSlot = extraViewSlot;
						}
						if (viewId == 0 || viewId == extraInventoryTab) {
							LsbObject *mapValueObject = LsbReader::lookupByUniquePathEntity(viewObject, "MapValue");
							
							TAG_LSB *indicesTag = LsbReader::createTagIfNeeded("Indices", &tagList);
							LsbObject *indicesObject = new LsbObject(true, indicesTag->index, indicesTag->tag, 0, mapValueObject, &tagList);
							
							TAG_LSB *mapKeyTag2 = LsbReader::createTagIfNeeded("MapKey", &tagList);
							LsbObject *mapKeyObject2 = new LsbObject(false, mapKeyTag2->index, mapKeyTag2->tag, 0x05, indicesObject, &tagList);
							mapKeyObject2->setData((char *)&creatorHandle, sizeof(creatorHandle));
							
							TAG_LSB *mapValueTag2 = LsbReader::createTagIfNeeded("MapValue", &tagList);
							LsbObject *mapValueObject2 = new LsbObject(false, mapValueTag2->index, mapValueTag2->tag, 0x05, indicesObject, &tagList);
							mapValueObject2->setData((char *)&currentSlot, sizeof(currentSlot));
							
							indicesObject->addChild(mapKeyObject2);
							indicesObject->addChild(mapValueObject2);
							mapValueObject->addChild(indicesObject);

							ensureInventoryCapacity(mapValueObject, currentSlot);
							
							TAG_LSB *itemsTag = LsbReader::createTagIfNeeded("Items", &tagList);
							LsbObject *itemsObject = new LsbObject(true, itemsTag->index, itemsTag->tag, 0, mapValueObject, &tagList);
							
							TAG_LSB *objectTag = LsbReader::createTagIfNeeded("Object", &tagList);
							LsbObject *objectObject = new LsbObject(false, objectTag->index, objectTag->tag, 0x05, itemsObject, &tagList);
							objectObject->setData((char *)&creatorHandle, sizeof(creatorHandle));
							
							itemsObject->addChild(objectObject);
							
							std::vector<LsbObject *> itemsObjects = LsbReader::lookupAllEntitiesWithName(mapValueObject, "Items");
							if (currentSlot < itemsObjects.size()) {
								LsbObject *childToReplace = itemsObjects[currentSlot];
								mapValueObject->replaceChild(childToReplace, itemsObject);
							}
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

LsbObject *GameCharacter::getInventoryObject() {
	long inventoryId = *((long *)LsbReader::lookupByUniquePathEntity(this->getObject(), "Inventory")->getData());
	LsbObject *inventoryCreators = LsbReader::lookupByUniquePath(globals, "Inventories/root/InventoryFactory/Creators");
	std::vector<LsbObject *> creatorMatches = LsbReader::findItemsByAttribute(inventoryCreators->getChildren(), "Object", (const char *)&inventoryId, sizeof(long));
	if (creatorMatches.size() == 1) {
		LsbObject *creator = creatorMatches[0];
		LsbObject *inventoryObject = LsbReader::getObjectFromCreator(creator, "Inventories");
		return inventoryObject;
	}
	return 0;
}

void GameCharacter::removeSkill(const char *skillName) {
	LsbObject *skillManagerObject = LsbReader::lookupByUniquePathEntity(this->getObject(), "SkillManager");
	if (skillManagerObject != 0) {
		std::vector<LsbObject *> skillObjects = getSkillList();
		for (int i=0; i<skillObjects.size(); ++i) {
			LsbObject *skillObject = skillObjects[i];
			LsbObject *mapKeyObject = LsbReader::lookupByUniquePathEntity(skillObject, "MapKey");
			if (mapKeyObject != 0) {
				std::string name = skillName;
				if (mapKeyObject->getData() == name) {
					skillManagerObject->removeChild(skillObject);
					break;
				}
			}
		}
		std::vector<LsbObject *> timeAddedObjects = LsbReader::lookupAllEntitiesWithName(skillManagerObject, "TimeItemAddedToSkillManager");
		for (int i=0; i<timeAddedObjects.size(); ++i) {
			LsbObject *timeAddedObject = timeAddedObjects[i];
			LsbObject *mapKeyObject = LsbReader::lookupByUniquePathEntity(timeAddedObject, "MapKey");
			if (mapKeyObject != 0) {
				std::string name = skillName;
				if (mapKeyObject->getData() == name) {
					skillManagerObject->removeChild(timeAddedObject);
					break;
				}
			}
		}
	}
}

std::vector<LsbObject *> GameCharacter::getAbilityList() {
	std::vector<LsbObject *> abilityObjects;
	LsbObject *playerUpgradeObject = LsbReader::lookupByUniquePathEntity(this->getObject(), "PlayerData/PlayerUpgrade");
	if (playerUpgradeObject != 0) {
		std::vector<LsbObject *> abilitiesObjects = LsbReader::lookupAllEntitiesWithName(playerUpgradeObject, "Abilities");
		for (int i=0; i<abilitiesObjects.size(); ++i) {
			abilityObjects.push_back(LsbReader::lookupByUniquePathEntity(abilitiesObjects[i], "Object"));
		}
	}
	return abilityObjects;
}

std::vector<LsbObject *> GameCharacter::getTraitList() {
	std::vector<LsbObject *> traitObjects;
	LsbObject *playerUpgradeObject = LsbReader::lookupByUniquePathEntity(this->getObject(), "PlayerData/PlayerUpgrade");
	if (playerUpgradeObject != 0) {
		std::vector<LsbObject *> traitsObjects = LsbReader::lookupAllEntitiesWithName(playerUpgradeObject, "Traits");
		for (int i=0; i<traitsObjects.size(); ++i) {
			traitObjects.push_back(LsbReader::lookupByUniquePathEntity(traitsObjects[i], "Object"));
		}
	}
	return traitObjects;
}

bool GameCharacter::hasTalent(long talentId) {
	LsbObject *upgradeObject = LsbReader::lookupByUniquePathEntity(this->getObject(), "PlayerData/PlayerUpgrade");
	if (upgradeObject != 0) {
		std::vector<LsbObject *> talentList = LsbReader::lookupAllEntitiesWithName(upgradeObject, "Talents");
		std::vector<LsbObject *> talentObjects;
		for (int i=0; i<talentList.size(); ++i) {
			LsbObject *innerObject = LsbReader::lookupByUniquePathEntity(talentList[i], "Object");
			if (innerObject != 0) {
				talentObjects.push_back(innerObject);
			}
		}
		long listIndex = talentId / 32;
		if (listIndex < talentObjects.size()) {
			unsigned long bitmask = 1 << (talentId % 32);
			unsigned long value = *((unsigned long *)talentObjects[listIndex]->getData());
			return (value & bitmask) != 0;
		}
	}
	return false;
}

void GameCharacter::setTalent(long talentId, bool enabled) {
	LsbObject *upgradeObject = LsbReader::lookupByUniquePathEntity(this->getObject(), "PlayerData/PlayerUpgrade");
	if (upgradeObject != 0) {
		std::vector<LsbObject *> talentList = LsbReader::lookupAllEntitiesWithName(upgradeObject, "Talents");
		std::vector<LsbObject *> talentObjects;
		for (int i=0; i<talentList.size(); ++i) {
			LsbObject *innerObject = LsbReader::lookupByUniquePathEntity(talentList[i], "Object");
			if (innerObject != 0) {
				talentObjects.push_back(innerObject);
			}
		}
		long listIndex = talentId / 32;
		if (listIndex < talentObjects.size()) {
			unsigned long bitmask = 1 << (talentId % 32);
			unsigned long value = *((unsigned long *)talentObjects[listIndex]->getData());
			if (enabled) {
				value = value | bitmask;
			} else {
				value = value & ~bitmask;
			}
			talentObjects[listIndex]->setData((char *)&value, sizeof(unsigned long));
		}
	}
}

GameCharacter::GameCharacter(std::vector<LsbObject *> &globals, std::vector<TAG_LSB *> &tagList) : tagList(tagList), globals(globals)
{
}
