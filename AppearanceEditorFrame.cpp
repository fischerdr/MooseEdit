#include "AppearanceEditorFrame.h"
#include "ui_AppearanceEditorFrame.h"

#define GLEW_STATIC
#include <gl/glew.h>
#include "zgranny.h"
#include "LsxReader.h"
#include "GlShader.h"
#include "GlShaderProgram.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>
#include <QTextStream>
#include <QFile>
#include <QColorDialog>

#include "nv_dds.h"

#define NUM_HEADS		15
#define NUM_HAIRS		15
#define NUM_UNDERWEARS	3

//typedef void (*glCompressedTexImage2DARB_t)(GLenum target, 	GLint level, 	GLenum internalformat, 	GLsizei width, 	GLsizei height, 	GLint border, 	GLsizei imageSize, 	const GLvoid * data);
//glCompressedTexImage2DARB_t glCompressedTexImage2DARB = (glCompressedTexImage2DARB_t)wglGetProcAddress("glCompressedTexImage2DARB");

bool AppearanceEditorFrame::didInitGlew = false;

AppearanceEditorFrame::AppearanceEditorFrame(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::AppearanceEditorFrame)
{
	ui->setupUi(this);
}

void AppearanceEditorFrame::generateFields() {
	boost::filesystem::ifstream fin("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Mods\\Main\\CharacterCreation\\properties.lsx", 
									std::ios_base::binary);
	if (fin) {
		LsxReader reader;
		std::vector<LsbObject *> objects = reader.loadFile(fin);
		if (objects.size() == 0) {
			MessageBoxA(0, "Failed to parse CharacterCreation properties!", 0, 0);
		}
		fin.close();
		{
			LsbObject *aiPersonalitiesObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/AiPersonalities");
			std::vector<LsbObject *> aiPersonalityObjects = LsbObject::lookupAllEntitiesWithName(aiPersonalitiesObject, "AiPersonality");
			for (int i=0; i<aiPersonalityObjects.size(); ++i) {
				LsbObject *aiPersonalityObject = aiPersonalityObjects[i];
				LsbObject *nameObject = LsbObject::lookupByUniquePathEntity(aiPersonalityObject, "Name");
				LsbObject *idObject = LsbObject::lookupByUniquePathEntity(aiPersonalityObject, "ID");
				aiPersonalities.push_back({nameObject->getData(), idObject->getData(), idObject->getData()});
			}
		}
		{
			LsbObject *voicesObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/Voices");
			std::vector<LsbObject *> voiceObjects = LsbObject::lookupAllEntitiesWithName(voicesObject, "Voice");
			for (int i=0; i<voiceObjects.size(); ++i) {
				LsbObject *voiceObject = voiceObjects[i];
				LsbObject *nameObject = LsbObject::lookupByUniquePathEntity(voiceObject, "Name");
				LsbObject *maleSpeakerIdObject = LsbObject::lookupByUniquePathEntity(voiceObject, "MaleSpeakerID");
				LsbObject *femaleSpeakerIdObject = LsbObject::lookupByUniquePathEntity(voiceObject, "FemaleSpeakerID");
				voices.push_back({nameObject->getData(), maleSpeakerIdObject->getData(), femaleSpeakerIdObject->getData()});
			}
		}
		{
			LsbObject *skinColorsObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/SkinColors");
			std::vector<LsbObject *> colorDescObjects = LsbObject::lookupAllEntitiesWithName(skinColorsObject, "ColorDesc");
			for (int i=0; i<colorDescObjects.size(); ++i) {
				LsbObject *colorDescObject = colorDescObjects[i];
				LsbObject *colorNameObject = LsbObject::lookupByUniquePathEntity(colorDescObject, "ColorName");
				LsbObject *valueObject = LsbObject::lookupByUniquePathEntity(colorDescObject, "Value");
				std::ostringstream ss;
				ss<<*((unsigned long*)valueObject->getData());
				skinColors.push_back({colorNameObject->getData(), ss.str(), ss.str()});
			}
		}
		for (int i=0; i<NUM_HEADS; ++i) {
			std::ostringstream ss;
			ss<<"Head "<<(i + 1);
			heads.push_back({ss.str(), "", ""});
		}
		for (int i=0; i<NUM_HAIRS; ++i) {
			std::ostringstream ss;
			ss<<"Hair "<<(i + 1);
			hairs.push_back({ss.str(), "", ""});
		}
		for (int i=0; i<NUM_UNDERWEARS; ++i) {
			std::ostringstream ss;
			ss<<"Underwear "<<(i + 1);
			underwears.push_back({ss.str(), "", ""});
		}
		{
			LsbObject *hairColorsObject = LsbObject::lookupByUniquePath(objects, "CharacterCreationProperties/root/HairColors");
			std::vector<LsbObject *> colorDescObjects = LsbObject::lookupAllEntitiesWithName(hairColorsObject, "ColorDesc");
			for (int i=0; i<colorDescObjects.size(); ++i) {
				LsbObject *colorDescObject = colorDescObjects[i];
				LsbObject *colorNameObject = LsbObject::lookupByUniquePathEntity(colorDescObject, "ColorName");
				LsbObject *valueObject = LsbObject::lookupByUniquePathEntity(colorDescObject, "Value");
				std::ostringstream ss;
				ss<<*((unsigned long*)valueObject->getData());
				hairColors.push_back({colorNameObject->getData(), ss.str(), ss.str()});
			}
		}
	} else {
		MessageBoxA(0, "Couldn't read CharacterCreation properties file!", 0, 0);
	}
	
	changeFieldValue("aiPersonalityLabel", aiPersonalityIdx, aiPersonalities);
	changeFieldValue("voiceLabel", voiceIdx, voices);
	changeFieldValue("skinColorLabel", skinColorIdx, skinColors);
	changeFieldValue("headLabel", headIdx, heads);
	changeFieldValue("hairLabel", hairIdx, hairs);
	changeFieldValue("hairColorLabel", hairColorIdx, hairColors);
	changeFieldValue("underwearLabel", underwearIdx, underwears);
	
	updateToCurrentSkinColor();
	updateToCurrentHairColor();
}

void AppearanceEditorFrame::updateFieldText(QLabel *label, std::vector<fieldValue_t> &updateVector, int index) {
	fieldValue_t &fieldValue = updateVector[index];
	label->setText(fieldValue.name.c_str());
}

void AppearanceEditorFrame::setup() {
	if (!didInitGlew) {
		didInitGlew = true;
		glewInit();
	}
			
	//cleanup();
	
	if (skinColor == 0) {
		skinColor = new VertexRGB({230, 188, 153, 255});
	}
	
	if (hairColor == 0) {
		hairColor = new VertexRGB({230, 188, 153, 255});
	}
	
	generateFields();
	
	GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
	//DDSLoader ddsLoader;
	//ddsLoader.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Body_A_DM.dds");
	//loadTexture(ddsLoader.getDataBuffer(), ddsLoader.getDataBufferSize(), ddsLoader.getWidth(), ddsLoader.getHeight(), ddsLoader.getFormat());
	//loadDDSTexture("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Body_A_DM.dds");
	
	nv_dds::CDDSImage image;
	GLuint texobj;
	image.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Body_A_DM.dds", false);
	glGenTextures(1, &texobj);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj);
	image.upload_texture2D();
	
	nv_dds::CDDSImage image5;
	GLuint texobj5;
	image5.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Body_A_NM.dds", false);
	glGenTextures(1, &texobj5);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj5);
	image5.upload_texture2D();
	
	nv_dds::CDDSImage image6;
	GLuint texobj6;
	image6.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Body_A_SM.dds", false);
	glGenTextures(1, &texobj6);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj6);
	image6.upload_texture2D();
	
	nv_dds::CDDSImage image2;
	GLuint texobj2;
	image2.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Head_A_DM.dds", false);
	glGenTextures(1, &texobj2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj2);
	image2.upload_texture2D();
	
	nv_dds::CDDSImage image7;
	GLuint texobj7;
	image7.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Head_A_NM.dds", false);
	glGenTextures(1, &texobj7);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj7);
	image7.upload_texture2D();
	
	nv_dds::CDDSImage image8;
	GLuint texobj8;
	image8.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Head_A_SM.dds", false);
	glGenTextures(1, &texobj8);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj8);
	image8.upload_texture2D();
	
	nv_dds::CDDSImage image3;
	GLuint texobj3;
	image3.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Hair_A_DM.dds", false);
	glGenTextures(1, &texobj3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj3);
	image3.upload_texture2D();
	
	nv_dds::CDDSImage image9;
	GLuint texobj9;
	image9.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Hair_A_NM.dds", false);
	glGenTextures(1, &texobj9);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj9);
	image9.upload_texture2D();
	
	nv_dds::CDDSImage image10;
	GLuint texobj10;
	image10.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Hair_A_SM.dds", false);
	glGenTextures(1, &texobj10);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj10);
	image10.upload_texture2D();
	
	nv_dds::CDDSImage image4;
	GLuint texobj4;
	image4.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_ARM_Cloth_A_Torso_ABC_Tx_00_DM.dds", false);
	glGenTextures(1, &texobj4);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj4);
	image4.upload_texture2D();

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
	
	std::vector<GLint> textures;
	
	textures.push_back(texobj);
	textures.push_back(texobj5);
	textures.push_back(texobj6);
	ZGrannyScene *grannyScene = 
			zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Body_A.GR2",
							   textures);
	glContext->addGrannyScene(grannyScene, textures, skinColor, shaderProgram);
	textures.clear();
	
	textures.push_back(texobj2);
	textures.push_back(texobj7);
	textures.push_back(texobj8);
	ZGrannyScene *grannyScene2 = 
			zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Head_A.GR2",
							   textures);
	glContext->addGrannyScene(grannyScene2, textures, skinColor, shaderProgram);
	textures.clear();
	
	textures.push_back(texobj3);
	textures.push_back(texobj9);
	textures.push_back(texobj10);
	ZGrannyScene *grannyScene3 = 
			zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Hair_A.GR2",
							   textures);
	glContext->addGrannyScene(grannyScene3, textures, skinColor, shaderProgram);
	textures.clear();
	
	textures.push_back(texobj4);
	ZGrannyScene *grannyScene4 = 
			zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_ARM_Cloth_A_Torso_A.GR2",
							   textures);
	//glContext->addGrannyScene(grannyScene4, textures);
	textures.clear();
}

AppearanceEditorFrame::~AppearanceEditorFrame()
{
	delete ui;
}

void AppearanceEditorFrame::showEvent(QShowEvent *)
{
	setup();
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
		color = boost::lexical_cast<unsigned long>(fieldValue.maleValue);
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
		color = boost::lexical_cast<unsigned long>(fieldValue.maleValue);
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
}

void AppearanceEditorFrame::on_headNext_clicked()
{
    changeFieldValue("headLabel", headIdx, heads, 1);
}

void AppearanceEditorFrame::on_hairPrev_clicked()
{
    changeFieldValue("hairLabel", hairIdx, hairs, -1);
}

void AppearanceEditorFrame::on_hairNext_clicked()
{
    changeFieldValue("hairLabel", hairIdx, hairs, 1);
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
}

void AppearanceEditorFrame::on_underwearNext_clicked()
{
    changeFieldValue("underwearLabel", underwearIdx, underwears, 1);
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
