#ifndef APPEARANCEEDITORFRAME_H
#define APPEARANCEEDITORFRAME_H

#include <QFrame>
#include "GlContextWidget.h"
#include "zgranny.h"

namespace Ui {
class AppearanceEditorFrame;
}

class AppearanceEditorFrame : public QFrame
{
	Q_OBJECT
	
public:
	explicit AppearanceEditorFrame(QWidget *parent = 0);
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
	
private:
	static bool didInitGlew;
	GlShaderProgram *shaderProgram = 0;
	VertexRGB *skinColor = 0;
	Ui::AppearanceEditorFrame *ui;
	void setup();
};

#endif // APPEARANCEEDITORFRAME_H
