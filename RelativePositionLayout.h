#ifndef RELATIVEPOSITIONLAYOUT_H
#define RELATIVEPOSITIONLAYOUT_H

#include <QLayout>
#include <QList>
#include <QWidget>

typedef struct {
	QLayoutItem *item;
	int x;
	int y;
	int w;
	int h;
} t_itemData;

class RelativePositionLayout : public QLayout
{
public:
    RelativePositionLayout(QWidget *parent): QLayout(parent) { 
		if (parent != 0) {
			originalW = parent->width();
			originalH = parent->height();
		}
	}
    RelativePositionLayout(int originalW, int originalH): originalW(originalW), originalH(originalH), QLayout() {}
    ~RelativePositionLayout();

    void addItem(QLayoutItem *item);
    QSize sizeHint() const;
    QSize minimumSize() const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    void setGeometry(const QRect &rect);
	
private:
    QList<t_itemData> list;
	int originalW;
	int originalH;
};

#endif // RELATIVEPOSITIONLAYOUT_H
