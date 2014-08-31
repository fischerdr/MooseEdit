#include "AppearanceEditorFrame.h"
#include "ui_AppearanceEditorFrame.h"

#define GLEW_STATIC
#include <gl/glew.h>
#include "zgranny.h"
#include "LsxReader.h"
#include "GlShader.h"
#include "GlShaderProgram.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <QTextStream>
#include <QFile>
#include <QColorDialog>

#include "nv_dds.h"

#define NUM_HEADS		15
#define NUM_HAIRS		15
#define NUM_UNDERWEARS	3

bool AppearanceEditorFrame::didInitGlew = false;

AppearanceEditorFrame::AppearanceEditorFrame(std::wstring gameDataPath, GameCharacter *character, QWidget *parent) :
	QFrame(parent), gameDataPath(gameDataPath), character(character),
	ui(new Ui::AppearanceEditorFrame)
{
	mainPak.loadFile(gameDataPath + L"Main.pak");
	texturesPak.loadFile(gameDataPath + L"Textures.pak");
	ui->setupUi(this);
}

void AppearanceEditorFrame::generateEquipmentModels() {
	for (int i=0; i<EQUIP_SLOTS; ++i) {
		GameItem *item = equipHandler->getItemAtSlot(i);
		if (item != 0) {
			std::vector<GLuint> textures;
			ZGrannyScene *scene = createModelForItem(item, textures);
			GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
			if (scene != 0) {
				MeshAttachmentPoint *attachment = new MeshAttachmentPoint;
				attachment->meshName = "PL_M_Body_A";
				switch (i) {
				case SLOT_HELMET:
					attachment->boneName = "Bip001 Head";
					break;
				case SLOT_BREAST:
					attachment->boneName = "Bip001 Spine1";
					break;
				case SLOT_GARMENT:
					attachment->boneName = "Bip001 Pelvis";
					break;
				case SLOT_WEAPON:
					attachment->boneName = "Bip001 R Hand";
					break;
				case SLOT_SHIELD:
					attachment->boneName = "Bip001 L Hand";
					break;
				case SLOT_RING_LEFT:
					attachment->boneName = "Bip001 L Finger0";
					break;
				case SLOT_BELT:
					attachment->boneName = "Bip001 Pelvis";
					break;
				case SLOT_BOOTS:
					attachment->boneName = "Bip001 L Foot";
					break;
				case SLOT_BRACERS:
					attachment->boneName = "Bip001 L Forearm";
					break;
				case SLOT_AMULET:
					attachment->boneName = "Bip001 Neck";
					break;
				case SLOT_RING_RIGHT:
					attachment->boneName = "Bip001 R Finger0";
					break;
				}
				equippedItems.push_back({scene, textures, attachment});
				glContext->addGrannyScene(scene, textures, 0, 0, shaderProgram, attachment);
			}
		}
	}
}

void AppearanceEditorFrame::loadEquipmentData() {
	{
		std::string extractPath = "Public/Main/Content/Assets/Items/Equipment/[PAK]_Weapons/Weapons.lsb";
		unsigned long fileSize;
		char *fileBytes = mainPak.extractFileIntoMemory(gameDataPath + L"Main.pak", extractPath, gameDataPath, false, &fileSize);
		if (fileBytes != 0) {
			std::stringstream ss;
			ss.rdbuf()->pubsetbuf(fileBytes, fileSize);
			LsbReader reader;
			std::vector<LsbObject *> resourceAssets = reader.loadFile(ss);
			weaponsResourceBankObject = LsbObject::lookupByUniquePath(resourceAssets, "ResourceBank");
			delete[] fileBytes;
		}
	}
	{
		std::string extractPath = "Public/Main/Content/Assets/Items/Armors/[PAK]_Armors_Player/Armors_Player.lsb";
		unsigned long fileSize;
		char *fileBytes = mainPak.extractFileIntoMemory(gameDataPath + L"Main.pak", extractPath, gameDataPath, false, &fileSize);
		if (fileBytes != 0) {
			std::stringstream ss;
			ss.rdbuf()->pubsetbuf(fileBytes, fileSize);
			LsbReader reader;
			std::vector<LsbObject *> resourceAssets = reader.loadFile(ss);
			armorsPlayerResourceBankObject = LsbObject::lookupByUniquePath(resourceAssets, "ResourceBank");
			delete[] fileBytes;
		}
	}
	{
		std::string extractPath = "Public/Main/Content/Assets/Characters/[PAK]_Player_Male/Player_Male.lsb";
		unsigned long fileSize;
		char *fileBytes = mainPak.extractFileIntoMemory(gameDataPath + L"Main.pak", extractPath, gameDataPath, false, &fileSize);
		if (fileBytes != 0) {
			std::stringstream ss;
			ss.rdbuf()->pubsetbuf(fileBytes, fileSize);
			LsbReader reader;
			std::vector<LsbObject *> resourceAssets = reader.loadFile(ss);
			playerMaleResourceBankObject = LsbObject::lookupByUniquePath(resourceAssets, "ResourceBank");
			delete[] fileBytes;
		}
	}
	{
		std::string extractPath = "Public/Main/Content/Assets/Characters/[PAK]_Player_Female/Player_Female.lsb";
		unsigned long fileSize;
		char *fileBytes = mainPak.extractFileIntoMemory(gameDataPath + L"Main.pak", extractPath, gameDataPath, false, &fileSize);
		if (fileBytes != 0) {
			std::stringstream ss;
			ss.rdbuf()->pubsetbuf(fileBytes, fileSize);
			LsbReader reader;
			std::vector<LsbObject *> resourceAssets = reader.loadFile(ss);
			playerFemaleResourceBankObject = LsbObject::lookupByUniquePath(resourceAssets, "ResourceBank");
			delete[] fileBytes;
		}
	}
	
	generateEquipmentModels();
}

void AppearanceEditorFrame::updateAllFields() {
	changeFieldValue("aiPersonalityLabel", aiPersonalityIdx, aiPersonalities);
	changeFieldValue("voiceLabel", voiceIdx, voices);
	changeFieldValue("skinColorLabel", skinColorIdx, skinColors);
	changeFieldValue("headLabel", headIdx, heads);
	changeFieldValue("hairLabel", hairIdx, hairs);
	changeFieldValue("hairColorLabel", hairColorIdx, hairColors);
	changeFieldValue("underwearLabel", underwearIdx, underwears);
	
	updateToCurrentSkinColor();
	updateToCurrentHairColor();
	updateToCurrentHair();
	updateToCurrentHead();
	updateToCurrentUnderwear();
}

void AppearanceEditorFrame::generateFields() {
	unsigned long fSize;
	std::string extractPath = "Mods/Main/CharacterCreation/properties.lsx";
	char *fileBytes = mainPak.extractFileIntoMemory(gameDataPath + L"Main.pak", extractPath, gameDataPath, false, &fSize);
	std::stringstream fileReader;
	fileReader.rdbuf()->pubsetbuf(fileBytes, fSize);
	if (fileBytes != 0) {
		LsxReader reader;
		std::vector<LsbObject *> objects = reader.loadFile(fileReader);
		if (objects.size() == 0) {
			MessageBoxA(0, "Failed to parse CharacterCreation properties!", 0, 0);
		}
		{
			LsbObject *racesObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/Races");
			std::vector<LsbObject *> raceDescObjects = LsbObject::lookupAllEntitiesWithName(racesObject, "RaceDesc");
			for (int i=0; i<raceDescObjects.size(); ++i) {
				LsbObject *raceDescObject = raceDescObjects[i];
				LsbObject *maleIconsObject = raceDescObject->lookupByUniquePath("MaleIcons");
				LsbObject *femaleIconsObject = raceDescObject->lookupByUniquePath("FemaleIcons");
				if (maleIconsObject != 0 && femaleIconsObject != 0) {
					std::vector<LsbObject *> maleIconsObjects = maleIconsObject->lookupAllEntitiesWithName(maleIconsObject, "MaleIcons");
					std::vector<LsbObject *> femaleIconsObjects = femaleIconsObject->lookupAllEntitiesWithName(femaleIconsObject, "FemaleIcons");
					for (int i=0; i<std::max(maleIconsObjects.size(), femaleIconsObjects.size()); ++i) {
						LsbObject *maleIconObject = 0;
						LsbObject *femaleIconObject = 0;
						if (i < maleIconsObjects.size()) {
							maleIconObject = maleIconsObjects[i];
						}
						if (i < femaleIconsObjects.size()) {
							femaleIconObject = femaleIconsObjects[i];
						}
						std::string maleIcon = "";
						std::string femaleIcon = "";
						if (maleIconObject != 0) {
							LsbObject *objectObject = maleIconObject->lookupByUniquePath("Object");
							maleIcon = objectObject->getData();
						}
						if (femaleIconObject != 0) {
							LsbObject *objectObject = femaleIconObject->lookupByUniquePath("Object");
							femaleIcon = objectObject->getData();
						}
						std::ostringstream ss;
						ss<<"Portrait "<<(i + 1);
						portraits.push_back({ss.str(), maleIcon, femaleIcon});
					}
				}
			}
		}
		{
			LsbObject *aiPersonalitiesObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/AiPersonalities");
			std::vector<LsbObject *> aiPersonalityObjects = LsbObject::lookupAllEntitiesWithName(aiPersonalitiesObject, "AiPersonality");
			for (int i=0; i<aiPersonalityObjects.size(); ++i) {
				LsbObject *aiPersonalityObject = aiPersonalityObjects[i];
				LsbObject *nameObject = aiPersonalityObject->lookupByUniquePath("Name");
				LsbObject *idObject = aiPersonalityObject->lookupByUniquePath("ID");
				aiPersonalities.push_back({nameObject->getData(), idObject->getData(), idObject->getData()});
			}
		}
		{
			LsbObject *voicesObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/Voices");
			std::vector<LsbObject *> voiceObjects = LsbObject::lookupAllEntitiesWithName(voicesObject, "Voice");
			for (int i=0; i<voiceObjects.size(); ++i) {
				LsbObject *voiceObject = voiceObjects[i];
				LsbObject *nameObject = voiceObject->lookupByUniquePath("Name");
				LsbObject *maleSpeakerIdObject = voiceObject->lookupByUniquePath("MaleSpeakerID");
				LsbObject *femaleSpeakerIdObject = voiceObject->lookupByUniquePath("FemaleSpeakerID");
				voices.push_back({nameObject->getData(), maleSpeakerIdObject->getData(), femaleSpeakerIdObject->getData()});
			}
		}
		{
			LsbObject *skinColorsObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/SkinColors");
			std::vector<LsbObject *> colorDescObjects = LsbObject::lookupAllEntitiesWithName(skinColorsObject, "ColorDesc");
			for (int i=0; i<colorDescObjects.size(); ++i) {
				LsbObject *colorDescObject = colorDescObjects[i];
				LsbObject *colorNameObject = colorDescObject->lookupByUniquePath("ColorName");
				LsbObject *valueObject = colorDescObject->lookupByUniquePath("Value");
				std::ostringstream ss;
				ss<<*((unsigned long*)valueObject->getData());
				skinColors.push_back({colorNameObject->getData(), ss.str(), ss.str()});
			}
		}
		for (int i=0; i<NUM_HEADS; ++i) {
			std::ostringstream ss;
			ss<<"Head "<<(i + 1);
			heads.push_back({ss.str(), "", ""});
			headTextures.push_back({ss.str(), "", ""});
		}
		heads[0].maleValue = "PL_M_Head_A";
		headTextures[0].maleValue = "PL_M_Head_A";
		heads[1].maleValue = "PL_M_Head_B";
		headTextures[1].maleValue = "PL_M_Head_B";
		heads[2].maleValue = "PL_M_Head_C";
		headTextures[2].maleValue = "PL_M_Head_C";
		heads[3].maleValue = "PL_M_Head_D";
		headTextures[3].maleValue = "PL_M_Head_D";
		heads[4].maleValue = "PL_M_Head_E";
		headTextures[4].maleValue = "PL_M_Head_E";
		heads[5].maleValue = "PL_M_Head_F";
		headTextures[5].maleValue = "PL_M_Head_F";
		heads[6].maleValue = "PL_M_Head_A";
		headTextures[6].maleValue = "PL_M_Head_G";
		heads[7].maleValue = "PL_M_Head_A";
		headTextures[7].maleValue = "PL_M_Head_H";
		heads[8].maleValue = "PL_M_Head_I";
		headTextures[8].maleValue = "PL_M_Head_I";
		heads[9].maleValue = "PL_M_Head_A";
		headTextures[9].maleValue = "PL_M_Head_L";
		heads[10].maleValue = "PL_M_Head_A";
		headTextures[10].maleValue = "PL_M_Head_M";
		heads[11].maleValue = "PL_M_Head_N";
		headTextures[11].maleValue = "PL_M_Head_N";
		heads[12].maleValue = "PL_M_Head_O";
		headTextures[12].maleValue = "PL_M_Head_O";
		heads[13].maleValue = "PL_M_Head_I";
		headTextures[13].maleValue = "PL_M_Head_J";
		heads[14].maleValue = "PL_M_Head_I";
		headTextures[14].maleValue = "PL_M_Head_K";
		
		for (int i=0; i<NUM_HEADS; ++i) {
			heads[i].femaleValue = "PL_F_Head_A";
			headTextures[i].femaleValue = "PL_F_Head_A";
		}
		
		for (int i=0; i<NUM_HAIRS; ++i) {
			std::ostringstream ss;
			ss<<"Hair "<<(i + 1);
			hairs.push_back({ss.str(), "", ""});
		}
		hairs[0].maleValue = "PL_M_Hair_A";
		hairs[1].maleValue = "PL_M_Hair_H";
		hairs[2].maleValue = "PL_M_Hair_J";
		hairs[3].maleValue = "PL_M_Hair_O";
		hairs[4].maleValue = "PL_M_Hair_F";
		hairs[5].maleValue = "PL_M_Hair_G";
		hairs[6].maleValue = "PL_M_Hair_C";
		hairs[7].maleValue = "PL_M_Hair_B";
		hairs[8].maleValue = "PL_M_Hair_L";
		hairs[9].maleValue = "PL_M_Hair_M";
		hairs[10].maleValue = "PL_M_Hair_N";
		hairs[11].maleValue = "PL_M_Hair_D";
		hairs[12].maleValue = "PL_M_Hair_K";
		hairs[13].maleValue = "PL_M_Hair_I";
		hairs[14].maleValue = "PL_M_Hair_E";
		
		hairs[0].femaleValue = "PL_F_Hair_A";
		hairs[1].femaleValue = "PL_F_Hair_B";
		hairs[2].femaleValue = "PL_F_Hair_C";
		hairs[3].femaleValue = "PL_F_Hair_D";
		hairs[4].femaleValue = "PL_F_Hair_E";
		hairs[5].femaleValue = "PL_F_Hair_F";
		hairs[6].femaleValue = "PL_F_Hair_G";
		hairs[7].femaleValue = "PL_F_Hair_H";
		hairs[8].femaleValue = "PL_F_Hair_I";
		hairs[9].femaleValue = "PL_F_Hair_J";
		hairs[10].femaleValue = "PL_F_Hair_K";
		hairs[11].femaleValue = "PL_F_Hair_L";
		hairs[12].femaleValue = "PL_F_Hair_M";
		hairs[13].femaleValue = "PL_F_Hair_N";
		hairs[14].femaleValue = "PL_F_Hair_O";
		
		for (int i=0; i<NUM_UNDERWEARS; ++i) {
			std::ostringstream ss;
			ss<<"Underwear "<<(i + 1);
			underwears.push_back({ss.str(), "", ""});
			underwearTextures.push_back({ss.str(), "", ""});
		}
		underwears[0].maleValue = "PL_M_Body_A";
		underwearTextures[0].maleValue = "PL_M_Body_A";
		underwears[1].maleValue = "PL_M_Body_A";
		underwearTextures[1].maleValue = "PL_M_Body_B";
		underwears[2].maleValue = "PL_M_Body_A";
		underwearTextures[2].maleValue = "PL_M_Body_C";
		
		underwears[0].femaleValue = "PL_F_Body_A";
		underwearTextures[0].femaleValue = "PL_F_Body_A";
		underwears[1].femaleValue = "PL_F_Body_A";
		underwearTextures[1].femaleValue = "PL_F_Body_B";
		underwears[2].femaleValue = "PL_F_Body_A";
		underwearTextures[2].femaleValue = "PL_F_Body_C";
		
		{
			LsbObject *hairColorsObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/HairColors");
			std::vector<LsbObject *> colorDescObjects = LsbObject::lookupAllEntitiesWithName(hairColorsObject, "ColorDesc");
			for (int i=0; i<colorDescObjects.size(); ++i) {
				LsbObject *colorDescObject = colorDescObjects[i];
				LsbObject *colorNameObject = colorDescObject->lookupByUniquePath("ColorName");
				LsbObject *valueObject = colorDescObject->lookupByUniquePath("Value");
				std::ostringstream ss;
				ss<<*((unsigned long*)valueObject->getData());
				hairColors.push_back({colorNameObject->getData(), ss.str(), ss.str()});
			}
		}
	} else {
		MessageBoxA(0, "Couldn't read CharacterCreation properties file!", 0, 0);
	}
	if (fileBytes != 0) {
		delete[] fileBytes;
	}
}

std::string AppearanceEditorFrame::getGR2(LsbObject *resourceBankObject, std::string &visualTemplate) {
	if (visualTemplate.size() == 0) {
		return "";
	}
	LsbObject *visualBankObject = resourceBankObject->lookupByUniquePath("root/VisualBank");
	std::vector<LsbObject *> matchingResourceObjects = LsbObject::findItemsByAttribute(visualBankObject->getChildren(), "MapKey", visualTemplate.c_str(), visualTemplate.length() + 1);
	if (matchingResourceObjects.size() == 1) {
		LsbObject *resourceObject = matchingResourceObjects[0];
		LsbObject *sourceFileObject = resourceObject->lookupByUniquePath("SourceFile");
		if (sourceFileObject != 0) {
			return sourceFileObject->toString();
		}
	}
	return "";
}

std::string AppearanceEditorFrame::getTextureFromTextureTemplate(LsbObject *resourceBankObject, std::string &textureTemplate) {
	if (textureTemplate.size() == 0) {
		return "";
	}
	LsbObject *visualBankObject = resourceBankObject->lookupByUniquePath("root/TextureBank");
	std::vector<LsbObject *> matchingResourceObjects = LsbObject::findItemsByAttribute(visualBankObject->getChildren(), 
																					   "MapKey", textureTemplate.c_str(), textureTemplate.length() + 1);
	if (matchingResourceObjects.size() == 1) {
		LsbObject *resourceObject = matchingResourceObjects[0];
		LsbObject *sourceFileObject = resourceObject->lookupByUniquePath("SourceFile");
		if (sourceFileObject != 0) {
			return sourceFileObject->toString();
		}
	}
	return "";
}

bool AppearanceEditorFrame::getTextureMaps(LsbObject *resourceBankObject, LsbObject *materialsResourceBankObject, std::string &visualTemplate, std::string &diffuseMap, std::string &normalMap, std::string &maskMap) {
	if (visualTemplate.size() == 0) {
		return false;
	}
	LsbObject *visualBankObject = resourceBankObject->lookupByUniquePath("root/VisualBank");
	std::vector<LsbObject *> matchingResourceObjects = LsbObject::findItemsByAttribute(visualBankObject->getChildren(), 
																					   "MapKey", visualTemplate.c_str(), visualTemplate.length() + 1);
	if (matchingResourceObjects.size() == 1) {
		LsbObject *resourceObject = matchingResourceObjects[0];
		LsbObject *objectsObject = resourceObject->lookupByUniquePath("Objects");
		if (objectsObject != 0) {
			LsbObject *materialIdObject = objectsObject->lookupByUniquePath("MaterialID");
			if (materialIdObject != 0) {
				std::string materialId = materialIdObject->getData();
				LsbObject *materialBankObject = materialsResourceBankObject->lookupByUniquePath("root/MaterialBank");
				std::vector<LsbObject *> matchingResourceObjects = LsbObject::findItemsByAttribute(materialBankObject->getChildren(), 
																								   "MapKey", materialId.c_str(), materialId.length() + 1);
				if (matchingResourceObjects.size() == 1) {
					LsbObject *resourceObject = matchingResourceObjects[0];
					std::vector<LsbObject *> texture2DParametersObjects = LsbObject::lookupAllEntitiesWithName(resourceObject, "Texture2DParameters");
					std::string diffuseTextureTemplate = "";
					std::string normalTextureTemplate = "";
					std::string maskTextureTemplate = "";
					for (int i=0; i<texture2DParametersObjects.size(); ++i) {
						LsbObject *texture2DParameterObject = texture2DParametersObjects[i];
						LsbObject *uniformNameObject = texture2DParameterObject->lookupByUniquePath("UniformName");
						if (uniformNameObject != 0) {
							std::string uniformName = uniformNameObject->getData();
							if (uniformName == "Texture2DParameter_DM") {
								LsbObject *idObject = texture2DParameterObject->lookupByUniquePath("ID");
								if (idObject != 0) {
									diffuseTextureTemplate = idObject->getData();
								}
							} else if (uniformName == "Texture2DParameter_NM") {
								LsbObject *idObject = texture2DParameterObject->lookupByUniquePath("ID");
								if (idObject != 0) {
									normalTextureTemplate = idObject->getData();
								}
							} else if (uniformName == "Texture2DParameter_SM") {
								LsbObject *idObject = texture2DParameterObject->lookupByUniquePath("ID");
								if (idObject != 0) {
									maskTextureTemplate = idObject->getData();
								}
							}
						}
					}
					diffuseMap = getTextureFromTextureTemplate(materialsResourceBankObject, diffuseTextureTemplate);
					normalMap = getTextureFromTextureTemplate(materialsResourceBankObject, normalTextureTemplate);
					maskMap = getTextureFromTextureTemplate(materialsResourceBankObject, maskTextureTemplate);
					if (diffuseMap.size() > 0 && normalMap.size() > 0 && maskMap.size() > 0) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

ZGrannyScene *AppearanceEditorFrame::createModelForItem(GameItem *item, std::vector<GLuint > &textures) {
	LsbObject *itemObject = item->getObject();
	if (itemObject != 0) {
		LsbObject *itemTemplateObject = itemObject->lookupByUniquePath("CurrentTemplate");
		if (itemTemplateObject != 0) {
			std::string itemTemplate = itemTemplateObject->getData();
			std::map<std::string, LsbObject *> &modTemplateMap = gamePakData->getModTemplateMap();
			std::map<std::string, LsbObject *> &rootTemplateMap = gamePakData->getRootTemplateMap();
			LsbObject *gameObject = 0;
			std::string visualTemplate = "";
			if (modTemplateMap.find(itemTemplate) != modTemplateMap.end()) {
				gameObject = modTemplateMap[itemTemplate];
				LsbObject *visualTemplateObject = gameObject->lookupByUniquePath("VisualTemplate");
				if (visualTemplateObject != 0) {
					visualTemplate = visualTemplateObject->getData();
				}
				
				LsbObject *templateNameObject = gameObject->lookupByUniquePath("TemplateName");
				if (templateNameObject != 0) {
					itemTemplate = templateNameObject->getData();
				}
			}
			if (visualTemplate.size() == 0 && rootTemplateMap.find(itemTemplate) != rootTemplateMap.end()) {
				gameObject = rootTemplateMap[itemTemplate];
				LsbObject *visualTemplateObject = gameObject->lookupByUniquePath("VisualTemplate");
				if (visualTemplateObject != 0) {
					visualTemplate = visualTemplateObject->getData();
				}
			}
			if (visualTemplate.size() > 0) {
				LsbObject *resourceBank = 0;
				LsbObject *statsObject = itemObject->lookupByUniquePath("Stats");
				if (statsObject != 0) {
					std::vector<StatsContainer *> &itemStats = gamePakData->getItemStats();
					StatsContainer *itemStat = GenStatsReader::getContainer(itemStats, statsObject->getData());
					if (itemStat != 0) {
						std::string slot = itemStat->getData("Slot");
						if (slot == "Weapon") {
							resourceBank = weaponsResourceBankObject;
						} else {
							resourceBank = armorsPlayerResourceBankObject;
						}
					}
				}
				if (resourceBank != 0) {
					std::string modelFile = this->getGR2(resourceBank, visualTemplate);
					std::string diffuseMap;
					std::string normalMap;
					std::string maskMap;
					if (resourceBank == armorsPlayerResourceBankObject && this->isMale) {
						this->getTextureMaps(resourceBank, playerMaleResourceBankObject, visualTemplate, diffuseMap, normalMap, maskMap);
					} else if (resourceBank == armorsPlayerResourceBankObject && !this->isMale) {
						this->getTextureMaps(resourceBank, playerMaleResourceBankObject, visualTemplate, diffuseMap, normalMap, maskMap);
					} else {
						this->getTextureMaps(resourceBank, resourceBank, visualTemplate, diffuseMap, normalMap, maskMap);
					}
					unsigned long fileSize;
					std::wstring temp = L"";
					char *fileBytes = mainPak.extractFileIntoMemory(gameDataPath + L"Main.pak", modelFile, temp, false, &fileSize);
					if (fileBytes != 0) {
						GLuint diffuseTextureId = -1;
						GLuint normalTextureId = -1;
						GLuint maskTextureId = -1;
						std::wstring tmp;
						unsigned long textureFileSize;
						char *diffuseBytes = texturesPak.extractFileIntoMemory(gameDataPath + L"Textures.pak", diffuseMap, tmp, false, &textureFileSize);
						if (diffuseBytes != 0) {
							nv_dds::CDDSImage textureImage;
							std::stringstream ss;
							ss.rdbuf()->pubsetbuf(diffuseBytes, textureFileSize);
							textureImage.load(ss, false);
							glGenTextures(1, &diffuseTextureId);
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, diffuseTextureId);
							textureImage.upload_texture2D();
							textures.push_back(diffuseTextureId);
							delete[] diffuseBytes;
						}
						char *normalBytes = texturesPak.extractFileIntoMemory(gameDataPath + L"Textures.pak", normalMap, tmp, false, &textureFileSize);
						if (normalBytes != 0) {
							nv_dds::CDDSImage textureImage;
							std::stringstream ss;
							ss.rdbuf()->pubsetbuf(normalBytes, textureFileSize);
							textureImage.load(ss, false);
							glGenTextures(1, &normalTextureId);
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, normalTextureId);
							textureImage.upload_texture2D();
							textures.push_back(normalTextureId);
							delete[] normalBytes;
						}
						char *maskBytes = texturesPak.extractFileIntoMemory(gameDataPath + L"Textures.pak", maskMap, tmp, false, &textureFileSize);
						if (maskBytes != 0) {
							nv_dds::CDDSImage textureImage;
							std::stringstream ss;
							ss.rdbuf()->pubsetbuf(maskBytes, textureFileSize);
							textureImage.load(ss, false);
							glGenTextures(1, &maskTextureId);
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, maskTextureId);
							textureImage.upload_texture2D();
							textures.push_back(maskTextureId);
							delete[] maskBytes;
						}
						ZGrannyScene *scene = zGrannyCreateSceneFromMemory(fileBytes, fileSize, textures);
						delete[] fileBytes;
						return scene;
					}
				}
			}
		}
	}
	return 0;
}

void AppearanceEditorFrame::updateFieldText(QLabel *label, std::vector<fieldValue_t> &updateVector, int index) {
	fieldValue_t &fieldValue = updateVector[index];
	label->setText(fieldValue.name.c_str());
}

void AppearanceEditorFrame::initIndexesToCustomData() {
	LsbObject *isMaleObject = playerCustomDataObject->lookupByUniquePath("IsMale");
	if (isMaleObject != 0) {
		bool isMale = *((bool *)isMaleObject->getData());
		this->isMale = isMale;
	}
	LsbObject *aiPersonalityObject = playerCustomDataObject->lookupByUniquePath("AiPersonality");
	if (aiPersonalityObject != 0) {
		std::string personality = aiPersonalityObject->getData();
		for (int i=0; i<aiPersonalities.size(); ++i) {
			fieldValue_t &aiPersonality = aiPersonalities[i];
			if (aiPersonality.currentValue(isMale) == personality) {
				aiPersonalityIdx = i;
				break;
			}
		}
	}
	LsbObject *speakerObject = playerCustomDataObject->lookupByUniquePath("Speaker");
	if (speakerObject != 0) {
		std::string speaker = speakerObject->getData();
		for (int i=0; i<voices.size(); ++i) {
			fieldValue_t &voice = voices[i];
			if (voice.currentValue(isMale) == speaker) {
				voiceIdx = i;
				break;
			}
		}
	}
	LsbObject *skinColorObject = playerCustomDataObject->lookupByUniquePath("SkinColor");
	if (skinColorObject != 0) {
		unsigned long skinColorValue = *((unsigned long *)skinColorObject->getData());
		std::ostringstream ss;
		ss<<skinColorValue;
		std::string skinColorText = ss.str();
		for (int i=0; i<skinColors.size(); ++i) {
			fieldValue_t &skinColor = skinColors[i];
			if (skinColor.currentValue(isMale) == skinColorText) {
				skinColorIdx = i;
				break;
			}
		}
		int r = (skinColorValue & 0x00FF0000) >> 16;
		int g = (skinColorValue & 0x0000FF00) >> 8;
		int b = (skinColorValue & 0x000000FF) >> 0;
		int a = (skinColorValue & 0xFF000000) >> 24;
		skinColor->r = r;
		skinColor->g = g;
		skinColor->b = b;
		skinColor->a = a;
	}
	LsbObject *randomObject = playerCustomDataObject->lookupByUniquePath("Random");
	if (randomObject != 0) {
		unsigned long random = *((unsigned long*)randomObject->getData());
		hairIdx = (random & 0xF0) >> 4;
		underwearIdx = (random & 0xF00) >> 8;
		headIdx = (random & 0x0F);
	}
	LsbObject *hairColorObject = playerCustomDataObject->lookupByUniquePath("HairColor");
	if (hairColorObject != 0) {
		unsigned long hairColorValue = *((unsigned long *)hairColorObject->getData());
		std::ostringstream ss;
		ss<<hairColorValue;
		std::string hairColorText = ss.str();
		for (int i=0; i<hairColors.size(); ++i) {
			fieldValue_t &hairColor = hairColors[i];
			if (hairColor.currentValue(isMale) == hairColorText) {
				hairColorIdx = i;
				break;
			}
		}
		int r = (hairColorValue & 0x00FF0000) >> 16;
		int g = (hairColorValue & 0x0000FF00) >> 8;
		int b = (hairColorValue & 0x000000FF) >> 0;
		int a = (hairColorValue & 0xFF000000) >> 24;
		hairColor->r = r;
		hairColor->g = g;
		hairColor->b = b;
		hairColor->a = a;
	}
}

void AppearanceEditorFrame::cleanup() {
	GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
	glContext->cleanup();
	if (skinColor != 0) {
		delete skinColor;
		skinColor = 0;
	}
	if (hairColor != 0) {
		delete hairColor;
		hairColor = 0;
	}
	if (underwearColor != 0) {
		delete underwearColor;
		underwearColor = 0;
	}
	if (shaderProgram != 0) {
		delete shaderProgram;
		shaderProgram = 0;
	}
	for (int i=0; i<equippedItems.size(); ++i) {
		equippedItemData_t &equippedItem = equippedItems[i];
		delete equippedItem.scene;
		delete equippedItem.attachmentPoint;
	}
	equippedItems.clear();
	if (currentHead != 0) {
		delete currentHead;
		currentHead = 0;
	}
	if (currentHair != 0) {
		delete currentHair;
		currentHair = 0;
	}
	if (currentUnderwear != 0) {
		delete currentUnderwear;
		currentUnderwear = 0;
	}
	if (playerCustomDataObject != 0) {
		delete playerCustomDataObject;
		playerCustomDataObject = 0;
	}
	portraits.clear();
	aiPersonalities.clear();
	voices.clear();
	skinColors.clear();
	heads.clear();
	headTextures.clear();
	hairs.clear();
	hairColors.clear();
	underwears.clear();
	underwearTextures.clear();
}

void AppearanceEditorFrame::setup() {
	if (!didInitGlew) {
		didInitGlew = true;
		glewInit();
	}
			
	cleanup();
	
	LsbObject *characterObject = character->getObject();
	if (characterObject != 0) {
		LsbObject *playerDataObject = characterObject->lookupByUniquePath("PlayerData");
		if (playerDataObject != 0) {
			LsbObject *playerCustomDataObject = playerDataObject->lookupByUniquePath("PlayerCustomData");
			oldPlayerCustomDataObject = playerCustomDataObject;
			this->playerCustomDataObject = new LsbObject(*oldPlayerCustomDataObject);
		}
	}
	
	isMale = true;
	
	if (skinColor == 0) {
		skinColor = new VertexRGB({230, 188, 153, 255});
	}
	if (hairColor == 0) {
		hairColor = new VertexRGB({230, 188, 153, 255});
	}
	if (underwearColor == 0) {
		underwearColor = new VertexRGB({93, 12, 8, 255});
	}
	
	QString mText;
	std::vector<GlShader> allShaders;
	
	QFile vertexShader(":/vertex.shd");
	if(!vertexShader.open(QFile::ReadOnly | QFile::Text)){
		MessageBoxA(0, "failed to open vertex.shd", 0, 0);
	}
	{
		QTextStream in(&vertexShader);
		mText = in.readAll();
		std::stringstream ss1(mText.toStdString());
		vertexShader.close();
		allShaders.push_back(GlShader(GL_VERTEX_SHADER, ss1));
		GlShader& shader = allShaders.back();
		if (!shader.compile()) {
			MessageBoxA(0, shader.getLastError().c_str(), 0, 0);
		}
	}
	
	QFile fragmentShader(":/fragment.shd");
	if(!fragmentShader.open(QFile::ReadOnly | QFile::Text)){
		MessageBoxA(0, "failed to open fragment.shd", 0, 0);
	}
	{
		QTextStream in(&fragmentShader);
		mText = in.readAll();
		std::stringstream ss1(mText.toStdString());
		fragmentShader.close();
		allShaders.push_back(GlShader(GL_FRAGMENT_SHADER, ss1));
		GlShader& shader = allShaders.back();
		if (!shader.compile()) {
			MessageBoxA(0, shader.getLastError().c_str(), 0, 0);
		}
	}
	
	shaderProgram = new GlShaderProgram(allShaders);
	if (!shaderProgram->link()) {
		MessageBoxA(0, shaderProgram->getLastError().c_str(), 0, 0);
	}
	
	generateFields();
	initIndexesToCustomData();
	updateAllFields();
	//loadEquipmentData();
}

AppearanceEditorFrame::~AppearanceEditorFrame()
{
	delete ui;
}

void AppearanceEditorFrame::showEvent(QShowEvent *)
{
	setup();
	GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
	glContext->resumeRendering();
}

void AppearanceEditorFrame::closeEvent(QCloseEvent *)
{
	GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
	glContext->pauseRendering();
}

QLabel *AppearanceEditorFrame::field(const char *fieldName) {
	return this->findChild<QLabel *>(fieldName);
}

void AppearanceEditorFrame::changeFieldValue(const char *labelName, int& idx, std::vector<fieldValue_t> &vec, int increment) {
	int newValue = idx + vec.size();
	newValue += increment;
	newValue %= vec.size();
    idx = newValue;
	updateFieldText(field(labelName), vec, idx);
	if (increment != 0) {
		
	}
}

void AppearanceEditorFrame::on_aiPersonalityPrev_clicked()
{
	changeFieldValue("aiPersonalityLabel", aiPersonalityIdx, aiPersonalities, -1);
}

void AppearanceEditorFrame::on_aiPersonalityNext_clicked()
{
	changeFieldValue("aiPersonalityLabel", aiPersonalityIdx, aiPersonalities, 1);
}

void AppearanceEditorFrame::on_voicePrev_clicked()
{
    changeFieldValue("voiceLabel", voiceIdx, voices, -1);
}

void AppearanceEditorFrame::on_voiceNext_clicked()
{
    changeFieldValue("voiceLabel", voiceIdx, voices, 1);
}

void AppearanceEditorFrame::updateToCurrentSkinColor() {
	fieldValue_t &fieldValue = skinColors[skinColorIdx];
	unsigned long color = 0;
	try {
		color = boost::lexical_cast<unsigned long>(fieldValue.currentValue(isMale));
	} catch (const boost::bad_lexical_cast& e) {
		
	}
	int r = (color & 0x00FF0000) >> 16;
	int g = (color & 0x0000FF00) >> 8;
	int b = (color & 0x000000FF) >> 0;
	int a = (color & 0xFF000000) >> 24;
	skinColor->r = r;
	skinColor->g = g;
	skinColor->b = b;
	skinColor->a = a;
}

void AppearanceEditorFrame::updateToCurrentHairColor() {
	fieldValue_t &fieldValue = hairColors[hairColorIdx];
	unsigned long color = 0;
	try {
		color = boost::lexical_cast<unsigned long>(fieldValue.currentValue(isMale));
	} catch (const boost::bad_lexical_cast& e) {
		
	}
	int r = (color & 0x00FF0000) >> 16;
	int g = (color & 0x0000FF00) >> 8;
	int b = (color & 0x000000FF) >> 0;
	int a = (color & 0xFF000000) >> 24;
	hairColor->r = r;
	hairColor->g = g;
	hairColor->b = b;
	hairColor->a = a;
}

void AppearanceEditorFrame::on_skinColorPrev_clicked()
{
    changeFieldValue("skinColorLabel", skinColorIdx, skinColors, -1);
	updateToCurrentSkinColor();
}

void AppearanceEditorFrame::on_skinColorNext_clicked()
{
    changeFieldValue("skinColorLabel", skinColorIdx, skinColors, 1);
	updateToCurrentSkinColor();
}

void AppearanceEditorFrame::on_headPrev_clicked()
{
    changeFieldValue("headLabel", headIdx, heads, -1);
	updateToCurrentHead();
}

void AppearanceEditorFrame::on_headNext_clicked()
{
    changeFieldValue("headLabel", headIdx, heads, 1);
	updateToCurrentHead();
}

void AppearanceEditorFrame::on_hairPrev_clicked()
{
    changeFieldValue("hairLabel", hairIdx, hairs, -1);
	updateToCurrentHair();
}

void AppearanceEditorFrame::on_hairNext_clicked()
{
    changeFieldValue("hairLabel", hairIdx, hairs, 1);
	updateToCurrentHair();
}

void AppearanceEditorFrame::updateToCurrentModel(ZGrannyScene *&current, std::vector<fieldValue_t> &models, std::vector<fieldValue_t> &textures, int index, VertexRGB *foreColor, VertexRGB *backColor) {
	GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
	std::string modelFile = models[index].currentValue(isMale);
	std::string textureFile = textures[index].currentValue(isMale);
	std::vector<GLuint> modelTextures;
	
	nv_dds::CDDSImage image3;
	GLuint texobj3;
	boost::filesystem::ifstream stream3(
				"C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\" + textureFile + "_DM.dds",
				std::ios_base::binary);
	unsigned long textureFileSize;
	std::wstring tmp = L"";
	std::string texturePath;
	char *fileBytes = 0;
	
	texturePath = "Public/Main/Assets/Textures/Characters/Player/" + textureFile + "_DM.dds";
	fileBytes = texturesPak.extractFileIntoMemory(gameDataPath + L"Textures.pak", texturePath, tmp, false, &textureFileSize);
	if (fileBytes != 0) {
		std::stringstream ss;
		ss.rdbuf()->pubsetbuf(fileBytes, textureFileSize);
		nv_dds::CDDSImage image3;
		GLuint texobj3;
		image3.load(ss, false);
		glGenTextures(1, &texobj3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texobj3);
		image3.upload_texture2D();
		modelTextures.push_back(texobj3);
		delete[] fileBytes;
	}
	
	texturePath = "Public/Main/Assets/Textures/Characters/Player/" + textureFile + "_NM.dds";
	fileBytes = texturesPak.extractFileIntoMemory(gameDataPath + L"Textures.pak", texturePath, tmp, false, &textureFileSize);
	if (fileBytes != 0) {
		std::stringstream ss;
		ss.rdbuf()->pubsetbuf(fileBytes, textureFileSize);
		nv_dds::CDDSImage image3;
		GLuint texobj3;
		image3.load(ss, false);
		glGenTextures(1, &texobj3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texobj3);
		image3.upload_texture2D();
		modelTextures.push_back(texobj3);
		delete[] fileBytes;
	}
	
	texturePath = "Public/Main/Assets/Textures/Characters/Player/" + textureFile + "_SM.dds";
	fileBytes = texturesPak.extractFileIntoMemory(gameDataPath + L"Textures.pak", texturePath, tmp, false, &textureFileSize);
	if (fileBytes == 0) {
		texturePath = "Public/Main/Assets/Textures/Characters/Player/" + textureFile + "_MSK.dds";
		fileBytes = texturesPak.extractFileIntoMemory(gameDataPath + L"Textures.pak", texturePath, tmp, false, &textureFileSize);
	}
	if (fileBytes != 0) {
		std::stringstream ss;
		ss.rdbuf()->pubsetbuf(fileBytes, textureFileSize);
		nv_dds::CDDSImage image3;
		GLuint texobj3;
		image3.load(ss, false);
		glGenTextures(1, &texobj3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texobj3);
		image3.upload_texture2D();
		modelTextures.push_back(texobj3);
		delete[] fileBytes;
	}
	
//	nv_dds::CDDSImage image9;
//	GLuint texobj9;
//	std::string nmDdsPath = "C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\" + textureFile + "_NM.dds";
//	boost::filesystem::path nmPath(nmDdsPath);
//	if (boost::filesystem::exists(nmPath)) {
//		boost::filesystem::ifstream stream9(
//					nmDdsPath,
//					std::ios_base::binary);
//		image9.load(stream9, false);
//		glGenTextures(1, &texobj9);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, texobj9);
//		image9.upload_texture2D();
//	}
	
//	nv_dds::CDDSImage image10;
//	GLuint texobj10;
//	std::string smDdsPath = "C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\" + textureFile + "_SM.dds";
//	boost::filesystem::path smPath(smDdsPath);
//	if (boost::filesystem::exists(smPath)) {
//		boost::filesystem::ifstream stream10(
//					smDdsPath,
//					std::ios_base::binary);
//		image10.load(stream10, false);
//	} else {
//		boost::filesystem::ifstream stream10(
//					"C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\" + textureFile + "_MSK.dds",
//					std::ios_base::binary);
//		image10.load(stream10, false);
//	}
//	glGenTextures(1, &texobj10);
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, texobj10);
//	image10.upload_texture2D();
	
	if (current != 0) {
		glContext->removeGrannyScene(current);
		zGrannyShutdownScene(current);
		delete current; //call granny free function?
	}
//	modelTextures.push_back(texobj3);
//	modelTextures.push_back(texobj9);
//	modelTextures.push_back(texobj10);
	unsigned long fSize;
	std::string extractPath = "Public/Main/Assets/Characters/Players/" + modelFile + ".GR2";
	fileBytes = mainPak.extractFileIntoMemory(gameDataPath + L"Main.pak", extractPath, gameDataPath, false, &fSize);
	if (fileBytes == 0) {
		MessageBoxA(0, "Failed to load model from game data", 0, 0);
	}
	current = 
			zGrannyCreateSceneFromMemory(fileBytes, fSize,
							   modelTextures);
	if (fileBytes != 0) {
		delete[] fileBytes;
	}
	glContext->addGrannyScene(current, modelTextures, foreColor, backColor, shaderProgram, 0);
	modelTextures.clear();
}

void AppearanceEditorFrame::updateToCurrentHair() {
	updateToCurrentModel(currentHair, hairs, hairs, hairIdx, 0, hairColor);
}

void AppearanceEditorFrame::updateToCurrentHead() {
	updateToCurrentModel(currentHead, heads, headTextures, headIdx, skinColor, hairColor);
}

void AppearanceEditorFrame::updateToCurrentUnderwear() {
	updateToCurrentModel(currentUnderwear, underwears, underwearTextures, underwearIdx, skinColor, underwearColor);
}

void AppearanceEditorFrame::on_hairColorPrev_clicked()
{
    changeFieldValue("hairColorLabel", hairColorIdx, hairColors, -1);
	updateToCurrentHairColor();
}

void AppearanceEditorFrame::on_hairColorNext_clicked()
{
    changeFieldValue("hairColorLabel", hairColorIdx, hairColors, 1);
	updateToCurrentHairColor();
}

void AppearanceEditorFrame::on_underwearPrev_clicked()
{
    changeFieldValue("underwearLabel", underwearIdx, underwears, -1);
	updateToCurrentUnderwear();
}

void AppearanceEditorFrame::on_underwearNext_clicked()
{
    changeFieldValue("underwearLabel", underwearIdx, underwears, 1);
	updateToCurrentUnderwear();
}

void AppearanceEditorFrame::on_cancelButton_clicked()
{
    this->close();
}

void AppearanceEditorFrame::on_skinColorPicker_clicked()
{
    QColorDialog dialog(this);
	dialog.setCurrentColor(QColor(skinColor->r, skinColor->g, skinColor->b, skinColor->a));
	dialog.setOption(QColorDialog::ShowAlphaChannel);
	if (dialog.exec()) {
		QColor selected = dialog.selectedColor();
		skinColor->r = selected.red();
		skinColor->g = selected.green();
		skinColor->b = selected.blue();
		skinColor->a = selected.alpha();
	}
}

void AppearanceEditorFrame::on_hairColorPicker_clicked()
{
	QColorDialog dialog(this);
	dialog.setCurrentColor(QColor(hairColor->r, hairColor->g, hairColor->b, hairColor->a));
	dialog.setOption(QColorDialog::ShowAlphaChannel);
	if (dialog.exec()) {
		QColor selected = dialog.selectedColor();
		hairColor->r = selected.red();
		hairColor->g = selected.green();
		hairColor->b = selected.blue();
		hairColor->a = selected.alpha();
	}
}
GamePakData *AppearanceEditorFrame::getGamePakData() const
{
	return gamePakData;
}

void AppearanceEditorFrame::setGamePakData(GamePakData *value)
{
	gamePakData = value;
}

EquipmentHandler *AppearanceEditorFrame::getEquipHandler() const
{
	return equipHandler;
}

void AppearanceEditorFrame::setEquipHandler(EquipmentHandler *value)
{
	equipHandler = value;
}


void AppearanceEditorFrame::on_armorToggleButton_clicked()
{
	showEquipped = !showEquipped;
	GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
	for (int i=0; i<equippedItems.size(); ++i) {
		equippedItemData_t &eid = equippedItems[i];
		if (showEquipped) {
			glContext->addGrannyScene(eid.scene, eid.textures, 0, 0, shaderProgram, eid.attachmentPoint);
		} else {
			glContext->removeGrannyScene(eid.scene);
		}
	}
}

void AppearanceEditorFrame::on_femaleButton_clicked()
{
	isMale = false;
    updateAllFields();
}

void AppearanceEditorFrame::on_maleButton_clicked()
{
	isMale = true;
    updateAllFields();
}
