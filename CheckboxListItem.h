#ifndef CHECKBOXLISTITEM_H
#define CHECKBOXLISTITEM_H

#include <QListWidget>
#include <QListWidgetItem>

class CheckboxListItem : public QListWidgetItem
{
	
public:
	explicit CheckboxListItem(QListWidget *parent = 0);
	bool operator< ( const QListWidgetItem & other ) const;
	
};

#endif // CHECKBOXLISTITEM_H
