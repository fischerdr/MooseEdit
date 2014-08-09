#include "GameItem.h"
#include <windows.h>
#include "LsbReader.h"
#include <vector>

LsbObject *GameItem::createNewItem(std::vector<TAG_LSB*> *tagList, LsbObject *_parent, unsigned long inventoryId, unsigned long characterCreatorHandle) {
	TAG_LSB *itemTag = LsbReader::createTagIfNeeded("Item", tagList);
	LsbObject *itemObject = new LsbObject(true, itemTag->index, itemTag->tag, 0, _parent, tagList);
	
	TAG_LSB *amountTag = LsbReader::createTagIfNeeded("Amount", tagList);
	LsbObject *amountObject = new LsbObject(false, amountTag->index, amountTag->tag, 0x04, itemObject, tagList);
	long amount = 1;
	amountObject->setData((char *)&amount, sizeof(amount));
	
	TAG_LSB *currentTemplateTag = LsbReader::createTagIfNeeded("CurrentTemplate", tagList);
	LsbObject *currentTemplateObject = new LsbObject(false, currentTemplateTag->index, currentTemplateTag->tag, 0x16, itemObject, tagList);
	std::string currentTemplate = "1c3c9c74-34a1-4685-989e-410dc080be6f";
	currentTemplateObject->setData(currentTemplate.c_str(), currentTemplate.length() + 1);
	
	TAG_LSB *currentTemplateTypeTag = LsbReader::createTagIfNeeded("CurrentTemplateType", tagList);
	LsbObject *currentTemplateTypeObject = new LsbObject(false, currentTemplateTypeTag->index, currentTemplateTypeTag->tag, 0x01, itemObject, tagList);
	unsigned char currentTemplateType = 0;
	currentTemplateTypeObject->setData((char *)&currentTemplateType, sizeof(currentTemplateType));
	
	TAG_LSB *flagsTag = LsbReader::createTagIfNeeded("Flags", tagList);
	LsbObject *flagsObject = new LsbObject(false, flagsTag->index, flagsTag->tag, 0x05, itemObject, tagList);
	unsigned long flags = 33240;
	flagsObject->setData((char *)&flags, sizeof(flags));
	
	TAG_LSB *globalTag = LsbReader::createTagIfNeeded("Global", tagList);
	LsbObject *globalObject = new LsbObject(false, globalTag->index, globalTag->tag, 0x13, itemObject, tagList);
	bool global = true;
	globalObject->setData((char *)&global, sizeof(global));
	
	TAG_LSB *inventoryTag = LsbReader::createTagIfNeeded("Inventory", tagList);
	LsbObject *inventoryObject = new LsbObject(false, inventoryTag->index, inventoryTag->tag, 0x05, itemObject, tagList);
	unsigned long inventory = 0;
	inventoryObject->setData((char *)&inventory, sizeof(inventory));
	
	TAG_LSB *isGeneratedTag = LsbReader::createTagIfNeeded("IsGenerated", tagList);
	LsbObject *isGeneratedObject = new LsbObject(false, isGeneratedTag->index, isGeneratedTag->tag, 0x13, itemObject, tagList);
	bool isGenerated = false;
	isGeneratedObject->setData((char *)&isGenerated, sizeof(isGenerated));
	
	TAG_LSB *isKeyTag = LsbReader::createTagIfNeeded("IsKey", tagList);
	LsbObject *isKeyObject = new LsbObject(false, isKeyTag->index, isKeyTag->tag, 0x13, itemObject, tagList);
	bool isKey = false;
	isKeyObject->setData((char *)&isKey, sizeof(isKey));
	
	TAG_LSB *keyTag = LsbReader::createTagIfNeeded("Key", tagList);
	LsbObject *keyObject = new LsbObject(false, keyTag->index, keyTag->tag, 0x16, itemObject, tagList);
	std::string key = "";
	keyObject->setData(key.c_str(), key.length() + 1);
	
	TAG_LSB *levelTag = LsbReader::createTagIfNeeded("Level", tagList);
	LsbObject *levelObject = new LsbObject(false, levelTag->index, levelTag->tag, 0x16, itemObject, tagList);
	std::string level = "";
	levelObject->setData(level.c_str(), level.length() + 1);

	TAG_LSB *lifeTimeTag = LsbReader::createTagIfNeeded("LifeTime", tagList);
	LsbObject *lifeTimeObject = new LsbObject(false, lifeTimeTag->index, lifeTimeTag->tag, 0x06, itemObject, tagList);
	float lifeTime = 0.0f;
	lifeTimeObject->setData((char *)&lifeTime, sizeof(lifeTime));
	
	TAG_LSB *lockLevelTag = LsbReader::createTagIfNeeded("LockLevel", tagList);
	LsbObject *lockLevelObject = new LsbObject(false, lockLevelTag->index, lockLevelTag->tag, 0x04, itemObject, tagList);
	long lockLevel = 1;
	lockLevelObject->setData((char *)&lockLevel, sizeof(lockLevel));
	
	TAG_LSB *originalTemplateTag = LsbReader::createTagIfNeeded("OriginalTemplate", tagList);
	LsbObject *originalTemplateObject = new LsbObject(false, originalTemplateTag->index, originalTemplateTag->tag, 0x16, itemObject, tagList);
	std::string originalTemplate = "1c3c9c74-34a1-4685-989e-410dc080be6f";
	originalTemplateObject->setData(originalTemplate.c_str(), originalTemplate.length() + 1);
	
	TAG_LSB *originalTemplateTypeTag = LsbReader::createTagIfNeeded("OriginalTemplateType", tagList);
	LsbObject *originalTemplateTypeObject = new LsbObject(false, originalTemplateTypeTag->index, originalTemplateTypeTag->tag, 0x01, itemObject, tagList);
	unsigned char originalTemplateType = 0;
	originalTemplateTypeObject->setData((char *)&originalTemplateType, sizeof(originalTemplateType));
	
	TAG_LSB *parentTag = LsbReader::createTagIfNeeded("Parent", tagList);
	LsbObject *parentObject = new LsbObject(false, parentTag->index, parentTag->tag, 0x05, itemObject, tagList);
	unsigned long parent = inventoryId;
	parentObject->setData((char *)&parent, sizeof(parent));
	
	TAG_LSB *rotateTag = LsbReader::createTagIfNeeded("Rotate", tagList);
	LsbObject *rotateObject = new LsbObject(false, rotateTag->index, rotateTag->tag, 0x0F, itemObject, tagList);
	char data[36];
	float *fData = (float *)&data;
	*fData = 1.00f;
	fData += 1;
	*fData = 0.00f;
	fData += 1;
	*fData = 0.00f;
	fData += 1;
	
	*fData = 0.00f;
	fData += 1;
	*fData = 1.00f;
	fData += 1;
	*fData = 0.00f;
	fData += 1;
	
	*fData = 0.00f;
	fData += 1;
	*fData = 0.00f;
	fData += 1;
	*fData = 1.00f;
	fData += 1;
	rotateObject->setData(data, sizeof(data));
	
	TAG_LSB *scaleTag = LsbReader::createTagIfNeeded("Scale", tagList);
	LsbObject *scaleObject = new LsbObject(false, scaleTag->index, scaleTag->tag, 0x06, itemObject, tagList);
	float scale = 1.0f;
	scaleObject->setData((char *)&scale, sizeof(scale));
	
	TAG_LSB *slotTag = LsbReader::createTagIfNeeded("Slot", tagList);
	LsbObject *slotObject = new LsbObject(false, slotTag->index, slotTag->tag, 0x03, itemObject, tagList);
	unsigned short slot = 0; //fixme
	slotObject->setData((char *)&slot, sizeof(slot));
	
	TAG_LSB *statsTag = LsbReader::createTagIfNeeded("Stats", tagList);
	LsbObject *statsObject = new LsbObject(false, statsTag->index, statsTag->tag, 0x16, itemObject, tagList);
	std::string stats = "Small_Gold";
	statsObject->setData(stats.c_str(), stats.length() + 1);
	
	TAG_LSB *surfaceCheckTimerTag = LsbReader::createTagIfNeeded("SurfaceCheckTimer", tagList);
	LsbObject *surfaceCheckTimerObject = new LsbObject(false, surfaceCheckTimerTag->index, surfaceCheckTimerTag->tag, 0x06, itemObject, tagList);
	float surfaceCheckTimer = 0.0f;
	surfaceCheckTimerObject->setData((char *)&surfaceCheckTimer, sizeof(surfaceCheckTimer));
	
	TAG_LSB *translateTag = LsbReader::createTagIfNeeded("Translate", tagList);
	LsbObject *translateObject = new LsbObject(false, translateTag->index, translateTag->tag, 0x0C, itemObject, tagList);
	char data2[12];
	float *fData2 = (float *)&data2;
	*fData2 = 0.00f;
	fData2 += 1;
	*fData2 = 0.00f;
	fData2 += 1;
	*fData2 = 0.00f;
	fData2 += 1;
	translateObject->setData(data2, sizeof(data2));
	
	TAG_LSB *treasureGeneratedTag = LsbReader::createTagIfNeeded("TreasureGenerated", tagList);
	LsbObject *treasureGeneratedObject = new LsbObject(false, treasureGeneratedTag->index, treasureGeneratedTag->tag, 0x13, itemObject, tagList);
	bool treasureGenerated = false;
	treasureGeneratedObject->setData((char *)&treasureGenerated, sizeof(treasureGenerated));
	
	TAG_LSB *unsoldGeneratedTag = LsbReader::createTagIfNeeded("UnsoldGenerated", tagList);
	LsbObject *unsoldGeneratedObject = new LsbObject(false, unsoldGeneratedTag->index, unsoldGeneratedTag->tag, 0x13, itemObject, tagList);
	bool unsoldGenerated = false;
	unsoldGeneratedObject->setData((char *)&unsoldGenerated, sizeof(unsoldGenerated));
	
	TAG_LSB *velocityTag = LsbReader::createTagIfNeeded("Velocity", tagList);
	LsbObject *velocityObject = new LsbObject(false, velocityTag->index, velocityTag->tag, 0x0C, itemObject, tagList);
	char data3[12];
	float *fData3 = (float *)&data3;
	*fData3 = 0.00f;
	fData3 += 1;
	*fData3 = 0.00f;
	fData3 += 1;
	*fData3 = 0.00f;
	fData3 += 1;
	velocityObject->setData(data3, sizeof(data3));
	
	TAG_LSB *vitalityTag = LsbReader::createTagIfNeeded("Vitality", tagList);
	LsbObject *vitalityObject = new LsbObject(false, vitalityTag->index, vitalityTag->tag, 0x04, itemObject, tagList);
	long vitality = -1;
	vitalityObject->setData((char *)&vitality, sizeof(vitality));
	
	TAG_LSB *ownerTag = LsbReader::createTagIfNeeded("owner", tagList);
	LsbObject *ownerObject = new LsbObject(false, ownerTag->index, ownerTag->tag, 0x05, itemObject, tagList);
	unsigned long owner = characterCreatorHandle;
	ownerObject->setData((char *)&owner, sizeof(owner));
	
	TAG_LSB *itemMachineTag = LsbReader::createTagIfNeeded("ItemMachine", tagList);
	LsbObject *itemMachineObject = new LsbObject(true, itemMachineTag->index, itemMachineTag->tag, 0, _parent, tagList);
	
	TAG_LSB *statusManagerTag = LsbReader::createTagIfNeeded("StatusManager", tagList);
	LsbObject *statusManagerObject = new LsbObject(true, statusManagerTag->index, statusManagerTag->tag, 0, _parent, tagList);
	
	TAG_LSB *variableManagerTag = LsbReader::createTagIfNeeded("VariableManager", tagList);
	LsbObject *variableManagerObject = new LsbObject(true, variableManagerTag->index, variableManagerTag->tag, 0, _parent, tagList);
	
	itemObject->addChild(amountObject);
	itemObject->addChild(currentTemplateObject);
	itemObject->addChild(currentTemplateTypeObject);
	itemObject->addChild(flagsObject);
	itemObject->addChild(globalObject);
	itemObject->addChild(inventoryObject);
	itemObject->addChild(isGeneratedObject);
	itemObject->addChild(isKeyObject);
	itemObject->addChild(keyObject);
	itemObject->addChild(levelObject);
	itemObject->addChild(lifeTimeObject);
	itemObject->addChild(lockLevelObject);
	itemObject->addChild(originalTemplateObject);
	itemObject->addChild(originalTemplateTypeObject);
	itemObject->addChild(parentObject);
	itemObject->addChild(rotateObject);
	itemObject->addChild(scaleObject);
	itemObject->addChild(slotObject);
	itemObject->addChild(statsObject);
	itemObject->addChild(surfaceCheckTimerObject);
	itemObject->addChild(translateObject);
	itemObject->addChild(treasureGeneratedObject);
	itemObject->addChild(unsoldGeneratedObject);
	itemObject->addChild(velocityObject);
	itemObject->addChild(vitalityObject);
	itemObject->addChild(ownerObject);
	itemObject->addChild(itemMachineObject);
	itemObject->addChild(statusManagerObject);
	itemObject->addChild(variableManagerObject);
	
//	if (_parent != 0) {
//		_parent->addChild(itemObject);
//	}
	return itemObject;
}

LsbObject *GameItem::getCreatorsObject(LsbObject *itemsObject) {
	if (itemsObject != 0) {
		LsbObject *itemFactoryObject = itemsObject->getParent();
		
		LsbObject *creatorsObject = LsbReader::lookupByUniquePathEntity(itemFactoryObject, "Creators");

		return creatorsObject;
	}
	return 0;
}

LsbObject *GameItem::createNewItemCreator(std::vector<TAG_LSB*> *tagList, LsbObject *itemsObject, LsbObject *itemObject) {
	if (itemsObject != 0 && itemObject != 0) {
		LsbObject *itemFactoryObject = itemsObject->getParent();
		
		LsbObject *creatorsObject = LsbReader::lookupByUniquePathEntity(itemFactoryObject, "Creators");
		
		TAG_LSB *creatorTag = LsbReader::createTagIfNeeded("Creator", tagList);
		LsbObject *creatorObject = new LsbObject(true, creatorTag->index, creatorTag->tag, 0, creatorsObject, tagList);
		
		TAG_LSB *handleTag = LsbReader::createTagIfNeeded("Handle", tagList);
		LsbObject *handleObject = new LsbObject(false, handleTag->index, handleTag->tag, 0x05, creatorObject, tagList);
		unsigned long handle = getNewItemCreatorHandle(itemsObject);
		handleObject->setData((char *)&handle, sizeof(handle));
		
		TAG_LSB *templateIDTag = LsbReader::createTagIfNeeded("TemplateID", tagList);
		LsbObject *templateIDObject = new LsbObject(false, templateIDTag->index, templateIDTag->tag, 0x16, creatorObject, tagList);
		LsbObject *currentTemplateObject = LsbReader::lookupByUniquePathEntity(itemObject, "CurrentTemplate");
		templateIDObject->setData(currentTemplateObject->getData(), currentTemplateObject->getDataSize());
		
		TAG_LSB *templateTypeTag = LsbReader::createTagIfNeeded("TemplateType", tagList);
		LsbObject *templateTypeObject = new LsbObject(false, templateTypeTag->index, templateTypeTag->tag, 0x01, creatorObject, tagList);
		LsbObject *currentTemplateTypeObject = LsbReader::lookupByUniquePathEntity(itemObject, "CurrentTemplateType");
		templateTypeObject->setData(currentTemplateTypeObject->getData(), currentTemplateTypeObject->getDataSize());
		
		creatorObject->addChild(handleObject);
		creatorObject->addChild(templateIDObject);
		creatorObject->addChild(templateTypeObject);
		
		return creatorObject;
	}
	
	return 0;
}

unsigned long GameItem::getNewItemCreatorHandle(LsbObject *itemsObject) {
	unsigned long handle = 0;
	if (itemsObject != 0) {
		LsbObject *itemFactoryObject = itemsObject->getParent();
		if (itemFactoryObject != 0) {
			LsbObject *creatorsObject = LsbReader::lookupByUniquePathEntity(itemFactoryObject, "Creators");
			handle = 67141632;
			if (creatorsObject->getChildren().size() > 0) {
				LsbObject *lastCreatorObject = creatorsObject->getChildren()[creatorsObject->getChildren().size() - 1];
				handle = *((unsigned long *)LsbReader::lookupByUniquePathEntity(lastCreatorObject, "Handle")->getData());
				++handle;
			}
		}
	}
	return handle;
}

LsbObject *GameItem::createGenerationDirectory() {
	LsbObject *generationObject = 0;
	
	LsbObject *itemObject = this->getObject();
	TAG_LSB *generationTag = LsbReader::createTagIfNeeded("Generation", tagList);
	if (generationTag != 0) {
		generationObject = new LsbObject(true, generationTag->index, generationTag->tag, 0, itemObject, tagList);
		itemObject->addChild(generationObject);
		
		LsbObject *statsObject = LsbReader::lookupByUniquePathEntity(itemObject, "Stats");

		LsbObject *statsDirectory = 0;
		std::vector<LsbObject *> statsObjects = LsbReader::lookupAllEntitiesWithName(itemObject, "Stats");
		for (int i=0; i<statsObjects.size(); ++i) {
			LsbObject *object = statsObjects[i];
			if (object->isDirectory()) {
				statsDirectory = object;
				break;
			}
		}
		
		LsbObject *isGeneratedObject = LsbReader::lookupByUniquePathEntity(itemObject, "IsGenerated");
		bool isGen = true;
		isGeneratedObject->setData((char *)&isGen, sizeof(bool));
		
		if (statsDirectory != 0) {
			LsbObject *isIdentObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "IsIdentified");
			long isIdent = 1;
			isIdentObject->setData((char *)&isIdent, sizeof(long));
		}

		TAG_LSB *baseTag = LsbReader::createTagIfNeeded("Base", tagList);
		LsbObject *newBase = new LsbObject(false, baseTag->index, baseTag->tag, 0x16, generationObject, tagList);
		newBase->setData(statsObject->getData(), statsObject->getDataSize());
		
		TAG_LSB *itemTypeTag = LsbReader::createTagIfNeeded("ItemType", tagList);
		LsbObject *newItemType = new LsbObject(false, itemTypeTag->index, itemTypeTag->tag, 0x16, generationObject, tagList);
		std::string itemTypeDefault = "Magic";
		newItemType->setData(itemTypeDefault.c_str(), itemTypeDefault.length() + 1);
		if (statsDirectory != 0) {
			LsbObject *itemTypeObject = LsbReader::lookupByUniquePathEntity(statsDirectory, "ItemType");
			std::string commonText = "Common";
			if (itemTypeObject->getData() == commonText) {
				itemTypeObject->setData(itemTypeDefault.c_str(), itemTypeDefault.length() + 1);
			} else {
				newItemType->setData(itemTypeObject->getData(), itemTypeObject->getDataSize());
			}
		}

		TAG_LSB *levelTag = LsbReader::createTagIfNeeded("Level", tagList);
		LsbObject *newLevel = new LsbObject(false, levelTag->index, levelTag->tag, 0x04, generationObject, tagList);
		long statsLevel = this->getItemLevel();
		newLevel->setData((char *)&statsLevel, sizeof(long));
		
		TAG_LSB *randomTag = LsbReader::createTagIfNeeded("Random", tagList);
		LsbObject *newRandom = new LsbObject(false, randomTag->index, randomTag->tag, 0x04, generationObject, tagList);
		long random = 1;
		newRandom->setData((char *)&random, sizeof(long));

		generationObject->addChild(newBase);
		generationObject->addChild(newItemType);
		generationObject->addChild(newLevel);
		generationObject->addChild(newRandom);
	}
	return generationObject;
}

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

unsigned long GameItem::getEquipmentSlot() const
{
	return equipmentSlot;
}

void GameItem::setEquipmentSlot(unsigned long value)
{
	equipmentSlot = value;
}

unsigned long GameItem::getConsumableSlot() const
{
	return consumableSlot;
}

void GameItem::setConsumableSlot(unsigned long value)
{
	consumableSlot = value;
}

unsigned long GameItem::getMagicalSlot() const
{
	return magicalSlot;
}

void GameItem::setMagicalSlot(unsigned long value)
{
	magicalSlot = value;
}

unsigned long GameItem::getIngredientSlot() const
{
	return ingredientSlot;
}

void GameItem::setIngredientSlot(unsigned long value)
{
	ingredientSlot = value;
}

unsigned long GameItem::getKeysSlot() const
{
	return keysSlot;
}

void GameItem::setKeysSlot(unsigned long value)
{
	keysSlot = value;
}

unsigned long GameItem::getMiscSlot() const
{
	return miscSlot;
}

void GameItem::setMiscSlot(unsigned long value)
{
	miscSlot = value;
}
GameItem::GameItem(std::vector<TAG_LSB *> *tagList) : tagList(tagList)
{
	this->itemName = "?";
	this->body = "";
}
