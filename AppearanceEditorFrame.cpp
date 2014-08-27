#include "AppearanceEditorFrame.h"
#include "ui_AppearanceEditorFrame.h"

#define GLEW_STATIC
#include <gl/glew.h>
#include "zgranny.h"
#include "LsxReader.h"
#include "GlShader.h"
#include "GlShaderProgram.h"

#include <boost/filesystem/fstream.hpp>
#include <QTextStream>
#include <QFile>

#include "nv_dds.h"

//typedef void (*glCompressedTexImage2DARB_t)(GLenum target, 	GLint level, 	GLenum internalformat, 	GLsizei width, 	GLsizei height, 	GLint border, 	GLsizei imageSize, 	const GLvoid * data);
//glCompressedTexImage2DARB_t glCompressedTexImage2DARB = (glCompressedTexImage2DARB_t)wglGetProcAddress("glCompressedTexImage2DARB");

bool AppearanceEditorFrame::didInitGlew = false;

AppearanceEditorFrame::AppearanceEditorFrame(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::AppearanceEditorFrame)
{
	ui->setupUi(this);
}

void AppearanceEditorFrame::setup() {
	if (!didInitGlew) {
		didInitGlew = true;
		glewInit();
	}
			
	//cleanup();
	
	boost::filesystem::ifstream fin("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Mods\\Main\\CharacterCreation\\properties.lsx", 
									std::ios_base::binary);
	if (fin) {
		LsxReader reader;
		std::vector<LsbObject *> objects = reader.loadFile(fin);
		if (objects.size() == 0) {
			MessageBoxA(0, "Failed to parse CharacterCreation properties!", 0, 0);
		}
		fin.close();
		
	} else {
		MessageBoxA(0, "Couldn't read CharacterCreation properties file!", 0, 0);
	}
	
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
	
	nv_dds::CDDSImage image3;
	GLuint texobj3;
	image3.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_Hair_A_DM.dds", false);
	glGenTextures(1, &texobj3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj3);
	image3.upload_texture2D();
	
	nv_dds::CDDSImage image4;
	GLuint texobj4;
	image4.load("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Textures\\Characters\\Player\\PL_M_ARM_Cloth_A_Torso_ABC_Tx_00_DM.dds", false);
	glGenTextures(1, &texobj4);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobj4);
	image4.upload_texture2D();
	
	if (skinColor == 0) {
		skinColor = new VertexRGB({255, 0, 0, 0});
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
	
	std::vector<GLint> textures;
	
	textures.push_back(texobj);
	textures.push_back(texobj5);
	textures.push_back(texobj6);
	ZGrannyScene *grannyScene = 
			zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Body_A.GR2",
							   textures);
	glContext->addGrannyScene(grannyScene, textures, 0, shaderProgram);
	textures.clear();
	
	textures.push_back(texobj2);
	ZGrannyScene *grannyScene2 = 
			zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Head_A.GR2",
							   textures);
	glContext->addGrannyScene(grannyScene2, textures);
	textures.clear();
	
	textures.push_back(texobj3);
	ZGrannyScene *grannyScene3 = 
			zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Hair_A.GR2",
							   textures);
	glContext->addGrannyScene(grannyScene3, textures);
	textures.clear();
	
	textures.push_back(texobj4);
	ZGrannyScene *grannyScene4 = 
			zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_ARM_Cloth_A_Torso_A.GR2",
							   textures);
	glContext->addGrannyScene(grannyScene4, textures);
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
