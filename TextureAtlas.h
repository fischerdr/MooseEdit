#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H
#include <QImage>
#include <map>
#include <string>
#include <boost/lexical_cast.hpp>

class TextureAtlas
{
	std::map<std::string, QImage> atlasMap;
	float scale = 1.0f;
	long absoluteSize = 0;
public:
	TextureAtlas();
	bool loadTextureAtlas(char *atlasBytes, int atlasSize, std::string& atlasDescriptor);
	bool getNamedTexture(const char *textureName, QImage *img);
	std::vector<std::string> getTextureNames();
	
	void setAbsoluteSize(long absoluteSize) {
		this->absoluteSize = absoluteSize;
	}
	
	long getAbsoluteSize() {
		return this->absoluteSize;
	}
	
	float getScale() {
		return scale;
	}
	
	void setScale(float scale) {
		this->scale = scale;
	}
};

#endif // TEXTUREATLAS_H
