#include "ItemGroup.h"


unsigned long ItemGroup::getLargestEquipmentSlot() const
{
	return largestEquipmentSlot;
}

void ItemGroup::setLargestEquipmentSlot(unsigned long value)
{
	largestEquipmentSlot = value;
}

unsigned long ItemGroup::getLargestConsumableSlot() const
{
	return largestConsumableSlot;
}

void ItemGroup::setLargestConsumableSlot(unsigned long value)
{
	largestConsumableSlot = value;
}

unsigned long ItemGroup::getLargestMagicalSlot() const
{
	return largestMagicalSlot;
}

void ItemGroup::setLargestMagicalSlot(unsigned long value)
{
	largestMagicalSlot = value;
}

unsigned long ItemGroup::getLargestIngredientSlot() const
{
	return largestIngredientSlot;
}

void ItemGroup::setLargestIngredientSlot(unsigned long value)
{
	largestIngredientSlot = value;
}

unsigned long ItemGroup::getLargestKeysSlot() const
{
	return largestKeysSlot;
}

void ItemGroup::setLargestKeysSlot(unsigned long value)
{
	largestKeysSlot = value;
}

unsigned long ItemGroup::getLargestMiscSlot() const
{
	return largestMiscSlot;
}

void ItemGroup::setLargestMiscSlot(unsigned long value)
{
	largestMiscSlot = value;
}
ItemGroup::ItemGroup()
{
}
