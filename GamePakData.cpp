#include "GamePakData.h"
#include "PakReader.h"
#include "LsbReader.h"
#include <QDir>
#include <boost/algorithm/string.hpp>
#include <sys/stat.h>
#include <QListWidget>
#include <fstream>
#include <windows.h>
#include <boost/filesystem/fstream.hpp>

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


std::map<std::string, LsbObject *> &GamePakData::getRootTemplateMap()
{
	return rootTemplateMap;
}

void GamePakData::setRootTemplateMap(const std::map<std::string, LsbObject *> &value)
{
	rootTemplateMap = value;
}

std::map<std::string, LsbObject *> &GamePakData::getModTemplateMap()
{
	return modTemplateMap;
}

void GamePakData::setModTemplateMap(const std::map<std::string, LsbObject *> &value)
{
	modTemplateMap = value;
}
void GamePakData::populateRootTemplateMap(std::vector<LsbObject *>& rootTemplates) {
	for (int i=0; i<rootTemplates.size(); ++i) {
		LsbObject *rootTemplate = rootTemplates[i];
		LsbObject *rootObject = LsbReader::lookupByUniquePathEntity(rootTemplate, "root");
		std::vector<LsbObject *> mapKeyObjects = LsbReader::extractPropertyForEachListItem(rootObject->getChildren(), "MapKey");
		for (int j=0; j<mapKeyObjects.size(); ++j) {
			LsbObject *mapKeyObject = mapKeyObjects[j];
			std::string templateKey = mapKeyObject->getData();
			rootTemplateMap[templateKey] = mapKeyObject->getParent();
		}
	}
}

void GamePakData::populateModTemplateMap(std::vector<LsbObject *>& modTemplates) {
	for (int i=0; i<modTemplates.size(); ++i) {
		LsbObject *modTemplate = modTemplates[i];
		LsbObject *rootObject = LsbReader::lookupByUniquePathEntity(modTemplate, "root");
		std::vector<LsbObject *> mapKeyObjects = LsbReader::extractPropertyForEachListItem(rootObject->getChildren(), "MapKey");
		for (int j=0; j<mapKeyObjects.size(); ++j) {
			LsbObject *mapKeyObject = mapKeyObjects[j];
			std::string templateKey = mapKeyObject->getData();
			modTemplateMap[templateKey] = mapKeyObject->getParent();
		}
	}
}


StatTemplateMap &GamePakData::getStatToTemplateMap() {
	return statToTemplateMap;
}

void GamePakData::setStatToTemplateMap(const StatTemplateMap &value) {
	statToTemplateMap = value;
}

void GamePakData::addTemplatesToStatTemplateMap(std::vector<LsbObject *> &templatesObjects, bool isRootTemplate) {
	if (templatesObjects.size() == 1) {
		LsbObject *templatesObject = LsbReader::lookupByUniquePath(templatesObjects, "Templates");
		if (templatesObject != 0) {
			LsbObject *rootObject = LsbReader::lookupByUniquePathEntity(templatesObject, "root");
			if (rootObject != 0) {
				for (int i=0; i<rootObject->getChildren().size(); ++i) {
					LsbObject *gameObject = rootObject->getChildren()[i];
					addTemplateToStatTemplateMap(gameObject, isRootTemplate);
				}
			}
		}
	}
}

void GamePakData::addTemplateToStatTemplateMap(LsbObject *gameObject, bool isRootTemplate) {
	if (gameObject != 0) {
		LsbObject *mapKeyObject = LsbReader::lookupByUniquePathEntity(gameObject, "MapKey");
		LsbObject *statsObject = LsbReader::lookupByUniquePathEntity(gameObject, "Stats");
		if (mapKeyObject != 0 && statsObject != 0) {
			std::string templateId = mapKeyObject->getData();
			std::string statId = statsObject->getData();
//			if (statToTemplateMap.find(statId) == statToTemplateMap.end()) {
//				statToTemplateMap[statId] = std::vector<ItemTemplateData>();
//			}
			std::vector<ItemTemplateData> &templateList = statToTemplateMap[statId];
			templateList.push_back(ItemTemplateData());
			ItemTemplateData &templateData = templateList.back();
			templateData.gameObject = gameObject;
			templateData.templateId = templateId;
			templateData.isRootTemplate = isRootTemplate;
		}
	}
}

void GamePakData::addItemLinksToStatTemplateMap(std::vector<StatsContainer *> &itemLinks) {
	for (int i=0; i<itemLinks.size(); ++i) {
		StatsContainer *link = itemLinks[i];
		std::string statId = link->getArg(0);
		std::string templateId = link->getArg(1);
//		if (statToTemplateMap.find(statId) == statToTemplateMap.end()) {
//			statToTemplateMap[statId] = std::vector<ItemTemplateData>();
//		}
		std::vector<ItemTemplateData> &templateList = statToTemplateMap[statId];
		bool found = false;
		for (int i=0; i<templateList.size(); ++i) {
			ItemTemplateData &templateData = templateList[i];
			if (templateData.templateId == templateId) {
				found = true;
				break;
			}
		}
		if (!found && rootTemplateMap.find(templateId) != rootTemplateMap.end()) {
			LsbObject *gameObject = rootTemplateMap[templateId];
			templateList.push_back(ItemTemplateData());
			ItemTemplateData &templateData = templateList.back();
			templateData.gameObject = gameObject;
			templateData.templateId = templateId;
			templateData.isRootTemplate = true;
		}
		if (!found && modTemplateMap.find(templateId) != modTemplateMap.end()) {
			LsbObject *gameObject = modTemplateMap[templateId];
			templateList.push_back(ItemTemplateData());
			ItemTemplateData &templateData = templateList.back();
			templateData.gameObject = gameObject;
			templateData.templateId = templateId;
			templateData.isRootTemplate = false;
		}
	}
}

void GamePakData::addModLookupsToStatTemplateMap() {
	for (StatTemplateMap::iterator it = statToTemplateMap.begin(); it != statToTemplateMap.end(); ++it) {
		std::vector<ItemTemplateData> &templateList = it->second;
		for (int i=0; i<templateList.size(); ++i) {
			ItemTemplateData &templateData = templateList[i];
			if (templateData.isRootTemplate) {
				std::string rootTemplateId = templateData.templateId;
				if (rootTemplateMods.find(rootTemplateId) != rootTemplateMods.end()) {
					std::vector<std::string > &modTemplateIds = rootTemplateMods[rootTemplateId];
					for (int j=0; j<modTemplateIds.size(); ++j) {
						std::string modTemplateId = modTemplateIds[j];
						bool found = false;
						for (int k=0; k<templateList.size(); ++k) {
							ItemTemplateData &templateData = templateList[k];
							if (templateData.templateId == modTemplateId) {
								found = true;
								break;
							}
						}
						if (!found && modTemplateMap.find(modTemplateId) != modTemplateMap.end()) {
							LsbObject *gameObject = modTemplateMap[modTemplateId];
							templateList.push_back(ItemTemplateData());
							ItemTemplateData &templateData = templateList.back();
							templateData.gameObject = gameObject;
							templateData.templateId = modTemplateId;
							templateData.isRootTemplate = false;
						}
					}
				}
			}
		}
	}
}

void GamePakData::addModsToRootModsList() {
	for (int i=0; i<modTemplates.size(); ++i) {
		LsbObject *modTemplate = modTemplates[i];
		LsbObject *rootObject = LsbReader::lookupByUniquePathEntity(modTemplate, "root");
		if (rootObject != 0) {
			for (int j=0; j<rootObject->getChildren().size(); ++j) {
				LsbObject *gameObject = rootObject->getChildren()[j];
				if (gameObject != 0) {
					LsbObject *mapKeyObject = LsbReader::lookupByUniquePathEntity(gameObject, "MapKey");
					LsbObject *templateNameObject = LsbReader::lookupByUniquePathEntity(gameObject, "TemplateName");
					if (mapKeyObject != 0 && templateNameObject != 0) {
						std::string modTemplateId = mapKeyObject->getData();
						std::string rootTemplateId = templateNameObject->getData();
//						if (rootTemplateMods.find(rootTemplateId) == rootTemplateMods.end()) {
//							rootTemplateMods[rootTemplateId] = std::vector<std::string >();
//						}
						std::vector<std::string > &templateList = rootTemplateMods[rootTemplateId];
						templateList.push_back(modTemplateId);
					}
				}
			}
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
void GamePakData::parsePakFile(std::wstring& pakPath, const char *pakExtractPath, std::wstring& outputDir, int processingType) {
	if (pakPath != lastPakPath) {
		pakReader.loadFile(pakPath);
		lastPakPath = pakPath;
	}
	std::string extractPath = pakExtractPath;
	unsigned long fileSize;
	char *fileBytes = pakReader.extractFileIntoMemory(pakPath, extractPath, outputDir, false, &fileSize);
	
	if (fileBytes == 0) {
		return;
	}
	
	std::istringstream fileByteStream;
	fileByteStream.rdbuf()->pubsetbuf(fileBytes, fileSize);
	switch (processingType) {
	
	case PROCESSING_TYPE_STATS: {
		stats = lsbReader.loadFile(fileByteStream);
		
		buildNameMappings();
		break;}
	case PROCESSING_TYPE_ROOTTEMPLATE: {
		std::vector<LsbObject *> rootTemplates = lsbReader.loadFile(fileByteStream);
		addTemplatesToStatTemplateMap(rootTemplates, true);
		this->addRootTemplates(rootTemplates);
		break;}
	case PROCESSING_TYPE_MODTEMPLATE: {
		std::vector<LsbObject *> modTemplates = lsbReader.loadFile(fileByteStream);
		addTemplatesToStatTemplateMap(modTemplates, false);
		this->addModTemplates(modTemplates);
		break;}
	case PROCESSING_TYPE_ITEMSTATS: {
		std::vector<StatsContainer *> itemStats = genStatsReader.loadFile(fileByteStream);
		this->addItemStats(itemStats);
		break;}
	case PROCESSING_TYPE_SKILLSTATS: {
		skillStats = genStatsReader.loadFile(fileByteStream);
		break;}
	case PROCESSING_TYPE_ITEMLINKS: {
		std::vector<StatsContainer *> itemLinks = genStatsReader.loadFile(fileByteStream);
		this->addItemLinks(itemLinks);
		break;}
		
	}
	delete[] fileBytes;
}

void GamePakData::load(std::wstring gameDataPath) {
	std::wstring pakMain(gameDataPath + L"Main.pak");
	std::wstring pakTextures(gameDataPath + L"Textures.pak");
	std::string iconLsx("Public/Main/GUI/icons.lsx");
	std::string portraitLsx("Public/Main/GUI/portraits.lsx");
	
	std::string iconDds("Public/Main/Assets/Textures/Icons/icons.dds");
	std::string portraitDds("Public/Main/Assets/Textures/Icons/portraits.dds");
	std::string inventoryCellDds("Public/Main/Assets/Textures/Icons/InventoryCell.dds");
	
	std::wstring tempDirectory = QDir::temp().absolutePath().toStdWString();
	
	std::wstring linkDirectory = tempDirectory;
	linkDirectory += L"\\links";
	_wmkdir(linkDirectory.c_str());
	
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
	std::wstring rootTemplateTemp = tempDirectory + L"\\RootTemplates";
	std::wstring modTemplateTemp = tempDirectory + L"\\ModTemplates";
	_wmkdir(rootTemplateTemp.c_str());
	_wmkdir(modTemplateTemp.c_str());
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
	
	populateRootTemplateMap(this->getRootTemplates());
	populateModTemplateMap(this->getModTemplates());

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
	
	addItemLinksToStatTemplateMap(itemLinks);
	
	addModsToRootModsList();
	addModLookupsToStatTemplateMap();
	
	pakReader.loadFile(pakMain);
	pakReader.extractFile(pakMain, iconLsx, tempDirectory, false);
	pakReader.extractFile(pakMain, portraitLsx, tempDirectory, false);
	pakReader.loadFile(pakTextures);
	pakReader.extractFile(pakTextures, iconDds, tempDirectory, false);
	pakReader.extractFile(pakTextures, portraitDds, tempDirectory, false);
	pakReader.extractFile(pakTextures, inventoryCellDds, tempDirectory, false);
	
	boost::replace_all(tempDirectory, "/", "\\");
	std::wstring iconsLsxPath = tempDirectory;
	iconsLsxPath += L"\\icons.lsx";
	std::wstring iconsDdsPath = tempDirectory;
	iconsDdsPath += L"\\icons.dds";
	std::wstring inventoryCellDdsPath = tempDirectory;
	inventoryCellDdsPath += L"\\inventoryCell.dds";
	std::wstring portraitsLsbPath = tempDirectory;
	portraitsLsbPath += L"\\portraits.dds";
	
	boost::filesystem::ifstream iconDdsFin(iconsDdsPath, std::ios::binary);
	int start = iconDdsFin.tellg();
	iconDdsFin.seekg(0, std::ios_base::end);
	int fileLengthDds = iconDdsFin.tellg() - start;
	iconDdsFin.seekg(0, std::ios_base::beg);
	char *fileBytesDds = new char[fileLengthDds];
	iconDdsFin.read(fileBytesDds, fileLengthDds);
	iconDdsFin.close();
	
	boost::filesystem::ifstream iconLsxFin(iconsLsxPath, std::ios::binary);
	start = iconLsxFin.tellg();
	iconLsxFin.seekg(0, std::ios_base::end);
	int fileLengthLsx = iconLsxFin.tellg() - start;
	iconLsxFin.seekg(0, std::ios_base::beg);
	char *fileBytesLsx = new char[fileLengthLsx];
	iconLsxFin.read(fileBytesLsx, fileLengthLsx);
	iconLsxFin.close();
	std::string lsxBytes(fileBytesLsx);
	
	boost::filesystem::ifstream inventoryCellDdsFin(inventoryCellDdsPath, std::ios::binary);
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
