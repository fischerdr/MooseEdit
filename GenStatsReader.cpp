#include "GenStatsReader.h"
#include <boost/tokenizer.hpp>

typedef boost::tokenizer<boost::escaped_list_separator<char> > LineTokenizer;

GenStatsReader::GenStatsReader()
{
}

StatsContainer *GenStatsReader::getContainer(std::vector<StatsContainer *> &containers, std::string containerName)
{
	for (int i=0; i<containers.size(); ++i) {
		StatsContainer& container = *containers[i];
		if (container.getArg(0) == containerName) {
			return &container;
		}
	}
	return 0;
}

std::vector<StatsContainer *> GenStatsReader::getContainersByContainerType(std::vector<StatsContainer *> &containers, std::string type)
{
	std::vector<StatsContainer *> result;
	for (int i=0; i<containers.size(); ++i) {
		StatsContainer& container = *containers[i];
		if (container.getContainerType() == type) {
			result.push_back(&container);
		}
	}
	return result;
}

std::vector<StatsContainer *> GenStatsReader::getSubclassContainers(std::vector<StatsContainer *> &containers, StatsContainer *superClass)
{
	std::vector<StatsContainer *> result;
	for (int i=0; i<containers.size(); ++i) {
		StatsContainer& container = *containers[i];
		if (container.getUsingName() == superClass->getArg(0)) {
			result.push_back(&container);
		}
	}
	return result;
}

std::vector<StatsContainer *> GenStatsReader::loadFile(std::istream& stream)
{
	std::vector<StatsContainer *> containers;
	if (!stream) {
		return containers;
	}
	std::string buffer;
	boost::escaped_list_separator<char> separator("",", ","\"");
	StatsContainer *current = 0;
	for (;;) {
		buffer = "";
		std::getline(stream, buffer, '\n');
		if (!stream) {
			break;
		}
		LineTokenizer tok(buffer, separator);
		int i = 0;
		std::string keyword = "";
		std::string last = "";
		bool isBoost = false;
		for(LineTokenizer::iterator it = tok.begin(); it != tok.end(); ++it) {
			const std::string& token = *it;
			if (keyword == "new" || keyword == "object" || (keyword == "modifier" && (token == "type" || last == "type"))) {
				if (token == "boost" && i == 1) {
					isBoost = true;
					keyword = "boost";
				} else {
					if (i == 1) {
						containers.push_back(new StatsContainer);
						current = containers[containers.size() - 1];
						current->setContainerType(token.c_str());
					} else {
						current->addArg(token.c_str());
					}
				}
			} else if (keyword == "type") {
				if (i == 1) {
					if (current != 0) {
						current->setType(token.c_str());
					}
				}
			} else if (keyword == "data" || keyword == "param" || keyword == "modifier") {
				if (i == 2) {
					if (current != 0) {
						current->addData(last, token);
					}
				}
			} else if (keyword == "using") {
				if (i == 1) {
					if (current != 0) {
						current->setUsing(getContainer(containers, token));
					}
				}
			} else if (keyword == "prefixname") {
				if (i == 1) {
					current->getPrefixList().push_back(token);
				}
			} else if (keyword == "suffixname") {
				if (i == 1) {
					current->getSuffixList().push_back(token);
				}
			} else if (isBoost && keyword == "boost") {
				if (i == 3) {
					current->setBoostName(last);
					current->setBoostValue(token);
				}
			}
			if (i == 0) {
				keyword = token;
			}
			last = token;
			++i;
		}
	}
	return containers;
}
