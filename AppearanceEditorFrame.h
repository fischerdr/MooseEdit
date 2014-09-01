#ifndef APPEARANCEEDITORFRAME_H
#define APPEARANCEEDITORFRAME_H

#include <QFrame>
#include "GlContextWidget.h"
#include "zgranny.h"
#include "PakReader.h"
#include "EquipmentHandler.h"
#include "GameCharacter.h"

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
} fieldValue_t;

typedef struct {
	ZGrannyScene *scene;
	std::vector<GLuint> textures;
	MeshAttachmentPoint *attachmentPoint;
} equippedItemData_t;

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
	
private:
	std::vector<fieldValue_t> portraits;
	std::vector<fieldValue_t> aiPersonalities;
	std::vector<fieldValue_t> voices;
	std::vector<fieldValue_t> skinColors;
	std::vector<fieldValue_t> heads;
	std::vector<fieldValue_t> headTextures;
	std::vector<fieldValue_t> hairs;
	std::vector<fieldValue_t> hairColors;
	std::vector<fieldValue_t> underwears;
	std::vector<fieldValue_t> underwearTextures;
	
	int portraitIdx = 0;
	int aiPersonalityIdx = 0;
	int voiceIdx = 0;
	int skinColorIdx = 0;
	int headIdx = 0;
	int hairIdx = 0;
	int hairColorIdx = 0;
	int underwearIdx = 0;
	
	bool shouldRender = true;
	static bool didInitGlew;
	GlShaderProgram *shaderProgram = 0;
	Ui::AppearanceEditorFrame *ui;
	bool isMale;
	std::wstring gameDataPath;
	static bool loadedPaks;
	static PakReader mainPak;
	static PakReader texturesPak;
	EquipmentHandler *equipHandler;
	GamePakData *gamePakData;
	bool showEquipped = true;
	GameCharacter *character;
	LsbObject *oldPlayerCustomDataObject = 0;
	LsbObject *playerCustomDataObject = 0;
	
	std::string portrait;
	VertexRGB *skinColor = 0;
	VertexRGB *hairColor = 0;
	VertexRGB *underwearColor = 0;
	
	LsbObject *weaponsResourceBankObject = 0;
	LsbObject *armorsPlayerResourceBankObject = 0;
	LsbObject *playerMaleResourceBankObject = 0;
	LsbObject *playerFemaleResourceBankObject = 0;
	
	ZGrannyScene *currentHair = 0;
	ZGrannyScene *currentHead = 0;
	ZGrannyScene *currentUnderwear = 0;
	std::vector<equippedItemData_t> equippedItems;
	
	void cleanup();
	void loadEquipmentData();
	void updateAllFields();
	void initIndexesToCustomData();
	void generateEquipmentModels();
	std::string getGR2(LsbObject *resourceBankObject, std::string &visualTemplate);
	std::string getTextureFromTextureTemplate(LsbObject *resourceBankObject, std::string &textureTemplate);
	bool getTextureMaps(LsbObject *resourceBankObject, LsbObject *materialsResourceBankObject, std::string &visualTemplate, std::string &diffuseMap, std::string &normalMap, std::string &maskMap);
	void updateToCurrentModel(ZGrannyScene *&current, std::vector<fieldValue_t> &models, std::vector<fieldValue_t> &textures, int index, VertexRGB *foreColor, VertexRGB *backColor);
	void updateToCurrentPortrait();
	void updatePortraitImage();
	void updateToCurrentHead();
	void updateToCurrentHair();
	void updateToCurrentUnderwear();
	void updateToCurrentSkinColor();
	void updateToCurrentHairColor();
	ZGrannyScene *createModelForItem(GameItem *item, std::vector<GLuint > &textures);
	void setup();
	void generateFields();
	QLabel *field(const char *fieldName);
	void changeFieldValue(const char *labelName, int& idx, std::vector<fieldValue_t> &vec, int increment = 0);
	void updateFieldText(QLabel *label, std::vector<fieldValue_t> &updateVector, int index);
	void updateObjectValue(const char *labelName, int& idx, std::vector<fieldValue_t> &vec);
};

#endif // APPEARANCEEDITORFRAME_H
