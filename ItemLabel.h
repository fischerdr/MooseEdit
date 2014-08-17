#ifndef ITEMLABEL_H
#define ITEMLABEL_H

#include <QLabel>
#include <QTextEdit>
#include <vector>
#include "TooltipFrame.h"
#include "GameItem.h"
#include "GenStatsReader.h"

class ItemLabel : public QLabel
{
	Q_OBJECT
public:
	explicit ItemLabel(std::vector<StatsContainer *>& allItemStats, QWidget *parent = 0, QWidget *mainWindow = 0);
	GameItem *getItem() {
		return this->item;
	}
	void setItem(GameItem *item) {
		this->item = item;
	}
	~ItemLabel() {
		if (tooltip != 0) {
			tooltip->hide();
			delete tooltip;
		}
	}

protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

private:
	bool ignoreEnterEvent = false;
	TooltipFrame *tooltip = 0;
	GameItem *item = 0;
	void setupTooltip();
	std::vector<StatsContainer *>& allItemStats;
	double calculateDamage(long damageStat, long itemLevel, long damageBoost);
	long getSummedStat(std::string statName);
	void displayItemStats(std::ostringstream &contentHtml);
	long getItemStatValue(std::string statName);
	long getModStatValue(std::string statName);
	long getSummedItemStat(std::string statName);
	long getPermBoostStatValue(std::string statName);
signals:
	
public slots:
	
};

#endif // ITEMLABEL_H
