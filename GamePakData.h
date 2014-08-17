#ifndef GAMEPAKDATA_H
#define GAMEPAKDATA_H

#include <string>
#include "TextureAtlas.h"
#include <vector>
#include "LsbObject.h"
#include "GenStatsReader.h"
#include <QImage>
#include <map>
#include <queue>

#include "LsbReader.h"
#include "PakReader.h"

struct GameDataQueueObject {
	std::wstring pakFile;
	std::string toExtract;
	std::wstring extractDir;
	int processingType;
};

struct ItemTemplateData {
	std::string templateId;
	LsbObject *gameObject;
	bool isRootTemplate;
};

class ExtractQueueCallback {
public:
	virtual void onExtractBegin(std::queue<GameDataQueueObject>& extractQueue) = 0;
	virtual void onExtractUpdate(std::queue<GameDataQueueObject>& extractQueue) = 0;
	virtual void onExtractEnd() = 0;
};

typedef std::map<std::string, std::vector<ItemTemplateData> > StatTemplateMap;

class GamePakData
{
	ExtractQueueCallback *extractQueueCallback = 0;
	TextureAtlas iconAtlas;
	std::vector<LsbObject *> stats;
	std::map<std::string, LsbObject *> rootTemplateMap;
	std::map<std::string, LsbObject *> modTemplateMap;
	std::map<std::string, std::vector<std::string > > rootTemplateMods;
	std::vector<LsbObject *> rootTemplates;
	std::vector<LsbObject *> modTemplates;
	std::vector<StatsContainer *> itemStats;
	std::vector<StatsContainer *> itemLinks;
	std::vector<StatsContainer *> skillStats;
	std::map<std::string, std::string> nameMappings;
	StatTemplateMap statToTemplateMap;
	QImage inventoryCellImg;
	QImage *inventoryCellPtr = 0;
	
	LsbReader lsbReader;
	PakReader pakReader;
	GenStatsReader genStatsReader;
	std::wstring lastPakPath = L"";

	void processExtractQueue(std::queue<GameDataQueueObject>& extractQueue);
	void addModLookupsToStatTemplateMap();
	void addModsToRootModsList();
	void addItemLinksToStatTemplateMap(std::vector<StatsContainer *> &itemLinks);
	void addTemplatesToStatTemplateMap(std::vector<LsbObject *> &templatesObjects, bool isRootTemplate);
	void addTemplateToStatTemplateMap(LsbObject *gameObject, bool isRootTemplate);
	void populateRootTemplateMap(std::vector<LsbObject *>& rootTemplates);
	void populateModTemplateMap(std::vector<LsbObject *>& modTemplates);
	void parsePakFile(std::wstring &pakPath, const char *pakExtractPath, std::wstring &outputDir, int processingType);
	void buildNameMappings();
	void addRootTemplates(std::vector<LsbObject *>& rootTemplates) {
		for (int i=0; i<rootTemplates.size(); ++i) {
			this->rootTemplates.push_back(rootTemplates[i]);
		}
	}
	void addModTemplates(std::vector<LsbObject *>& modTemplates) {
		for (int i=0; i<modTemplates.size(); ++i) {
			this->modTemplates.push_back(modTemplates[i]);
		}
	}
	void addItemStats(std::vector<StatsContainer *>& itemStats) {
		for (int i=0; i<itemStats.size(); ++i) {
			this->itemStats.push_back(itemStats[i]);
		}
	}
	void addItemLinks(std::vector<StatsContainer *>& itemLinks) {
		for (int i=0; i<itemLinks.size(); ++i) {
			this->itemLinks.push_back(itemLinks[i]);
		}
	}
public:
	GamePakData();
	~GamePakData();
	void load(std::wstring gameDataPath);
	void registerExtractQueueCallback(ExtractQueueCallback *extractQueueCallback) {
		this->extractQueueCallback = extractQueueCallback;
	}
	QImage *getInventoryCellImg() {
		return inventoryCellPtr;
	}
	std::vector<LsbObject *> &getStats();
	void setStats(const std::vector<LsbObject *> &value);
	std::vector<LsbObject *> &getRootTemplates();
	void setRootTemplates(const std::vector<LsbObject *> &value);
	std::vector<LsbObject *> &getModTemplates();
	void setModTemplates(const std::vector<LsbObject *> &value);
	std::vector<StatsContainer *> &getItemStats();
	void setItemStats(const std::vector<StatsContainer *> &value);
	std::vector<StatsContainer *> &getItemLinks() ;
	void setItemLinks(const std::vector<StatsContainer *> &value);
	std::vector<StatsContainer *> &getSkillStats();
	void setSkillStats(const std::vector<StatsContainer *> &value);
	std::map<std::string, std::string> &getNameMappings();
	void setNameMappings(const std::map<std::string, std::string> &value);
	TextureAtlas &getIconAtlas();
	void setIconAtlas(const TextureAtlas &value);
	std::map<std::string, LsbObject *> &getRootTemplateMap();
	void setRootTemplateMap(const std::map<std::string, LsbObject *> &value);
	std::map<std::string, LsbObject *> &getModTemplateMap();
	void setModTemplateMap(const std::map<std::string, LsbObject *> &value);
	StatTemplateMap &getStatToTemplateMap();
	void setStatToTemplateMap(const StatTemplateMap &value);
};

#endif // GAMEPAKDATA_H
