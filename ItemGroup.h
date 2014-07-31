#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include <vector>
#include "GameItem.h"
#include "LsbObject.h"
#include "LsbReader.h"

class ItemGroup
{
	std::vector<GameItem *> items;
	std::map<unsigned short, GameItem *> slotMap;
	unsigned short largestSlot = 0;
public:
	ItemGroup();
	std::vector<GameItem *>& getItems() {
		return items;
	}
	void addItem(GameItem *item) {
		unsigned short slot = item->getSlot();
		if (slot == SLOT_INVALID && item != 0 && item->getObject() != 0) {
			LsbObject *slotObject = LsbReader::lookupByUniquePathEntity(item->getObject(), "Slot");
			slot = *((unsigned short *)slotObject->getData());
		}
		if (slot != SLOT_INVALID) {
			if (slot > largestSlot) {
				largestSlot = slot;
			}
			slotMap[slot] = item;
		}
		items.push_back(item);
	}
	bool removeItem(GameItem *item) {
		for (int i=0; i<items.size(); ++i) {
			if (items[i] == item) {
				items.erase(items.begin() + i);
				return true;
			}
		}
		return false;
	}

	GameItem *getItemBySlot(unsigned short slot) {
		if (slotMap.find(slot) != slotMap.end()) {
			return slotMap[slot];
		}
		return 0;
	}
	unsigned short getLargestSlot() {
		return largestSlot;
	}
};

#endif // ITEMGROUP_H
