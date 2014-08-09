#ifndef CHARACTERTAB_H
#define CHARACTERTAB_H

#include <QWidget>
#include "InventoryHandler.h"
#include "mainwindow.h"
#include "ItemEditFrame.h"
#include "RelativePositionLayout.h"
#include "SkillEditFrame.h"
#include "EquipmentHandler.h"

namespace Ui {
class characterTab;
}

class characterTab : public QWidget, ItemEditCallback
{
	Q_OBJECT
	
public:
	explicit characterTab(std::vector<TAG_LSB *> *tagList, LsbObject *itemsObject, QWidget *parent = 0);
	~characterTab();
	void setCharacter(GameCharacter *character);
	GameCharacter *getCharacter() {
		return character;
	}
	void setItemEditHandler(InventoryHandler *itemEditHandler) {
		this->itemEditHandler = itemEditHandler;
	}
	
	std::vector<StatsContainer *> getAllItemStats() const;
	void setAllItemStats(const std::vector<StatsContainer *> &value);
	void onItemEdited(GameItem *newItem, GameItem *oldItem);
	
	std::map<std::string, std::string> *getNameMappings() const;
	void setNameMappings(std::map<std::string, std::string> *value);
	
	std::vector<StatsContainer *> getItemLinks() const;
	void setItemLinks(const std::vector<StatsContainer *> &value);
	void setSkillStats(std::vector<StatsContainer *> *skillStats);
	
	EquipmentHandler *getEquipmentHandler() const;
	void setEquipmentHandler(EquipmentHandler *handler);
	
private slots:
	
	void on_nameEdit_textEdited(const QString &arg1);
	
	void on_prevCharButton_released();
	
	void on_nextCharButton_released();
	
	void on_strEdit_textEdited(const QString &arg1);
	
	void on_dexEdit_textEdited(const QString &arg1);
	
	void on_intEdit_textEdited(const QString &arg1);
	
	void on_consEdit_textEdited(const QString &arg1);
	
	void on_spdEdit_textEdited(const QString &arg1);
	
	void on_perEdit_textEdited(const QString &arg1);
	
	void on_inventoryScrollArea_customContextMenuRequested(const QPoint &pos);
	
	void redraw_inventory();

	void on_talentList_itemClicked(QListWidgetItem *item);
	
	void on_talentsAvailableEdit_textEdited(const QString &arg1);
	
	void on_talentList_itemChanged(QListWidgetItem *item);
	
	void on_talentList_customContextMenuRequested(const QPoint &pos);
	
	void on_abilitiesAvailableEdit_textEdited(const QString &arg1);
	
	void on_skillsButton_released();
	
protected:
	void showEvent(QShowEvent *);

private:
	LsbObject *itemsObject;
	Ui::characterTab *ui;
	MainWindow *mainWindow;
	GameCharacter *character;
	void adjustAttribute(long attribId, long newValue);
	InventoryHandler *itemEditHandler;
	EquipmentHandler *equipmentHandler;
	std::vector<StatsContainer *> allItemStats;
	std::vector<StatsContainer *> itemLinks;
	std::vector<TAG_LSB *> *tagList;
	std::vector<StatsContainer *> talents;
	std::vector<StatsContainer *> abilities;
	std::vector<StatsContainer *> traits;
	std::map<std::string, std::string> *nameMappings = 0;
	RelativePositionLayout *relPosLayout = 0;
	SkillEditFrame *skillEditFrame;
	int lastWidth = 0;
	void addWidgetsToLayout(QWidget *widget, QLayout *layout);
	bool populateTalentsView();
	bool populateAbilitiesView();
	bool populateTraitsView();
};

#endif // CHARACTERTAB_H
