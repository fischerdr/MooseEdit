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

characterTab::characterTab(std::vector<TAG_LSB *> *tagList, QWidget *parent) :
	QWidget(parent), tagList(tagList),
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
				rightLabel->setText(trait->getArg(0).c_str());
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
					rightEdit->setText(ssRight.str().c_str());
				}
				item->setToolTip(trait->getData("tip").c_str());
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
	LsbObject *abilityPointsObject = LsbReader::lookupByUniquePathEntity(characterObject, "PlayerData/PlayerUpgrade/AbilityPoints");
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
	LsbObject *talentPointsObject = LsbReader::lookupByUniquePathEntity(characterObject, "PlayerData/PlayerUpgrade/TalentPoints");
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
	LsbObject *playerUpgrades = LsbReader::lookupByUniquePathEntity(character->getObject(), "PlayerData/PlayerUpgrade");
	std::vector<LsbObject *> attributes = LsbReader::lookupAllEntitiesWithName(playerUpgrades, "Attributes");
	std::vector<LsbObject *> attributeData = LsbReader::extractPropertyForEachListItem(attributes, "Object");
	int idx = 0;
	strEdit->setText(attributeData[idx++]->toString().c_str());
	dexEdit->setText(attributeData[idx++]->toString().c_str());
	intEdit->setText(attributeData[idx++]->toString().c_str());
	consEdit->setText(attributeData[idx++]->toString().c_str());
	spdEdit->setText(attributeData[idx++]->toString().c_str());
	perEdit->setText(attributeData[idx++]->toString().c_str());
	
	populateTalentsView();
	populateAbilitiesView();
	populateTraitsView();
}

characterTab::~characterTab()
{
	delete skillEditFrame;
	
	delete ui;
}

void characterTab::on_nameEdit_textEdited(const QString &text)
{
	LsbObject *nameObject = LsbReader::lookupByUniquePathEntity(character->getObject(), "PlayerData/PlayerCustomData/Name");
	std::string newName = text.toStdString();
	long allocBytes = (newName.length() + 1) * 2;
	char *alloc = new char[allocBytes];
	mbstowcs((wchar_t *)alloc, newName.c_str(), newName.length());
	nameObject->setData(alloc, allocBytes);
	delete []alloc;
}

void characterTab::on_prevCharButton_released()
{
	mainWindow->getCharacterGroup().getTabWidget()->setCurrentWidget(mainWindow->getCharacterGroup().getPreviousCharacter(this->getCharacter())->getWidget());
}

void characterTab::on_nextCharButton_released()
{
    mainWindow->getCharacterGroup().getTabWidget()->setCurrentWidget(mainWindow->getCharacterGroup().getNextCharacter(this->getCharacter())->getWidget());
}

void characterTab::adjustAttribute(long attribId, long newValue) {
	LsbObject *playerUpgrades = LsbReader::lookupByUniquePathEntity(character->getObject(), "PlayerData/PlayerUpgrade");
	std::vector<LsbObject *> attributes = LsbReader::lookupAllEntitiesWithName(playerUpgrades, "Attributes");
	std::vector<LsbObject *> attributeData = LsbReader::extractPropertyForEachListItem(attributes, "Object");
	attributeData[attribId]->setData((char *)&newValue, sizeof(long));
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
	this->character->getInventoryHandler()->getItems()->removeItem(oldItem);
	GameItem *copy = new GameItem(*newItem);
	parent->replaceChild(oldItem->getObject(), copy->getObject());
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
			ItemEditFrame *itemEditFrame = new ItemEditFrame(allItemStats, itemLinks, item, itemEditHandler, this, tagList, *nameMappings);
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
		//addWidgetsToLayout(frame, relPosLayout);
		for (int i=0; i<frame->children().size(); ++i) {
			QObject *object = frame->children()[i];
			if (object->isWidgetType()) {
				QWidget *widget = (QWidget *)object;
				relPosLayout->addWidget(widget);
				//addWidgetsToLayout(widget, relPosLayout);
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
	LsbObject *talentPointsObject = LsbReader::lookupByUniquePathEntity(characterObject, "PlayerData/PlayerUpgrade/TalentPoints");
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
	LsbObject *talentPointsObject = LsbReader::lookupByUniquePathEntity(characterObject, "PlayerData/PlayerUpgrade/AbilityPoints");
	if (talentPointsObject != 0) {
		talentPointsObject->setData((char *)&value, sizeof(long));
	}
}

void characterTab::on_skillsButton_released()
{
    skillEditFrame->show();
}
