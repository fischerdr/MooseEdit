#include "CheckboxListItem.h"

bool CheckboxListItem::operator< ( const QListWidgetItem & other ) const {
	if (this->checkState() != other.checkState()) {
		if (this->checkState() == Qt::Checked) {
			return true;
		} else {
			return false;
		}
	} else {
		return this->text() < other.text();
	}
}

CheckboxListItem::CheckboxListItem(QListWidget *parent) :
	QListWidgetItem(parent)
{
	
}
