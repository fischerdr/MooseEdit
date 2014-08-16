#ifndef ITEMEDITFRAME_H
#define ITEMEDITFRAME_H

#include <QFrame>
#include <vector>
#include "GenStatsReader.h"
#include "StatsView.h"
#include "ItemGeneral.h"
#include "GameItem.h"
#include "InventoryHandler.h"
#include "ItemTemplateWidget.h"
#include "GamePakData.h"

namespace Ui {
class ItemEditFrame;
}

class ItemEditCallback {
public:
	virtual void onItemEdited(GameItem *newItem, GameItem *oldItem) = 0;
};

class BaseStatsViewSelectCallback;
class ModsViewAddCallback;
class ModsPickerSelectCallback;
class ModsPickerCancelCallback;
class PermBoostViewAddCallback;
class PermBoostPickerSelectCallback;
class PermBoostPickerCancelCallback;
class PermBoostViewRemoveCallback;
class ModsViewRemoveCallback;

class ItemEditFrame : public QFrame, GeneralEditCallback, TableEditCallback, TemplateEditCallback
{
	Q_OBJECT
	
	friend class BaseStatsViewSelectCallback;
	friend class ModsViewAddCallback;
	friend class ModsPickerSelectCallback;
	friend class ModsPickerCancelCallback;
	friend class PermBoostViewAddCallback;
	friend class PermBoostPickerSelectCallback;
	friend class PermBoostPickerCancelCallback;
	friend class PermBoostViewRemoveCallback;
	friend class ModsViewRemoveCallback;
public:
	explicit ItemEditFrame(std::vector<StatsContainer *> &allItemStats, std::vector<StatsContainer *> &itemLinks, GameItem *item, InventoryHandler *itemEditHandler, 
						   ItemEditCallback *itemEditCallback, std::vector<TAG_LSB *> *tagList, std::map<std::string, std::string> &nameMappings, 
						   StatTemplateMap &statToTemplateMap, QWidget *parent = 0);
	void onEdit();
	void onEdit(DataContainerTreeItem *&selectedItem, QTreeWidget *statsTree, QTableWidgetItem *editedItem, QTableWidget *table);
	void onTemplateEdit(std::string &newTemplate);
	~ItemEditFrame();
	
private slots:
	void on_baseStatsButton_released();
	
	void on_modsButton_released();
	
	void on_generalButton_released();
	
	void on_cancelButton_released();
	
	void on_acceptButton_released();
	
	void on_permBoostButton_released();
	
	void on_exportButton_released();
	
	void on_importButton_released();
	
	void on_templateButton_released();
	
private:
	StatTemplateMap &statToTemplateMap;
	std::string currentDir = ".";
	Ui::ItemEditFrame *ui;
	ItemGeneral *generalView;
	StatsView *baseStatsView;
	StatsView *modsView;
	StatsView *modsPicker;
	StatsView *permBoostView;
	StatsView *permBoostPicker;
	ItemTemplateWidget *itemTemplateWidget;
	std::vector<StatsContainer *> &allItemStats;
	std::vector<StatsContainer *> &itemLinks;
	std::vector<TAG_LSB *> *tagList;
	BaseStatsViewSelectCallback *baseStatsViewSelectCallback;
	ModsViewAddCallback *modsViewAddCallback;
	ItemEditCallback *itemEditCallback;
	ModsPickerSelectCallback *modsPickerSelectCallback;
	ModsPickerCancelCallback *modsPickerCancelCallback;
	PermBoostViewAddCallback *permBoostViewAddCallback;
	PermBoostPickerSelectCallback *permBoostPickerSelectCallback;
	PermBoostPickerCancelCallback *permBoostPickerCancelCallback;
	PermBoostViewRemoveCallback *permBoostViewRemoveCallback;
	ModsViewRemoveCallback *modsViewRemoveCallback;
	std::map<std::string, std::string> &nameMappings;
	GameItem *item;
	GameItem *oldItem;
	InventoryHandler *itemEditHandler;
	void redraw();
	void hideAllViews();
	void refreshViewData();
};

#endif // ITEMEDITFRAME_H
