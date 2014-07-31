#include "RelativePositionLayout.h"
#include <iostream>

RelativePositionLayout::~RelativePositionLayout()
{
	QLayoutItem *item;
	while ((item = takeAt(0)))
		delete item;
}

void RelativePositionLayout::addItem(QLayoutItem *item)
{
	t_itemData itemData;
	itemData.x = item->geometry().x();
	itemData.y = item->geometry().y();
	itemData.w = item->geometry().width();
	itemData.h = item->geometry().height();
	itemData.item = item;
	list.push_back(itemData);
}

QSize RelativePositionLayout::sizeHint() const
{
    QSize s(0,0);
    int n = list.count();
    if (n > 0)
        s = QSize(100,70); //start with a nice default size
    int i = 0;
    while (i < n) {
        QLayoutItem *o = list.at(i).item;
        s = s.expandedTo(o->sizeHint());
        ++i;
    }
    return s + n*QSize(spacing(), spacing());
}

QSize RelativePositionLayout::minimumSize() const
{
    QSize s(0,0);
    int n = list.count();
    int i = 0;
    while (i < n) {
        QLayoutItem *o = list.at(i).item;
        s = s.expandedTo(o->minimumSize());
        ++i;
    }
    return s + n*QSize(spacing(), spacing());
}

int RelativePositionLayout::count() const
{
	return list.size();
}

QLayoutItem *RelativePositionLayout::itemAt(int index) const
{
	if (index < list.size()) {
		return list[index].item;
	}
	return 0;
}

QLayoutItem *RelativePositionLayout::takeAt(int index)
{
	if (index < list.size()) {
		QLayoutItem *item = list[index].item;
		list.erase(list.begin() + index);
		return item;
	}
	return 0;
}

void RelativePositionLayout::setGeometry(const QRect &rect)
{
	//std::cout<<"rw = "<<rect.width()<<" rh = "<<rect.height()<<'\n';
	//std::cout<<"ow = "<<originalW<<" oh = "<<originalH<<'\n';
	float wIncrease = (float)rect.width() / originalW;
	float hIncrease = (float)rect.height() / originalH;
	for (int i=0; i<list.size(); ++i) {
		QLayoutItem *item = list[i].item;
		int x = list[i].x * wIncrease;
		int y = list[i].y * hIncrease;
		int w = list[i].w * wIncrease;
		int h = list[i].h * hIncrease;
		QRect newGeom(x, y, w, h);
		//std::cout<<"name = "<<item->widget()->objectName().toStdString()<<'\n';
//		std::cout<<"results: "<<
//				   "x"<<x<<" "<<
//				   "y"<<y<<" "<<
//				   "w"<<w<<" "<<
//				   "h"<<h<<" "<<
//				   "oh"<<list[i].h<<" "<<
//				   '\n';
		item->setGeometry(newGeom);
	}
}
