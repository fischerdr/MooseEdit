#include "EquipmentHandler.h"
#include <QMenu>

EquipmentHandler::EquipmentHandler(QImage emptySlotImage, std::vector<LsbObject *> &stats, std::vector<LsbObject *> &rootTemplates, 
								   std::vector<LsbObject *> &modTemplates, TextureAtlas &iconAtlas, std::vector<StatsContainer *> &itemStats, 
								   std::map<std::string, std::string> &nameMappings, QWidget *parentWidget, QWidget *mainWindow, std::vector<StatsContainer *> &itemLinks, std::vector<TAG_LSB *> &tagList) :
	emptySlotImage(emptySlotImage), stats(stats), rootTemplates(rootTemplates), modTemplates(modTemplates), iconAtlas(iconAtlas), itemStats(itemStats),
	nameMappings(nameMappings), parentWidget(parentWidget), mainWindow(mainWindow), itemLinks(itemLinks), tagList(tagList)
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

void EquipmentHandler::onItemEdited(GameItem *newItem, GameItem *oldItem) {
	InventoryHandler *handler = 0;
	int i;
	for (i=0; i<EQUIP_SLOTS; ++i) {
		GameItem *equipItem = getItemAtSlot(i);
		if (equipItem == oldItem) {
			handler = equipHandler[i];
			break;
		}
	}
	if (handler != 0) {
		LsbObject *parent = oldItem->getObject()->getParent();
		handler->getItems()->removeItem(oldItem);
		GameItem *copy = new GameItem(*newItem);
		parent->replaceChild(oldItem->getObject(), copy->getObject());
		delete oldItem;
		handler->getItems()->addItem(copy);
		
		std::ostringstream ss;
		ss<<"slot"<<i;
		QWidget *target = parentWidget->findChild<QWidget *>(ss.str().c_str());
		target->parentWidget()->setMinimumWidth(target->parentWidget()->width());
		target->parentWidget()->setMaximumWidth(target->parentWidget()->width());
		target->parentWidget()->setMinimumHeight(target->parentWidget()->height());
		target->parentWidget()->setMaximumHeight(target->parentWidget()->height());
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
					if (item != 0) {
						ItemEditFrame *itemEditFrame = new ItemEditFrame(itemStats, itemLinks, item, 
																		 new InventoryHandler(emptySlotImage, stats, rootTemplates, modTemplates, iconAtlas, itemStats, nameMappings),
																		 this, &tagList, nameMappings);
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
				item->setSlot(0);
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
	//parentWidget->setLayout(new RelativePositionLayout(parentWidget));
	for (int i=0; i<EQUIP_SLOTS; ++i) {
		equipHandler[i] = new InventoryHandler(emptySlotImage, stats, rootTemplates, modTemplates, iconAtlas, itemStats, nameMappings);
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

