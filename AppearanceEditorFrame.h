#ifndef APPEARANCEEDITORFRAME_H
#define APPEARANCEEDITORFRAME_H

#include <QFrame>
#include "GlContextWidget.h"
#include "zgranny.h"
#include "PakReader.h"
#include "EquipmentHandler.h"
#include "GameCharacter.h"
#include "GenStatsReader.h"

#define TEXT_INVALID	" (Invalid)"

namespace Ui {
class AppearanceEditorFrame;
}

typedef struct {
	std::string name;
	std::string maleValue;
	std::string femaleValue;
	std::string &currentValue(bool isMale) {
		if (isMale) {
			return maleValue;
		} else {
			return femaleValue;
		}
	}
	void setValue(std::string &value, bool isMale) {
		if (isMale) {
			maleValue = value;
		} else {
			femaleValue = value;
		}
	}
} fieldValue_t;

typedef struct {
	ZGrannyScene *scene;
	std::vector<GLuint> textures;
	MeshAttachmentPoint *attachmentPoint;
} equippedItemData_t;

struct itemColor {
	VertexRGB color1;
	VertexRGB color2;
	VertexRGB color3;
};

class AppearanceChangeCallback {
public:
	virtual void onAppearanceChange(LsbObject *oldPlayerCustomDataObject, LsbObject *newPlayerCustomDataObject) = 0;
};

class AppearanceEditorFrame : public QFrame
{
	Q_OBJECT
	
public:
	explicit AppearanceEditorFrame(std::wstring gameDataPath, GameCharacter *character, QWidget *parent = 0);
	~AppearanceEditorFrame();
	void showEvent(QShowEvent *);
	void closeEvent(QCloseEvent *);
	void keyPressEvent(QKeyEvent* e) {
		GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
		glContext->keyPressEvent(e);
	}
	void keyReleaseEvent(QKeyEvent* e) {
		GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
		glContext->keyReleaseEvent(e);
	}
	
	EquipmentHandler *getEquipHandler() const;
	void setEquipHandler(EquipmentHandler *value);
	
	GamePakData *getGamePakData() const;
	void setGamePakData(GamePakData *value);
	void registerAppearanceChangeCallback(AppearanceChangeCallback *appearanceChangeCallback);
	
private slots:
	void on_aiPersonalityNext_clicked();
	
	void on_aiPersonalityPrev_clicked();
	
	void on_voicePrev_clicked();
	
	void on_voiceNext_clicked();
	
	void on_skinColorPrev_clicked();
	
	void on_skinColorNext_clicked();
	
	void on_headPrev_clicked();
	
	void on_headNext_clicked();
	
	void on_hairPrev_clicked();
	
	void on_hairNext_clicked();
	
	void on_hairColorPrev_clicked();
	
	void on_hairColorNext_clicked();
	
	void on_underwearPrev_clicked();
	
	void on_underwearNext_clicked();
	
	void on_cancelButton_clicked();
	
	void on_skinColorPicker_clicked();
	
	void on_hairColorPicker_clicked();
	
	void on_armorToggleButton_clicked();
	
	void on_femaleButton_clicked();
	
	void on_maleButton_clicked();
	
	void on_portraitPrev_clicked();
	
	void on_portraitNext_clicked();
	
	void on_applyButton_clicked();
	
private:
	std::vector<fieldValue_t> portraits;
	std::vector<fieldValue_t> aiPersonalities;
	std::vector<fieldValue_t> voices;
	std::vector<fieldValue_t> skinColors;
	std::vector<fieldValue_t> heads;
	std::vector<fieldValue_t> headDiffuse;
	std::vector<fieldValue_t> headNormal;
	std::vector<fieldValue_t> headSpecular;
	std::vector<fieldValue_t> headMask;
	std::vector<fieldValue_t> hairs;
	std::vector<fieldValue_t> hairDiffuse;
	std::vector<fieldValue_t> hairNormal;
	std::vector<fieldValue_t> hairSpecular;
	std::vector<fieldValue_t> hairMask;
	std::vector<fieldValue_t> hairColors;
	std::vector<fieldValue_t> underwears;
	std::vector<fieldValue_t> underwearDiffuse;
	std::vector<fieldValue_t> underwearNormal;
	std::vector<fieldValue_t> underwearSpecular;
	std::vector<fieldValue_t> underwearMask;
	std::string textureDiffuseOverride;
	std::string textureNormalOverride;
	std::string textureSpecularOverride;
	std::string textureMaskOverride;
	VertexRGB *maleOverrideColor = 0;
	VertexRGB *femaleOverrideColor = 0;
	
	std::vector<fieldValue_t> henchHeads;
	std::vector<fieldValue_t> henchHeadDiffuse;
	std::vector<fieldValue_t> henchHeadNormal;
	std::vector<fieldValue_t> henchHeadSpecular;
	std::vector<fieldValue_t> henchHeadMask;
	std::vector<fieldValue_t> henchHairs;
	std::vector<fieldValue_t> henchHairDiffuse;
	std::vector<fieldValue_t> henchHairNormal;
	std::vector<fieldValue_t> henchHairSpecular;
	std::vector<fieldValue_t> henchHairMask;
	std::vector<fieldValue_t> henchUnderwears;
	std::vector<fieldValue_t> henchUnderwearDiffuse;
	std::vector<fieldValue_t> henchUnderwearNormal;
	std::vector<fieldValue_t> henchUnderwearSpecular;
	std::vector<fieldValue_t> henchUnderwearMask;
	//std::vector<fieldValue_t> underwearTextures;
	
	int portraitIdx = 0;
	int aiPersonalityIdx = 0;
	int voiceIdx = 0;
	int skinColorIdx = 0;
	int headIdx = 0;
	int hairIdx = 0;
	int hairColorIdx = 0;
	int underwearIdx = 0;
	
	bool hasVisualHelmet;
	static bool didInitGlew;
	GlShaderProgram *shaderProgram = 0;
	GlShaderProgram *equipmentShaderProgram = 0;
	Ui::AppearanceEditorFrame *ui;
	bool isMale;
	bool isHench;
	std::wstring gameDataPath;
	static bool loadedPaks;
	static PakReader mainPak;
	static PakReader texturesPak;
	static LsbObject *playersTemplateObjects;
	EquipmentHandler *equipHandler;
	GamePakData *gamePakData;
	bool showEquipped = true;
	GameCharacter *character;
	LsbObject *oldPlayerCustomDataObject = 0;
	LsbObject *playerCustomDataObject = 0;
	AppearanceChangeCallback *appearanceChangeCallback = 0;
	
	std::string portrait;
	VertexRGB *skinColor = 0;
	VertexRGB *hairColor = 0;
	VertexRGB *underwearColor = 0;
	
	itemColor helmColor;
	itemColor breastColor;
	itemColor breastColor2;
	itemColor breastColor3;
	itemColor bootColor;
	itemColor bracerColor;
	itemColor garmentColor;
	
	static std::vector<StatsContainer *> itemColorStats;
	
	LsbObject *weaponsResourceBankObject = 0;
	LsbObject *armorsPlayerResourceBankObject = 0;
	LsbObject *playerMaleResourceBankObject = 0;
	LsbObject *playerFemaleResourceBankObject = 0;
	
	ZGrannyScene *currentHair = 0;
	ZGrannyScene *currentHead = 0;
	ZGrannyScene *currentUnderwear = 0;
	std::vector<equippedItemData_t> equippedItems;
	
	void cleanup();
	void cleanupEquipmentData();
	void loadEquipmentData();
	void updateAllFields();
	void initIndexesToCustomData();
	std::string getItemStatText(GameItem *item, std::string statName);
	std::string getModStatText(GameItem *item, std::string statName);
	std::string getPermBoostStatText(GameItem *item, std::string statName);
	std::string getFinalItemStatText(GameItem *item, std::string statName);
	unsigned long hexToNumber(std::string hex);
	void generateEquipmentModels();
	std::string getGR2(LsbObject *resourceBankObject, std::string &visualTemplate);
	std::string getTextureFromTextureTemplate(LsbObject *resourceBankObject, std::string &textureTemplate);
	bool _getTextureMaps(LsbObject *materialsResourceBankObject, std::string &materialId, std::string &diffuseMap, std::string &normalMap, 
											   std::string &specularMap, std::string &maskMap);
	bool getTextureMaps(LsbObject *resourceBankObject, LsbObject *materialsResourceBankObject, std::string &visualTemplate, std::string &diffuseMap, std::string &normalMap, 
						std::string &specularMap, std::string &maskMap);
	bool updateToCurrentModel(ZGrannyScene *&current, std::vector<fieldValue_t> &models, std::vector<fieldValue_t> &diffuse, 
							  std::vector<fieldValue_t> &normal, std::vector<fieldValue_t> &specular, std::vector<fieldValue_t> &mask, 
							  int index, VertexRGB *foreColor, VertexRGB *backColor, bool useOverride);
	void updatePortraitData();
	void updateToCurrentPortrait();
	void updatePortraitImage();
	bool updateToCurrentHead();
	bool updateToCurrentHair();
	bool updateToCurrentUnderwear();
	void updateToCurrentSkinColor();
	void updateToCurrentHairColor();
	void updateColorData(const char *colorPath, VertexRGB *colorInfo);
	void getVisualInfoForItem(LsbObject *equipmentObject, std::string &visualTemplate, std::string &customBodyMaterial, bool& armorLookup);
	ZGrannyScene *createModelForItem(GameItem *item, std::vector<GLuint > &textures);
	void setup();
	void generateFields();
	void populateFieldValuesForTemplate(std::string templateId, std::string fieldType, std::string namePrefix, 
										std::vector<fieldValue_t> &modelFields, std::vector<fieldValue_t> &diffuseFields, 
										std::vector<fieldValue_t> &normalFields, std::vector<fieldValue_t> &specularFields, std::vector<fieldValue_t> &maskFields, bool isMale);
	QLabel *field(const char *fieldName);
	void changeFieldValue(const char *labelName, int& idx, std::vector<fieldValue_t> &vec, int increment = 0);
	void updateFieldText(QLabel *label, std::vector<fieldValue_t> &updateVector, int index);
	void appendFieldText(std::string labelName, std::string text);
	void updateObjectValue(const char *labelName, int& idx, std::vector<fieldValue_t> &vec);
};

#endif // APPEARANCEEDITORFRAME_H
