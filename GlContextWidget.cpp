#include "GlContextWidget.h"
#include <math.h>
#include <gl/glu.h>

GlContextWidget::GlContextWidget(QWidget *parent) :
	QGLWidget(parent)
{
	posX = 1.0;
	posY = 2.15;
	posZ = -2.0;
	
	addX = 0.0;
	addY = 0.0;
	addZ = 0.0;
	
	radius = 3.0;
	azimuth = 120.0;
	inclination = 320.0;
	
	lookatX = 0.0;
	lookatY = 1.15;
	lookatZ = 0.0;
}

void GlContextWidget::showEvent(QShowEvent *event)
{
	if (!timerConnected) {
		timerConnected = true;
		connect(&frameTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
		
		double secondsPerFrame = 1.0/framesPerSecond;
		unsigned long msPerFrame = 1000 * secondsPerFrame;
		frameTimer.start(msPerFrame);
	}
	this->makeCurrent();
}

void GlContextWidget::hideEvent(QHideEvent *event)
{
	frameTimer.stop();
}

void GlContextWidget::closeEvent(QCloseEvent *event)
{
	frameTimer.stop();
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
   glDisable(GL_BLEND);        // Turn Blending Off
   glEnable(GL_DEPTH_TEST);    // Turn Depth Testing On
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
#include <sstream>

void GlContextWidget::paintGL() {
	if (!this->isVisible()) {
		return;
	}
	
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
	
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		std::ostringstream ss;
		ss<<"err1: "<<gluErrorString(err);
		std::cout<<ss.str()<<'\n';
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
			GLfloat worldPos[3];
			worldPos[0] = 0.0f;
			worldPos[1] = 0.0f;
			worldPos[2] = 0.0f;
			if (attachments[i] != 0) {
				MeshAttachmentPoint *attachment = attachments[i];
				if (attachment->boneName.size() != 0 && attachment->meshName.size() != 0) {
					for (int j=0; j<grannyScenes.size(); ++j) {
						ZGrannyScene *scene = grannyScenes[j];
						for (int k=0; k<scene->modelCount; ++k) {
							ZGrannyModel &model = scene->models[k];
							if (zGrannyGetObbCenter2(attachment->boneName, &model, &grannyScenes[i]->models[0], worldPos)) {
								//std::cout<<grannyScenes[i]->models[0].meshes[0].grannyMesh->Name<<'\n';
								//							   std::cout<<worldPos[0]<<' '<<
								//										worldPos[1]<<' '<<
								//													   worldPos[2]<<' '<<'\n';
							}
							
							for (int m=0; m<model.meshCount; ++m) {
								ZGrannyMesh &mesh = model.meshes[m];
								if (mesh.grannyMesh->Name == attachment->meshName) {
									//								   if (zGrannyGetObbCenter(attachment->boneName, &mesh, worldPos)) {
									//									   std::cout<<grannyScenes[i]->models[0].meshes[0].grannyMesh->Name<<'\n';
									//									   std::cout<<worldPos[0]<<' '<<
									//												worldPos[1]<<' '<<
									//															   worldPos[2]<<' '<<'\n';
									//								   }
								}
							}
						}
					}
				}
			}
			//		   std::string vText = "none";
			//		   if (vertexRGBs[i] != 0) {
			//			   std::ostringstream ss;
			//			   ss<<vertexRGBs[i]->r<<' '<<
			//				   vertexRGBs[i]->g<<' '<<
			//				   vertexRGBs[i]->b<<' ';
			//			   vText = ss.str();
			//		   }
			//		   std::cout<<"Rendering "<<grannyScenes[i]->models[0].meshes[0].grannyMesh->Name<<' '<<vText<<'\n';
			zGrannyRenderScene(grannyScenes[i], textureIds[i], vertexRGBs[i], vertexRGB2s[i], shaderPrograms[i], worldPos);
			//zGrannyRenderSkeleton(grannyScenes[i]->models[0].skeleton, grannyScenes[i]->models[0].worldPose);
		}
	}
	
	if ((err = glGetError()) != GL_NO_ERROR) {
		std::ostringstream ss;
		ss<<"err5: "<<gluErrorString(err);
		std::cout<<ss.str()<<'\n';
	}
	polarToCartesian();
	glLoadIdentity();
	double finalX = posX + addX;
	double finalY = posY + addY;
	double finalZ = posZ + addZ;
	gluLookAt(finalX, finalY, finalZ,
			  lookatX, lookatY, lookatZ, 
			  0, 1, 0);

	if ((err = glGetError()) != GL_NO_ERROR) {
		std::ostringstream ss;
		ss<<"err3: "<<gluErrorString(err);
		std::cout<<ss.str()<<'\n';
	}
}

void GlContextWidget::cleanup() {
	frameTimer.stop();
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		std::ostringstream ss;
		ss<<"err0: "<<gluErrorString(err);
		std::cout<<ss.str()<<'\n';
	}
	for (int i=0; i<grannyScenes.size(); ++i) {
		zGrannyShutdownScene(grannyScenes[i]);
		std::vector<GLuint > &textures = textureIds[i];
		if (textures.size() > 0) {
			GLuint *textureArray = new GLuint[textures.size()];
			for (int j=0; j<textures.size(); ++j) {
				textureArray[j] = textures[j];
			}
			glDeleteTextures(textures.size(), textureArray);
			delete[] textureArray;
		}
		GLenum err;
		if ((err = glGetError()) != GL_NO_ERROR) {
			std::ostringstream ss;
			ss<<"err02: "<<gluErrorString(err);
			std::cout<<ss.str()<<'\n';
		}
		if (shaderPrograms[i] != 0) {
			GlShaderProgram *program = shaderPrograms[i];
			if (glIsProgram(program->getProgram()) == GL_TRUE) {
				std::cout<<"Delete p = "<<program->getProgram()<<'\n';
				glDeleteProgram(program->getProgram());
			}
			if ((err = glGetError()) != GL_NO_ERROR) {
				std::ostringstream ss;
				ss<<"err01: "<<gluErrorString(err)<<' '<<shaderPrograms[i]->getProgram();
				std::cout<<ss.str()<<'\n';
			}
		}
		removeGrannyScene(grannyScenes[i]);
		--i;
	}
	//frameTimer.start();
}

void GlContextWidget::pauseRendering()
{
	frameTimer.stop();
}

void GlContextWidget::resumeRendering()
{
	frameTimer.start();
}

void GlContextWidget::addGrannyScene(ZGrannyScene *scene, std::vector<GLuint> &textures)
{
	addGrannyScene(scene, textures, 0, 0, 0, 0);
}

void GlContextWidget::addGrannyScene(ZGrannyScene *scene, std::vector<GLuint > &textures, 
									 VertexRGB *vertexRgb, VertexRGB *vertexRgb2, GlShaderProgram *shaderProgram, MeshAttachmentPoint *attachment)
{
	grannyScenes.push_back(scene);
	textureIds.push_back(textures);
	vertexRGBs.push_back(vertexRgb);
	vertexRGB2s.push_back(vertexRgb2);
	shaderPrograms.push_back(shaderProgram);
	attachments.push_back(attachment);
}

int GlContextWidget::getSceneCount() {
	return grannyScenes.size();
}

bool GlContextWidget::removeGrannyScene(ZGrannyScene *scene) {
	for (int i=0; i<grannyScenes.size(); ++i) {
		if (grannyScenes[i] == scene) {
			grannyScenes.erase(grannyScenes.begin() + i);
			textureIds.erase(textureIds.begin() + i);
			vertexRGBs.erase(vertexRGBs.begin() + i);
			vertexRGB2s.erase(vertexRGB2s.begin() + i);
			shaderPrograms.erase(shaderPrograms.begin() + i);
			attachments.erase(attachments.begin() + i);
			return true;
		}
	}
	return false;
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
			if (!(inclination + angleY >= 360.0) && !(inclination + angleY <= 180.0)) {
				addAngle(&inclination, angleY, true);
			}
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

