#ifndef GENSTATSREADER_H
#define GENSTATSREADER_H

#include <istream>
#include <vector>
#include <map>
#include <algorithm>

class StatsContainer {
    std::string containerType;
    std::vector<std::string> containerArgs;
    StatsContainer *usingContainer = 0;
    std::string type;
    std::map<std::string, std::string> dataMap;
	std::vector<std::string> prefixList;
	std::vector<std::string> suffixList;
	std::string boostName;
	std::string boostValue;
public:
	StatsContainer() {
		;
	}
	~StatsContainer() {
		;
	}
	std::string getBoostName() const {
		return boostName;
	}
	std::string getBoostValue() const {
		return boostValue;
	}
	void setBoostName(std::string boostName) {
		this->boostName = boostName;
	}
	void setBoostValue(std::string boostValue) {
		this->boostValue = boostValue;
	}
	std::vector<std::string>& getPrefixList() {
		return prefixList;
	}
	std::vector<std::string>& getSuffixList() {
		return suffixList;
	}
    std::string& getContainerType() {
        return this->containerType;
    }
    void setContainerType(const char *type) {
        this->containerType = type;
    }
	void setUsing(StatsContainer *usingContainer) {
		this->usingContainer = usingContainer;
	}
	StatsContainer *getUsing() const {
		return this->usingContainer;
	}
	std::string getUsingName() {
		if (this->usingContainer != 0) {
			return this->usingContainer->getArg(0);
		}
		return "";
	}
	long getArgCount() {
		return containerArgs.size();
	}
    std::string getArg(long n) const {
        if (n < containerArgs.size()) {
            return containerArgs[n];
        }
        else {
            return "";
        }
    }
	void addArg(const char *arg) {
		containerArgs.push_back(arg);
	}
    std::string& getType() {
        return this->type;
    }
    void setType(const char *type) {
        this->type = type;
    }
    void addData(std::string key, std::string value) {
        dataMap[key] = value;
    }
    std::string getData(std::string key) {
        if (dataMap.find(key) != dataMap.end()) {
            return dataMap[key];
        } else {
            if (usingContainer != 0) {
                return usingContainer->getData(key);
            }
        }
        return "";
    }
	std::map<std::string, std::string> getBaseDataMap() {
		return dataMap;
	}
};

class GenStatsReader
{
public:
    GenStatsReader();
    std::vector<StatsContainer *> loadFile(std::istream& stream);
    static StatsContainer *getContainer(std::vector<StatsContainer *>& containers, std::string containerName);
	static std::vector<StatsContainer *> getContainersByContainerType(std::vector<StatsContainer *> &containers, std::string type);
	static std::vector<StatsContainer *> getSubclassContainers(std::vector<StatsContainer *> &containers, StatsContainer *superClass);
	static void freeContainers(std::vector<StatsContainer *> &containers) {
		for (int i=0; i<containers.size(); ++i) {
			delete containers[i];
		}
	}
};

#endif // GENSTATSREADER_H
