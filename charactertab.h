#ifndef CHARACTERTAB_H
#define CHARACTERTAB_H

#include <QWidget>
#include "InventoryHandler.h"
#include "mainwindow.h"
#include "ItemEditFrame.h"
#include "RelativePositionLayout.h"

namespace Ui {
class characterTab;
}

class characterTab : public QWidget, ItemEditCallback
{
	Q_OBJECT
	
public:
	explicit characterTab(std::vector<TAG_LSB *> *tagList, QWidget *parent = 0);
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

	void on_listWidget_itemClicked(QListWidgetItem *item);
	
protected:
	void showEvent(QShowEvent *);

private:
	Ui::characterTab *ui;
	MainWindow *mainWindow;
	GameCharacter *character;
	void adjustAttribute(long attribId, long newValue);
	InventoryHandler *itemEditHandler;
	std::vector<StatsContainer *> allItemStats;
	std::vector<StatsContainer *> itemLinks;
	std::vector<TAG_LSB *> *tagList;
	std::map<std::string, std::string> *nameMappings = 0;
	RelativePositionLayout *relPosLayout = 0;
	int lastWidth = 0;
	void addWidgetsToLayout(QWidget *widget, QLayout *layout);
};

#endif // CHARACTERTAB_H
