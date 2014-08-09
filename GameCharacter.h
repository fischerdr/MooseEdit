#ifndef GAMECHARACTER_H
#define GAMECHARACTER_H

#include <QWidget>
#include "LsbObject.h"
#include "ItemGroup.h"
#include "InventoryHandler.h"

#define ATTRIB_STR	0
#define ATTRIB_DEX	1
#define ATTRIB_INT	2
#define ATTRIB_CONS	3
#define ATTRIB_SPD	4
#define ATTRIB_PER	5

class GameCharacter
{
	LsbObject *object;
	std::string name;
	QWidget *widget;
	ItemGroup inventory;
	std::vector<TAG_LSB *> &tagList;
	std::vector<LsbObject *> &globals;
	
	InventoryHandler *inventoryHandler;
	
	void ensureInventoryCapacity(LsbObject *viewMapValueObject, unsigned long viewSlot);
public:
	GameCharacter(std::vector<LsbObject *> &globals, std::vector<TAG_LSB *> &tagList);
	~GameCharacter() {
		delete widget;
	}
	ItemGroup& getInventory() {
		return inventory;
	}
	std::string& getName() {
		return name;
	}
	void setName(std::string& name) {
		this->name = name;
	}
	LsbObject *getObject() {
		return object;
	}
	void setObject(LsbObject *character) {
		this->object = character;
	}
	QWidget *getWidget() {
		return this->widget;
	}
	void setWidget(QWidget *widget) {
		this->widget = widget;
	}
	InventoryHandler *getInventoryHandler() const {
		return inventoryHandler;
	}
	void setInventoryHandler(InventoryHandler *value) {
		this->inventoryHandler = value;
	}
	bool hasTalent(long talentId);
	void setTalent(long talentId, bool enabled);
	std::vector<LsbObject *> getAbilityList();
	std::vector<LsbObject *> getTraitList();
	std::vector<LsbObject *> getSkillList();
	void addSkill(const char *skillName);
	void removeSkill(const char *skillName);
	LsbObject *getInventoryObject();
	unsigned long getInventoryId();
	unsigned long getCreatorId();
	bool addItemToInventoryObject(LsbObject *itemCreatorObject, unsigned long viewSlot, unsigned long extraInventoryTab, unsigned long extraViewSlot, bool equippedItem);
};

#endif // GAMECHARACTER_H
