#include "TraitWidget.h"
#include "ui_TraitWidget.h"
#include <boost/lexical_cast.hpp>

TraitWidget::TraitWidget(GameCharacter *character, std::vector<StatsContainer *> &traits, QWidget *parent) :
	QWidget(parent), character(character), traits(traits),
	ui(new Ui::TraitWidget)
{
	ui->setupUi(this);
}

TraitWidget::~TraitWidget()
{
	delete ui;
}

void TraitWidget::on_traitScroll_valueChanged(int value)
{
	QScrollBar *traitScroll = this->findChild<QScrollBar *>("traitScroll");
	QLineEdit *leftEdit = this->findChild<QLineEdit *>("leftEdit");
	QLineEdit *rightEdit = this->findChild<QLineEdit *>("rightEdit");
    int mid = (traitScroll->maximum() + traitScroll->minimum())/2;
	if (value == mid) {
		return;
	}
	if (value < mid) {
		std::string leftText = leftEdit->text().toStdString();
		long value = 0;
		try {
			value = boost::lexical_cast<long>(leftText);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
		++value;
		std::ostringstream ss;
		ss<<value;
		leftEdit->setText(ss.str().c_str());
	} else {
		std::string rightText = rightEdit->text().toStdString();
		long value = 0;
		try {
			value = boost::lexical_cast<long>(rightText);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
		++value;
		std::ostringstream ss;
		ss<<value;
		rightEdit->setText(ss.str().c_str());
	}
	traitScroll->setValue(mid);
}

void TraitWidget::editTextChanged(const QString &text, QLabel *label) {
	short value = 0;
	try {
		value = boost::lexical_cast<short>(text.toStdString());
	} catch(const boost::bad_lexical_cast& e) {
		
	}
	
	std::vector<LsbObject *> traitObjects = this->character->getTraitList();
	if (traitObjects.size() != 0) {
		StatsContainer *trait = GenStatsReader::getContainer(traits, label->text().toStdString());
		if (trait != 0) {
			std::string idText = trait->getData("id");
			long id = -1;
			try {
				id = boost::lexical_cast<long>(idText);
			} catch (const boost::bad_lexical_cast& e) {
				
			}
			if (id != -1 && id < traitObjects.size()) {
				LsbObject *traitObject = traitObjects[id];
				if (traitObject != 0) {
					traitObject->setData((char *)&value, sizeof(short));
				}
			}
		}
	}
}

void TraitWidget::on_leftEdit_textChanged(const QString &text)
{
	QLabel *label = this->findChild<QLabel *>("leftLabel");
	editTextChanged(text, label);
}

void TraitWidget::on_rightEdit_textChanged(const QString &text)
{
	QLabel *label = this->findChild<QLabel *>("rightLabel");
	editTextChanged(text, label);
}
