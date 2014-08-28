#include "TooltipFrame.h"
#include "ui_TooltipFrame.h"

TooltipFrame::TooltipFrame(QWidget *parent) :
	QFrame(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
	ui(new Ui::TooltipFrame)
{
	setAttribute(Qt::WA_ShowWithoutActivating);
	ui->setupUi(this);
}

TooltipFrame::~TooltipFrame()
{
	delete ui;
}
