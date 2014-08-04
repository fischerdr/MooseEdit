#include "AbilityGroupItem.h"
#include "ui_AbilityGroupItem.h"
#include <boost/lexical_cast.hpp>

AbilityGroupItem::AbilityGroupItem(GameCharacter *character, std::vector<StatsContainer *> &abilities, QWidget *parent) :
	QWidget(parent), character(character), abilities(abilities),
	ui(new Ui::AbilityGroupItem)
{
	ui->setupUi(this);
}

AbilityGroupItem::~AbilityGroupItem()
{
	delete ui;
}

void AbilityGroupItem::on_abilityEdit_textEdited(const QString &text)
{
    long value = 0;
	try {
		value = boost::lexical_cast<long>(text.toStdString());
	} catch(const boost::bad_lexical_cast& e) {
		
	}
	
	QLabel *abilityLabel = this->findChild<QLabel *>("abilityLabel");
	std::vector<LsbObject *> abilityObjects = this->character->getAbilityList();
	if (abilityObjects.size() != 0) {
		StatsContainer *ability = GenStatsReader::getContainer(abilities, abilityLabel->text().toStdString());
		if (ability != 0) {
			std::string idText = ability->getData("id");
			long id = -1;
			try {
				id = boost::lexical_cast<long>(idText);
			} catch (const boost::bad_lexical_cast& e) {
				
			}
			if (id != -1 && id < abilityObjects.size()) {
				LsbObject *abilityObject = abilityObjects[id];
				if (abilityObject != 0) {
					abilityObject->setData((char *)&value, sizeof(long));
				}
			}
		}
	}
}
