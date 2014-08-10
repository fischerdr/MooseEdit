#include "GamePakData.h"
#include "PakReader.h"
#include "LsbReader.h"
#include <QDir>
#include <boost/algorithm/string.hpp>
#include <sys/stat.h>
#include <QListWidget>
#include <fstream>
#include <windows.h>

std::vector<LsbObject *> &GamePakData::getStats()
{
	return stats;
}

void GamePakData::setStats(const std::vector<LsbObject *> &value)
{
	stats = value;
}

std::vector<LsbObject *> &GamePakData::getRootTemplates()
{
	return rootTemplates;
}

void GamePakData::setRootTemplates(const std::vector<LsbObject *> &value)
{
	rootTemplates = value;
}

std::vector<LsbObject *> &GamePakData::getModTemplates()
{
	return modTemplates;
}

void GamePakData::setModTemplates(const std::vector<LsbObject *> &value)
{
	modTemplates = value;
}

std::vector<StatsContainer *> &GamePakData::getItemStats()
{
	return itemStats;
}

void GamePakData::setItemStats(const std::vector<StatsContainer *> &value)
{
	itemStats = value;
}

std::vector<StatsContainer *> &GamePakData::getItemLinks()
{
	return itemLinks;
}

void GamePakData::setItemLinks(const std::vector<StatsContainer *> &value)
{
	itemLinks = value;
}

std::vector<StatsContainer *> &GamePakData::getSkillStats()
{
	return skillStats;
}

void GamePakData::setSkillStats(const std::vector<StatsContainer *> &value)
{
	skillStats = value;
}

std::map<std::string, std::string> &GamePakData::getNameMappings()
{
	return nameMappings;
}

void GamePakData::setNameMappings(const std::map<std::string, std::string> &value)
{
	nameMappings = value;
}

TextureAtlas &GamePakData::getIconAtlas()
{
	return iconAtlas;
}

void GamePakData::setIconAtlas(const TextureAtlas &value)
{
	iconAtlas = value;
}
void GamePakData::buildNameMappings()
{
	LsbObject *stringKeysRoot = LsbReader::lookupByUniquePath(stats, "TranslatedStringKeys/root");
	for (int i=0; i<stringKeysRoot->getChildren().size(); ++i) {
		LsbObject *child = stringKeysRoot->getChildren()[i];
		LsbObject *uuidObject = LsbReader::lookupByUniquePathEntity(child, "UUID");
		LsbObject *contentObject = LsbReader::lookupByUniquePathEntity(child, "Content");
		if (uuidObject != 0 && contentObject != 0) {
			nameMappings[uuidObject->getData()] = contentObject->getData();
		}
	}
}

void GamePakData::load(std::string gameDataPath) {
	LsbReader reader;
	
	PakReader pakReader;
	std::string pakMain(gameDataPath + "Main.pak");
	std::string pakTextures(gameDataPath + "Textures.pak");
	std::string iconLsx("Public/Main/GUI/icons.lsx");
	std::string portraitLsx("Public/Main/GUI/portraits.lsx");
	std::string statsLsb("Public/Main/Localization/Stats.lsb");
	std::string armorsLsb("Public/Main/RootTemplates/Armors.lsb");
	std::string lootLsb("Public/Main/RootTemplates/Loot.lsb");
	std::string uniqueLsb("Public/Main/RootTemplates/Unique.lsb");
	std::string consumablesLsb("Public/Main/RootTemplates/Consumables.lsb");
	std::string equipmentLsb("Public/Main/RootTemplates/Equipment.lsb");
	std::string toolsLsb("Public/Main/RootTemplates/Tools.lsb");
	std::string booksScrollsLsb("Public/Main/RootTemplates/Books_Scrolls.lsb");
	std::string modHomesteadLsb("Mods/Main/Globals/Homestead/Items/items-LS2011_AXEL.lsb");
	std::string modCysealLsb("Mods/Main/Globals/Cyseal/Items/items.lsb");
	std::string modCysealEvelynLsb("Mods/Main/Globals/Cyseal/Items/items-LS2012_JORIS.lsb");
	std::string weaponStatsTxt("Public/Main/Stats/Generated/Data/Weapon.txt");
	std::string deltaModStatsTxt("Public/Main/Stats/Generated/DeltaModifier.txt");
	std::string armorStatsTxt("Public/Main/Stats/Generated/Data/Armor.txt");
	std::string objectStatsTxt("Public/Main/Stats/Generated/Data/Object.txt");
	std::string potionStatsTxt("Public/Main/Stats/Generated/Data/Potion.txt");
	std::string shieldStatsTxt("Public/Main/Stats/Generated/Data/Shield.txt");
	std::string modifiersStatsTxt("Public/Main/Stats/Generated/Structure/Modifiers.txt");
	std::string skillStatsTxt("Public/Main/Stats/Generated/Data/SkillData.txt");
	
	std::string iconDds("Public/Main/Assets/Textures/Icons/icons.dds");
	std::string portraitDds("Public/Main/Assets/Textures/Icons/portraits.dds");
	std::string inventoryCellDds("Public/Main/Assets/Textures/Icons/InventoryCell.dds");
	
	std::string weaponLinksTxt("Public/Main/Stats/Generated/Links/Weapon.txt");
	std::string armorLinksTxt("Public/Main/Stats/Generated/Links/Armor.txt");
	std::string objectLinksTxt("Public/Main/Stats/Generated/Links/Object.txt");
	std::string potionLinksTxt("Public/Main/Stats/Generated/Links/Potion.txt");
	std::string shieldLinksTxt("Public/Main/Stats/Generated/Links/Shield.txt");
	
	std::string tempDirectory = QDir::temp().absolutePath().toStdString();
	
	std::string linkDirectory = tempDirectory;
	linkDirectory += "\\links";
	mkdir(linkDirectory.c_str());
	
	pakReader.loadFile(pakMain);
	pakReader.extractFile(pakMain, iconLsx, tempDirectory, false);
	pakReader.extractFile(pakMain, portraitLsx, tempDirectory, false);
	pakReader.extractFile(pakMain, statsLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, armorsLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, lootLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, uniqueLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, consumablesLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, equipmentLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, toolsLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, booksScrollsLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, modHomesteadLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, modCysealLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, modCysealEvelynLsb, tempDirectory, false);
	pakReader.extractFile(pakMain, weaponStatsTxt, tempDirectory, false);
	pakReader.extractFile(pakMain, deltaModStatsTxt, tempDirectory, false);
	pakReader.extractFile(pakMain, armorStatsTxt, tempDirectory, false);
	pakReader.extractFile(pakMain, objectStatsTxt, tempDirectory, false);
	pakReader.extractFile(pakMain, potionStatsTxt, tempDirectory, false);
	pakReader.extractFile(pakMain, shieldStatsTxt, tempDirectory, false);
	pakReader.extractFile(pakMain, modifiersStatsTxt, tempDirectory, false);
	pakReader.extractFile(pakMain, skillStatsTxt, tempDirectory, false);
	pakReader.extractFile(pakMain, weaponLinksTxt, linkDirectory, false);
	pakReader.extractFile(pakMain, armorLinksTxt, linkDirectory, false);
	pakReader.extractFile(pakMain, objectLinksTxt, linkDirectory, false);
	pakReader.extractFile(pakMain, potionLinksTxt, linkDirectory, false);
	pakReader.extractFile(pakMain, shieldLinksTxt, linkDirectory, false);
	pakReader.loadFile(pakTextures);
	pakReader.extractFile(pakTextures, iconDds, tempDirectory, false);
	pakReader.extractFile(pakTextures, portraitDds, tempDirectory, false);
	pakReader.extractFile(pakTextures, inventoryCellDds, tempDirectory, false);
	
	boost::replace_all(tempDirectory, "/", "\\");
	std::string iconsLsxPath = tempDirectory;
	iconsLsxPath += "\\icons.lsx";
	std::string iconsDdsPath = tempDirectory;
	iconsDdsPath += "\\icons.dds";
	std::string inventoryCellDdsPath = tempDirectory;
	inventoryCellDdsPath += "\\inventoryCell.dds";
	std::string statsLsbPath = tempDirectory;
	statsLsbPath += "\\Stats.lsb";
	std::string armorsLsbPath = tempDirectory;
	armorsLsbPath += "\\Armors.lsb";
	std::string lootLsbPath = tempDirectory;
	lootLsbPath += "\\Loot.lsb";
	std::string portraitsLsbPath = tempDirectory;
	portraitsLsbPath += "\\portraits.dds";
	std::string uniqueLsbPath = tempDirectory;
	uniqueLsbPath += "\\Unique.lsb";
	std::string consumablesLsbPath = tempDirectory;
	consumablesLsbPath += "\\Consumables.lsb";
	std::string equipmentLsbPath = tempDirectory;
	equipmentLsbPath += "\\Equipment.lsb";
	std::string toolsLsbPath = tempDirectory;
	toolsLsbPath += "\\Tools.lsb";
	std::string booksScrollsLsbPath = tempDirectory;
	booksScrollsLsbPath += "\\Books_Scrolls.lsb";
	std::string modHomesteadLsbPath = tempDirectory;
	modHomesteadLsbPath += "\\items-LS2011_AXEL.lsb";
	std::string modCysealLsbPath = tempDirectory;
	modCysealLsbPath += "\\items.lsb";
	std::string modCysealEvelynLsbPath = tempDirectory;
	modCysealEvelynLsbPath += "\\items-LS2012_JORIS.lsb";
	std::string weaponStatsPath = tempDirectory;
	weaponStatsPath += "\\Weapon.txt";
	std::string deltaModStatsPath = tempDirectory;
	deltaModStatsPath += "\\DeltaModifier.txt";
	std::string armorStatsPath = tempDirectory;
	armorStatsPath += "\\Armor.txt";
	std::string objectStatsPath = tempDirectory;
	objectStatsPath += "\\Object.txt";
	std::string potionStatsPath = tempDirectory;
	potionStatsPath += "\\Potion.txt";
	std::string shieldStatsPath = tempDirectory;
	shieldStatsPath += "\\Shield.txt";
	std::string modifiersStatsPath = tempDirectory;
	modifiersStatsPath += "\\Modifiers.txt";
	std::string skillStatsPath = tempDirectory;
	skillStatsPath += "\\SkillData.txt";
	
	std::string weaponLinkPath = linkDirectory;
	weaponLinkPath += "\\Weapon.txt";
	std::string armorLinkPath = linkDirectory;
	armorLinkPath += "\\Armor.txt";
	std::string objectLinkPath = linkDirectory;
	objectLinkPath += "\\Object.txt";
	std::string potionLinkPath = linkDirectory;
	potionLinkPath += "\\Potion.txt";
	std::string shieldLinkPath = linkDirectory;
	shieldLinkPath += "\\Shield.txt";
	
	std::ifstream iconDdsFin(iconsDdsPath.c_str(), std::ios::binary);
	int start = iconDdsFin.tellg();
	iconDdsFin.seekg(0, std::ios_base::end);
	int fileLengthDds = iconDdsFin.tellg() - start;
	iconDdsFin.seekg(0, std::ios_base::beg);
	char *fileBytesDds = new char[fileLengthDds];
	iconDdsFin.read(fileBytesDds, fileLengthDds);
	iconDdsFin.close();
	
	std::ifstream iconLsxFin(iconsLsxPath.c_str(), std::ios::binary);
	start = iconLsxFin.tellg();
	iconLsxFin.seekg(0, std::ios_base::end);
	int fileLengthLsx = iconLsxFin.tellg() - start;
	iconLsxFin.seekg(0, std::ios_base::beg);
	char *fileBytesLsx = new char[fileLengthLsx];
	iconLsxFin.read(fileBytesLsx, fileLengthLsx);
	iconLsxFin.close();
	std::string lsxBytes(fileBytesLsx);
	
	std::ifstream inventoryCellDdsFin(inventoryCellDdsPath.c_str(), std::ios::binary);
	start = inventoryCellDdsFin.tellg();
	inventoryCellDdsFin.seekg(0, std::ios_base::end);
	int fileLengthInventoryCellDds = inventoryCellDdsFin.tellg() - start;
	inventoryCellDdsFin.seekg(0, std::ios_base::beg);
	char *fileBytesInventoryCell = new char[fileLengthInventoryCellDds];
	inventoryCellDdsFin.read(fileBytesInventoryCell, fileLengthInventoryCellDds);
	inventoryCellDdsFin.close();
	
	std::ifstream statsLsbFin(statsLsbPath.c_str(), std::ios::binary);
	stats = reader.loadFile(statsLsbFin);
	statsLsbFin.close();
	
	buildNameMappings();
	
	std::ifstream armorsLsbFin(armorsLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> armorTemplates = reader.loadFile(armorsLsbFin);
	armorsLsbFin.close();
	this->addRootTemplates(armorTemplates);
	
	std::ifstream lootLsbFin(lootLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> lootTemplates = reader.loadFile(lootLsbFin);
	lootLsbFin.close();
	this->addRootTemplates(lootTemplates);
	
	std::ifstream uniqueLsbFin(uniqueLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> uniqueTemplates = reader.loadFile(uniqueLsbFin);
	uniqueLsbFin.close();
	this->addRootTemplates(uniqueTemplates);
	
	std::ifstream consumablesLsbFin(consumablesLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> consumablesTemplates = reader.loadFile(consumablesLsbFin);
	consumablesLsbFin.close();
	this->addRootTemplates(consumablesTemplates);
	
	std::ifstream equipmentLsbFin(equipmentLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> equipmentTemplates = reader.loadFile(equipmentLsbFin);
	equipmentLsbFin.close();
	this->addRootTemplates(equipmentTemplates);
	
	std::ifstream toolsLsbFin(toolsLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> toolsTemplates = reader.loadFile(toolsLsbFin);
	toolsLsbFin.close();
	this->addRootTemplates(toolsTemplates);
	
	std::ifstream booksScrollsLsbFin(booksScrollsLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> booksScrollsTemplates = reader.loadFile(booksScrollsLsbFin);
	booksScrollsLsbFin.close();
	this->addRootTemplates(booksScrollsTemplates);
	
	std::ifstream modHomesteadLsbFin(modHomesteadLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> modHomesteadTemplates = reader.loadFile(modHomesteadLsbFin);
	modHomesteadLsbFin.close();
	this->addModTemplates(modHomesteadTemplates);
	
	std::ifstream modCysealLsbFin(modCysealLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> modCysealTemplates = reader.loadFile(modCysealLsbFin);
	modCysealLsbFin.close();
	this->addModTemplates(modCysealTemplates);
	
	std::ifstream modCysealEvelynLsbFin(modCysealEvelynLsbPath.c_str(), std::ios::binary);
	std::vector<LsbObject *> modCysealEvelynTemplates = reader.loadFile(modCysealEvelynLsbFin);
	modCysealEvelynLsbFin.close();
	this->addModTemplates(modCysealEvelynTemplates);
	
	GenStatsReader statReader;
	
	std::ifstream weaponStatsFin(weaponStatsPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> weaponStats = statReader.loadFile(weaponStatsFin);
	weaponStatsFin.close();
	this->addItemStats(weaponStats);
	
	std::ifstream deltaModStatsFin(deltaModStatsPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> deltaModStats = statReader.loadFile(deltaModStatsFin);
	deltaModStatsFin.close();
	this->addItemStats(deltaModStats);
	
	std::ifstream armorStatsFin(armorStatsPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> armorStats = statReader.loadFile(armorStatsFin);
	armorStatsFin.close();
	this->addItemStats(armorStats);
	
	std::ifstream objectStatsFin(objectStatsPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> objectStats = statReader.loadFile(objectStatsFin);
	objectStatsFin.close();
	this->addItemStats(objectStats);
	
	std::ifstream potionStatsFin(potionStatsPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> potionStats = statReader.loadFile(potionStatsFin);
	potionStatsFin.close();
	this->addItemStats(potionStats);
	
	std::ifstream shieldStatsFin(shieldStatsPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> shieldStats = statReader.loadFile(shieldStatsFin);
	shieldStatsFin.close();
	this->addItemStats(shieldStats);
	
	std::ifstream modifiersStatsFin(modifiersStatsPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> modifiersStats = statReader.loadFile(modifiersStatsFin);
	modifiersStatsFin.close();
	this->addItemStats(modifiersStats);
	
	std::ifstream skillStatsFin(skillStatsPath.c_str(), std::ios::binary);
	skillStats = statReader.loadFile(skillStatsFin);
	skillStatsFin.close();
	
	std::ifstream weaponLinkFin(weaponLinkPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> weaponLink = statReader.loadFile(weaponLinkFin);
	weaponLinkFin.close();
	this->addItemLinks(weaponLink);
	
	std::ifstream armorLinkFin(armorLinkPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> armorLink = statReader.loadFile(armorLinkFin);
	armorLinkFin.close();
	this->addItemLinks(armorLink);
	
	std::ifstream objectLinkFin(objectLinkPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> objectLink = statReader.loadFile(objectLinkFin);
	objectLinkFin.close();
	this->addItemLinks(objectLink);
	
	std::ifstream potionLinkFin(potionLinkPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> potionLink = statReader.loadFile(potionLinkFin);
	potionLinkFin.close();
	this->addItemLinks(potionLink);
	
	std::ifstream shieldLinkFin(shieldLinkPath.c_str(), std::ios::binary);
	std::vector<StatsContainer *> shieldLink = statReader.loadFile(shieldLinkFin);
	shieldLinkFin.close();
	this->addItemLinks(shieldLink);
	
	if (!iconAtlas.loadTextureAtlas(fileBytesDds, fileLengthDds, lsxBytes)) {
		MessageBoxA(0, "failed to load Atlas", 0, 0);
	}
	
	if (inventoryCellImg.loadFromData((const uchar *)fileBytesInventoryCell, fileLengthInventoryCellDds, "DDS")) {
		inventoryCellPtr = &inventoryCellImg;
	}
	
	delete []fileBytesDds;
	delete []fileBytesLsx;
	delete []fileBytesInventoryCell;
}

GamePakData::GamePakData()
{
	
}
