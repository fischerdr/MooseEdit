#include "GameItem.h"
#include <windows.h>


std::string GameItem::getStatsText() const
{
	return statsText;
}

void GameItem::setStatsText(const std::string &value)
{
	statsText = value;
}

std::vector<LsbObject *>& GameItem::getPermBoosts()
{
	return permBoosts;
}

void GameItem::setPermBoosts(const std::vector<LsbObject *> &value)
{
	permBoosts = value;
}

long GameItem::getGenerationRandom() const
{
	return generationRandom;
}

void GameItem::setGenerationRandom(long value)
{
	generationRandom = value;
}

long GameItem::getDurability() const
{
	return durability;
}

void GameItem::setDurability(long value)
{
	durability = value;
}
GameItem::GameItem()
{
	this->itemName = "?";
	this->body = "";
}
