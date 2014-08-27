#include "GlContextWidget.h"
#include <math.h>
#include <gl/glu.h>

GlContextWidget::GlContextWidget(QWidget *parent) :
	QGLWidget(parent)
{
	posX = 1.0;
	posY = 2.0;
	posZ = -2.0;
	
	radius = 3.0;
	azimuth = 120.0;
	inclination = 320.0;
	
	lookatX = 0.0;
	lookatY = 1.0;
	lookatZ = 0.0;

}

void GlContextWidget::showEvent(QShowEvent *event)
{
	connect(&frameTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
}

void GlContextWidget::hideEvent(QHideEvent *event)
{
	disconnect(&frameTimer);
}

double movementVelocity = 5;

bool mouseLeft = false;

bool leftDown = false;
bool rightDown = false;
bool upDown = false;
bool downDown = false;

bool minusDown = false;
bool plusDown = false;

bool numpadUp = false;
bool numpadDown = false;
bool numpadLeft = false;
bool numpadRight = false;

bool numpadStar = false;
bool numpadDiv = false;

void GlContextWidget::initializeGL() {
#ifdef USE_GLEW
   GLenum err = glewInit();
   if(GLEW_OK != err) {
      ERROR("GLEW Error: %s\n", glewGetErrorString(err));
      std::exit(EXIT_FAILURE);
   }
   LOG("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif
   glDisable(GL_BLEND);        // Turn Blending Off
   glEnable(GL_DEPTH_TEST);    // Turn Depth Testing On

   double secondsPerFrame = 1.0/framesPerSecond;
   unsigned long msPerFrame = 1000 * secondsPerFrame;
   frameTimer.start(msPerFrame);
}

void GlContextWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
    
    if (h == 0) h = 1;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, w/(double)h, 0.01, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	updateGL();
}
#include <iostream>
void GlContextWidget::paintGL() {
	if (leftDown) {
		posX -= movementVelocity / framesPerSecond;
	}
	if (rightDown) {
		posX += movementVelocity / framesPerSecond;
	}
	if (upDown) {
		posY += movementVelocity / framesPerSecond;
	}
	if (downDown) {
		posY -= movementVelocity / framesPerSecond;
	}
	if (plusDown) {
		posZ -= movementVelocity / framesPerSecond;
	}
	if (minusDown) {
		posZ += movementVelocity / framesPerSecond;
	}
	if (numpadUp) {
		lookatY += movementVelocity / framesPerSecond;
	}
	if (numpadDown) {
		lookatY -= movementVelocity / framesPerSecond;
	}
	if (numpadRight) {
		lookatX += movementVelocity / framesPerSecond;
	}
	if (numpadLeft) {
		lookatX -= movementVelocity / framesPerSecond;
	}
	if (numpadStar) {
		lookatZ -= movementVelocity / framesPerSecond;
	}
	if (numpadDiv) {
		lookatZ += movementVelocity / framesPerSecond;
	}
	
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glScalef(-1.0f, 1.0f, 1.0f);
   glEnable( GL_LINE_SMOOTH );
   glEnable( GL_POLYGON_SMOOTH );
   glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
   glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glEnable(GL_MULTISAMPLE);
   
   if (grannyScenes.size() > 0) {
	   for (int i=0; i<grannyScenes.size(); ++i) {
		   zGrannyRenderScene(grannyScenes[i], textureIds[i], vertexRGBs[i], shaderPrograms[i]);
	   }
   }
   
   polarToCartesian();
   glLoadIdentity();
   gluLookAt(posX + addX, posY + addY, posZ + addZ,
   lookatX, lookatY, lookatZ, 
			 0, 1, 0);
}

void GlContextWidget::addGrannyScene(ZGrannyScene *scene, std::vector<GLint> &textures)
{
	addGrannyScene(scene, textures, 0, 0);
}

void GlContextWidget::addGrannyScene(ZGrannyScene *scene, std::vector<GLint > &textures, VertexRGB *vertexRgb, GlShaderProgram *shaderProgram)
{
	grannyScenes.push_back(scene);
	textureIds.push_back(textures);
	vertexRGBs.push_back(vertexRgb);
	shaderPrograms.push_back(shaderProgram);
}

void GlContextWidget::keyPressEvent(QKeyEvent* e) {
	switch (e->key()) {
	 case Qt::Key_Left:
		leftDown = true;
		break;
	case Qt::Key_Right:
		rightDown = true;
		break;
	case Qt::Key_Up:
		upDown = true;
		break;
	case Qt::Key_Down:
		downDown = true;
		break;
	case Qt::Key_Minus:
		minusDown = true;
		break;
	case Qt::Key_Plus:
		plusDown = true;
		break;
	case Qt::Key_Equal:
		plusDown = true;
		break;
	case Qt::Key_8:
		numpadUp = true;
		break;
	case Qt::Key_2:
		numpadDown = true;
		break;
	case Qt::Key_4:
		numpadLeft = true;
		break;
	case Qt::Key_6:
		numpadRight = true;
		break;
	case Qt::Key_Asterisk:
		numpadStar = true;
		break;
	case Qt::Key_Slash:
		numpadDiv = true;
		break;
	}
}
void GlContextWidget::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	 case Qt::Key_Left:
		leftDown = false;
		break;
	case Qt::Key_Right:
		rightDown = false;
		break;
	case Qt::Key_Up:
		upDown = false;
		break;
	case Qt::Key_Down:
		downDown = false;
		break;
	case Qt::Key_Minus:
		minusDown = false;
		break;
	case Qt::Key_Plus:
		plusDown = false;
		break;
	case Qt::Key_Equal:
		plusDown = false;
		break;
	case Qt::Key_8:
		numpadUp = false;
		break;
	case Qt::Key_2:
		numpadDown = false;
		break;
	case Qt::Key_4:
		numpadLeft = false;
		break;
	case Qt::Key_6:
		numpadRight = false;
		break;
	case Qt::Key_Asterisk:
		numpadStar = false;
		break;
	case Qt::Key_Slash:
		numpadDiv = false;
		break;
	}
}

void GlContextWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		mouseLeft = true;
	} else {
		mouseLeft = false;
	}
	lastMousePos = event->pos();
}

void GlContextWidget::mouseReleaseEvent(QMouseEvent *event)
{
	
}

void GlContextWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (lastMousePos.x() >= 0 && lastMousePos.y() >= 0) {
		int deltaX = event->pos().x() - lastMousePos.x();
		int deltaY = event->pos().y() - lastMousePos.y();
		if (mouseLeft) {
			double angleX = deltaX * 0.5;
			double angleY = deltaY * 0.5;
			addAngle(&azimuth, angleX, false);
			addAngle(&inclination, angleY, true);
		} else {
			double multiplier = 2;
			double resultY = deltaY * 0.001 * multiplier;
			addY += resultY;
			lookatY += resultY;
		}
	}
	lastMousePos = event->pos();
}

void GlContextWidget::wheelEvent(QWheelEvent *event)
{
	double multiplier = 2.5;
	double radiusDelta = -(event->delta()) * 0.001 * multiplier;
	radius += radiusDelta;
}

//ZGrannyScene *GlContextWidget::getGrannyScene() const
//{
//	return grannyScene;
//}

//void GlContextWidget::setGrannyScene(ZGrannyScene *value)
//{
//	grannyScene = value;
//}

