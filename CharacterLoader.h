#ifndef CHARACTERLOADER_H
#define CHARACTERLOADER_H

#include <vector>
#include <QWidget>
#include "GameItem.h"
#include "CharacterGroup.h"


typedef std::map<unsigned long, unsigned long> ViewSlotMap;
struct ItemHandleData {
	long characterId;
	ViewSlotMap viewSlotMap;
};

class CharacterLoader
{
	std::vector<std::vector<GameItem *> > equipmentSets;
public:
	CharacterLoader();
	CharacterGroup& getCharacterGroup() {
		return characters;
	}
	CharacterGroup characters;
	void load(std::vector<LsbObject *> &globals, std::vector<TAG_LSB *> *globalTagList, QWidget *mainWindow);
	std::vector<std::vector<GameItem *> > &getEquipmentSets();
	void setEquipmentSets(const std::vector<std::vector<GameItem *> > &value);
};

#endif // CHARACTERLOADER_H
