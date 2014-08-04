#include "ExpandableGroupBox.h"

ExpandableGroupBox::ExpandableGroupBox(QWidget *parent) :
	QGroupBox(parent)
{
	this->connect(this, SIGNAL(clicked()), SLOT(on_clicked()));
	this->setChecked(false);
	on_clicked();
}

void ExpandableGroupBox::on_clicked()
{
	if (this->isChecked()) {
		this->setFlat(false);
		for (int i=0; i<this->children().size(); ++i) {
			QObject *object = this->children()[i];
			if (object->isWidgetType()) {
				QWidget *widget = (QWidget *)object;
				widget->setVisible(true);
			}
		}
	} else {
		this->setFlat(true);
		for (int i=0; i<this->children().size(); ++i) {
			QObject *object = this->children()[i];
			if (object->isWidgetType()) {
				QWidget *widget = (QWidget *)object;
				widget->setVisible(false);
			}
		}
	}
}
