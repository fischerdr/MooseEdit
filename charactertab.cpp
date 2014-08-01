#include "charactertab.h"
#include "ui_charactertab.h"
#include <boost/lexical_cast.hpp>
#include <QPushButton>
#include <windows.h>
#include <QMenu>
#include <QTimer>

characterTab::characterTab(std::vector<TAG_LSB *> *tagList, QWidget *parent) :
	QWidget(parent), tagList(tagList),
	ui(new Ui::characterTab)
{
	ui->setupUi(this);
	mainWindow = (MainWindow *)parent;
	QPushButton *prevCharButton = this->findChild<QPushButton *>("prevCharButton");
	QPushButton *nextCharButton = this->findChild<QPushButton *>("nextCharButton");
	QLineEdit *nameEdit = this->findChild<QLineEdit *>("nameEdit");
	QScrollArea *inventoryScrollArea = this->findChild<QScrollArea *>("inventoryScrollArea");
	inventoryScrollArea->setContextMenuPolicy(Qt::CustomContextMenu);
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
}

characterTab::~characterTab()
{
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
	adjustAttribute(ATTRIB_STR, boost::lexical_cast<long>(text.toStdString()));
}

void characterTab::on_dexEdit_textEdited(const QString &text)
{
    adjustAttribute(ATTRIB_DEX, boost::lexical_cast<long>(text.toStdString()));
}

void characterTab::on_intEdit_textEdited(const QString &text)
{
    adjustAttribute(ATTRIB_INT, boost::lexical_cast<long>(text.toStdString()));
}

void characterTab::on_consEdit_textEdited(const QString &text)
{
    adjustAttribute(ATTRIB_CONS, boost::lexical_cast<long>(text.toStdString()));
}

void characterTab::on_spdEdit_textEdited(const QString &text)
{
    adjustAttribute(ATTRIB_SPD, boost::lexical_cast<long>(text.toStdString()));
}

void characterTab::on_perEdit_textEdited(const QString &text)
{
    adjustAttribute(ATTRIB_PER, boost::lexical_cast<long>(text.toStdString()));
}

void characterTab::onItemEdited(GameItem *newItem, GameItem *oldItem) {
	LsbObject *parent = oldItem->getObject()->getParent();
	this->character->getInventoryHandler()->getItems()->removeItem(oldItem);
	GameItem *copy = new GameItem(*newItem);
	parent->replaceChild(oldItem->getObject(), copy->getObject());
	delete oldItem;
	this->character->getInventoryHandler()->getItems()->addItem(copy);
	this->character->getInventoryHandler()->draw(this->findChild<QWidget *>("inventoryContents"), this->parentWidget());
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
		std::cout<<"MW="<<this->findChild<QWidget *>("inventoryContents")->maximumWidth()<<'\n';
		std::cout<<"PW="<<this->findChild<QWidget *>("inventoryContents")->parentWidget()->width()<<'\n';
		this->findChild<QWidget *>("inventoryContents")->setMaximumWidth(this->findChild<QWidget *>("inventoryContents")->parentWidget()->width());
		this->findChild<QWidget *>("inventoryContents")->setMinimumWidth(this->findChild<QWidget *>("inventoryContents")->parentWidget()->width());
		lastWidth = this->findChild<QWidget *>("inventoryContents")->parentWidget()->width();
		std::cout<<"MW2="<<this->findChild<QWidget *>("inventoryContents")->maximumWidth()<<'\n';
		std::cout<<"PW2="<<this->findChild<QWidget *>("inventoryContents")->parentWidget()->width()<<'\n';
		this->getCharacter()->getInventoryHandler()->draw(this->findChild<QWidget *>("inventoryContents"), this->parentWidget());
	}
}

void characterTab::showEvent(QShowEvent *)
{
	if (relPosLayout == 0) {
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

void characterTab::on_listWidget_itemClicked(QListWidgetItem *item)
{
	if (item->checkState() == Qt::Checked) {
		item->setCheckState(Qt::Unchecked);
	}
	else {
		item->setCheckState(Qt::Checked);
	}
}
