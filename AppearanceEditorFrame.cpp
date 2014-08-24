#include "AppearanceEditorFrame.h"
#include "ui_AppearanceEditorFrame.h"

#include "GL/gl.h"
#include "zgranny.h"
#include "DDSLoader.h"
#include "dds.h"
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
	
//	glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, image.get_format(), 
//	 image.get_width(), image.get_height(), 0, image.get_size(), 
//	 image);
	
//	for (int i = 0; i < image.get_num_mipmaps(); i++)
//	{
//	 nv_dds::CSurface mipmap = image.get_mipmap(i);
	
//	 glCompressedTexImage2DARB(GL_TEXTURE_2D, i+1, image.get_format(), 
//		 mipmap.get_width(), mipmap.get_height(), 0, mipmap.get_size(), 
//		 mipmap);
//	} 
	
	ZGrannyScene *grannyScene = zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Body_A.GR2");
	//glContext->setGrannyScene(grannyScene);
	glContext->addGrannyScene(grannyScene, texobj);
	
	ZGrannyScene *grannyScene2 = zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Head_A.GR2");
	glContext->addGrannyScene(grannyScene2, texobj2);
}

AppearanceEditorFrame::~AppearanceEditorFrame()
{
	delete ui;
}

void AppearanceEditorFrame::showEvent(QShowEvent *)
{
	setup();
}
