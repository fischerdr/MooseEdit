#ifndef EQUIPMENTHANDLER_H
#define EQUIPMENTHANDLER_H

#include "InventoryHandler.h"
#include "ItemEditFrame.h"
#include "GameItem.h"
#include "TextureAtlas.h"
#include <string>
#include <vector>

#define EQUIP_SLOTS 12
#define SLOT_HELMET		0
#define SLOT_BREAST		1
#define SLOT_GARMENT	2
#define SLOT_WEAPON		3
#define SLOT_SHIELD		4
#define SLOT_RING_RIGHT	5
#define SLOT_BELT		6
#define SLOT_BOOTS		7
#define SLOT_BRACERS	8
#define SLOT_AMULET		9
#define SLOT_RING_LEFT	11

class EquipmentHandler : public QObject, public ItemEditCallback
{
	Q_OBJECT
	
private:
	InventoryHandler *equipHandler[EQUIP_SLOTS];
	
	QImage emptySlotImage;
	std::vector<LsbObject *> &stats;
	std::vector<LsbObject *> &rootTemplates;
	std::vector<LsbObject *> &modTemplates;
	TextureAtlas &iconAtlas;
	std::vector<StatsContainer *> &itemStats;
	std::map<std::string, std::string> &nameMappings;
	std::vector<StatsContainer *>& itemLinks;
	std::vector<TAG_LSB *>& tagList;
	void initInventoryHandlers();
	QWidget *parentWidget;
	QWidget *mainWindow;
	
public slots:
	void customContextRequested(const QPoint& pos);
	
public:
	~EquipmentHandler();
	EquipmentHandler(QImage emptySlotImage, std::vector<LsbObject *>& stats, std::vector<LsbObject *>& rootTemplates, 
					 std::vector<LsbObject *>& modTemplates, TextureAtlas& iconAtlas, std::vector<StatsContainer *>& itemStats,
					 std::map<std::string, std::string>& nameMappings, QWidget *parentWidget, QWidget *mainWindow, std::vector<StatsContainer *>& itemLinks,
					 std::vector<TAG_LSB *>& tagList);
	void addItem(GameItem *item);
	void drawAll();
	void onItemEdited(GameItem *newItem, GameItem *oldItem);
	GameItem *getItemAtSlot(unsigned short slot);
};

#endif // EQUIPMENTHANDLER_H

