#ifndef TRAITWIDGET_H
#define TRAITWIDGET_H

#include <QWidget>
#include "GenStatsReader.h"
#include "GameCharacter.h"

namespace Ui {
class TraitWidget;
}

class TraitWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit TraitWidget(GameCharacter *character, std::vector<StatsContainer *> &traits, QWidget *parent = 0);
	~TraitWidget();
	
private slots:
	void on_traitScroll_valueChanged(int value);
	
	void on_leftEdit_textChanged(const QString &arg1);
	
	void on_rightEdit_textChanged(const QString &arg1);
	
private:
	Ui::TraitWidget *ui;
	GameCharacter *character;
	std::vector<StatsContainer *> &traits;
	void editTextChanged(const QString &text, QLabel *label);
};

#endif // TRAITWIDGET_H
