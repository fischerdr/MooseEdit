#ifndef GLCONTEXTWIDGET_H
#define GLCONTEXTWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include "zgranny.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <cmath>

class GlContextWidget : public QGLWidget
{
	Q_OBJECT
public:
	explicit GlContextWidget(QWidget *parent = 0);
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	
	ZGrannyScene *getGrannyScene() const;
	void setGrannyScene(ZGrannyScene *value);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent *event);
	
signals:
	
public slots:
	
private:
	void addAngle(double *angle, double toAdd) {
		*angle += toAdd;
		*angle = fmod(*angle + 360.0, 360.0);
	}
	
	double d2r(double degrees) {
		return degrees * 3.1415926535897932385 / 180.0;
	}
	
	void polarToCartesian() {
		double r_inclination = d2r(inclination);
		double r_azimuth = d2r(azimuth);
		posX = radius * sin(r_inclination) * cos(r_azimuth);
		posZ = radius * sin(r_inclination) * sin(r_azimuth);
		posY = radius * cos(r_inclination);
	}
	
	QPoint lastMousePos = QPoint(-1, -1);
	const long framesPerSecond = 60;
	QTimer frameTimer;
	ZGrannyScene *grannyScene = 0;
	double posX;
	double posY;
	double posZ;
	
	double radius;
	double inclination;
	double azimuth;
	
	double lookatX;
	double lookatY;
	double lookatZ;
};

#endif // GLCONTEXTWIDGET_H
