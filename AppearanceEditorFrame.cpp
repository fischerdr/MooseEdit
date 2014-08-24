#include "AppearanceEditorFrame.h"
#include "ui_AppearanceEditorFrame.h"

#include "GL/gl.h"
#include "zgranny.h"
#include "nv_dds.h"

typedef void (*glCompressedTexImage2DARB_t)(GLenum target, 	GLint level, 	GLenum internalformat, 	GLsizei width, 	GLsizei height, 	GLint border, 	GLsizei imageSize, 	const GLvoid * data);
glCompressedTexImage2DARB_t glCompressedTexImage2DARB = (glCompressedTexImage2DARB_t)wglGetProcAddress("glCompressedTexImage2DARB");

AppearanceEditorFrame::AppearanceEditorFrame(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::AppearanceEditorFrame)
{
	ui->setupUi(this);
}

void AppearanceEditorFrame::setup() {
	//cleanup();
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
		//skinColor = new VertexRGB({255, 0, 0, 0});
	}
	
	ZGrannyScene *grannyScene = zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Body_A.GR2");
	glContext->addGrannyScene(grannyScene, texobj, skinColor);
	
	ZGrannyScene *grannyScene2 = zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Head_A.GR2");
	glContext->addGrannyScene(grannyScene2, texobj2);
	
	ZGrannyScene *grannyScene3 = zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Hair_A.GR2");
	glContext->addGrannyScene(grannyScene3, texobj3);
	
	ZGrannyScene *grannyScene4 = zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_ARM_Cloth_A_Torso_A.GR2");
	glContext->addGrannyScene(grannyScene4, texobj4);
}

AppearanceEditorFrame::~AppearanceEditorFrame()
{
	delete ui;
}

void AppearanceEditorFrame::showEvent(QShowEvent *)
{
	setup();
}
