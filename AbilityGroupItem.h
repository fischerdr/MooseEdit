#ifndef ABILITYGROUPITEM_H
#define ABILITYGROUPITEM_H

#include <QWidget>
#include <vector>
#include "GameCharacter.h"
#include "GenStatsReader.h"

namespace Ui {
class AbilityGroupItem;
}

class AbilityGroupItem : public QWidget
{
	Q_OBJECT
	
public:
	explicit AbilityGroupItem(GameCharacter *character, std::vector<StatsContainer *> &abilities, QWidget *parent = 0);
	~AbilityGroupItem();
	
private slots:
	void on_abilityEdit_textEdited(const QString &arg1);
	
private:
	Ui::AbilityGroupItem *ui;
	GameCharacter *character;
	std::vector<StatsContainer *> &abilities;
};

#endif // ABILITYGROUPITEM_H
