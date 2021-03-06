#include "charactertab.h"
#include "ui_charactertab.h"
#include <boost/lexical_cast.hpp>
#include <QPushButton>
#include <windows.h>
#include <QMenu>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include "CheckboxListItem.h"
#include "AbilityGroupItem.h"
#include "ExpandableGroupBox.h"
#include "TraitWidget.h"

characterTab::characterTab(std::vector<TAG_LSB *> *tagList, std::wstring gameDataPath, LsbObject *itemsObject, CharacterLoader *characterLoader, QTabWidget *tabWidget, QWidget *parent) :
	QWidget(parent), tagList(tagList), itemsObject(itemsObject), tabWidget(tabWidget), statToTemplateMap(statToTemplateMap), characterLoader(characterLoader), gameDataPath(gameDataPath),
	ui(new Ui::characterTab)
{
	ui->setupUi(this);
	mainWindow = (MainWindow *)parent;
	QPushButton *prevCharButton = this->findChild<QPushButton *>("prevCharButton");
	QPushButton *nextCharButton = this->findChild<QPushButton *>("nextCharButton");
	QLineEdit *nameEdit = this->findChild<QLineEdit *>("nameEdit");
	QListWidget *talentList = this->findChild<QListWidget *>("talentList");
	QScrollArea *inventoryScrollArea = this->findChild<QScrollArea *>("inventoryScrollArea");
	inventoryScrollArea->setContextMenuPolicy(Qt::CustomContextMenu);
	talentList->setContextMenuPolicy(Qt::CustomContextMenu);
}

long characterTab::levelFromExperience(long experience) {
	for (int i=0; i<experienceRequired.size(); ++i) {
		if (experience < experienceRequired[i]) {
			return i;
		}
	}
	return 0;
}

bool characterTab::loadExperienceData() {	
	QFile mFile(":/experience.txt");
	if(!mFile.open(QFile::ReadOnly | QFile::Text)){
		return false;
	}

	QTextStream in(&mFile);
	QString mText = in.readAll();
	std::stringstream ss(mText.toStdString());
	mFile.close();
	
	std::string input = "";
	for (;;) {
		std::getline(ss, input);
		if (!ss || input.length() == 0) {
			break;
		}
		
		unsigned long exp = 0;
		try {
			exp = boost::lexical_cast<unsigned long>(input);
		} catch (const boost::bad_lexical_cast& e) {
			
		}

		experienceRequired.push_back(exp);
	}
	
	return true;
}

bool characterTab::populateTraitsView() {
	QFile mFile(":/parsed_traits.txt");
	if(!mFile.open(QFile::ReadOnly | QFile::Text)){
		return false;
	}

	QTextStream in(&mFile);
	QString mText = in.readAll();
	std::stringstream ss(mText.toStdString());
	mFile.close();
	GenStatsReader reader;
	traits = reader.loadFile(ss);
	StatsContainer *last = 0;
	std::vector<LsbObject *> traitObjects = this->getCharacter()->getTraitList();
	QWidget *traitsGroup = this->findChild<QWidget *>("traitsGroup");
	for (int i=0; i<traits.size(); ++i) {
		StatsContainer *trait = traits[i];
		
		if (((i + 1) % 2) == 0) {
			if (trait != 0 && last != 0) {
				TraitWidget *item = new TraitWidget(this->getCharacter(), traits, traitsGroup);
				QLabel *leftLabel = item->findChild<QLabel *>("leftLabel");
				QLabel *rightLabel = item->findChild<QLabel *>("rightLabel");
				QLineEdit *leftEdit = item->findChild<QLineEdit *>("leftEdit");
				QLineEdit *rightEdit = item->findChild<QLineEdit *>("rightEdit");
				leftLabel->setText(last->getArg(0).c_str());
				leftLabel->setToolTip(last->getData("tip").c_str());
				rightLabel->setText(trait->getArg(0).c_str());
				rightLabel->setToolTip(trait->getData("tip").c_str());
				std::string idText = "";
				
				idText = last->getData("id");
				long leftId = -1;
				try {
					leftId = boost::lexical_cast<long>(idText);
				} catch (const boost::bad_lexical_cast& e) {
					
				}
				
				idText = trait->getData("id");
				long rightId = -1;
				try {
					rightId = boost::lexical_cast<long>(idText);
				} catch (const boost::bad_lexical_cast& e) {
					
				}
				
				if (leftId < traitObjects.size() && rightId < traitObjects.size()) {
					short leftValue = *((short*)traitObjects[leftId]->getData());
					short rightValue = *((short*)traitObjects[rightId]->getData());
					std::ostringstream ssLeft;
					ssLeft<<leftValue;
					std::ostringstream ssRight;
					ssRight<<rightValue;
					leftEdit->setText(ssLeft.str().c_str());
					leftEdit->setToolTip(last->getData("tip").c_str());
					rightEdit->setText(ssRight.str().c_str());
					rightEdit->setToolTip(trait->getData("tip").c_str());
				}
				traitsGroup->layout()->addWidget(item);
			}
		}
		last = trait;
	}
	return true;
}

bool characterTab::populateAbilitiesView() {
	QLineEdit *abilitiesAvailableEdit = this->findChild<QLineEdit *>("abilitiesAvailableEdit");
	LsbObject *characterObject = this->getCharacter()->getObject();
	LsbObject *abilityPointsObject = characterObject->lookupByUniquePath("PlayerData/PlayerUpgrade/AbilityPoints");
	if (abilityPointsObject != 0) {
		long value = *((long *)abilityPointsObject->getData());
		std::ostringstream ss;
		ss<<value;
		abilitiesAvailableEdit->setText(ss.str().c_str());
	}
	
	QFile mFile(":/parsed_abilities.txt");
	if(!mFile.open(QFile::ReadOnly | QFile::Text)){
		return false;
	}

	QTextStream in(&mFile);
	QString mText = in.readAll();
	std::stringstream ss(mText.toStdString());
	mFile.close();
	GenStatsReader reader;
	abilities = reader.loadFile(ss);
	std::vector<LsbObject *> abilityObjects = this->getCharacter()->getAbilityList();
	QList<ExpandableGroupBox *> groupBoxes = this->findChildren<ExpandableGroupBox *>();
	for (int i=0; i<abilities.size(); ++i) {
		StatsContainer *ability = abilities[i];
		
		AbilityGroupItem *item = 0;
		ExpandableGroupBox *currentGroupBox = 0;
		for (int i=0; i<groupBoxes.size(); ++i) {
			ExpandableGroupBox *groupBox = groupBoxes[i];
			if (ability->getType() == groupBox->title().toStdString()) {
				item = new AbilityGroupItem(this->getCharacter(), abilities, groupBox);
				groupBox->layout()->addWidget(item);
				currentGroupBox = groupBox;
				break;
			}
		}
		if (item != 0) {
			QLabel *abilityLabel = item->findChild<QLabel *>("abilityLabel");
			QLineEdit *abilityEdit = item->findChild<QLineEdit *>("abilityEdit");
			abilityLabel->setText(ability->getArg(0).c_str());
			std::string idText = ability->getData("id");
			long id = -1;
			try {
				id = boost::lexical_cast<long>(idText);
			} catch (const boost::bad_lexical_cast& e) {
				
			}
			if (id < abilityObjects.size()) {
				long value = *((long*)abilityObjects[id]->getData());
				std::ostringstream ss;
				ss<<value;
				abilityEdit->setText(ss.str().c_str());
			}
			item->setToolTip(ability->getData("tip").c_str());
			
			if (currentGroupBox != 0) {
				currentGroupBox->setChecked(true);
				currentGroupBox->on_clicked();
			}
		}
	}
	return true;
}

bool characterTab::populateTalentsView() {
	QLineEdit *talentsAvailableEdit = this->findChild<QLineEdit *>("talentsAvailableEdit");
	LsbObject *characterObject = this->getCharacter()->getObject();
	LsbObject *talentPointsObject = characterObject->lookupByUniquePath("PlayerData/PlayerUpgrade/TalentPoints");
	if (talentPointsObject != 0) {
		long value = *((long *)talentPointsObject->getData());
		std::ostringstream ss;
		ss<<value;
		talentsAvailableEdit->setText(ss.str().c_str());
	}
	
	QFile mFile(":/parsed_talents.txt");
	if(!mFile.open(QFile::ReadOnly | QFile::Text)){
		return false;
	}

	QTextStream in(&mFile);
	QString mText = in.readAll();
	std::stringstream ss(mText.toStdString());
	mFile.close();
	GenStatsReader reader;
	talents = reader.loadFile(ss);
	std::cout<<talents.size()<<'\n';
	QListWidget *talentList = this->findChild<QListWidget *>("talentList");
	talentList->setSortingEnabled(true);
	for (int i=0; i<talents.size(); ++i) {
		StatsContainer *talent = talents[i];
		
		CheckboxListItem *item = new CheckboxListItem();
		item->setText(talent->getArg(0).c_str());
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		std::string idText = talent->getData("id");
		long id = -1;
		try {
			id = boost::lexical_cast<long>(idText);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
		item->setCheckState(Qt::Unchecked);
		if (id != -1) {
			if (this->getCharacter()->hasTalent(id)) {
				item->setCheckState(Qt::Checked);
			}
		}
		item->setToolTip(talent->getData("tip").c_str());
		talentList->addItem(item);
	}
	return true;
}

void characterTab::setSkillStats(std::vector<StatsContainer *> *skillStats) {
	skillEditFrame = new SkillEditFrame(character, skillStats);
}

void characterTab::setCharacter(GameCharacter *character) {
	this->character = character;
	
	QLineEdit *strEdit = this->findChild<QLineEdit *>("strEdit");
	QLineEdit *dexEdit = this->findChild<QLineEdit *>("dexEdit");
	QLineEdit *intEdit = this->findChild<QLineEdit *>("intEdit");
	QLineEdit *consEdit = this->findChild<QLineEdit *>("consEdit");
	QLineEdit *spdEdit = this->findChild<QLineEdit *>("spdEdit");
	QLineEdit *perEdit = this->findChild<QLineEdit *>("perEdit");
	QLineEdit *statsAvailableEdit = this->findChild<QLineEdit *>("statsAvailableEdit");
	LsbObject *playerUpgrades = character->getObject()->lookupByUniquePath("PlayerData/PlayerUpgrade");
	LsbObject *attributePointsObject = playerUpgrades->lookupByUniquePath("AttributePoints");
	std::vector<LsbObject *> attributes = LsbObject::lookupAllEntitiesWithName(playerUpgrades, "Attributes");
	std::vector<LsbObject *> attributeData = LsbObject::extractPropertyForEachListItem(attributes, "Object");
	int idx = 0;
	strEdit->setText(attributeData[idx++]->toString().c_str());
	dexEdit->setText(attributeData[idx++]->toString().c_str());
	intEdit->setText(attributeData[idx++]->toString().c_str());
	consEdit->setText(attributeData[idx++]->toString().c_str());
	spdEdit->setText(attributeData[idx++]->toString().c_str());
	perEdit->setText(attributeData[idx++]->toString().c_str());
	statsAvailableEdit->setText(attributePointsObject->toString().c_str());
	
	populateTalentsView();
	populateAbilitiesView();
	populateTraitsView();
	
	loadExperienceData();
	QLineEdit *levelEdit = this->findChild<QLineEdit *>("levelEdit");
	LsbObject *characterObject = this->getCharacter()->getObject();
	LsbObject *experienceObject = characterObject->lookupByUniquePath("Stats/Experience");
	if (experienceObject != 0) {
		unsigned long exp = *((unsigned long *)experienceObject->getData());
		unsigned long level = levelFromExperience(exp);
		std::ostringstream ss;
		ss<<level;
		levelEdit->setText(ss.str().c_str());
	}
	
	QLabel *nextLevelLabel = this->findChild<QLabel *>("nextLevelLabel");
	QLineEdit *expEdit = this->findChild<QLineEdit *>("expEdit");
	if (experienceObject != 0) {
		long exp = *((long *)experienceObject->getData());
		std::ostringstream ss;
		ss<<exp;
		expEdit->setText(ss.str().c_str());
		
		long level = levelFromExperience(exp);
		if (level > 0) {
			--level;
			if ((level + 1) < experienceRequired.size()) {
				long expReq = experienceRequired[level + 1];
				expReq -= exp;
				std::stringstream ss;
				ss<<expReq;
				nextLevelLabel->setText(ss.str().c_str());
			}
		}
	}
	
	appearanceEditorFrame = new AppearanceEditorFrame(gameDataPath, character);
	appearanceEditorFrame->registerAppearanceChangeCallback(this);
}

characterTab::~characterTab()
{
	delete skillEditFrame;
	
	delete ui;
}

void characterTab::on_nameEdit_textEdited(const QString &text)
{
	LsbObject *nameObject = character->getObject()->lookupByUniquePath("PlayerData/PlayerCustomData/Name");
	std::wstring newName = text.toStdWString();
	nameObject->setData((char *)newName.c_str(), (newName.length() + 1) * 2);
	if (tabWidget != 0) {
		for (int i=0; i<tabWidget->count(); ++i) {
			if (tabWidget->widget(i) == this->getCharacter()->getWidget()) {
				tabWidget->setTabText(i, QString::fromStdWString(newName));
			}
		}
	}
}

void characterTab::on_prevCharButton_released()
{
	characterLoader->getCharacterGroup().getTabWidget()->setCurrentWidget(characterLoader->getCharacterGroup().getPreviousCharacter(this->getCharacter())->getWidget());
}

void characterTab::on_nextCharButton_released()
{
    characterLoader->getCharacterGroup().getTabWidget()->setCurrentWidget(characterLoader->getCharacterGroup().getNextCharacter(this->getCharacter())->getWidget());
}

void characterTab::adjustAttribute(long attribId, long newValue) {
	LsbObject *playerUpgrades = character->getObject()->lookupByUniquePath("PlayerData/PlayerUpgrade");
	std::vector<LsbObject *> attributes = LsbObject::lookupAllEntitiesWithName(playerUpgrades, "Attributes");
	std::vector<LsbObject *> attributeData = LsbObject::extractPropertyForEachListItem(attributes, "Object");
	attributeData[attribId]->setData((char *)&newValue, sizeof(long));
}
GamePakData *characterTab::getGamePakData() const
{
	return gamePakData;
}

void characterTab::setGamePakData(GamePakData *value)
{
	gamePakData = value;
	appearanceEditorFrame->setGamePakData(gamePakData);
}

void characterTab::onAppearanceChange(LsbObject *oldPlayerCustomDataObject, LsbObject *newPlayerCustomDataObject) {
	if (oldPlayerCustomDataObject == 0 || newPlayerCustomDataObject == 0) {
		return;
	}
	LsbObject *copyOldPlayerCustomDataObject = new LsbObject(*oldPlayerCustomDataObject);
	LsbObject *copyPlayerCustomDataObject = new LsbObject(*newPlayerCustomDataObject);
	bool success = false;
	LsbObject *playerCustomDataObject = character->getPlayerCustomDataObject();
	if (playerCustomDataObject != 0) {
		LsbObject *playerDataObject = playerCustomDataObject->getParent();
		if (playerDataObject != 0) {
			if (playerDataObject->replaceChild(playerCustomDataObject, copyPlayerCustomDataObject)) {
				success = true;
				LsbObject *oldIsMaleObject = copyOldPlayerCustomDataObject->lookupByUniquePath("IsMale");
				LsbObject *newIsMaleObject = copyPlayerCustomDataObject->lookupByUniquePath("IsMale");
				if (oldIsMaleObject != 0 && newIsMaleObject != 0) {
					bool oldIsMale = *((bool *)oldIsMaleObject->getData());
					bool newIsMale = *((bool *)newIsMaleObject->getData());
					if (oldIsMale != newIsMale) {
						LsbObject *characterObject = character->getObject();
						LsbObject *currentTemplateObject = characterObject->lookupByUniquePath("CurrentTemplate");
						if (currentTemplateObject != 0) {
							std::string currentTemplate = currentTemplateObject->getData();
							if (currentTemplate == ROOT_TEMPLATE_FEMALE || currentTemplate == ROOT_TEMPLATE_MALE) {
								if (newIsMale == true) {
									currentTemplate = ROOT_TEMPLATE_MALE;
								} else {
									currentTemplate = ROOT_TEMPLATE_FEMALE;
								}
								currentTemplateObject->setData(currentTemplate.c_str(), currentTemplate.length() + 1);
							}
							if (currentTemplate == ROOT_TEMPLATE_HENCHMALE || currentTemplate == ROOT_TEMPLATE_HENCHFEMALE) {
								if (newIsMale == true) {
									currentTemplate = ROOT_TEMPLATE_HENCHMALE;
								} else {
									currentTemplate = ROOT_TEMPLATE_HENCHFEMALE;
								}
								currentTemplateObject->setData(currentTemplate.c_str(), currentTemplate.length() + 1);
							}
						}
					}
				}
			}
		}
	}
	updateToCurrentPortrait();
	if (!success) {
		MessageBoxA(0, "Failed to change appearance", 0, 0);
	}
	delete copyOldPlayerCustomDataObject;
}

void characterTab::updateToCurrentPortrait() {
	QLabel *portraitLabel = this->findChild<QLabel *>("portraitLabel");
	LsbObject *playerCustomDataObject = character->getPlayerCustomDataObject();
	if (playerCustomDataObject != 0) {
		LsbObject *iconObject = playerCustomDataObject->lookupByUniquePath("Icon");
		if (iconObject != 0) {
			std::string icon = iconObject->getData();
			if (gamePakData != 0) {
				
				LsbObject *characterObject = character->getObject();
				if (characterObject != 0) {
					LsbObject *originalTemplateIdObject = characterObject->lookupByUniquePath("OriginalTemplate");
					std::string origTemplateId = "";
					if (originalTemplateIdObject != 0) {
						origTemplateId = originalTemplateIdObject->getData();
					}
					if (origTemplateId == "5c5447e5-c1cf-4677-b84b-006d9be3f075") {
						icon = "Portrait_CP_Madora";
					} else if (origTemplateId == "80240f83-778e-4753-850b-48b05729589c") {
						icon = "Portrait_CP_Jahan";
					}
				}
				
				QImage image;
				bool success = gamePakData->getPortraitAtlas().getNamedTexture(icon.c_str(), &image);
				if (success) {
					if (portraitLabel != 0) {
						portraitLabel->setPixmap(QPixmap::fromImage(image));
					}
				}
			}
		}
	}
}

EquipmentHandler *characterTab::getEquipmentHandler() const
{
	return equipmentHandler;
}

void characterTab::setEquipmentHandler(EquipmentHandler *handler)
{
	equipmentHandler = handler;
	if (equipmentHandler != 0) {
		equipmentHandler->drawAll();
	}
	appearanceEditorFrame->setEquipHandler(handler);
}

std::vector<StatsContainer *> characterTab::getItemLinks() const
{
	return itemLinks;
}

void characterTab::setItemLinks(const std::vector<StatsContainer *> &value)
{
	itemLinks = value;
}

std::map<std::string, std::string> *characterTab::getNameMappings() const
{
	return nameMappings;
}

void characterTab::setNameMappings(std::map<std::string, std::string> *value)
{
	nameMappings = value;
}

std::vector<StatsContainer *> characterTab::getAllItemStats() const
{
	return allItemStats;
}

void characterTab::setAllItemStats(const std::vector<StatsContainer *> &value)
{
	allItemStats = value;
}


void characterTab::on_strEdit_textEdited(const QString &text)
{
	try {
		adjustAttribute(ATTRIB_STR, boost::lexical_cast<long>(text.toStdString()));
	} catch (const boost::bad_lexical_cast& e) {
		
	}
}

void characterTab::on_dexEdit_textEdited(const QString &text)
{
	try {
	   adjustAttribute(ATTRIB_DEX, boost::lexical_cast<long>(text.toStdString()));
	} catch (const boost::bad_lexical_cast& e) {
		
	}
}

void characterTab::on_intEdit_textEdited(const QString &text)
{
	try {
		adjustAttribute(ATTRIB_INT, boost::lexical_cast<long>(text.toStdString()));
	} catch (const boost::bad_lexical_cast& e) {
		
	}
}

void characterTab::on_consEdit_textEdited(const QString &text)
{
	try {
		adjustAttribute(ATTRIB_CONS, boost::lexical_cast<long>(text.toStdString()));
	} catch (const boost::bad_lexical_cast& e) {
		
	}
}

void characterTab::on_spdEdit_textEdited(const QString &text)
{
	try {
		adjustAttribute(ATTRIB_SPD, boost::lexical_cast<long>(text.toStdString()));
	} catch (const boost::bad_lexical_cast& e) {
		
	}
}

void characterTab::on_perEdit_textEdited(const QString &text)
{
	try {
		adjustAttribute(ATTRIB_PER, boost::lexical_cast<long>(text.toStdString()));
	} catch (const boost::bad_lexical_cast& e) {
		
	}
}

void characterTab::onItemEdited(GameItem *newItem, GameItem *oldItem) {
	LsbObject *parent = oldItem->getObject()->getParent();
	GameItem *copy = new GameItem(*newItem);
	bool isAddedItem = false;
	if (!this->character->getInventoryHandler()->getItems()->removeItem(oldItem)) {
		isAddedItem = true;
		//item did not exist in inventoryHandler: item is newly added to inventory
		itemsObject->addChild(copy->getObject());
		LsbObject *slotObject = copy->getObject()->lookupByUniquePath("Slot");
		if (slotObject != 0) {
			unsigned short newSlot = this->character->getInventoryHandler()->getItems()->getLargestInternalSlot() + 1;
			//unsigned short newSlot = copy->getRenderSlot() + 15;
			slotObject->setData((char *)&newSlot, sizeof(newSlot));
		}
		
		LsbObject *newCreatorObject = GameItem::createNewItemCreator(tagList, itemsObject, copy->getObject());
		LsbObject *creatorsObject = GameItem::getCreatorsObject(itemsObject);
		creatorsObject->addChild(newCreatorObject);
		
		LsbObject *newItemObject = LsbObject::getObjectFromCreator(newCreatorObject, "Items");
		//LsbObject *newItemObject = copy->getObject();
		LsbObject *statsObject = newItemObject->lookupByUniquePath("Stats");
		std::string inventoryTab = "";
		if (statsObject != 0) {
			std::string statName = statsObject->getData();
			StatsContainer *itemStat = GenStatsReader::getContainer(allItemStats, statName);
			if (itemStat != 0) {
				inventoryTab = itemStat->getData("InventoryTab");
			}
		}
		unsigned long extraInventoryTab = 0;
		unsigned long extraViewSlot = 0;
		if (inventoryTab == "Equipment") {
			extraInventoryTab = 2;
			extraViewSlot = this->character->getInventoryHandler()->getItems()->getLargestEquipmentSlot() + 1;
			copy->setEquipmentSlot(extraViewSlot);
		} else if (inventoryTab == "Consumable") {
			extraInventoryTab = 3;
			extraViewSlot = this->character->getInventoryHandler()->getItems()->getLargestConsumableSlot() + 1;
			copy->setConsumableSlot(extraViewSlot);
		} else if (inventoryTab == "Magical") {
			extraInventoryTab = 4;
			extraViewSlot = this->character->getInventoryHandler()->getItems()->getLargestMagicalSlot() + 1;
			copy->setMagicalSlot(extraViewSlot);
		} else if (inventoryTab == "Ingredient") {
			extraInventoryTab = 5;
			extraViewSlot = this->character->getInventoryHandler()->getItems()->getLargestIngredientSlot() + 1;
			copy->setIngredientSlot(extraViewSlot);
		} else if (inventoryTab == "Keys") {
			extraInventoryTab = 6;
			extraViewSlot = this->character->getInventoryHandler()->getItems()->getLargestKeysSlot() + 1;
			copy->setKeysSlot(extraViewSlot);
		} else if (inventoryTab == "Misc") {
			extraInventoryTab = 7;
			extraViewSlot = this->character->getInventoryHandler()->getItems()->getLargestMiscSlot() + 1;
			copy->setMiscSlot(extraViewSlot);
		}
		
		character->addItemToInventoryObject(newCreatorObject, copy->getRenderSlot(), extraInventoryTab, extraViewSlot, false);
	}
	if (!parent->replaceChild(oldItem->getObject(), copy->getObject())) {
		if (!isAddedItem) {
			MessageBoxA(0, "Child replace failed!", 0, 0);
		}
	}
	delete oldItem;
	this->character->getInventoryHandler()->getItems()->addItem(copy);
	this->character->getInventoryHandler()->draw(this->findChild<QWidget *>("inventoryContents"), this->parentWidget(), true);
}

void characterTab::on_inventoryScrollArea_customContextMenuRequested(const QPoint &pos)
{
	QScrollArea *inventoryScrollArea = this->findChild<QScrollArea *>("inventoryScrollArea");
	QWidget *inventoryContents = inventoryScrollArea->findChild<QWidget *>("inventoryContents");
	QPoint itemPos = inventoryContents->mapFromParent(pos);
	GameItem *originalItem = this->getCharacter()->getInventoryHandler()->getItemAtPoint(itemPos);
	GameItem *item = originalItem;
    QMenu contextMenu(this);
	if (item != 0) {
		contextMenu.addAction("&Edit Item");
	}
	else {
		contextMenu.addAction("&Add Item");
	}
	QAction *result = contextMenu.exec(inventoryScrollArea->mapToGlobal(pos));
	if (result) {
		if (item != 0) {
			ItemEditFrame *itemEditFrame = new ItemEditFrame(allItemStats, itemLinks, item, itemEditHandler, this, tagList, *nameMappings, *statToTemplateMap, *randTable);
			itemEditFrame->registerCloseCallback(this);
		} else {
			LsbObject *itemObject = GameItem::createNewItem(tagList, itemsObject, character->getInventoryId(), character->getCreatorId());
			GameItem *newItem = new GameItem(tagList);
			newItem->setObject(itemObject);
			newItem->setRenderSlot(character->getInventoryHandler()->slotAtPoint(itemPos));
			ItemEditFrame *itemEditFrame = new ItemEditFrame(allItemStats, itemLinks, newItem, itemEditHandler, this, tagList, *nameMappings, *statToTemplateMap, *randTable);
			itemEditFrame->registerCloseCallback(this);
		}
	}
}

void characterTab::addWidgetsToLayout(QWidget *widget, QLayout *layout) {
	layout->addWidget(widget);
	for (int i=0; i<widget->children().size(); ++i) {
		QObject *object = widget->children()[i];
		if (object->isWidgetType()) {
			QWidget *child = (QWidget *)object;
			//relPosLayout->addWidget(widget);
			addWidgetsToLayout(child, layout);
		}
	}
}

void characterTab::redraw_inventory()
{
	if (lastWidth != this->findChild<QWidget *>("inventoryContents")->parentWidget()->width()) {
		this->findChild<QWidget *>("inventoryContents")->setMaximumWidth(this->findChild<QWidget *>("inventoryContents")->parentWidget()->width());
		this->findChild<QWidget *>("inventoryContents")->setMinimumWidth(this->findChild<QWidget *>("inventoryContents")->parentWidget()->width());
		lastWidth = this->findChild<QWidget *>("inventoryContents")->parentWidget()->width();
		this->getCharacter()->getInventoryHandler()->draw(this->findChild<QWidget *>("inventoryContents"), this->parentWidget(), true);
		if (equipmentHandler != 0) {
			equipmentHandler->drawAll();
		}
	}
}

void characterTab::refreshIconSizes() {
	this->redraw_inventory();
}

void characterTab::showEvent(QShowEvent *)
{
	if (relPosLayout == 0) {
		QWidget *equipmentWidget = this->findChild<QWidget *>("equipmentWidget");
		RelativePositionLayout *relPosLayout2 = new RelativePositionLayout(equipmentWidget);
		for (int i=0; i<equipmentWidget->children().size(); ++i) {
			QObject *object = equipmentWidget->children()[i];
			if (object->isWidgetType()) {
				QWidget *widget = (QWidget *)object;
				relPosLayout2->addWidget(widget);
			}
		}
		equipmentWidget->setLayout(relPosLayout2);
		
		QFrame *frame = this->findChild<QFrame *>("frame");
		relPosLayout = new RelativePositionLayout(frame);
		for (int i=0; i<frame->children().size(); ++i) {
			QObject *object = frame->children()[i];
			if (object->isWidgetType()) {
				QWidget *widget = (QWidget *)object;
				relPosLayout->addWidget(widget);
			}
		}
		frame->setLayout(relPosLayout);
		
		this->setLayout(new QGridLayout(this));
		this->layout()->setMargin(0);
		this->layout()->setSpacing(0);
		this->layout()->addWidget(frame);
	}
	QTimer::singleShot(0, this, SLOT(redraw_inventory()));
}
StatTemplateMap *characterTab::getStatToTemplateMap() const {
	return statToTemplateMap;
}

void characterTab::setStatToTemplateMap(StatTemplateMap *value) {
	statToTemplateMap = value;
}


void characterTab::on_talentList_itemClicked(QListWidgetItem *item)
{
	if (item->checkState() == Qt::Checked) {
		item->setCheckState(Qt::Unchecked);
	}
	else {
		item->setCheckState(Qt::Checked);
	}
	item->listWidget()->setCurrentItem(0);
}

void characterTab::on_talentsAvailableEdit_textEdited(const QString &text)
{
    long value = 0;
	try {
		value = boost::lexical_cast<long>(text.toStdString());
	} catch (const boost::bad_lexical_cast& e) {
		;
	}
	LsbObject *characterObject = this->getCharacter()->getObject();
	LsbObject *talentPointsObject = characterObject->lookupByUniquePath("PlayerData/PlayerUpgrade/TalentPoints");
	if (talentPointsObject != 0) {
		talentPointsObject->setData((char *)&value, sizeof(long));
	}
}

void characterTab::on_talentList_itemChanged(QListWidgetItem *item)
{
	StatsContainer *talent = GenStatsReader::getContainer(talents, item->text().toStdString());
	if (talent != 0) {
		std::string idText = talent->getData("id");
		long id = -1;
		try {
			id = boost::lexical_cast<long>(idText);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
		if (item->checkState() == Qt::Checked) {
			this->getCharacter()->setTalent(id, true);
		} else {
			this->getCharacter()->setTalent(id, false);
		}
	}
}

void characterTab::on_talentList_customContextMenuRequested(const QPoint &pos)
{
	QListWidget *talentList = this->findChild<QListWidget *>("talentList");
    QMenu menu;
	menu.addAction("&Check all");
	menu.addAction("&Uncheck all");
	QAction *result = menu.exec(talentList->mapToGlobal(pos));
	if (result != 0) {
		if (result->text() == "&Check all") {
			for (int i=0; i<talentList->count(); ++i) {
				QListWidgetItem *item = talentList->item(i);
				item->setCheckState(Qt::Checked);
			}
		} else if (result->text() == "&Uncheck all") {
			talentList->setSortingEnabled(false);
			for (int i=0; i<talentList->count(); ++i) {
				QListWidgetItem *item = talentList->item(i);
				item->setCheckState(Qt::Unchecked);
			}
			talentList->setSortingEnabled(true);
			talentList->sortItems();
		}
	}
}

void characterTab::on_abilitiesAvailableEdit_textEdited(const QString &text)
{
	long value = 0;
	try {
		value = boost::lexical_cast<long>(text.toStdString());
	} catch (const boost::bad_lexical_cast& e) {
		;
	}
	LsbObject *characterObject = this->getCharacter()->getObject();
	LsbObject *talentPointsObject = characterObject->lookupByUniquePath("PlayerData/PlayerUpgrade/AbilityPoints");
	if (talentPointsObject != 0) {
		talentPointsObject->setData((char *)&value, sizeof(long));
	}
}

void characterTab::on_skillsButton_released()
{
    skillEditFrame->show();
}

void characterTab::on_statsAvailableEdit_textEdited(const QString &text)
{
	long value = 0;
	try {
		value = boost::lexical_cast<long>(text.toStdString());
	} catch (const boost::bad_lexical_cast& e) {
		;
	}
	LsbObject *characterObject = this->getCharacter()->getObject();
	LsbObject *attribPointsObject = characterObject->lookupByUniquePath("PlayerData/PlayerUpgrade/AttributePoints");
	if (attribPointsObject != 0) {
		attribPointsObject->setData((char *)&value, sizeof(long));
	}
}

void characterTab::on_levelEdit_textEdited(const QString &text)
{
	bool valid = true;
	QLineEdit *expEdit = this->findChild<QLineEdit *>("expEdit");
	long value = 1;
	try {
		value = boost::lexical_cast<long>(text.toStdString());
	} catch (const boost::bad_lexical_cast& e) {
		valid = false;
	}
	--value;
	if (value >= 0 && value < experienceRequired.size()) {
		long exp = experienceRequired[value];
		if (valid) {
			modifyExp(exp);
			std::ostringstream ss;
			ss<<exp;
			expEdit->setText(ss.str().c_str());
		}
	}
}

void characterTab::on_appearanceButton_released()
{
	if (appearanceEditorFrame != 0) {
		LsbObject *characterObject = character->getObject();
		bool doShow = true;
		std::string errorMessage;
		LsbObject *currentTemplateObject = characterObject->lookupByUniquePath("CurrentTemplate");
		if (currentTemplateObject != 0) {
			std::string currentTemplate = currentTemplateObject->getData();
			const static std::string cannotEdit = " appearance cannot be edited.";
			if (currentTemplate == ROOT_TEMPLATE_MADORA) {
				errorMessage = "Madora's" + cannotEdit;
				doShow = false;
			}
			else if (currentTemplate == ROOT_TEMPLATE_JAHAN) {
				errorMessage = "Jahan's" + cannotEdit;
				doShow = false;
			}
		}
		if (doShow) {
			appearanceEditorFrame->show();
		} else {
			QMessageBox msg;
			msg.setText(errorMessage.c_str());
			msg.exec();
		}
	}
}

void characterTab::modifyExp(long experience) {
	QLabel *nextLevelLabel = this->findChild<QLabel *>("nextLevelLabel");
	LsbObject *characterObject = this->getCharacter()->getObject();
	LsbObject *experienceObject = characterObject->lookupByUniquePath("Stats/Experience");
	if (experienceObject != 0) {
		experienceObject->setData((char *)&experience, sizeof(experience));
		long level = levelFromExperience(experience);
		if (level > 0) {
			--level;
			if ((level + 1) < experienceRequired.size()) {
				long expReq = experienceRequired[level + 1];
				expReq -= experience;
				std::stringstream ss;
				ss<<expReq;
				nextLevelLabel->setText(ss.str().c_str());
			}
		}
	}
}

void characterTab::on_expEdit_textEdited(const QString &text)
{
	QLineEdit *levelEdit = this->findChild<QLineEdit *>("levelEdit");
	bool valid = true;
	long value = 0;
	try {
		value = boost::lexical_cast<long>(text.toStdString());
	} catch (const boost::bad_lexical_cast& e) {
		valid = false;
	}
	if (valid) {
		modifyExp(value);
		
		unsigned long level = levelFromExperience(value);
		std::ostringstream ss;
		ss<<level;
		levelEdit->setText(ss.str().c_str());
	}
}
