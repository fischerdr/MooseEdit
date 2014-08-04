#ifndef EXPANDABLEGROUPBOX_H
#define EXPANDABLEGROUPBOX_H

#include <QGroupBox>

class ExpandableGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	explicit ExpandableGroupBox(QWidget *parent = 0);
	
signals:
	
public slots:
	void on_clicked();
	
};

#endif // EXPANDABLEGROUPBOX_H
