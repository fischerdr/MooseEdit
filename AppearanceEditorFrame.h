#ifndef APPEARANCEEDITORFRAME_H
#define APPEARANCEEDITORFRAME_H

#include <QFrame>
#include "GlContextWidget.h"
#include "zgranny.h"
#include "PakReader.h"

namespace Ui {
class AppearanceEditorFrame;
}

typedef struct {
	std::string name;
	std::string maleValue;
	std::string femaleValue;
} fieldValue_t;

class AppearanceEditorFrame : public QFrame
{
	Q_OBJECT
	
public:
	explicit AppearanceEditorFrame(std::wstring gameDataPath, QWidget *parent = 0);
	~AppearanceEditorFrame();
	void showEvent(QShowEvent *);
	void keyPressEvent(QKeyEvent* e) {
		GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
		glContext->keyPressEvent(e);
	}
	void keyReleaseEvent(QKeyEvent* e) {
		GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
		glContext->keyReleaseEvent(e);
	}
	
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
	
private:
	std::vector<fieldValue_t> aiPersonalities;
	std::vector<fieldValue_t> voices;
	std::vector<fieldValue_t> skinColors;
	std::vector<fieldValue_t> heads;
	std::vector<fieldValue_t> headTextures;
	std::vector<fieldValue_t> hairs;
	std::vector<fieldValue_t> hairColors;
	std::vector<fieldValue_t> underwears;
	std::vector<fieldValue_t> underwearTextures;
	
	int aiPersonalityIdx = 0;
	int voiceIdx = 0;
	int skinColorIdx = 0;
	int headIdx = 0;
	int hairIdx = 0;
	int hairColorIdx = 0;
	int underwearIdx = 0;
	
	static bool didInitGlew;
	GlShaderProgram *shaderProgram = 0;
	VertexRGB *skinColor = 0;
	VertexRGB *hairColor = 0;
	VertexRGB *underwearColor = 0;
	Ui::AppearanceEditorFrame *ui;
	bool isMale;
	std::wstring gameDataPath;
	PakReader mainPak;
	
	ZGrannyScene *currentHair = 0;
	ZGrannyScene *currentHead = 0;
	ZGrannyScene *currentUnderwear = 0;
	
	void updateToCurrentModel(ZGrannyScene *&current, std::vector<fieldValue_t> &models, std::vector<fieldValue_t> &textures, int index, VertexRGB *foreColor, VertexRGB *backColor);
	void updateToCurrentHead();
	void updateToCurrentHair();
	void updateToCurrentUnderwear();
	void updateToCurrentSkinColor();
	void updateToCurrentHairColor();
	void setup();
	void generateFields();
	QLabel *field(const char *fieldName);
	void changeFieldValue(const char *labelName, int& idx, std::vector<fieldValue_t> &vec, int increment = 0);
	void updateFieldText(QLabel *label, std::vector<fieldValue_t> &updateVector, int index);
};

#endif // APPEARANCEEDITORFRAME_H
