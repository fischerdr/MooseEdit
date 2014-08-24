#include "AppearanceEditorFrame.h"
#include "ui_AppearanceEditorFrame.h"

#include "GL/gl.h"
#include "zgranny.h"

AppearanceEditorFrame::AppearanceEditorFrame(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::AppearanceEditorFrame)
{
	ui->setupUi(this);
}

void AppearanceEditorFrame::setup() {
	GlContextWidget *glContext = this->findChild<GlContextWidget *>("glContext");
	ZGrannyScene *grannyScene = zGrannyCreateScene("C:\\Program Files (x86)\\Steam\\SteamApps\\common\\Divinity - Original Sin\\Data\\out\\Public\\Main\\Assets\\Characters\\Players\\PL_M_Body_A.GR2");
	//zGrannyRenderScene(grannyScene);
	glContext->setGrannyScene(grannyScene);
//	glClearColor(0,0,0,0);
//	  glClear(GL_COLOR_BUFFER_BIT);
	
//	  glBegin(GL_TRIANGLES);
//	  {
//	    glColor3f(1,0,0);
//	    glVertex2f(0,0);
	
//	    glColor3f(0,1,0);
//	    glVertex2f(.5,0);
	
//	    glColor3f(0,0,1);
//	    glVertex2f(.5,.5);
//	  }
//	  glEnd();
}

AppearanceEditorFrame::~AppearanceEditorFrame()
{
	delete ui;
}

void AppearanceEditorFrame::showEvent(QShowEvent *)
{
	setup();
}
