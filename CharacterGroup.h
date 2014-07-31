#ifndef CHARACTERGROUP_H
#define CHARACTERGROUP_H

#include <QTabWidget>
#include "GameCharacter.h"

class CharacterGroup
{
	std::vector<GameCharacter *> characters;
	QTabWidget *tabWidget;
public:
	CharacterGroup();
	std::vector<GameCharacter *>& getCharacters() {
		return characters;
	}
	void addCharacter(GameCharacter *character) {
		characters.push_back(character);
	}

	QTabWidget *getTabWidget() {
		return this->tabWidget;
	}
	
	void setTabWidget(QTabWidget *tabWidget) {
		this->tabWidget = tabWidget;
	}
	
	GameCharacter *getNextCharacter(GameCharacter *current);
	GameCharacter *getPreviousCharacter(GameCharacter *current);
};

#endif // CHARACTERGROUP_H
