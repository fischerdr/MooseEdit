#include "TooltipFrame.h"
#include "ui_TooltipFrame.h"

TooltipFrame::TooltipFrame(QWidget *parent) :
	QFrame(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
	ui(new Ui::TooltipFrame)
{
	ui->setupUi(this);
}

TooltipFrame::~TooltipFrame()
{
	delete ui;
}
