#include "GameCharacter.h"

std::vector<LsbObject *> GameCharacter::getSkillList() {
	std::vector<LsbObject *> skillObjects;
	LsbObject *skillManagerObject = this->getObject()->lookupByUniquePath("SkillManager");
	if (skillManagerObject != 0) {
		return LsbObject::lookupAllEntitiesWithName(skillManagerObject, "Skills");
	}
	return skillObjects;
}

void GameCharacter::addSkill(const char *skillName) {
	LsbObject *skillManagerObject = this->getObject()->lookupByUniquePath("SkillManager");
	if (skillManagerObject != 0) {
		TAG_LSB *skillsTag = LsbObject::createTagIfNeeded("Skills", &tagList);
		LsbObject *skillsObject = new LsbObject(true, skillsTag->index, skillsTag->tag, 0, skillManagerObject, &tagList);
		
		TAG_LSB *activeCooldownTag = LsbObject::createTagIfNeeded("ActiveCooldown", &tagList);
		LsbObject *activeCooldownObject = new LsbObject(false, activeCooldownTag->index, activeCooldownTag->tag, 6, skillsObject, &tagList);
		float activeCooldown = 0.0f;
		activeCooldownObject->setData((char *)&activeCooldown, sizeof(float));
		
		TAG_LSB *isLearnedTag = LsbObject::createTagIfNeeded("IsLearned", &tagList);
		LsbObject *isLearnedObject = new LsbObject(false, isLearnedTag->index, isLearnedTag->tag, 19, skillsObject, &tagList);
		bool isLearned = true;
		isLearnedObject->setData((char *)&isLearned, sizeof(bool));
		
		TAG_LSB *mapKeyTag = LsbObject::createTagIfNeeded("MapKey", &tagList);
		LsbObject *mapKeyObject = new LsbObject(false, mapKeyTag->index, mapKeyTag->tag, 0x16, skillsObject, &tagList);
		mapKeyObject->setData(skillName, strlen(skillName) + 1);
		
		skillsObject->addChild(activeCooldownObject);
		skillsObject->addChild(isLearnedObject);
		skillsObject->addChild(mapKeyObject);
		
		skillManagerObject->insertLast(skillsObject, "Skills");
		
		TAG_LSB *timeItemAddedToSkillManagerTag = LsbObject::createTagIfNeeded("TimeItemAddedToSkillManager", &tagList);
		LsbObject *timeItemAddedToSkillManagerObject = new LsbObject(true, timeItemAddedToSkillManagerTag->index, timeItemAddedToSkillManagerTag->tag, 0, skillManagerObject, &tagList);
		
		TAG_LSB *mapKeyTag2 = LsbObject::createTagIfNeeded("MapKey", &tagList);
		LsbObject *mapKeyObject2 = new LsbObject(false, mapKeyTag2->index, mapKeyTag2->tag, 0x16, timeItemAddedToSkillManagerObject, &tagList);
		mapKeyObject2->setData(skillName, strlen(skillName) + 1);
		
		TAG_LSB *mapValueTag = LsbObject::createTagIfNeeded("MapValue", &tagList);
		LsbObject *mapValueObject = new LsbObject(false, mapValueTag->index, mapValueTag->tag, 0x07, timeItemAddedToSkillManagerObject, &tagList);
		double mapValue = 0;
		mapValueObject->setData((char *)&mapValue, sizeof(mapValue));
		
		timeItemAddedToSkillManagerObject->addChild(mapKeyObject2);
		timeItemAddedToSkillManagerObject->addChild(mapValueObject);
		
		skillManagerObject->addChild(timeItemAddedToSkillManagerObject);
	}
}

unsigned long GameCharacter::getInventoryId() {
	return *((long *)this->getObject()->lookupByUniquePath("Inventory")->getData());
}

unsigned long GameCharacter::getCreatorId() {
	LsbObject *creatorObject = LsbObject::getObjectCreator(this->getObject());
	if (creatorObject != 0) {
		LsbObject *handleObject = creatorObject->lookupByUniquePath("Handle");
		if (handleObject != 0) {
			return *((unsigned long *)handleObject->getData());
		}
	}
	return 0;
}

void GameCharacter::ensureInventoryCapacity(LsbObject *viewMapValueObject, unsigned long viewSlot) {
	std::vector<LsbObject *> itemsObjects = LsbObject::lookupAllEntitiesWithName(viewMapValueObject, "Items");
	long additionalSlotsRequired = (viewSlot + 1) - itemsObjects.size();
	for (int i=0; i<additionalSlotsRequired; ++i) {
		TAG_LSB *itemsTag = LsbObject::createTagIfNeeded("Items", &tagList);
		LsbObject *itemsObject = new LsbObject(true, itemsTag->index, itemsTag->tag, 0, viewMapValueObject, &tagList);
		
		TAG_LSB *objectTag = LsbObject::createTagIfNeeded("Object", &tagList);
		LsbObject *objectObject = new LsbObject(false, objectTag->index, objectTag->tag, 0x05, itemsObject, &tagList);
		unsigned long invalidHandle = 0;
		objectObject->setData((char *)&invalidHandle, sizeof(invalidHandle));
		
		itemsObject->addChild(objectObject);
		
		viewMapValueObject->addChild(itemsObject);
	}
}

bool GameCharacter::addItemToInventoryObject(LsbObject *itemCreatorObject, unsigned long viewSlot, unsigned long extraInventoryTab, unsigned long extraViewSlot, bool equippedItem) {
	LsbObject *handleObject = itemCreatorObject->lookupByUniquePath("Handle");
	if (handleObject != 0) {
		unsigned long creatorHandle = *((unsigned long *)handleObject->getData());
		LsbObject *inventoryObject = this->getInventoryObject();
		
		{
			TAG_LSB *timeItemAddedToInventoryTag = LsbObject::createTagIfNeeded("TimeItemAddedToInventory", &tagList);
			LsbObject *timeAddedToInventoryObject = new LsbObject(true, timeItemAddedToInventoryTag->index, timeItemAddedToInventoryTag->tag, 0, inventoryObject, &tagList);
			
			TAG_LSB *mapKeyTag = LsbObject::createTagIfNeeded("MapKey", &tagList);
			LsbObject *mapKeyObject = new LsbObject(false, mapKeyTag->index, mapKeyTag->tag, 0x05, timeAddedToInventoryObject, &tagList);
			unsigned long mapKey = creatorHandle;
			mapKeyObject->setData((char *)&mapKey, sizeof(mapKey));
			
			TAG_LSB *mapValueTag = LsbObject::createTagIfNeeded("MapValue", &tagList);
			LsbObject *mapValueObject = new LsbObject(false, mapValueTag->index, mapValueTag->tag, 0x07, timeAddedToInventoryObject, &tagList);
			double mapValue = 0.0;
			mapValueObject->setData((char *)&mapValue, sizeof(mapValue));
			
			timeAddedToInventoryObject->addChild(mapKeyObject);
			timeAddedToInventoryObject->addChild(mapValueObject);
			
			inventoryObject->insertLast(timeAddedToInventoryObject, "TimeItemAddedToInventory");
		}
		
		if (!equippedItem) {
			std::vector<LsbObject *> viewsObjects = LsbObject::lookupAllEntitiesWithName(inventoryObject, "Views");
			for (int i=0; i<viewsObjects.size(); ++i) {
				LsbObject *viewObject = viewsObjects[i];
				if (viewObject != 0) {
					LsbObject *mapKeyObject = viewObject->lookupByUniquePath("MapKey");
					if (mapKeyObject != 0) {
						unsigned long viewId = *((long *)mapKeyObject->getData());
						
						unsigned long currentSlot = viewSlot;
						if (extraInventoryTab != 0 && viewId == extraInventoryTab) {
							currentSlot = extraViewSlot;
						}
						if (viewId == 0 || viewId == extraInventoryTab) {
							LsbObject *mapValueObject = viewObject->lookupByUniquePath("MapValue");
							
							TAG_LSB *indicesTag = LsbObject::createTagIfNeeded("Indices", &tagList);
							LsbObject *indicesObject = new LsbObject(true, indicesTag->index, indicesTag->tag, 0, mapValueObject, &tagList);
							
							TAG_LSB *mapKeyTag2 = LsbObject::createTagIfNeeded("MapKey", &tagList);
							LsbObject *mapKeyObject2 = new LsbObject(false, mapKeyTag2->index, mapKeyTag2->tag, 0x05, indicesObject, &tagList);
							mapKeyObject2->setData((char *)&creatorHandle, sizeof(creatorHandle));
							
							TAG_LSB *mapValueTag2 = LsbObject::createTagIfNeeded("MapValue", &tagList);
							LsbObject *mapValueObject2 = new LsbObject(false, mapValueTag2->index, mapValueTag2->tag, 0x05, indicesObject, &tagList);
							mapValueObject2->setData((char *)&currentSlot, sizeof(currentSlot));
							
							indicesObject->addChild(mapKeyObject2);
							indicesObject->addChild(mapValueObject2);
							mapValueObject->insertLast(indicesObject, "Indices");

							ensureInventoryCapacity(mapValueObject, currentSlot);
							
							TAG_LSB *itemsTag = LsbObject::createTagIfNeeded("Items", &tagList);
							LsbObject *itemsObject = new LsbObject(true, itemsTag->index, itemsTag->tag, 0, mapValueObject, &tagList);
							
							TAG_LSB *objectTag = LsbObject::createTagIfNeeded("Object", &tagList);
							LsbObject *objectObject = new LsbObject(false, objectTag->index, objectTag->tag, 0x05, itemsObject, &tagList);
							objectObject->setData((char *)&creatorHandle, sizeof(creatorHandle));
							
							itemsObject->addChild(objectObject);
							
							std::vector<LsbObject *> itemsObjects = LsbObject::lookupAllEntitiesWithName(mapValueObject, "Items");
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
	long inventoryId = *((long *)this->getObject()->lookupByUniquePath("Inventory")->getData());
	LsbObject *inventoryCreators = LsbObject::lookupByUniquePath(globals, "Inventories/root/InventoryFactory/Creators");
	std::vector<LsbObject *> creatorMatches = LsbObject::findItemsByAttribute(inventoryCreators->getChildren(), "Object", (const char *)&inventoryId, sizeof(long));
	if (creatorMatches.size() == 1) {
		LsbObject *creator = creatorMatches[0];
		LsbObject *inventoryObject = LsbObject::getObjectFromCreator(creator, "Inventories");
		return inventoryObject;
	}
	return 0;
}

void GameCharacter::removeSkill(const char *skillName) {
	LsbObject *skillManagerObject = this->getObject()->lookupByUniquePath("SkillManager");
	if (skillManagerObject != 0) {
		std::vector<LsbObject *> skillObjects = getSkillList();
		for (int i=0; i<skillObjects.size(); ++i) {
			LsbObject *skillObject = skillObjects[i];
			LsbObject *mapKeyObject = skillObject->lookupByUniquePath("MapKey");
			if (mapKeyObject != 0) {
				std::string name = skillName;
				if (mapKeyObject->getData() == name) {
					skillManagerObject->removeChild(skillObject);
					break;
				}
			}
		}
		std::vector<LsbObject *> timeAddedObjects = LsbObject::lookupAllEntitiesWithName(skillManagerObject, "TimeItemAddedToSkillManager");
		for (int i=0; i<timeAddedObjects.size(); ++i) {
			LsbObject *timeAddedObject = timeAddedObjects[i];
			LsbObject *mapKeyObject = timeAddedObject->lookupByUniquePath("MapKey");
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
	LsbObject *playerUpgradeObject = this->getObject()->lookupByUniquePath("PlayerData/PlayerUpgrade");
	if (playerUpgradeObject != 0) {
		std::vector<LsbObject *> abilitiesObjects = LsbObject::lookupAllEntitiesWithName(playerUpgradeObject, "Abilities");
		for (int i=0; i<abilitiesObjects.size(); ++i) {
			abilityObjects.push_back(abilitiesObjects[i]->lookupByUniquePath("Object"));
		}
	}
	return abilityObjects;
}

std::vector<LsbObject *> GameCharacter::getTraitList() {
	std::vector<LsbObject *> traitObjects;
	LsbObject *playerUpgradeObject = this->getObject()->lookupByUniquePath("PlayerData/PlayerUpgrade");
	if (playerUpgradeObject != 0) {
		std::vector<LsbObject *> traitsObjects = LsbObject::lookupAllEntitiesWithName(playerUpgradeObject, "Traits");
		for (int i=0; i<traitsObjects.size(); ++i) {
			traitObjects.push_back(traitsObjects[i]->lookupByUniquePath("Object"));
		}
	}
	return traitObjects;
}

bool GameCharacter::hasTalent(long talentId) {
	LsbObject *upgradeObject = this->getObject()->lookupByUniquePath("PlayerData/PlayerUpgrade");
	if (upgradeObject != 0) {
		std::vector<LsbObject *> talentList = LsbObject::lookupAllEntitiesWithName(upgradeObject, "Talents");
		std::vector<LsbObject *> talentObjects;
		for (int i=0; i<talentList.size(); ++i) {
			LsbObject *innerObject = talentList[i]->lookupByUniquePath("Object");
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
	LsbObject *upgradeObject = this->getObject()->lookupByUniquePath("PlayerData/PlayerUpgrade");
	if (upgradeObject != 0) {
		std::vector<LsbObject *> talentList = LsbObject::lookupAllEntitiesWithName(upgradeObject, "Talents");
		std::vector<LsbObject *> talentObjects;
		for (int i=0; i<talentList.size(); ++i) {
			LsbObject *innerObject = talentList[i]->lookupByUniquePath("Object");
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
