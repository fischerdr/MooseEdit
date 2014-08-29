#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include <vector>
#include "GameItem.h"
#include "LsbObject.h"
#include "LsbReader.h"

class ItemGroup
{
	std::vector<GameItem *> items;
	std::map<unsigned long, GameItem *> renderSlotMap;
	std::map<unsigned short, GameItem *> internalSlotMap;
	unsigned long largestRenderSlot = 0;
	unsigned short largestInternalSlot = 0;
	unsigned long largestEquipmentSlot = 0;
	unsigned long largestConsumableSlot = 0;
	unsigned long largestMagicalSlot = 0;
	unsigned long largestIngredientSlot = 0;
	unsigned long largestKeysSlot = 0;
	unsigned long largestMiscSlot = 0;
public:
	ItemGroup();
	std::vector<GameItem *>& getItems() {
		return items;
	}
	void addItem(GameItem *item) {
		if (item != 0 && item->getObject() != 0) {
			LsbObject *slotObject = item->getObject()->lookupByUniquePath("Slot");
			unsigned short internalSlot = SLOT_INVALID;
			if (slotObject != 0) {
				internalSlot = *((unsigned short *)slotObject->getData());
			}
			if (internalSlot != SLOT_INVALID) {
				if (internalSlot > largestInternalSlot) {
					largestInternalSlot = internalSlot;
				}
				internalSlotMap[internalSlot] = item;
			}
		}
		unsigned long renderSlot = item->getRenderSlot();
		if (renderSlot > largestRenderSlot) {
			if (renderSlot != SLOT_INVALID) {
				largestRenderSlot = renderSlot;
			}
		}
		unsigned long equipmentSlot = item->getEquipmentSlot();
		if (equipmentSlot > largestEquipmentSlot) {
			largestEquipmentSlot = equipmentSlot;
		}
		unsigned long consumableSlot = item->getConsumableSlot();
		if (consumableSlot > largestConsumableSlot) {
			largestConsumableSlot = consumableSlot;
		}
		unsigned long magicalSlot = item->getMagicalSlot();
		if (magicalSlot > largestMagicalSlot) {
			largestMagicalSlot = magicalSlot;
		}
		unsigned long ingredientSlot = item->getIngredientSlot();
		if (ingredientSlot > largestIngredientSlot) {
			largestIngredientSlot = ingredientSlot;
		}
		unsigned long keysSlot = item->getKeysSlot();
		if (keysSlot > largestKeysSlot) {
			largestKeysSlot = keysSlot;
		}
		unsigned long miscSlot = item->getMiscSlot();
		if (miscSlot > largestMiscSlot) {
			largestMiscSlot = miscSlot;
		}
		
		renderSlotMap[renderSlot] = item;
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

	GameItem *getItemByRenderSlot(unsigned long renderSlot) {
		if (renderSlotMap.find(renderSlot) != renderSlotMap.end()) {
			return renderSlotMap[renderSlot];
		}
		return 0;
	}
	unsigned long getLargestRenderSlot() {
		return largestRenderSlot;
	}
	unsigned short getLargestInternalSlot() {
		return largestInternalSlot;
	}
	unsigned long getLargestEquipmentSlot() const;
	void setLargestEquipmentSlot(unsigned long value);
	unsigned long getLargestConsumableSlot() const;
	void setLargestConsumableSlot(unsigned long value);
	unsigned long getLargestMagicalSlot() const;
	void setLargestMagicalSlot(unsigned long value);
	unsigned long getLargestIngredientSlot() const;
	void setLargestIngredientSlot(unsigned long value);
	unsigned long getLargestKeysSlot() const;
	void setLargestKeysSlot(unsigned long value);
	unsigned long getLargestMiscSlot() const;
	void setLargestMiscSlot(unsigned long value);
};

#endif // ITEMGROUP_H
