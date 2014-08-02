#include "GameCharacter.h"

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

GameCharacter::GameCharacter()
{
}
