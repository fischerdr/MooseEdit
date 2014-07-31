#include "textureatlas.h"
#include <tinyxml/tinyxml.h>

struct IconUVData {
	float U1;
	float U2;
	float V1;
	float V2;
};

typedef std::map<std::string, IconUVData> iconDataMap;
bool TextureAtlas::loadTextureAtlas(char *atlasBytes, int atlasSize, std::string& atlasDescriptor) {
	iconDataMap iconData;
	TiXmlDocument doc;
	//doc.LoadFile(iconsLsxPath.c_str());
	doc.Parse(atlasDescriptor.c_str());
	TiXmlElement *element = doc.FirstChildElement("save");
	element = element->FirstChildElement("region");
	std::string iconAttrib = "IconUVList";
	std::string mapKeyAttrib = "MapKey";
	std::string U1Attrib = "U1";
	std::string U2Attrib = "U2";
	std::string V1Attrib = "V1";
	std::string V2Attrib = "V2";
	
	while (element != 0 && element->Attribute("id") != iconAttrib) {
		element = element->NextSiblingElement();
	}
	if (element != 0) {
		element = element->FirstChildElement("node");
		element = element->FirstChildElement("children");
		element = element->FirstChildElement("node");
		while (element != 0) {
			TiXmlElement *subelement = element->FirstChildElement("attribute");
			std::string key;
			IconUVData data;
			while (subelement != 0) {
				if (subelement->Attribute("id") == mapKeyAttrib) {
					key = subelement->Attribute("value");
				} else if (subelement->Attribute("id") == U1Attrib) {
					data.U1 = boost::lexical_cast<float>(subelement->Attribute("value"));
				} else if (subelement->Attribute("id") == U2Attrib) {
					data.U2 = boost::lexical_cast<float>(subelement->Attribute("value"));
				} else if (subelement->Attribute("id") == V1Attrib) {
					data.V1 = boost::lexical_cast<float>(subelement->Attribute("value"));
				} else if (subelement->Attribute("id") == V2Attrib) {
					data.V2 = boost::lexical_cast<float>(subelement->Attribute("value"));
				}
				subelement = subelement->NextSiblingElement();
			}
			iconData[key] = data;
			element = element->NextSiblingElement();
		}
	}
	
	QImage qAtlas;
	if (!qAtlas.loadFromData((const uchar *)atlasBytes, atlasSize, "DDS"))
		return false;
	int w = qAtlas.width();
	int h = qAtlas.height();
	for (iconDataMap::iterator it = iconData.begin(); it != iconData.end(); ++it) {
		int imgX = w*iconData[it->first].U1;
		int imgY = h*iconData[it->first].V1;
		int imgW = w*iconData[it->first].U2 - imgX;
		int imgH = h*iconData[it->first].V2 - imgY;
		atlasMap[it->first] = qAtlas.copy(imgX, imgY, imgW, imgH);
	}
	return true;
}

std::vector<std::string> TextureAtlas::getTextureNames() {
	std::vector<std::string> textureNames;
	for (std::map<std::string, QImage>::iterator it = atlasMap.begin(); it != atlasMap.end(); ++it) {
		textureNames.push_back(it->first);
	}
	return textureNames;
}

QImage *TextureAtlas::getNamedTexture(const char *textureName) {
	std::string texture = textureName;
	if (atlasMap.find(texture) != atlasMap.end()) {
		if (absoluteSize != 0) {
			atlasMap[texture] = atlasMap[texture].scaled(absoluteSize, absoluteSize);
		}
		else {
			if (scale != 1.0f) {
				int w = atlasMap[texture].width();
				int h = atlasMap[texture].height();
				atlasMap[texture] = atlasMap[texture].scaled(w * scale, h * scale);
			}
		}
		return &atlasMap[texture];
	}
	return 0;
}

TextureAtlas::TextureAtlas()
{
}
