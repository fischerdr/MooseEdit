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
		LsbObject *mapKeyObject = new LsbObject(false, mapKeyTag->index, mapKeyTag->tag, 22, skillsObject, &tagList);
		mapKeyObject->setData(skillName, strlen(skillName) + 1);
		
		skillsObject->addChild(activeCooldownObject);
		skillsObject->addChild(isLearnedObject);
		skillsObject->addChild(mapKeyObject);
		
		skillManagerObject->addChild(skillsObject);
	}
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
