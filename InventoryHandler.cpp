#include "InventoryHandler.h"
#include <QPixmap>
#include <algorithm>
#include <QPainter>
#include "ItemLabel.h"
#include <QScrollArea>
#include <QTextItem>

long InventoryHandler::getItemX(long slotNumber) {
	return (slotNumber * iconSize) % (iconSize * itemsPerRow);
}

long InventoryHandler::getItemY(long slotNumber) {
	return slotNumber/itemsPerRow * iconSize;
}

struct RarityRectRGB {
	int r;
	int g;
	int b;
	int a;
};

/**
 * @brief InventoryHandler::draw
 * @param parent Window which acts as container for generated item labels
 * @param mainWindow Window which acts as container for item label tooltips
 */
void InventoryHandler::draw(QWidget *parent, QWidget *mainWindow, bool drawBackground) {
	for (int i=0; i<itemLabels.size(); ++i) {
		delete itemLabels[i];
	}
	itemLabels.clear();
	int newWidth = parent->width();
	if (parent->maximumWidth() != 0 && newWidth > parent->maximumWidth()) {
		newWidth = parent->maximumWidth();
	}
	if (parent->minimumWidth() != 0 && newWidth < parent->maximumWidth()) {
		newWidth = parent->minimumWidth();
	}
	iconSize = (newWidth / itemsPerRow);
	long slotsToDisplay = (itemsPerRow * ((items.getLargestRenderSlot()/itemsPerRow) + 2));
	slotsToDisplay = std::max(minSlots, slotsToDisplay);
	slotsToDisplay = std::min(maxSlots, slotsToDisplay);
	iconAtlas.setAbsoluteSize(iconSize);
	std::cout<<"Root templates = "<<rootTemplates.size()<<'\n';
	std::cout<<"Iterating through "<<slotsToDisplay<<" slots"<<'\n';
	for (int i=0; i<slotsToDisplay; ++i) {
		ItemLabel *label = new ItemLabel(itemStats, parent, mainWindow);
		itemLabels.push_back(label);
		label->setFixedSize(iconSize, iconSize);
		label->move(getItemX(i), getItemY(i));
		GameItem *item = items.getItemByRenderSlot(i);
		if (item != 0) {
			std::cout<<"Processing item "<<i<<'\n';
			label->setItem(item);
			LsbObject *itemObject = item->getObject();
			if (itemObject == 0) {
				continue;
			}
			LsbObject *amountObject = LsbReader::lookupByUniquePathEntity(itemObject, "Amount");
			if (amountObject != 0) {
				item->setItemAmount(*((long *)amountObject->getData()));
			}
			LsbObject *statsObject = LsbReader::lookupByUniquePathEntity(itemObject, "Stats");
			std::vector<LsbObject *> statsObjects = LsbReader::lookupAllEntitiesWithName(itemObject, "Stats");
			LsbObject *statsDirObject = 0;
			for (int i=0; i<statsObjects.size(); ++i) {
				if (statsObjects[i]->isDirectory()) {
					statsDirObject = statsObjects[i];
					break;
				}
			}
			if (statsDirObject != 0) {
				LsbObject *itemTypeObject = LsbReader::lookupByUniquePathEntity(statsDirObject, "ItemType");
				LsbObject *levelObject = LsbReader::lookupByUniquePathEntity(statsDirObject, "Level");
				LsbObject *durabilityObject = LsbReader::lookupByUniquePathEntity(statsDirObject, "Durability");
				char *itemType = itemTypeObject->getData();
				long level = *((long *)levelObject->getData());
				long dura = *((long *)durabilityObject->getData());
				item->setItemRarity(itemType);
				item->setItemLevel(level);
				item->setDurability(dura);
				item->setHasStatsDirectory(true);
				std::vector<LsbObject *> permBoosts = LsbReader::lookupAllEntitiesWithName(statsDirObject, "PermanentBoost");
				item->getPermBoosts().clear();
				for (int i=0; i<permBoosts.size(); ++i) {
					LsbObject *permBoost = permBoosts[i];
					for (int j=0; j<permBoost->getChildren().size(); ++j) {
						LsbObject *permBoostEntry = permBoost->getChildren()[j];
						item->getPermBoosts().push_back(permBoostEntry);
					}
				}
			} else {
				item->setHasStatsDirectory(false);
				item->setItemRarity("Common");
			}
			LsbObject *currentTemplateObject = LsbReader::lookupByUniquePathEntity(itemObject, "CurrentTemplate");
			char *currentTemplate = currentTemplateObject->getData();
			LsbObject *generationObject = LsbReader::lookupByUniquePathEntity(itemObject, "Generation");
			char *statsText = statsObject->getData();
			if (generationObject != 0) {
				std::vector<LsbObject *> boostObjects = LsbReader::lookupAllEntitiesWithName(generationObject, "Boost");
				LsbObject *baseObject = LsbReader::lookupByUniquePathEntity(generationObject, "Base");
				if (baseObject != 0) {
					statsText = baseObject->getData();
				}
				item->getBoosts().clear();
				for (int i=0; i<boostObjects.size(); ++i) {
					LsbObject *boostObject = LsbReader::lookupByUniquePathEntity(boostObjects[i], "Object");
					if (boostObject != 0) {
						char *boostName = boostObject->getData();
						item->addBoost(GenStatsReader::getContainer(itemStats, boostName));
					}
				}
				
				LsbObject *randomObject = LsbReader::lookupByUniquePathEntity(generationObject, "Random");
				if (randomObject != 0) {
					item->setGenerationRandom(*((long *)randomObject->getData()));
				}
			}
			item->setStatsText(statsText);
			if (nameMappings.find(statsText) != nameMappings.end()) {
				item->setItemName(nameMappings[statsText].c_str());
			}
			item->setItemStats(GenStatsReader::getContainer(itemStats, statsText));
			long dataSize = currentTemplateObject->getDataSize();
			
			long matchCount = 0;
			LsbObject *displayNameObject = 0;
			LsbObject *match = 0;
			char *iconName = 0;
			char *description = 0;
			
			match = 0;
			if (modTemplateMap.find(currentTemplate) != modTemplateMap.end()) {
				match = modTemplateMap[currentTemplate];
			}
			if (match != 0) {
				std::cout<<"mod match for "<<currentTemplate<<'\n';
				LsbObject *iconObject = LsbReader::lookupByUniquePathEntity(match, "Icon");
				if (iconObject != 0) {
					iconName = iconObject->getData();
				}
				currentTemplate = LsbReader::lookupByUniquePathEntity(match, "TemplateName")->getData();
				displayNameObject = LsbReader::lookupByUniquePathEntity(match, "DisplayName");
				if (displayNameObject != 0) {
					item->setItemName(displayNameObject->getData());
				}
				LsbObject *descriptionObject = LsbReader::lookupByUniquePathEntity(match, "Description");
				if (descriptionObject != 0) {
					description = descriptionObject->getData();
				}
			}
			matchCount = 0;

			match = 0;
			if (rootTemplateMap.find(currentTemplate) != rootTemplateMap.end()) {
				match = rootTemplateMap[currentTemplate];
			}
			if (match != 0) {
				if (iconName == 0) {
					LsbObject *iconObject = LsbReader::lookupByUniquePathEntity(match, "Icon");
					if (iconObject != 0) {
						iconName = iconObject->getData();
					}
				}
				if (description == 0) {
					LsbObject *descriptionObject = LsbReader::lookupByUniquePathEntity(match, "Description");
					if (descriptionObject != 0) {
						description = descriptionObject->getData();
					}
				}
				if (description != 0) {
					item->setBody(description);
				} else {
					item->setBody("");
				}
				
				if (displayNameObject == 0) {
					displayNameObject = LsbReader::lookupByUniquePathEntity(match, "DisplayName");
					if (displayNameObject != 0) {
						item->setItemName(displayNameObject->getData());
					}
				}
				
				QImage image;
				if (iconName != 0 && iconAtlas.getNamedTexture(iconName, &image)) {
					QPixmap result(image.size());
					if (!drawBackground) {
						result.fill(Qt::transparent);
					}
					QPainter painter(&result);
					if (drawBackground) {
						painter.drawImage(QPoint(0, 0), emptySlotImage);
					}
					
					if (item->getItemRarity() != "Common") {
						static std::map<std::string, RarityRectRGB> rarityRgbMap;
						if (rarityRgbMap.size() == 0) {
							int alpha = 0x40;
							rarityRgbMap["Magic"] = RarityRectRGB({0x18, 0x8E, 0xDE, alpha});
							rarityRgbMap["Rare"] = RarityRectRGB({0x00, 0xAA, 0x00, alpha});
							rarityRgbMap["Legendary"] = RarityRectRGB({0xFF, 0x96, 0x00, alpha});
							rarityRgbMap["Unique"] = RarityRectRGB({0xC7, 0xA7, 0x58, alpha});
						}
						if (rarityRgbMap.find(item->getItemRarity()) != rarityRgbMap.end()) {
							RarityRectRGB& rarityRectRgb = rarityRgbMap[item->getItemRarity()];
							QPainterPath path;
							QRect iconRect(0, 0, iconSize, iconSize);
							path.addRect(iconRect);
							QBrush brush;
							brush.setStyle(Qt::SolidPattern);
							brush.setColor(QColor::fromRgb(rarityRectRgb.r, rarityRectRgb.g, rarityRectRgb.b, rarityRectRgb.a));
							painter.setBrush(brush);
							QPen pen;
							pen.setColor(QColor::fromRgb(rarityRectRgb.r, rarityRectRgb.g, rarityRectRgb.b));
							pen.setStyle(Qt::SolidLine);
							pen.setWidth(3);
							painter.setPen(pen);
							painter.drawPath(path);
						}
					}
					
					painter.drawImage(QPoint(0, 0), image);
					
					if (item->getItemAmount() > 1) {
						std::ostringstream ss;
						ss<<item->getItemAmount();
						
						QFont font("Copperplate Gothic", 13, 80, false);
						{
							painter.setFont(font);
						}
						QRect textRect = painter.boundingRect(0, 0, iconSize, iconSize, 0, ss.str().c_str());
						QPoint drawPoint((iconSize - textRect.right()), iconSize);
						
						QPainterPath path;
						path.addText(drawPoint, font, ss.str().c_str());
						
						QPen pen;
						pen.setWidth(1);
						pen.setStyle(Qt::SolidLine);
						pen.setColor(Qt::black);
						painter.setPen(pen);
						//painter.setPen(Qt::NoPen);
						 
						QBrush brush;
						brush.setColor(Qt::white);
						brush.setStyle(Qt::SolidPattern);
						painter.setBrush(brush);
						//painter.scale(1.2, 1.2);
						painter.drawPath(path);
					}
					
					label->setPixmap(result);
					painter.end();
				}
				else {
					if (iconName == 0) {
						std::cout<<"No icon name defined!\n";
					} else {
						std::cout<<"Failed to find texture "<<iconName<<'\n';
					}
				}
			}
			if (matchCount != 1) {
				std::cout<<"Mismatch for template ID "<<currentTemplate<<", matchCount = "<<matchCount<<"("<<statsText<<")"<<'\n';
			}
			std::string prefix = "";
			std::string suffix = "";
			for (int i=0; i<item->getBoosts().size(); ++i) {
				StatsContainer *boostStats = item->getBoosts()[i];
				
				if (boostStats != 0) {
					long itemRandom = item->getGenerationRandom();
					if (boostStats->getPrefixList().size() > 0 && prefix.size() == 0) {
						prefix = boostStats->getPrefixList()[itemRandom % boostStats->getPrefixList().size()];
					}
					else if (boostStats->getSuffixList().size() > 0 && suffix.size() == 0) {
						suffix = boostStats->getSuffixList()[itemRandom % boostStats->getSuffixList().size()];
					}
				}
			}
			std::string newName = "";
			if (prefix.size() > 0) {
				item->setNamePrefix(prefix);
				newName += prefix;
				newName += " ";
			}
			newName += item->getItemName();
			if (suffix.size() > 0) {
				item->setNameSuffix(suffix);
				newName += " ";
				newName += suffix;
			}
			item->setAffixedName(newName);
		}
		else {
			if (drawBackground) {
				label->setPixmap(QPixmap::fromImage(emptySlotImage));
			}
		}
		label->show();
	}
	long width = getItemX(slotsToDisplay - 1) + iconSize;
	long height = getItemY(slotsToDisplay - 1) + iconSize;
	if (parent->sizePolicy().horizontalPolicy() == QSizePolicy::Fixed && 
			parent->sizePolicy().verticalPolicy() == QSizePolicy::Fixed) {
		parent->setFixedSize(width, height);
	}
}

unsigned long InventoryHandler::slotAtPoint(const QPoint &pt) {
	unsigned long column = pt.x()/iconSize;
	unsigned long row = pt.y()/iconSize;
	return row * itemsPerRow + column;
}

InventoryHandler::~InventoryHandler() {
	for (int i=0; i<itemLabels.size(); ++i) {
		//delete itemLabels[i];
	}
	itemLabels.clear();
}

GameItem *InventoryHandler::getItemAtPoint(const QPoint &pt)
{
	for (int i=0; i<items.getItems().size(); ++i) {
		GameItem *item = items.getItems()[i];
		unsigned long x = getItemX(item->getRenderSlot());
		unsigned long y = getItemY(item->getRenderSlot());
		long right = x + iconSize;
		long bottom = y + iconSize;
		if (pt.x() >= x && pt.x() <= right &&
				pt.y() >= y && pt.y() <= bottom) {
			return item;
		}
	}
	return 0;
}

InventoryHandler::InventoryHandler(QImage emptySlotImage, std::vector<LsbObject *>& stats, 
								   std::vector<LsbObject *> &rootTemplates, std::vector<LsbObject *> &modTemplates, TextureAtlas& iconAtlas, std::vector<StatsContainer *> &itemStats,
								   std::map<std::string, std::string> &nameMappings, std::map<std::string, LsbObject *> &rootTemplateMap, std::map<std::string, LsbObject *> &modTemplateMap) : 
	stats(stats), rootTemplates(rootTemplates), iconAtlas(iconAtlas), modTemplates(modTemplates), itemStats(itemStats), nameMappings(nameMappings),
	rootTemplateMap(rootTemplateMap), modTemplateMap(modTemplateMap)
{
	this->emptySlotImage = emptySlotImage;
}
