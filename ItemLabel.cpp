#include "ItemLabel.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <QLayout>
#include "LsbObject.h"

double ItemLabel::calculateDamage(long damageStat, long itemLevel, long damageBoost) {
	//return floor(floor((itemLevel+1)*((50*(exp(((damageStat+1)*0.1) - 0.1) - 1.067999958992004l) + 10)))/4.0l) * (1 + damageBoost/100.0l);
	return (itemLevel+1)*((50.0l*(exp(((damageStat+1)*0.1) - 0.1) - 1.067999958992004l) + 10))* 0.25 * (1 + damageBoost/100.0l);
}

long ItemLabel::getItemStatValue(std::string statName) {
	StatsContainer *itemStats = item->getItemStats();
	long value = 0;
	std::string data = itemStats->getData(statName);
	try {
		value = boost::lexical_cast<long>(data);
	}
	catch (const boost::bad_lexical_cast& e) {
		
	}
	return value;
}

long ItemLabel::getModStatValue(std::string statName) {
	long value = 0;
	std::vector<StatsContainer *> boosts = item->getBoosts();
	for (int i=0; i<boosts.size(); ++i) {
		StatsContainer *boostStats = boosts[i];
		
		StatsContainer *itemBoost = 0;
		itemBoost = GenStatsReader::getContainer(allItemStats, boostStats->getBoostName());
		if (itemBoost != 0) {
			std::string statData = itemBoost->getData(statName);
			if (statData.size() != 0) {
				try {
					value += boost::lexical_cast<long>(statData);
				}
				catch (const boost::bad_lexical_cast& e) {
					
				}
			}
		}
	}
	return value;
}

long ItemLabel::getSummedItemStat(std::string statName) {
	long summation = 0;
	summation += getItemStatValue(statName);
	summation += getModStatValue(statName);
	return summation;
}

long ItemLabel::getPermBoostStatValue(std::string statName) {
	long value = 0;
	for (int i=0; i<item->getPermBoosts().size(); ++i) {
		LsbObject *permBoost = item->getPermBoosts()[i];
		if (permBoost->getName() == statName) {
			std::string permBoostData = permBoost->getData();
			value += *((long*)permBoost->getData());
		}
	}
	return value;
}

long ItemLabel::getSummedStat(std::string statName) {
	long summation = 0;
	summation += getItemStatValue(statName);
	summation += getModStatValue(statName);
	summation += getPermBoostStatValue(statName);
	return summation;
}

void ItemLabel::displayItemStats(std::ostringstream &contentHtml) {
	long baseCritChance = 0;
	if (getSummedItemStat("CriticalChance") != 0) {
		baseCritChance = -1 + 2*getSummedItemStat("CriticalChance") + ceil(item->getItemLevel()/2.0f);
	}
	long critChance = baseCritChance + getPermBoostStatValue("CriticalChance");
	if (getSummedStat("CriticalChance") != 0) {
		if (getSummedStat("CriticalChance") > getItemStatValue("CriticalChance")) {
			contentHtml<<"<font color=#188EDE size=2>Critical Chance: +"<<critChance<<"%</font><br/>";
		}
		else {
			contentHtml<<"<font color=#DBDBDB size=2>Critical Chance: +"<<critChance<<"%</font><br/>";
		}
	}
	
	long range = getSummedStat("WeaponRange");
	if (range > 1) {
		contentHtml<<"<font color=#188EDE size=2>Range: +"<<range<<"</font><br/>";
	}
	
	float movementF = getPermBoostStatValue("Movement")/100.0f + 0.04f*getSummedItemStat("Movement")*item->getItemLevel() + 0.1f;
	if (getSummedStat("Movement") != 0) {
		contentHtml<<"<font color=#188EDE size=2>Movement: +"<<boost::format("%.2f") % movementF<<"</font><br/>";
	}
	long initiative = getSummedStat("Initiative");
	if (initiative != 0) {
		if (getSummedStat("Initiative") > getItemStatValue("Initiative")) {
			contentHtml<<"<font color=#188EDE size=2>Initiative: +"<<initiative<<"</font><br/>";
		}
		else {
			contentHtml<<"<font color=#DBDBDB size=2>Initiative: +"<<initiative<<"</font><br/>";
		}
	}
	long strength = getSummedItemStat("StrengthBoost") + getSummedStat("Strength");
	if (strength != 0) {
		contentHtml<<"<font color=#188EDE size=2>Strength: +"<<strength<<"</font><br/>";
	}
	long dexterity = getSummedItemStat("DexterityBoost") + getSummedStat("Dexterity");
	if (dexterity != 0) {
		contentHtml<<"<font color=#188EDE size=2>Dexterity: +"<<dexterity<<"</font><br/>";
	}
	long intel = getSummedItemStat("IntelligenceBoost") + getSummedStat("Intelligence");
	if (intel != 0) {
		contentHtml<<"<font color=#188EDE size=2>Intelligence: +"<<intel<<"</font><br/>";
	}
	long cons = getSummedItemStat("ConstitutionBoost") + getSummedStat("Constitution");
	if (cons != 0) {
		contentHtml<<"<font color=#188EDE size=2>Constitution: +"<<cons<<"</font><br/>";
	}
	long speed = ceil(item->getItemLevel()*getSummedItemStat("SpeedBoost")/10.0f) + getSummedStat("Speed");
	if (speed != 0) {
		contentHtml<<"<font color=#188EDE size=2>Speed: +"<<speed<<"</font><br/>";
	}
	long perception = getSummedItemStat("PerceptionBoost") + getSummedStat("Perception");
	if (perception != 0) {
		contentHtml<<"<font color=#188EDE size=2>Perception: +"<<perception<<"</font><br/>";
	}
	
	long singleHanded = getSummedItemStat("SingleHanded");
	if (singleHanded != 0) {
		contentHtml<<"<font color=#188EDE size=2>Single-handed: +"<<singleHanded<<"</font><br/>";
	}
	long shield = getSummedItemStat("Shield");
	if (shield != 0) {
		contentHtml<<"<font color=#188EDE size=2>Shield Specialist: +"<<shield<<"</font><br/>";
	}
	long crafting = getSummedItemStat("Crafting");
	if (crafting != 0) {
		contentHtml<<"<font color=#188EDE size=2>Crafting: +"<<crafting<<"</font><br/>";
	}
	long bodybuilding = getSummedItemStat("BodyBuilding");
	if (bodybuilding != 0) {
		contentHtml<<"<font color=#188EDE size=2>Bodybuilding: +"<<bodybuilding<<"</font><br/>";
	}
	long lockpicking = getSummedItemStat("LockPicking");
	if (lockpicking != 0) {
		contentHtml<<"<font color=#188EDE size=2>Lock-picking: +"<<lockpicking<<"</font><br/>";
	}
	
	long sight = getSummedItemStat("SightBoost") + getSummedStat("Sight");
	float sightF = sight / 100.0f;
	if (sight != 0) {
		contentHtml<<"<font color=#188EDE size=2>Sight: +"<<boost::format("%.2f") % sightF<<"</font><br/>";
	}
	long hearing = getSummedItemStat("HearingBoost") + getSummedStat("Hearing");
	float hearingF = hearing / 100.0f;
	if (hearing != 0) {
		contentHtml<<"<font color=#188EDE size=2>Hearing: +"<<boost::format("%.2f") % hearingF<<"</font><br/>";
	}
	
	long fire = getSummedItemStat("Fire") * 5;
	if (fire != 0) {
		contentHtml<<"<font color=#188EDE size=2>Fire resistance: +"<<fire<<"%</font><br/>";
	}
	long air = getSummedItemStat("Air") * 5;
	if (air != 0) {
		contentHtml<<"<font color=#188EDE size=2>Air resistance: +"<<air<<"%</font><br/>";
	}
	long water = getSummedItemStat("Water") * 5;
	if (water != 0) {
		contentHtml<<"<font color=#188EDE size=2>Water resistance: +"<<water<<"%</font><br/>";
	}
	long earth = getSummedItemStat("Earth") * 5;
	if (earth != 0) {
		contentHtml<<"<font color=#188EDE size=2>Earth resistance: +"<<earth<<"%</font><br/>";
	}
	
	long vitBoost = 0;
	if (getSummedItemStat("VitalityBoost") != 0) {
		vitBoost = 10 + item->getItemLevel()*getSummedItemStat("VitalityBoost")/2;
	}
	long vitality = vitBoost + getSummedStat("Vitality");
	if ((getSummedItemStat("VitalityBoost") + getSummedStat("Vitality")) != 0) {
		contentHtml<<"<font color=#188EDE size=2>HP: +"<<vitality<<"</font><br/>";
	}
}

void ItemLabel::setupTooltip()
{
	QLabel *headerLabel = tooltip->findChild<QLabel *>("headerLabel");
	QTextEdit *tooltipEdit = tooltip->findChild<QTextEdit *>("tooltipEdit");
	QLabel *weightLabel = tooltip->findChild<QLabel *>("weightLabel");
	QLabel *goldLabel = tooltip->findChild<QLabel *>("goldLabel");
	QLabel *rarityLabel = tooltip->findChild<QLabel *>("rarityLabel");
	std::ostringstream contentHtml;
	if (item != 0) {
		headerLabel->setText(item->getItemName().c_str());
		std::string colorText = "";
		if (item->getItemRarity() != "Common") {
			std::ostringstream ss;
			if (item->getItemRarity() == "Magic") {
				colorText = "#188EDE";
			} else if (item->getItemRarity() == "Rare") {
				colorText = "#00AA00";
			} else if (item->getItemRarity() == "Legendary") {
				colorText = "#FF9600";
			} else if (item->getItemRarity() == "Unique") {
				colorText = "#C7A758";
			}
			ss<<"\n#"<<rarityLabel->objectName().toStdString()<<" { color : "<<colorText<<" }";
			std::string styleSheet = rarityLabel->styleSheet().toStdString();
			if (!boost::algorithm::contains(styleSheet, "color")) {				
				styleSheet.append(ss.str().c_str());
				rarityLabel->setStyleSheet(styleSheet.c_str());
			}
			ss.clear();
			ss.str("");
			ss<<"\n#"<<headerLabel->objectName().toStdString()<<" { color : "<<colorText<<" }";
			std::string styleSheetHeader = headerLabel->styleSheet().toStdString();
			if (!boost::algorithm::contains(styleSheetHeader, "color")) {				
				styleSheetHeader.append(ss.str().c_str());
				headerLabel->setStyleSheet(styleSheetHeader.c_str());
			}
			rarityLabel->setText(item->getItemRarity().c_str());
		}
		if (item->hasStatsDirectory()) {
			std::ostringstream ss;
			ss<<"Item Level "<<item->getItemLevel();
			contentHtml<<"<font color=#DBDBDB size=2>"<<ss.str().c_str()<<"</font><br/>";
			//tooltipEdit->append(ss.str().c_str());
			ss.clear();
			ss.str("");
		}
		StatsContainer *itemStats = item->getItemStats();
		if (itemStats != 0) {
			std::ostringstream ss;
			
			long weight = getSummedStat("Weight");
			float weightValue = weight /= 1000;
			ss<<boost::format("%.2g") % weightValue;
			
			weightLabel->setText(ss.str().c_str());
			ss.clear();
			ss.str("");
			
			ss<<getSummedStat("Value");
			//multiply by count
			goldLabel->setText(ss.str().c_str());
			ss.clear();
			ss.str("");
			
			std::vector<StatsContainer *> boosts = item->getBoosts();
			if (itemStats->getType() == "Weapon") {
				long parsedDamage = 0;
				long parsedDamageRange = 0;
				long parsedBoost = 0;
				try {\
					parsedDamage = boost::lexical_cast<long>(getSummedItemStat("Damage"));
					parsedBoost = getSummedStat("DamageBoost");
					parsedDamageRange = boost::lexical_cast<long>(getSummedItemStat("Damage Range"));
				} catch (const boost::bad_lexical_cast& cast) {
					;
				}

				double dmgMinCalc = this->calculateDamage(parsedDamage, item->getItemLevel(), parsedBoost);
				long dmgMin = round(dmgMinCalc);
				long dmgMax = dmgMinCalc + this->calculateDamage(parsedDamageRange, item->getItemLevel(), parsedBoost);
				ss<<itemStats->getData("Damage Type")<<": "<<dmgMin<<"-"<<dmgMax;
				contentHtml<<"<font color=#DBDBDB size=5>"<<ss.str().c_str()<<"</font><br/>";
				//tooltipEdit->append(ss.str().c_str());
				ss.clear();
				ss.str("");
				
				std::string prefix = "";
				std::string suffix = "";
				for (int i=0; i<boosts.size(); ++i) {
					StatsContainer *boostStats = boosts[i];
					
					StatsContainer *itemBoost = 0;
					if (boostStats != 0) {
						long itemRandom = item->getGenerationRandom();
						if (boostStats->getPrefixList().size() > 0 && prefix.size() == 0) {
							prefix = boostStats->getPrefixList()[itemRandom % boostStats->getPrefixList().size()];
						}
						else if (boostStats->getSuffixList().size() > 0 && suffix.size() == 0) {
							suffix = boostStats->getSuffixList()[itemRandom % boostStats->getSuffixList().size()];
						}
						itemBoost = GenStatsReader::getContainer(allItemStats, boostStats->getBoostName());
					}
					if (itemBoost != 0) {
						long parsedDamageMod = 0;
						try {
							parsedDamageMod = boost::lexical_cast<long>(itemBoost->getData("DamageFromBase"));
						}
						catch (const boost::bad_lexical_cast& cast) {
							;
						}
						float mod = parsedDamageMod / 100.0f;
						long eleMinDmg = dmgMin * mod;
						long eleMaxDmg = dmgMax * mod;
						if (parsedDamageMod > 0) {
							contentHtml<<"<font color=#DBDBDB size=5>"<<itemBoost->getData("Damage Type")<<": "<<eleMinDmg<<"-"<<eleMaxDmg<<"</font><br/>";
						}
					}
				}
				std::string newHeader = "";
				if (prefix.size() > 0) {
					newHeader += prefix;
					newHeader += " ";
				}
				newHeader += item->getItemName();
				if (suffix.size() > 0) {
					newHeader += " ";
					newHeader += suffix;
				}
				headerLabel->setText(newHeader.c_str());

				contentHtml<<"<font color=#008000 size=2>Attack: "<<getSummedStat("AttackAPCost")<<" Action Point(s)</font><br/>";
				float critDamage = (1 + getSummedItemStat("CriticalDamage")/2.0f + getPermBoostStatValue("CriticalDamage")/10.0f);
				if (critDamage > 1) {
					if (getSummedStat("CriticalDamage") > getItemStatValue("CriticalDamage")) {
						contentHtml<<"<font color=#188EDE size=2>Critical Damage: x"<<critDamage<<"</font><br/>";
					} else {
						contentHtml<<"<font color=#DBDBDB size=2>Critical Damage: x"<<critDamage<<"</font><br/>";
					}
				}
				
				//bonus stats
				displayItemStats(contentHtml);
				
				bool isUnbreakable = false;
				if (itemStats->getData("Flags") == "Unbreakable") {
					isUnbreakable = true;
				}
				
				{
					std::string extraProp = itemStats->getData("ExtraProperties");
					if (extraProp.size() != 0) {
						std::vector<std::string> properties;
						boost::split(properties, extraProp, boost::is_any_of(","));
						if (properties.size() == 3) {
							contentHtml<<"<font color=#C7A758 size=2>"<<properties[1]<<"% chance to set "<<properties[0]<<" Status</font><br/>";
						}
					}
				}
				
				//proc effects
				for (int i=0; i<boosts.size(); ++i) {
					StatsContainer *boostStats = boosts[i];
					
					StatsContainer *itemBoost = 0;
					itemBoost = GenStatsReader::getContainer(allItemStats, boostStats->getBoostName());
					if (itemBoost != 0) {
						std::string extraProp = itemBoost->getData("ExtraProperties");
						if (extraProp.size() != 0) {
							std::vector<std::string> properties;
							boost::split(properties, extraProp, boost::is_any_of(","));
							if (properties.size() == 3) {
								contentHtml<<"<font color=#C7A758 size=2>"<<properties[1]<<"% chance to set "<<properties[0]<<" Status</font><br/>";
							}
						}
						
						if (itemBoost->getData("Flags") == "Unbreakable") {
							isUnbreakable = true;
						}
					}
					if (isUnbreakable) {
						contentHtml<<"<font color=#C7A758 size=2>Unbreakable</font><br/>";
					}
				}
				
				contentHtml<<"<font color=#DBDBDB size=2>Durability "<<item->getDurability()<<"/"<<getSummedStat("Durability")<<"0</font><br/>";
				contentHtml<<"<font color=#707070 size=2>Requires "<<itemStats->getData("Requirements")<<"</font><br/>";
			}
			
			if (itemStats->getType() == "Armor") {
				contentHtml<<"<font color=#DBDBDB size=5>Armour rating: "<<getSummedStat("Armor Defense Value")<<"</font><br/>";
				
				std::string prefix = "";
				std::string suffix = "";
				for (int i=0; i<boosts.size(); ++i) {
					StatsContainer *boostStats = boosts[i];
					
					StatsContainer *itemBoost = 0;
					if (boostStats != 0) {
						long itemRandom = item->getGenerationRandom();
						if (boostStats->getPrefixList().size() > 0 && prefix.size() == 0) {
							prefix = boostStats->getPrefixList()[itemRandom % boostStats->getPrefixList().size()];
						}
						else if (boostStats->getSuffixList().size() > 0 && suffix.size() == 0) {
							suffix = boostStats->getSuffixList()[itemRandom % boostStats->getSuffixList().size()];
						}
						itemBoost = GenStatsReader::getContainer(allItemStats, boostStats->getBoostName());
					}
				}
				std::string newHeader = "";
				if (prefix.size() > 0) {
					newHeader += prefix;
					newHeader += " ";
				}
				newHeader += item->getItemName();
				if (suffix.size() > 0) {
					newHeader += " ";
					newHeader += suffix;
				}
				headerLabel->setText(newHeader.c_str());
				
				//bonus stats
				displayItemStats(contentHtml);
				
				contentHtml<<"<font color=#DBDBDB size=2>Durability "<<item->getDurability()<<"/"<<getSummedStat("Durability")<<"0</font><br/>";
				std::string requirementsText = itemStats->getData("Requirements");
				if (requirementsText.size() > 0) {
					contentHtml<<"<font color=#707070 size=2>Requires "<<itemStats->getData("Requirements")<<"</font><br/>";
				}
			}
			
			if (itemStats->getType() == "Shield") {
				if (getSummedItemStat("Armor Defense Value") != 0) {
					contentHtml<<"<font color=#DBDBDB size=5>Armour rating: "<<getSummedItemStat("Armor Defense Value")<<"</font><br/>";
				}
				if (getSummedItemStat("Blocking") != 0) {
					long block = 6 + item->getItemLevel() * 0.184962f * getSummedItemStat("Blocking");
					contentHtml<<"<font color=#DBDBDB size=5>Blocking: "<<block<<"%</font><br/>";
				}
				
				std::string prefix = "";
				std::string suffix = "";
				for (int i=0; i<boosts.size(); ++i) {
					StatsContainer *boostStats = boosts[i];
					
					StatsContainer *itemBoost = 0;
					if (boostStats != 0) {
						long itemRandom = item->getGenerationRandom();
						if (boostStats->getPrefixList().size() > 0 && prefix.size() == 0) {
							prefix = boostStats->getPrefixList()[itemRandom % boostStats->getPrefixList().size()];
						}
						else if (boostStats->getSuffixList().size() > 0 && suffix.size() == 0) {
							suffix = boostStats->getSuffixList()[itemRandom % boostStats->getSuffixList().size()];
						}
						itemBoost = GenStatsReader::getContainer(allItemStats, boostStats->getBoostName());
					}
				}
				std::string newHeader = "";
				if (prefix.size() > 0) {
					newHeader += prefix;
					newHeader += " ";
				}
				newHeader += item->getItemName();
				if (suffix.size() > 0) {
					newHeader += " ";
					newHeader += suffix;
				}
				headerLabel->setText(newHeader.c_str());
				
				//bonus stats
				displayItemStats(contentHtml);
				
				contentHtml<<"<font color=#DBDBDB size=2>Durability "<<item->getDurability()<<"/"<<getSummedStat("Durability")<<"0</font><br/>";
				std::string requirementsText = itemStats->getData("Requirements");
				if (requirementsText.size() > 0) {
					contentHtml<<"<font color=#707070 size=2>Requires "<<itemStats->getData("Requirements")<<"</font><br/>";
				}
			}
			
		}
	}
	if (item->getBody().size() > 0) {
		contentHtml<<item->getBody().c_str()<<"<br/>";
	}
	
	QFontMetrics metrics(headerLabel->fontMetrics());
	long textWidth = metrics.width(headerLabel->text());
	if (textWidth > tooltip->width()) {
		tooltip->setFixedWidth(textWidth + 20);
	}
	
	tooltipEdit->setHtml(contentHtml.str().c_str());
	QFrame *tooltipHeader = tooltip->findChild<QFrame *>("tooltipHeader");
	QFrame *tooltipFooter = tooltip->findChild<QFrame *>("tooltipFooter");
	
	//TODO: do something less ghetto here
	ignoreEnterEvent = true;
	tooltip->show();
	tooltip->resize(tooltip->width(), 30 + tooltipEdit->document()->size().height() + tooltipHeader->height() + tooltipFooter->height());
	tooltip->hide();
	ignoreEnterEvent = false;
}

void ItemLabel::enterEvent(QEvent *event)
{
	if (ignoreEnterEvent)
		return;
	if (this->item != 0) {
		setupTooltip();
		bool leftIcon = true;
		if (this->x() >= this->parentWidget()->width()/2) {
			leftIcon = false;
		}
		QWidget *main = tooltip->parentWidget();
		QPoint currentPt = this->rect().topLeft();
		if (leftIcon) {
			currentPt.setX(currentPt.x() + this->width());
		}
		else {
			currentPt.setX(currentPt.x() - tooltip->width());
		}
		QPoint globalPt = this->mapToGlobal(currentPt);
		QPoint pt = main->mapFromGlobal(globalPt);
		long tooltipEnd = pt.y() + tooltip->height();
		if (tooltipEnd > main->height()) {
			long diff = tooltipEnd - main->height();
			pt.setY(pt.y() - diff);
		}
		tooltip->move(pt);
		tooltip->show();
		tooltip->raise();
	}
}

void ItemLabel::leaveEvent(QEvent *event)
{
	tooltip->hide();
}

ItemLabel::ItemLabel(std::vector<StatsContainer *> &itemStats, QWidget *parent, QWidget *mainWindow) :
	QLabel(parent), allItemStats(itemStats)
{
	this->setMouseTracking(true);
	tooltip = new TooltipFrame(mainWindow);
	tooltip->hide();
}
