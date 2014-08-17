#include "EquipmentHandler.h"
#include <QMenu>

EquipmentHandler::EquipmentHandler(QImage emptySlotImage, std::vector<LsbObject *> &stats, std::vector<LsbObject *> &rootTemplates, 
								   std::vector<LsbObject *> &modTemplates, TextureAtlas &iconAtlas, std::vector<StatsContainer *> &itemStats, 
								   std::map<std::string, std::string> &nameMappings, QWidget *parentWidget, QWidget *mainWindow, 
								   std::vector<StatsContainer *> &itemLinks, std::vector<TAG_LSB *> &tagList,
								   LsbObject *itemsObject, GameCharacter *character, std::map<std::string, LsbObject *> &rootTemplateMap, std::map<std::string, LsbObject *> &modTemplateMap, StatTemplateMap &statToTemplateMap) :
	emptySlotImage(emptySlotImage), stats(stats), rootTemplates(rootTemplates), modTemplates(modTemplates), iconAtlas(iconAtlas), itemStats(itemStats),
	nameMappings(nameMappings), parentWidget(parentWidget), mainWindow(mainWindow), itemLinks(itemLinks), tagList(tagList), itemsObject(itemsObject), character(character),
	rootTemplateMap(rootTemplateMap), modTemplateMap(modTemplateMap), statToTemplateMap(statToTemplateMap)
{
	initInventoryHandlers();
}

GameItem *EquipmentHandler::getItemAtSlot(unsigned short slot) {
	if (slot >= 0 && slot < EQUIP_SLOTS) {
		InventoryHandler *handler = equipHandler[slot];
		if (handler->getItems()->getItems().size() > 0) {
			return handler->getItems()->getItems()[0];
		}
	}
	return 0;
}
#include <windows.h>
void EquipmentHandler::onItemEdited(GameItem *newItem, GameItem *oldItem) {
	InventoryHandler *handler = 0;
	GameItem *copy = new GameItem(*newItem);
	int i;
	for (i=0; i<EQUIP_SLOTS; ++i) {
		GameItem *equipItem = getItemAtSlot(i);
		if (equipItem == oldItem) {
			handler = equipHandler[i];
			break;
		}
	}
	bool isAddedItem = false;
	if (handler == 0) {
		isAddedItem = true;
		//item is newly added
		LsbObject *itemObject = copy->getObject();
		itemsObject->addChild(itemObject);
		LsbObject *slotObject = LsbReader::lookupByUniquePathEntity(itemObject, "Slot");
		unsigned short slot = EQUIP_SLOTS;
		if (slotObject != 0) {
			slot = *((unsigned short *)slotObject->getData());
		}
		LsbObject *newCreatorObject = GameItem::createNewItemCreator(&tagList, itemsObject, itemObject);
		LsbObject *creatorsObject = GameItem::getCreatorsObject(itemsObject);
		creatorsObject->addChild(newCreatorObject);
		if (!character->addItemToInventoryObject(newCreatorObject, 0, 0, 0, true))
			MessageBoxA(0, "error", 0, 0);
		
		handler = equipHandler[slot];
		i = slot;
	}
	if (handler != 0 && i < EQUIP_SLOTS) {
		LsbObject *parent = oldItem->getObject()->getParent();
		handler->getItems()->removeItem(oldItem);
		if (!parent->replaceChild(oldItem->getObject(), copy->getObject())) {
			if (!isAddedItem) {
				MessageBoxA(0, "Child replace failed!", 0, 0);
			}
		}
		delete oldItem;
		handler->getItems()->addItem(copy);
		
		std::ostringstream ss;
		ss<<"slot"<<i;
		QWidget *target = parentWidget->findChild<QWidget *>(ss.str().c_str());
		if (target != 0) {
			handler->draw(target, mainWindow, false);
		}
	}
}

void EquipmentHandler::customContextRequested(const QPoint& pos) {
	QObject *sender = QObject::sender();
	if (sender != 0) {
		QWidget *senderWidget = (QWidget *)sender;
		std::string slotText = senderWidget->objectName().toStdString();
		if (slotText.length() > 0) {
			std::string slotConstant = "slot";
			std::string slotNumber = slotText.substr(slotConstant.length(), slotText.length() - slotConstant.length());
			unsigned short slot = -1;
			try {
				slot = boost::lexical_cast<unsigned short>(slotNumber);
			} catch (const boost::bad_lexical_cast& e) {
				
			}

			if (slot >= 0 && slot < EQUIP_SLOTS) {
				InventoryHandler *handler = equipHandler[slot];
				QPoint globalPos = senderWidget->mapToGlobal(pos);
				GameItem *originalItem = handler->getItemAtPoint(pos);
				GameItem *item = originalItem;
				QMenu contextMenu(senderWidget);
				if (item != 0) {
					contextMenu.addAction("&Edit Item");
				}
				else {
					contextMenu.addAction("&Add Item");
				}
				QAction *result = contextMenu.exec(globalPos);
				if (result) {
					InventoryHandler *newHandler = new InventoryHandler(emptySlotImage, stats, rootTemplates, modTemplates, iconAtlas, itemStats, nameMappings,
																		rootTemplateMap, modTemplateMap);
					if (item != 0) {
						ItemEditFrame *itemEditFrame = new ItemEditFrame(itemStats, itemLinks, item, newHandler,
																		 this, &tagList, nameMappings, statToTemplateMap);
					} else {
						LsbObject *itemObject = GameItem::createNewItem(&tagList, itemsObject, character->getInventoryId(), character->getCreatorId());
						LsbObject *slotObject = LsbReader::lookupByUniquePathEntity(itemObject, "Slot");
						if (slotObject != 0) {
							slotObject->setData((char *)&slot, sizeof(slot));
						}
						GameItem *newItem = new GameItem(&tagList);
						newItem->setObject(itemObject);
						newItem->setRenderSlot(character->getInventoryHandler()->slotAtPoint(pos));
						ItemEditFrame *itemEditFrame = new ItemEditFrame(itemStats, itemLinks, newItem, newHandler, 
																		 this, &tagList, nameMappings, statToTemplateMap);
					}
				}
			}
		}
	}
}

EquipmentHandler::~EquipmentHandler()
{
	
}

void EquipmentHandler::addItem(GameItem *item) {
	LsbObject *slotObject = LsbReader::lookupByUniquePathEntity(item->getObject(), "Slot");
	if (slotObject != 0) {
		unsigned short slot = *((unsigned short *)slotObject->getData());
		if (slot >= 0 && slot < EQUIP_SLOTS) {
			InventoryHandler *handler = equipHandler[slot];
			if (handler->getItems()->getItems().size() == 0) {
				item->setRenderSlot(0);
				handler->getItems()->addItem(item);
			}
		}
	}
}

void EquipmentHandler::drawAll()
{
	for (int i=0; i<EQUIP_SLOTS; ++i) {
		InventoryHandler *handler = equipHandler[i];
		std::ostringstream ss;
		ss<<"slot"<<i;
		QWidget *target = parentWidget->findChild<QWidget *>(ss.str().c_str());
		if (target != 0) {
			handler->draw(target, mainWindow, false);
		}
	}
}

void EquipmentHandler::initInventoryHandlers() {
	for (int i=0; i<EQUIP_SLOTS; ++i) {
		equipHandler[i] = new InventoryHandler(emptySlotImage, stats, rootTemplates, modTemplates, iconAtlas, itemStats, nameMappings, rootTemplateMap, modTemplateMap);
		equipHandler[i]->setMinSlots(1);
		equipHandler[i]->setMaxSlots(1);
		equipHandler[i]->setIconsPerRow(1);
		
		std::ostringstream ss;
		ss<<"slot"<<i;
		QWidget *target = parentWidget->findChild<QWidget *>(ss.str().c_str());
		if (target != 0) {
			target->setContextMenuPolicy(Qt::CustomContextMenu);
			this->connect(target, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customContextRequested(QPoint)));
		}
	}
}

