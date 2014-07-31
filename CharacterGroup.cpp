#include "CharacterGroup.h"

GameCharacter *CharacterGroup::getNextCharacter(GameCharacter *current) {
	for (int i=0; i<this->getCharacters().size(); ++i) {
		if (this->getCharacters()[i] == current) {
			return this->getCharacters()[(i + 1) % this->getCharacters().size()];
		}
	}
	return 0;
}

GameCharacter *CharacterGroup::getPreviousCharacter(GameCharacter *current) {
	for (int i=0; i<this->getCharacters().size(); ++i) {
		if (this->getCharacters()[i] == current) {
			return this->getCharacters()[(i + this->getCharacters().size() - 1) % this->getCharacters().size()];
		}
	}
	return 0;
}

CharacterGroup::CharacterGroup()
{
}
