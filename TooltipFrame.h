#ifndef TOOLTIPFRAME_H
#define TOOLTIPFRAME_H

#include <QFrame>

namespace Ui {
class TooltipFrame;
}

class TooltipFrame : public QFrame
{
	Q_OBJECT
	
public:
	explicit TooltipFrame(QWidget *parent = 0);
	~TooltipFrame();
	
private:
	Ui::TooltipFrame *ui;
};

#endif // TOOLTIPFRAME_H
