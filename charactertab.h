#ifndef CHARACTERTAB_H
#define CHARACTERTAB_H

#include <QWidget>
#include "InventoryHandler.h"
#include "mainwindow.h"
#include "ItemEditFrame.h"
#include "RelativePositionLayout.h"
#include "SkillEditFrame.h"
#include "EquipmentHandler.h"
#include "CharacterLoader.h"
#include "AppearanceEditorFrame.h"
#include "GamePakData.h"

#define ROOT_TEMPLATE_FEMALE	"878372b3-9280-4819-b8b0-4ca76dea8ad1"
#define ROOT_TEMPLATE_MALE		"878372b3-9280-4819-b8b0-4ca76dea8ad2"
#define ROOT_TEMPLATE_HENCHMALE		"23f3af72-437c-4aff-91ca-914ef6e6ebb7"
#define ROOT_TEMPLATE_HENCHFEMALE	"3ec36a84-93a1-409f-83d6-4b7b17745df6"
#define ROOT_TEMPLATE_MADORA		"5c5447e5-c1cf-4677-b84b-006d9be3f075"
#define ROOT_TEMPLATE_JAHAN			"80240f83-778e-4753-850b-48b05729589c"

namespace Ui {
class characterTab;
}

class characterTab : public QWidget, ItemEditCallback, ItemEditorCloseCallback, AppearanceChangeCallback
{
	Q_OBJECT
	
public:
	explicit characterTab(std::vector<TAG_LSB *> *tagList, std::wstring gameDataPath, LsbObject *itemsObject, CharacterLoader *characterLoader, QTabWidget *tabWidget = 0, QWidget *parent = 0);
	~characterTab();
	void refreshIconSizes();
	void updateToCurrentPortrait();
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
	
	StatTemplateMap *getStatToTemplateMap() const;
	void setStatToTemplateMap(StatTemplateMap *value);
	
	void onClose(ItemEditFrame *itemEditFrame) {
		delete itemEditFrame;
	}
	
	GamePakData *getGamePakData() const;
	void setGamePakData(GamePakData *value);
	void onAppearanceChange(LsbObject *oldPlayerCustomDataObject, LsbObject *newPlayerCustomDataObject);
	
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
	
	void on_statsAvailableEdit_textEdited(const QString &text);
	
	void on_levelEdit_textEdited(const QString &text);
	
	void on_appearanceButton_released();
	
	void on_expEdit_textEdited(const QString &text);
	
protected:
	void showEvent(QShowEvent *);

private:
	CharacterLoader *characterLoader;
	StatTemplateMap *statToTemplateMap;
	std::vector<long> experienceRequired;
	std::wstring gameDataPath;
	QTabWidget *tabWidget;
	bool loadExperienceData();
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
	SkillEditFrame *skillEditFrame = 0;
	AppearanceEditorFrame *appearanceEditorFrame = 0;
	GamePakData *gamePakData;
	int lastWidth = 0;
	void addWidgetsToLayout(QWidget *widget, QLayout *layout);
	long levelFromExperience(long experience);
	void modifyExp(long experience);
	bool populateTalentsView();
	bool populateAbilitiesView();
	bool populateTraitsView();
};

#endif // CHARACTERTAB_H
