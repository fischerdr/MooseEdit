#ifndef INVENTORYHANDLER_H
#define INVENTORYHANDLER_H

#include <QLabel>
#include <QImage>
#include <QWidget>
#include <vector>
#include <QPoint>
#include "LsbObject.h"
#include "ItemGroup.h"
#include "TextureAtlas.h"
#include "LsbReader.h"
#include "ItemLabel.h"
#include "GenStatsReader.h"

class InventoryHandler
{
	std::vector<ItemLabel *> itemLabels;
	std::vector<LsbObject *>& stats;
	std::vector<LsbObject *>& rootTemplates;
	std::vector<LsbObject *>& modTemplates;
	std::vector<StatsContainer *>& itemStats;
	TextureAtlas& iconAtlas;
	QImage emptySlotImage;
	long getItemX(long slotNumber);
	long getItemY(long slotNumber);
	ItemGroup items;
	std::map<std::string, std::string> &nameMappings;
	std::map<std::string, LsbObject *> &rootTemplateMap;
	std::map<std::string, LsbObject *> &modTemplateMap;
	
	void buildNameMappings();
	long itemsPerRow = 10;
	long minSlots = 50;
	long maxSlots = 1000000;
	long iconSize = 48;
public:
	//static const long ICON_SIZE = 48;
	void setIconSize(long iconSize) {
		this->iconSize = iconSize;
	}
	long getIconSize() {
		return this->iconSize;
	}
	void setIconsPerRow(long itemsPerRow) {
		this->itemsPerRow = itemsPerRow;
	}
	void setMinSlots(long minSlots) {
		this->minSlots = minSlots;
	}
	long getMinSlots() {
		return minSlots;
	}
	void setMaxSlots(long maxSlots) {
		this->maxSlots = maxSlots;
	}
	long getMaxSlots() {
		return maxSlots;
	}
	long getItemsPerRow() {
		return itemsPerRow;
	}
	ItemGroup *getItems() {
		return &items;
	}
	unsigned long slotAtPoint(const QPoint &pt);
	void clearItemLabels() {
		this->itemLabels.clear();
	}
	~InventoryHandler();
	InventoryHandler(QImage emptySlotImage, std::vector<LsbObject *>& stats, std::vector<LsbObject *>& rootTemplates, 
					 std::vector<LsbObject *>& modTemplates, TextureAtlas& iconAtlas, std::vector<StatsContainer *>& itemStats, std::map<std::string, std::string>& nameMappings,
					 std::map<std::string, LsbObject *> &rootTemplateMap, std::map<std::string, LsbObject *> &modTemplateMap);
	void draw(QWidget *parent, QWidget *mainWindow, bool drawBackground);
	GameItem *getItemAtPoint(const QPoint& pt);
};

#endif // INVENTORYHANDLER_H
