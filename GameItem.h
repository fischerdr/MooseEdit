#ifndef GAMEITEM_H
#define GAMEITEM_H

#include "LsbObject.h"
#include "GenStatsReader.h"
#include <string>
#include <vector>

#define SLOT_INVALID 0xFFFF

class GameItem
{
	LsbObject *object;
	unsigned short slot = SLOT_INVALID;
	std::string itemName;
	std::string body;
	StatsContainer *itemStats = 0;
	std::vector<StatsContainer *> boosts;
	std::vector<LsbObject *> permBoosts;
	long itemLevel;
	long durability;
	std::string itemRarity;
	std::string statsText;
	bool statsDirectory = false;
	long generationRandom;
	std::vector<TAG_LSB*> *tagList;
public:
	GameItem(std::vector<TAG_LSB*> *tagList);
	GameItem(const GameItem &other) : tagList(other.tagList) {
		this->object = new LsbObject(*other.object);
		this->slot = other.slot;
		this->itemName = other.itemName;
		this->body = other.body;
		this->itemStats = other.itemStats;
		this->boosts = other.boosts;
		this->itemLevel = other.itemLevel;
		this->itemRarity = other.itemRarity;
		this->statsText = other.statsText;
		this->statsDirectory = other.statsDirectory;
		this->generationRandom = other.generationRandom;
	}
	LsbObject *createStatsDirectory();
	LsbObject *createGenerationDirectory();
	bool removeStatsDirectory();
	bool removeGenerationDirectory();
	LsbObject *getStatsDirectory();
	LsbObject *getGenerationDirectory();
	void setHasStatsDirectory(bool statsDirectory) {
		this->statsDirectory = statsDirectory;
	}
	bool hasStatsDirectory() {
		return this->statsDirectory;
	}
	void setSlot(unsigned short slot) {
		this->slot = slot;
	}
	unsigned short getSlot() {
		return this->slot;
	}
	LsbObject *getObject() {
		return object;
	}
	void setObject(LsbObject *object) {
		this->object = object;
	}
	std::string& getItemName() {
		return this->itemName;
	}
	void setItemName(const char *itemName) {
		this->itemName = itemName;
	}
	std::string& getBody() {
		return this->body;
	}
	void setBody(const char *body) {
		this->body = body;
	}
	StatsContainer *getItemStats() const {
		return itemStats;
	}
	void setItemStats(StatsContainer *value) {
		itemStats = value;
	}
	std::vector<StatsContainer *> &getBoosts() {
		return boosts;
	}
	void addBoost(StatsContainer *value) {
		boosts.push_back(value);
	}
	void removeBoost(StatsContainer *value) {
		for (int i=0; i<boosts.size(); ++i) {
			StatsContainer *current = boosts[i];
			if (current == value) {
				boosts.erase(boosts.begin() + i);
				break;
			}
		}
	}
	long getItemLevel() const {
		return itemLevel;
	}
	void setItemLevel(long value) {
		itemLevel = value;
	}
	std::string getItemRarity() const {
		return itemRarity;
	}
	void setItemRarity(const std::string &value) {
		itemRarity = value;
	}
	std::string getStatsText() const;
	void setStatsText(const std::string &value);
	std::vector<LsbObject *> &getPermBoosts();
	void setPermBoosts(const std::vector<LsbObject *> &value);
	long getGenerationRandom() const;
	void setGenerationRandom(long value);
	long getDurability() const;
	void setDurability(long value);
};

#endif // GAMEITEM_H
