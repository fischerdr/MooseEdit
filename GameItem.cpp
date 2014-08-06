#include "GameItem.h"
#include <windows.h>
#include "LsbReader.h"
#include <vector>

LsbObject *GameItem::createStatsDirectory() {
	TAG_LSB *statsTag = LsbReader::createTagIfNeeded("Stats", tagList);
	LsbObject *newStatsDir = new LsbObject(true, statsTag->index, statsTag->tag, 0, this->getObject(), tagList);
	
	TAG_LSB *customRequirementsTag = LsbReader::createTagIfNeeded("CustomRequirements", tagList);
	LsbObject *newCustomRequirements = new LsbObject(false, customRequirementsTag->index, customRequirementsTag->tag, 0x13, newStatsDir, tagList);
	bool custRequirements = false;
	newCustomRequirements->setData((char *)&custRequirements, sizeof(bool));
	
	TAG_LSB *durabilityTag = LsbReader::createTagIfNeeded("Durability", tagList);
	LsbObject *newDurability = new LsbObject(false, durabilityTag->index, durabilityTag->tag, 0x04, newStatsDir, tagList);
	long durability = 100;
	newDurability->setData((char *)&durability, sizeof(long));
	
	TAG_LSB *durabilityCounterTag = LsbReader::createTagIfNeeded("DurabilityCounter", tagList);
	LsbObject *newDurabilityCounter = new LsbObject(false, durabilityCounterTag->index, durabilityCounterTag->tag, 0x04, newStatsDir, tagList);
	long durabilityCounter = 8;
	newDurabilityCounter->setData((char *)&durabilityCounter, sizeof(long));
	
	TAG_LSB *isIdentifiedTag = LsbReader::createTagIfNeeded("IsIdentified", tagList);
	LsbObject *newIsIdentified = new LsbObject(false, isIdentifiedTag->index, isIdentifiedTag->tag, 0x04, newStatsDir, tagList);
	long isIdentified = 1;
	newIsIdentified->setData((char *)&isIdentified, sizeof(long));
	
	TAG_LSB *thisTypeTag = LsbReader::createTagIfNeeded("ItemType", tagList);
	LsbObject *newthisType = new LsbObject(false, thisTypeTag->index, thisTypeTag->tag, 0x16, newStatsDir, tagList);
	std::string thisType = "Magic";
	newthisType->setData(thisType.c_str(), thisType.length() + 1);
	
	TAG_LSB *levelTag = LsbReader::createTagIfNeeded("Level", tagList);
	LsbObject *newLevel = new LsbObject(false, levelTag->index, levelTag->tag, 0x04, newStatsDir, tagList);
	long level = 1;
	newLevel->setData((char *)&level, sizeof(long));
	
	TAG_LSB *repairDurabilityPenaltyTag = LsbReader::createTagIfNeeded("RepairDurabilityPenalty", tagList);
	LsbObject *newRepairDurabilityPenalty = new LsbObject(false, repairDurabilityPenaltyTag->index, repairDurabilityPenaltyTag->tag, 0x04, newStatsDir, tagList);
	long repairDurabilityPenalty = 0;
	newRepairDurabilityPenalty->setData((char *)&repairDurabilityPenalty, sizeof(long));
	
	newStatsDir->addChild(newCustomRequirements);
	newStatsDir->addChild(newDurability);
	newStatsDir->addChild(newDurabilityCounter);
	newStatsDir->addChild(newIsIdentified);
	newStatsDir->addChild(newthisType);
	newStatsDir->addChild(newLevel);
	newStatsDir->addChild(newRepairDurabilityPenalty);
	
	this->getObject()->addChild(newStatsDir);
	return newStatsDir;
}

LsbObject *GameItem::getStatsDirectory() {
	LsbObject *statsDirectory = 0;
	std::vector<LsbObject *> statsObjects = LsbReader::lookupAllEntitiesWithName(this->getObject(), "Stats");
	for (int i=0; i<statsObjects.size(); ++i) {
		LsbObject *object = statsObjects[i];
		if (object->isDirectory()) {
			statsDirectory = object;
			break;
		}
	}
	
	return statsDirectory;
}

LsbObject *GameItem::getGenerationDirectory() {
	LsbObject *generationDir = LsbReader::lookupByUniquePathEntity(this->getObject(), "Generation");
	return generationDir;
}

bool GameItem::removeStatsDirectory()
{
	LsbObject *statsDir = getStatsDirectory();
	if (statsDir != 0) {
		return this->getObject()->removeChild(statsDir);
	}
	return false;
}

bool GameItem::removeGenerationDirectory()
{
	LsbObject *generationDir = LsbReader::lookupByUniquePathEntity(this->getObject(), "Generation");
	if (generationDir != 0) {
		LsbObject *isGeneratedObject = LsbReader::lookupByUniquePathEntity(this->getObject(), "IsGenerated");
		bool isGen = false;
		isGeneratedObject->setData((char *)&isGen, sizeof(bool));
		return this->getObject()->removeChild(generationDir);
	}
	return false;
}

std::string GameItem::getStatsText() const
{
	return statsText;
}

void GameItem::setStatsText(const std::string &value)
{
	statsText = value;
}

std::vector<LsbObject *>& GameItem::getPermBoosts()
{
	return permBoosts;
}

void GameItem::setPermBoosts(const std::vector<LsbObject *> &value)
{
	permBoosts = value;
}

long GameItem::getGenerationRandom() const
{
	return generationRandom;
}

void GameItem::setGenerationRandom(long value)
{
	generationRandom = value;
}

long GameItem::getDurability() const
{
	return durability;
}

void GameItem::setDurability(long value)
{
	durability = value;
}
GameItem::GameItem(std::vector<TAG_LSB *> *tagList) : tagList(tagList)
{
	this->itemName = "?";
	this->body = "";
}
