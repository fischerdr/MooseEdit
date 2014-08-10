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

#define PROCESSING_TYPE_NONE			0
#define PROCESSING_TYPE_STATS			1
#define PROCESSING_TYPE_ROOTTEMPLATE	2
#define PROCESSING_TYPE_MODTEMPLATE		3
#define PROCESSING_TYPE_ITEMSTATS		4
#define PROCESSING_TYPE_SKILLSTATS		5
#define PROCESSING_TYPE_ITEMLINKS		6

void GamePakData::parsePakFile(std::string& pakPath, const char *pakExtractPath, std::string& outputDir, int processingType) {
	if (pakPath != lastPakPath) {
		pakReader.loadFile(pakPath);
		lastPakPath = pakPath;
	}
	std::string extractPath = pakExtractPath;
	pakReader.extractFile(pakPath, extractPath, outputDir, false);
	std::string extractedPath = pakReader.getLastExtractPath();
	
	switch (processingType) {
	
	case PROCESSING_TYPE_STATS: {
		std::ifstream lsbFin(extractedPath.c_str(), std::ios::binary);
		stats = lsbReader.loadFile(lsbFin);
		lsbFin.close();
		
		buildNameMappings();
		break;}
	case PROCESSING_TYPE_ROOTTEMPLATE: {
		std::ifstream lsbFin(extractedPath.c_str(), std::ios::binary);
		std::vector<LsbObject *> rootTemplates = lsbReader.loadFile(lsbFin);
		lsbFin.close();
		this->addRootTemplates(rootTemplates);
		break;}
	case PROCESSING_TYPE_MODTEMPLATE: {
		std::ifstream lsbFin(extractedPath.c_str(), std::ios::binary);
		std::vector<LsbObject *> modTemplates = lsbReader.loadFile(lsbFin);
		lsbFin.close();
		this->addModTemplates(modTemplates);
		break;}
	case PROCESSING_TYPE_ITEMSTATS: {
		std::ifstream lsbFin(extractedPath.c_str(), std::ios::binary);
		std::vector<StatsContainer *> itemStats = genStatsReader.loadFile(lsbFin);
		lsbFin.close();
		this->addItemStats(itemStats);
		break;}
	case PROCESSING_TYPE_SKILLSTATS: {
		std::ifstream lsbFin(extractedPath.c_str(), std::ios::binary);
		skillStats = genStatsReader.loadFile(lsbFin);
		lsbFin.close();
		break;}
	case PROCESSING_TYPE_ITEMLINKS: {
		std::ifstream lsbFin(extractedPath.c_str(), std::ios::binary);
		std::vector<StatsContainer *> itemLinks = genStatsReader.loadFile(lsbFin);
		lsbFin.close();
		this->addItemLinks(itemLinks);
		break;}
		
	}
}

void GamePakData::load(std::string gameDataPath) {
	std::string pakMain(gameDataPath + "Main.pak");
	std::string pakTextures(gameDataPath + "Textures.pak");
	std::string iconLsx("Public/Main/GUI/icons.lsx");
	std::string portraitLsx("Public/Main/GUI/portraits.lsx");
	
	std::string iconDds("Public/Main/Assets/Textures/Icons/icons.dds");
	std::string portraitDds("Public/Main/Assets/Textures/Icons/portraits.dds");
	std::string inventoryCellDds("Public/Main/Assets/Textures/Icons/InventoryCell.dds");
	
	std::string tempDirectory = QDir::temp().absolutePath().toStdString();
	
	std::string linkDirectory = tempDirectory;
	linkDirectory += "\\links";
	mkdir(linkDirectory.c_str());
	
	parsePakFile(pakMain, "Public/Main/Localization/Stats.lsb", tempDirectory, PROCESSING_TYPE_STATS);
//	parsePakFile(pakMain, "Public/Main/RootTemplates/Armors.lsb", tempDirectory, PROCESSING_TYPE_ROOTTEMPLATE);
//	parsePakFile(pakMain, "Public/Main/RootTemplates/Loot.lsb", tempDirectory, PROCESSING_TYPE_ROOTTEMPLATE);
//	parsePakFile(pakMain, "Public/Main/RootTemplates/Unique.lsb", tempDirectory, PROCESSING_TYPE_ROOTTEMPLATE);
//	parsePakFile(pakMain, "Public/Main/RootTemplates/Consumables.lsb", tempDirectory, PROCESSING_TYPE_ROOTTEMPLATE);
//	parsePakFile(pakMain, "Public/Main/RootTemplates/Equipment.lsb", tempDirectory, PROCESSING_TYPE_ROOTTEMPLATE);
//	parsePakFile(pakMain, "Public/Main/RootTemplates/Tools.lsb", tempDirectory, PROCESSING_TYPE_ROOTTEMPLATE);
//	parsePakFile(pakMain, "Public/Main/RootTemplates/Books_Scrolls.lsb", tempDirectory, PROCESSING_TYPE_ROOTTEMPLATE);
	
	std::string rootTemplatesPath = "Public/Main/RootTemplates/";
	std::string modTemplatesPath = "Mods/Main/Globals/";
	std::string modTemplatesPath2 = "Mods/Main/Levels/";
	std::string itemPathText = "/Items/";
	std::vector<std::string> allMainFiles = pakReader.getFileList();
	std::string rootTemplateTemp = tempDirectory + "\\RootTemplates";
	std::string modTemplateTemp = tempDirectory + "\\ModTemplates";
	mkdir(rootTemplateTemp.c_str());
	mkdir(modTemplateTemp.c_str());
	for (int i=0; i<allMainFiles.size(); ++i) {
		std::string &mainFile = allMainFiles[i];
		if (boost::contains(mainFile, itemPathText)) {
			 if (boost::starts_with(mainFile, modTemplatesPath) || boost::starts_with(mainFile, modTemplatesPath2)) {
				parsePakFile(pakMain, mainFile.c_str(), modTemplateTemp, PROCESSING_TYPE_MODTEMPLATE);
			}
		} else {
			if (boost::starts_with(mainFile, rootTemplatesPath)) {
				parsePakFile(pakMain, mainFile.c_str(), rootTemplateTemp, PROCESSING_TYPE_ROOTTEMPLATE);
			}
		}
	}
	
	//load all root templates
	//pakReader.getFileList();
	
	//load all mod templates
	//pakReader.getFileList()
	
//	parsePakFile(pakMain, "Mods/Main/Globals/Homestead/Items/items-LS2011_AXEL.lsb", tempDirectory, PROCESSING_TYPE_MODTEMPLATE);
//	parsePakFile(pakMain, "Mods/Main/Globals/Cyseal/Items/items.lsb", tempDirectory, PROCESSING_TYPE_MODTEMPLATE);
//	parsePakFile(pakMain, "Mods/Main/Globals/Cyseal/Items/items-LS2012_JORIS.lsb", tempDirectory, PROCESSING_TYPE_MODTEMPLATE);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Data/Weapon.txt", tempDirectory, PROCESSING_TYPE_ITEMSTATS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/DeltaModifier.txt", tempDirectory, PROCESSING_TYPE_ITEMSTATS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Data/Armor.txt", tempDirectory, PROCESSING_TYPE_ITEMSTATS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Data/Object.txt", tempDirectory, PROCESSING_TYPE_ITEMSTATS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Data/Potion.txt", tempDirectory, PROCESSING_TYPE_ITEMSTATS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Data/Shield.txt", tempDirectory, PROCESSING_TYPE_ITEMSTATS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Structure/Modifiers.txt", tempDirectory, PROCESSING_TYPE_ITEMSTATS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Data/SkillData.txt", tempDirectory, PROCESSING_TYPE_SKILLSTATS);
		
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Links/Weapon.txt", linkDirectory, PROCESSING_TYPE_ITEMLINKS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Links/Armor.txt", linkDirectory, PROCESSING_TYPE_ITEMLINKS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Links/Object.txt", linkDirectory, PROCESSING_TYPE_ITEMLINKS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Links/Potion.txt", linkDirectory, PROCESSING_TYPE_ITEMLINKS);
	parsePakFile(pakMain, "Public/Main/Stats/Generated/Links/Shield.txt", linkDirectory, PROCESSING_TYPE_ITEMLINKS);
	
	pakReader.loadFile(pakMain);
	pakReader.extractFile(pakMain, iconLsx, tempDirectory, false);
	pakReader.extractFile(pakMain, portraitLsx, tempDirectory, false);
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
	std::string portraitsLsbPath = tempDirectory;
	portraitsLsbPath += "\\portraits.dds";
	
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
