// Frozen Alpha 2 enum contract, captured at upstream ffd88d4.
// Keep these literal IDs stable: resources and saves depend on them.
// Deliberate changes require compatibility review, not automatic regeneration.

// Sorcery::Enums::Character::Align
{
	using enum Sorcery::Enums::Character::Align;
	check(NO_ALIGN, "NO_ALIGN", 0);
	check(GOOD, "GOOD", 1);
	check(NEUTRAL, "NEUTRAL", 2);
	check(EVIL, "EVIL", 3);
}

// Sorcery::Enums::Character::Attribute
{
	using enum Sorcery::Enums::Character::Attribute;
	check(NO_ATTRIBUTE, "NO_ATTRIBUTE", 0);
	check(STRENGTH, "STRENGTH", 1);
	check(IQ, "IQ", 2);
	check(PIETY, "PIETY", 3);
	check(VITALITY, "VITALITY", 4);
	check(AGILITY, "AGILITY", 5);
	check(LUCK, "LUCK", 6);
}

// Sorcery::Enums::Character::Class
{
	using enum Sorcery::Enums::Character::Class;
	check(NO_CLASS, "NO_CLASS", 0);
	check(FIGHTER, "FIGHTER", 1);
	check(MAGE, "MAGE", 2);
	check(PRIEST, "PRIEST", 3);
	check(THIEF, "THIEF", 4);
	check(BISHOP, "BISHOP", 5);
	check(SAMURAI, "SAMURAI", 6);
	check(LORD, "LORD", 7);
	check(NINJA, "NINJA", 8);
}

// Sorcery::Enums::Character::Location
{
	using enum Sorcery::Enums::Character::Location;
	check(NO_LOCATION, "NO_LOCATION", -1);
	check(PARTY, "PARTY", 0);
	check(TAVERN, "TAVERN", 1);
	check(TEMPLE, "TEMPLE", 2);
	check(MAZE, "MAZE", 3);
	check(TRAINING, "TRAINING", 4);
}

// Sorcery::Enums::Character::Race
{
	using enum Sorcery::Enums::Character::Race;
	check(NO_RACE, "NO_RACE", 0);
	check(HUMAN, "HUMAN", 1);
	check(ELF, "ELF", 2);
	check(DWARF, "DWARF", 3);
	check(GNOME, "GNOME", 4);
	check(HOBBIT, "HOBBIT", 5);
}

// Sorcery::Enums::Character::Status
{
	using enum Sorcery::Enums::Character::Status;
	check(OK, "OK", 0);
	check(AFRAID, "AFRAID", 1);
	check(SILENCED, "SILENCED", 2);
	check(ASLEEP, "ASLEEP", 3);
	check(HELD, "HELD", 4);
	check(STONED, "STONED", 5);
	check(DEAD, "DEAD", 6);
	check(ASHES, "ASHES", 7);
	check(LOST, "LOST", 8);
}

// Sorcery::Enums::CharacterSlot
{
	using enum Sorcery::Enums::CharacterSlot;
	check(INSPECT, "INSPECT", 0);
	check(STAY, "STAY", 1);
	check(CHOOSE, "CHOOSE", 2);
	check(HELP, "HELP", 3);
	check(RESTART, "RESTART", 4);
	check(STORE, "STORE", 5);
	check(TITHE, "TITHE", 6);
	check(PAY, "PAY", 7);
	check(EDIT, "EDIT", 8);
	check(LEGATE, "LEGATE", 9);
	check(DELETE, "DELETE", 10);
	check(TRAP, "TRAP", 11);
}

// Sorcery::Enums::Chests::State
{
	using enum Sorcery::Enums::Chests::State;
	check(MENU, "MENU", 0);
	check(CHOOSE_OPEN_CHARACTER, "CHOOSE_OPEN_CHARACTER", 1);
	check(CHOOSE_INSPECT_CHARACTER, "CHOOSE_INSPECT_CHARACTER", 2);
	check(CHOOSE_CALFO_CHARACTER, "CHOOSE_CALFO_CHARACTER", 3);
	check(CHOOSE_DISARM_CHARACTER, "CHOOSE_DISARM_CHARACTER", 4);
	check(CHOOSE_TRAP, "CHOOSE_TRAP", 5);
	check(SHOW_RESULT, "SHOW_RESULT", 6);
	check(DONE, "DONE", 7);
}

// Sorcery::Enums::Input::Mode
{
	using enum Sorcery::Enums::Input::Mode;
	check(NONE, "NONE", 0);
	check(MENU, "MENU", 1);
	check(OPTION, "OPTION", 2);
	check(CONFIRMATION, "CONFIRMATION", 3);
	check(NOTIFICATION, "NOTIFICATION", 4);
	check(TEXT_INPUT, "TEXT_INPUT", 5);
	check(NUMBER_INPUT, "NUMBER_INPUT", 6);
	check(ALLOCATE_STATS, "ALLOCATE_STATS", 7);
	check(FILE_VIEWER, "FILE_VIEWER", 8);
	check(REVIEW_AND_CONFIRM, "REVIEW_AND_CONFIRM", 9);
	check(CHARACTER_BROWSER, "CHARACTER_BROWSER", 10);
	check(ENGINE, "ENGINE", 11);
}

// Sorcery::Enums::Items::Category
{
	using enum Sorcery::Enums::Items::Category;
	check(NO_ITEM_CATEGORY, "NO_ITEM_CATEGORY", 0);
	check(WEAPON, "WEAPON", 1);
	check(ARMOUR, "ARMOUR", 2);
	check(SHIELD, "SHIELD", 3);
	check(HELMET, "HELMET", 4);
	check(GAUNTLETS, "GAUNTLETS", 5);
	check(MISCELLANEOUS, "MISCELLANEOUS", 6);
	check(SCROLL, "SCROLL", 7);
	check(POTION, "POTION", 8);
	check(SPECIAL_ITEM, "SPECIAL_ITEM", 9);
}

// Sorcery::Enums::Items::Effects::Defensive
{
	using enum Sorcery::Enums::Items::Effects::Defensive;
	check(NO_DEF_EFFECT, "NO_DEF_EFFECT", 0);
	check(PROTECTION_VS_ANIMAL, "PROTECTION_VS_ANIMAL", 1);
	check(PROTECTION_VS_CLERIC, "PROTECTION_VS_CLERIC", 2);
	check(PROTECTION_VS_DEMON, "PROTECTION_VS_DEMON", 3);
	check(PROTECTION_VS_DRAGON, "PROTECTION_VS_DRAGON", 4);
	check(PROTECTION_VS_ENCHANTED, "PROTECTION_VS_ENCHANTED", 5);
	check(PROTECTION_VS_FIGHTER, "PROTECTION_VS_FIGHTER", 6);
	check(PROTECTION_VS_GIANT, "PROTECTION_VS_GIANT", 7);
	check(PROTECTION_VS_INSECT, "PROTECTION_VS_INSECT", 8);
	check(PROTECTION_VS_MAGE, "PROTECTION_VS_MAGE", 9);
	check(PROTECTION_VS_MYTHICAL, "PROTECTION_VS_MYTHICAL", 10);
	check(PROTECTION_VS_THIEF, "PROTECTION_VS_THIEF", 11);
	check(PROTECTION_VS_UNDEAD, "PROTECTION_VS_UNDEAD", 12);
	check(PROTECTION_VS_WERE, "PROTECTION_VS_WERE", 13);
	check(RESIST_COLD, "RESIST_COLD", 14);
	check(RESIST_FIRE, "RESIST_FIRE", 15);
	check(RESIST_MAGIC, "RESIST_MAGIC", 16);
	check(RESIST_PHYSICAL, "RESIST_PHYSICAL", 17);
	check(RESIST_LEVEL_DRAIN, "RESIST_LEVEL_DRAIN", 18);
	check(RESIST_POISON, "RESIST_POISON", 19);
	check(RESIST_STONING, "RESIST_STONING", 20);
	check(PREVENT_DECAPITATION, "PREVENT_DECAPITATION", 21);
}

// Sorcery::Enums::Items::Effects::Invoke
{
	using enum Sorcery::Enums::Items::Effects::Invoke;
	check(NO_INV_EFFECT, "NO_INV_EFFECT", 0);
	check(BECOME_NINJA, "BECOME_NINJA", 1);
	check(HEAL_ALL, "HEAL_ALL", 2);
	check(INC_HP, "INC_HP", 3);
	check(INC_STRENGTH, "INC_STRENGTH", 4);
	check(AGE_BY_A_YEAR, "AGE_BY_A_YEAR", 5);
	check(BECOME_LORD, "BECOME_LORD", 6);
	check(BECOME_SAMURAI, "BECOME_SAMURAI", 7);
	check(GRANT_50000_EXP, "GRANT_50000_EXP", 8);
	check(GRANT_50000_GOLD, "GRANT_50000_GOLD", 9);
	check(STATUS_TO_LOST, "STATUS_TO_LOST", 10);
}

// Sorcery::Enums::Items::Effects::Offensive
{
	using enum Sorcery::Enums::Items::Effects::Offensive;
	check(NO_OFF_EFFECT, "NO_OFF_EFFECT", 0);
	check(PURPOSED_VS_ANIMAL, "PURPOSED_VS_ANIMAL", 1);
	check(PURPOSED_VS_CLERIC, "PURPOSED_VS_CLERIC", 2);
	check(PURPOSED_VS_DEMON, "PURPOSED_VS_DEMON", 3);
	check(PURPOSED_VS_DRAGON, "PURPOSED_VS_DRAGON", 4);
	check(PURPOSED_VS_ENCHANTED, "PURPOSED_VS_ENCHANTED", 5);
	check(PURPOSED_VS_FIGHTER, "PURPOSED_VS_FIGHTER", 6);
	check(PURPOSED_VS_GIANT, "PURPOSED_VS_GIANT", 7);
	check(PURPOSED_VS_INSECT, "PURPOSED_VS_INSECT", 8);
	check(PURPOSED_VS_MAGE, "PURPOSED_VS_MAGE", 9);
	check(PURPOSED_VS_MYTHICAL, "PURPOSED_VS_MYTHICAL", 10);
	check(PURPOSED_VS_THIEF, "PURPOSED_VS_THIEF", 11);
	check(PURPOSED_VS_UNDEAD, "PURPOSED_VS_UNDEAD", 12);
	check(PURPOSED_VS_WERE, "PURPOSED_VS_WERE", 13);
	check(AUTOKILL, "AUTOKILL", 14);
}

// Sorcery::Enums::Items::TypeID
{
	using enum Sorcery::Enums::Items::TypeID;
	check(BROKEN_ITEM, "BROKEN_ITEM", 0);
	check(LONG_SWORD, "LONG_SWORD", 1);
	check(SHORT_SWORD, "SHORT_SWORD", 2);
	check(ANOINTED_MACE, "ANOINTED_MACE", 3);
	check(ANOINTED_FLAIL, "ANOINTED_FLAIL", 4);
	check(STAFF, "STAFF", 5);
	check(DAGGER, "DAGGER", 6);
	check(SMALL_SHIELD, "SMALL_SHIELD", 7);
	check(LARGE_SHIELD, "LARGE_SHIELD", 8);
	check(ROBES, "ROBES", 9);
	check(LEATHER_ARMOR, "LEATHER_ARMOR", 10);
	check(CHAIN_MAIL, "CHAIN_MAIL", 11);
	check(BREAST_PLATE, "BREAST_PLATE", 12);
	check(PLATE_MAIL, "PLATE_MAIL", 13);
	check(HELM, "HELM", 14);
	check(POTION_OF_DIOS, "POTION_OF_DIOS", 15);
	check(POTION_OF_LATUMOFIS, "POTION_OF_LATUMOFIS", 16);
	check(LONG_SWORD_PLUS_1, "LONG_SWORD_PLUS_1", 17);
	check(SHORT_SWORD_PLUS_1, "SHORT_SWORD_PLUS_1", 18);
	check(MACE_PLUS_1, "MACE_PLUS_1", 19);
	check(STAFF_OF_MOGREF, "STAFF_OF_MOGREF", 20);
	check(SCROLL_OF_KATINO, "SCROLL_OF_KATINO", 21);
	check(LEATHER_PLUS_1, "LEATHER_PLUS_1", 22);
	check(CHAIN_MAIL_PLUS_1, "CHAIN_MAIL_PLUS_1", 23);
	check(PLATE_MAIL_PLUS_1, "PLATE_MAIL_PLUS_1", 24);
	check(SHIELD_PLUS_1, "SHIELD_PLUS_1", 25);
	check(BREAST_PLATE_PLUS_1, "BREAST_PLATE_PLUS_1", 26);
	check(SCROLL_OF_BADIOS, "SCROLL_OF_BADIOS", 27);
	check(SCROLL_OF_HALITO, "SCROLL_OF_HALITO", 28);
	check(LONG_SWORD_MINUS_1, "LONG_SWORD_MINUS_1", 29);
	check(SHORT_SWORD_MINUS_1, "SHORT_SWORD_MINUS_1", 30);
	check(MACE_MINUS_1, "MACE_MINUS_1", 31);
	check(STAFF_PLUS_2, "STAFF_PLUS_2", 32);
	check(SLAYER_OF_DRAGONS, "SLAYER_OF_DRAGONS", 33);
	check(HELM_PLUS_1, "HELM_PLUS_1", 34);
	check(LEATHER_MINUS_1, "LEATHER_MINUS_1", 35);
	check(CHAIN_MINUS_1, "CHAIN_MINUS_1", 36);
	check(BREAST_PLATE_MINUS_1, "BREAST_PLATE_MINUS_1", 37);
	check(SHIELD_MINUS_1, "SHIELD_MINUS_1", 38);
	check(AMULET_OF_JEWELS, "AMULET_OF_JEWELS", 39);
	check(SCROLL_OF_BADIOS_2, "SCROLL_OF_BADIOS_2", 40);
	check(POTION_OF_SOPIC, "POTION_OF_SOPIC", 41);
	check(LONG_SWORD_PLUS_2, "LONG_SWORD_PLUS_2", 42);
	check(SHORT_SWORD_PLUS_2, "SHORT_SWORD_PLUS_2", 43);
	check(MACE_PLUS_2, "MACE_PLUS_2", 44);
	check(SCROLL_OF_LOMILWA, "SCROLL_OF_LOMILWA", 45);
	check(SCROLL_OF_DILTO, "SCROLL_OF_DILTO", 46);
	check(GLOVES_OF_COPPER, "GLOVES_OF_COPPER", 47);
	check(LEATHER_PLUS_2, "LEATHER_PLUS_2", 48);
	check(CHAIN_PLUS_2, "CHAIN_PLUS_2", 49);
	check(PLATE_MAIL_PLUS_2, "PLATE_MAIL_PLUS_2", 50);
	check(SHIELD_PLUS_2, "SHIELD_PLUS_2", 51);
	check(HELM_PLUS_2_EVIL, "HELM_PLUS_2_EVIL", 52);
	check(POTION_OF_DIAL, "POTION_OF_DIAL", 53);
	check(RING_OF_PORFIC, "RING_OF_PORFIC", 54);
	check(WERE_SLAYER, "WERE_SLAYER", 55);
	check(MAGE_MASHER, "MAGE_MASHER", 56);
	check(MACE_OF_POISON, "MACE_OF_POISON", 57);
	check(STAFF_OF_MONTINO, "STAFF_OF_MONTINO", 58);
	check(BLADE_CUSINART, "BLADE_CUSINART", 59);
	check(AMULET_OF_MANIFO, "AMULET_OF_MANIFO", 60);
	check(ROD_OF_FLAME, "ROD_OF_FLAME", 61);
	check(CHAIN_PLUS_2_EVIL, "CHAIN_PLUS_2_EVIL", 62);
	check(PLATE_PLUS_2_NEUTRAL, "PLATE_PLUS_2_NEUTRAL", 63);
	check(SHIELD_PLUS_3_EVIL, "SHIELD_PLUS_3_EVIL", 64);
	check(AMULET_OF_MAKANITO, "AMULET_OF_MAKANITO", 65);
	check(HELM_OF_MALOR, "HELM_OF_MALOR", 66);
	check(SCROLL_OF_BADIAL, "SCROLL_OF_BADIAL", 67);
	check(SHORT_SWORD_MINUS_2, "SHORT_SWORD_MINUS_2", 68);
	check(DAGGER_PLUS_2, "DAGGER_PLUS_2", 69);
	check(MACE_MINUS_2, "MACE_MINUS_2", 70);
	check(STAFF_MINUS_2, "STAFF_MINUS_2", 71);
	check(DAGGER_OF_SPEED, "DAGGER_OF_SPEED", 72);
	check(ROBE_OF_CURSES, "ROBE_OF_CURSES", 73);
	check(LEATHER_MINUS_2, "LEATHER_MINUS_2", 74);
	check(CHAIN_MINUS_2, "CHAIN_MINUS_2", 75);
	check(BREAST_PLATE_MINUS_2, "BREAST_PLATE_MINUS_2", 76);
	check(SHIELD_MINUS_2, "SHIELD_MINUS_2", 77);
	check(HELM_OF_CURSES, "HELM_OF_CURSES", 78);
	check(BREAST_PLATE_PLUS_2, "BREAST_PLATE_PLUS_2", 79);
	check(GLOVES_OF_SILVER, "GLOVES_OF_SILVER", 80);
	check(SWORD_PLUS_3_EVIL, "SWORD_PLUS_3_EVIL", 81);
	check(S_MINUS_SWORD_PLUS_3_EVIL, "S_MINUS_SWORD_PLUS_3_EVIL", 82);
	check(DAGGER_OF_THIEVES, "DAGGER_OF_THIEVES", 83);
	check(BREAST_PLATE_PLUS_3, "BREAST_PLATE_PLUS_3", 84);
	check(GARB_OF_LORDS, "GARB_OF_LORDS", 85);
	check(MURAMASA_BLADE, "MURAMASA_BLADE", 86);
	check(SHURIKENS, "SHURIKENS", 87);
	check(COLD_CHAIN_MAIL, "COLD_CHAIN_MAIL", 88);
	check(PLATE_PLUS_3_EVIL, "PLATE_PLUS_3_EVIL", 89);
	check(SHIELD_PLUS_3, "SHIELD_PLUS_3", 90);
	check(RING_OF_HEALING, "RING_OF_HEALING", 91);
	check(PRIESTS_RING, "PRIESTS_RING", 92);
	check(RING_OF_DEATH, "RING_OF_DEATH", 93);
	check(AMULET_OF_WERDNA, "AMULET_OF_WERDNA", 94);
	check(STATUE_OF_BEAR, "STATUE_OF_BEAR", 95);
	check(STATUE_OF_FROG, "STATUE_OF_FROG", 96);
	check(KEY_OF_BRONZE, "KEY_OF_BRONZE", 97);
	check(KEY_OF_SILVER, "KEY_OF_SILVER", 98);
	check(KEY_OF_GOLD, "KEY_OF_GOLD", 99);
	check(BLUE_RIBBON, "BLUE_RIBBON", 100);
}

// Sorcery::Enums::Magic::CastContext
{
	using enum Sorcery::Enums::Magic::CastContext;
	check(FIELD, "FIELD", 0);
	check(COMBAT, "COMBAT", 1);
	check(TRAPS, "TRAPS", 2);
}

// Sorcery::Enums::Magic::SpellCategory
{
	using enum Sorcery::Enums::Magic::SpellCategory;
	check(NO_CATEGORY, "NO_CATEGORY", 0);
	check(HEALING, "HEALING", 1);
	check(FIELD, "FIELD", 2);
	check(SUPPORT, "SUPPORT", 3);
	check(DISABLE, "DISABLE", 4);
	check(ATTACK, "ATTACK", 5);
}

// Sorcery::Enums::Magic::SpellID
{
	using enum Sorcery::Enums::Magic::SpellID;
	check(NO_SPELL, "NO_SPELL", -1);
	check(DUMAPIC, "DUMAPIC", 0);
	check(HALITO, "HALITO", 1);
	check(KATINO, "KATINO", 2);
	check(MOGREF, "MOGREF", 3);
	check(DILTO, "DILTO", 4);
	check(SOPIC, "SOPIC", 5);
	check(MAHALITO, "MAHALITO", 6);
	check(MOLITO, "MOLITO", 7);
	check(DALTO, "DALTO", 8);
	check(LAHALITO, "LAHALITO", 9);
	check(MORLIS, "MORLIS", 10);
	check(MADALTO, "MADALTO", 11);
	check(MAKANITO, "MAKANITO", 12);
	check(MAMORLIS, "MAMORLIS", 13);
	check(HAMAN, "HAMAN", 14);
	check(LAKANITO, "LAKANITO", 15);
	check(MASOPIC, "MASOPIC", 16);
	check(ZILWAN, "ZILWAN", 17);
	check(MAHAMAN, "MAHAMAN", 18);
	check(MALOR, "MALOR", 19);
	check(TILTOWAIT, "TILTOWAIT", 20);
	check(BADIOS, "BADIOS", 21);
	check(DIOS, "DIOS", 22);
	check(KALKI, "KALKI", 23);
	check(MILWA, "MILWA", 24);
	check(PORFIC, "PORFIC", 25);
	check(CALFO, "CALFO", 26);
	check(MANIFO, "MANIFO", 27);
	check(MATU, "MATU", 28);
	check(MONTINO, "MONTINO", 29);
	check(BAMATU, "BAMATU", 30);
	check(DIALKO, "DIALKO", 31);
	check(LATUMAPIC, "LATUMAPIC", 32);
	check(LOMILWA, "LOMILWA", 33);
	check(BADIAL, "BADIAL", 34);
	check(DIAL, "DIAL", 35);
	check(LATUMOFIS, "LATUMOFIS", 36);
	check(MAPORFIC, "MAPORFIC", 37);
	check(BADI, "BADI", 38);
	check(BADIALMA, "BADIALMA", 39);
	check(DI, "DI", 40);
	check(DIALMA, "DIALMA", 41);
	check(KANDI, "KANDI", 42);
	check(LITOKAN, "LITOKAN", 43);
	check(LOKTOFEIT, "LOKTOFEIT", 44);
	check(LORTO, "LORTO", 45);
	check(MABADI, "MABADI", 46);
	check(MADI, "MADI", 47);
	check(KADORTO, "KADORTO", 48);
	check(MALIKTO, "MALIKTO", 49);
}

// Sorcery::Enums::Magic::SpellType
{
	using enum Sorcery::Enums::Magic::SpellType;
	check(NO_SPELL_TYPE, "NO_SPELL_TYPE", 0);
	check(ARCANE, "ARCANE", 1);
	check(DIVINE, "DIVINE", 2);
}

// Sorcery::Enums::Map::Direction
{
	using enum Sorcery::Enums::Map::Direction;
	check(NO_DIRECTION, "NO_DIRECTION", -1);
	check(NORTH, "NORTH", 0);
	check(EAST, "EAST", 1);
	check(SOUTH, "SOUTH", 2);
	check(WEST, "WEST", 3);
}

// Sorcery::Enums::Monsters::Category
{
	using enum Sorcery::Enums::Monsters::Category;
	check(NO_MONSTER_CATEGORY, "NO_MONSTER_CATEGORY", 0);
	check(HUMANOID, "HUMANOID", 1);
	check(ANIMAL, "ANIMAL", 2);
	check(DEMON, "DEMON", 3);
	check(DRAGON, "DRAGON", 4);
	check(ENCHANTED, "ENCHANTED", 5);
	check(GIANT, "GIANT", 6);
	check(INSECT, "INSECT", 7);
	check(MYTHICAL, "MYTHICAL", 8);
	check(UNDEAD, "UNDEAD", 9);
	check(WERE, "WERE", 10);
}

// Sorcery::Enums::Monsters::Class
{
	using enum Sorcery::Enums::Monsters::Class;
	check(NO_CLASS, "NO_CLASS", 0);
	check(FIGHTER, "FIGHTER", 1);
	check(MAGE, "MAGE", 2);
	check(PRIEST, "PRIEST", 3);
	check(THIEF, "THIEF", 4);
	check(BISHOP, "BISHOP", 5);
	check(SAMURAI, "SAMURAI", 6);
	check(LORD, "LORD", 7);
	check(NINJA, "NINJA", 8);
}

// Sorcery::Enums::Monsters::TypeID
{
	using enum Sorcery::Enums::Monsters::TypeID;
	check(BUBBLY_SLIME, "BUBBLY_SLIME", 0);
	check(ORC, "ORC", 1);
	check(KOBOLD, "KOBOLD", 2);
	check(UNDEAD_KOBOLD, "UNDEAD_KOBOLD", 3);
	check(ROGUE, "ROGUE", 4);
	check(BUSHWACKER, "BUSHWACKER", 5);
	check(HIGHWAYMAN, "HIGHWAYMAN", 6);
	check(ZOMBIE, "ZOMBIE", 7);
	check(CREEPING_CRUD, "CREEPING_CRUD", 8);
	check(GAS_CLOUD, "GAS_CLOUD", 9);
	check(LVL_1_MAGE, "LVL_1_MAGE", 10);
	check(LVL_1_PRIEST, "LVL_1_PRIEST", 11);
	check(CREEPING_COIN, "CREEPING_COIN", 12);
	check(LVL_1_NINJA, "LVL_1_NINJA", 13);
	check(VORPAL_BUNNY, "VORPAL_BUNNY", 14);
	check(CAPYBARA, "CAPYBARA", 15);
	check(GIANT_TOAD, "GIANT_TOAD", 16);
	check(COYOTE, "COYOTE", 17);
	check(LVL_3_PRIEST, "LVL_3_PRIEST", 18);
	check(LVL_3_SAMURAI, "LVL_3_SAMURAI", 19);
	check(LVL_3_NINJA, "LVL_3_NINJA", 20);
	check(WERE_BEAR, "WERE_BEAR", 21);
	check(DRAGON_FLY, "DRAGON_FLY", 22);
	check(ROTTING_CORPSE, "ROTTING_CORPSE", 23);
	check(OGRE, "OGRE", 24);
	check(HUGE_SPIDER_A, "HUGE_SPIDER_A", 25);
	check(WERERAT, "WERERAT", 26);
	check(BORING_BEETLE, "BORING_BEETLE", 27);
	check(GAS_DRAGON, "GAS_DRAGON", 28);
	check(PRIESTESS, "PRIESTESS", 29);
	check(SWORDSMAN, "SWORDSMAN", 30);
	check(HUGE_SPIDER_B, "HUGE_SPIDER_B", 31);
	check(ATTACK_DOG, "ATTACK_DOG", 32);
	check(GARGOYLE, "GARGOYLE", 33);
	check(GRAVE_MIST, "GRAVE_MIST", 34);
	check(DRAGON_PUPPY, "DRAGON_PUPPY", 35);
	check(WEREWOLF, "WEREWOLF", 36);
	check(SHADE, "SHADE", 37);
	check(BISHOP, "BISHOP", 38);
	check(MINOR_DAIMYO, "MINOR_DAIMYO", 39);
	check(LVL_5_MAGE, "LVL_5_MAGE", 40);
	check(LVL_4_THIEF, "LVL_4_THIEF", 41);
	check(KILLER_WOLF, "KILLER_WOLF", 42);
	check(SPIRIT, "SPIRIT", 43);
	check(GIANT_SPIDER, "GIANT_SPIDER", 44);
	check(WERETIGER, "WERETIGER", 45);
	check(MEDUSALIZARD, "MEDUSALIZARD", 46);
	check(LVL_5_PRIEST, "LVL_5_PRIEST", 47);
	check(LVL_6_NINJA, "LVL_6_NINJA", 48);
	check(LVL_7_MAGE_A, "LVL_7_MAGE_A", 49);
	check(MASTER_THIEF_A, "MASTER_THIEF_A", 50);
	check(MAJOR_DAIMYO, "MAJOR_DAIMYO", 51);
	check(HIGH_PRIEST_A, "HIGH_PRIEST_A", 52);
	check(CHAMP_SAMURAI, "CHAMP_SAMURAI", 53);
	check(ARCH_MAGE_A, "ARCH_MAGE_A", 54);
	check(MASTER_THIEF_B, "MASTER_THIEF_B", 55);
	check(GAZE_HOUND, "GAZE_HOUND", 56);
	check(OGRE_LORD, "OGRE_LORD", 57);
	check(TROLL, "TROLL", 58);
	check(LIFESTEALER, "LIFESTEALER", 59);
	check(NIGHTSTALKER, "NIGHTSTALKER", 60);
	check(WYVERN, "WYVERN", 61);
	check(LVL_8_PRIEST, "LVL_8_PRIEST", 62);
	check(LVL_10_FIGHTER, "LVL_10_FIGHTER", 63);
	check(LVL_7_MAGE_B, "LVL_7_MAGE_B", 64);
	check(LVL_7_THIEF, "LVL_7_THIEF", 65);
	check(LVL_8_NINJA, "LVL_8_NINJA", 66);
	check(EARTH_GIANT, "EARTH_GIANT", 67);
	check(LESSER_DEMON, "LESSER_DEMON", 68);
	check(CHIMERA, "CHIMERA", 69);
	check(FIRE_GIANT, "FIRE_GIANT", 70);
	check(GORGON, "GORGON", 71);
	check(LVL_8_BISHOP, "LVL_8_BISHOP", 72);
	check(LVL_8_FIGHTER, "LVL_8_FIGHTER", 73);
	check(LVL_10_MAGE, "LVL_10_MAGE", 74);
	check(THIEF, "THIEF", 75);
	check(MASTER_NINJA, "MASTER_NINJA", 76);
	check(MURPHYS_GHOST, "MURPHYS_GHOST", 77);
	check(WILL_O_WISP, "WILL_O_WISP", 78);
	check(BLEEB, "BLEEB", 79);
	check(FROST_GIANT, "FROST_GIANT", 80);
	check(FIRE_DRAGON, "FIRE_DRAGON", 81);
	check(HIGH_PRIEST_B, "HIGH_PRIEST_B", 82);
	check(HIGH_WIZARD, "HIGH_WIZARD", 83);
	check(MASTER_THIEF_C, "MASTER_THIEF_C", 84);
	check(HATAMOTO, "HATAMOTO", 85);
	check(VAMPIRE, "VAMPIRE", 86);
	check(GREATER_DEMON, "GREATER_DEMON", 87);
	check(POISON_GIANT, "POISON_GIANT", 88);
	check(DRAGON_ZOMBIE, "DRAGON_ZOMBIE", 89);
	check(RAVER_LORD, "RAVER_LORD", 90);
	check(THE_HIGH_MASTER, "THE_HIGH_MASTER", 91);
	check(FLACK, "FLACK", 92);
	check(ARCH_MAGE_B, "ARCH_MAGE_B", 93);
	check(MAELIFIC, "MAELIFIC", 94);
	check(VAMPIRE_LORD, "VAMPIRE_LORD", 95);
	check(WERDNA, "WERDNA", 96);
	check(HIGH_NINJA, "HIGH_NINJA", 97);
	check(HIGH_PRIEST_C, "HIGH_PRIEST_C", 98);
	check(LVL_7_MAGE_C_, "LVL_7_MAGE_C_", 99);
	check(LVL_7_FIGHTER, "LVL_7_FIGHTER", 100);
}

// Sorcery::Enums::Screen
{
	using enum Sorcery::Enums::Screen;
	check(NONE, "NONE", 0);
	check(SPLASH, "SPLASH", 1);
	check(MAINMENU, "MAINMENU", 2);
	check(OPTIONS, "OPTIONS", 3);
	check(LICENSE, "LICENSE", 4);
	check(COMPENDIUM, "COMPENDIUM", 5);
	check(ATLAS, "ATLAS", 6);
	check(BESTIARY, "BESTIARY", 7);
	check(MUSEUM, "MUSEUM", 8);
	check(SPELLBOOK, "SPELLBOOK", 9);
	check(CASTLE, "CASTLE", 10);
	check(EDGEOFTOWN, "EDGEOFTOWN", 11);
	check(TAVERN, "TAVERN", 12);
	check(INN, "INN", 13);
	check(TEMPLE, "TEMPLE", 14);
	check(SHOP, "SHOP", 15);
	check(TRAINING, "TRAINING", 16);
	check(RESTART, "RESTART", 17);
	check(ADD, "ADD", 18);
	check(REMOVE, "REMOVE", 19);
	check(ENGINE, "ENGINE", 20);
	check(INSPECT, "INSPECT", 21);
	check(REORDER, "REORDER", 22);
	check(AUTOMAP, "AUTOMAP", 23);
	check(GRAVEYARD, "GRAVEYARD", 24);
	check(VICTORY, "VICTORY", 25);
	check(CHEST, "CHEST", 26);
	check(STAY, "STAY", 27);
	check(CHOOSE, "CHOOSE", 28);
	check(RECOVERY, "RECOVERY", 29);
	check(LEVELUP, "LEVELUP", 30);
	check(NOLEVELUP, "NOLEVELUP", 31);
	check(PAY, "PAY", 32);
	check(HEAL, "HEAL", 33);
	check(RESULTS, "RESULTS", 34);
	check(STORE, "STORE", 35);
	check(BUY, "BUY", 36);
	check(SELL, "SELL", 37);
	check(IDENTIFY, "IDENTIFY", 38);
	check(UNCURSE, "UNCURSE", 39);
	check(CREATE, "CREATE", 40);
	check(ROSTER, "ROSTER", 41);
	check(SELECT, "SELECT", 42);
	check(RETRAIN, "RETRAIN", 43);
	check(EDIT, "EDIT", 44);
	check(RECLASS, "RECLASS", 45);
	check(RENAME, "RENAME", 46);
	check(LEGATE, "LEGATE", 47);
	check(RITE, "RITE", 48);
	check(DELETE, "DELETE", 49);
	check(CREATE_NAME, "CREATE_NAME", 50);
	check(CREATE_RACE, "CREATE_RACE", 51);
	check(CREATE_ALIGNMENT, "CREATE_ALIGNMENT", 52);
	check(CREATE_CLASS, "CREATE_CLASS", 53);
	check(CREATE_CONFIRM, "CREATE_CONFIRM", 54);
}

// Sorcery::Enums::System::Error
{
	using enum Sorcery::Enums::System::Error;
	check(NO_ERROR, "NO_ERROR", 0);
	check(JSON_PARSE_ERROR, "JSON_PARSE_ERROR", 1);
	check(UNKNOWN_STRING_KEY, "UNKNOWN_STRING_KEY", 2);
	check(UNKNOWN_COMPONENT, "UNKNOWN_COMPONENT", 3);
	check(OPTIONAL_RETURNED, "OPTIONAL_RETURNED", 4);
	check(UNHANDLED_EXCEPTION, "UNHANDLED_EXCEPTION", 6);
	check(VFX_ERROR, "VFX_ERROR", 7);
}

// Sorcery::Enums::System::Random
{
	using enum Sorcery::Enums::System::Random;
	check(NO_DICE, "NO_DICE", 1);
	check(D2, "D2", 2);
	check(D3, "D3", 3);
	check(D4, "D4", 4);
	check(D5, "D5", 5);
	check(D6, "D6", 6);
	check(D7, "D7", 7);
	check(D8, "D8", 8);
	check(D9, "D9", 9);
	check(D10, "D10", 10);
	check(D12, "D12", 12);
	check(D15, "D15", 13);
	check(D16, "D16", 14);
	check(D20, "D20", 15);
	check(D50, "D50", 16);
	check(D52, "D52", 17);
	check(D70, "D70", 18);
	check(D100, "D100", 19);
	check(D165, "D165", 20);
	check(D300, "D300", 21);
	check(D1000, "D1000", 22);
	check(D2000, "D2000", 23);
	check(ZERO_TO_2, "ZERO_TO_2", 24);
	check(ZERO_TO_3, "ZERO_TO_3", 25);
	check(ZERO_TO_4, "ZERO_TO_4", 26);
	check(ZERO_TO_7, "ZERO_TO_7", 27);
	check(ZERO_TO_8, "ZERO_TO_8", 28);
	check(ZERO_TO_14, "ZERO_TO_14", 29);
	check(ZERO_TO_19, "ZERO_TO_19", 30);
	check(ZERO_TO_29, "ZERO_TO_29", 31);
	check(ZERO_TO_34, "ZERO_TO_34", 32);
	check(ZERO_TO_69, "ZERO_TO_69", 33);
	check(ZERO_TO_99, "ZERO_TO_99", 34);
	check(ZERO_TO_100, "ZERO_TO_100", 35);
	check(ZERO_TO_101, "ZERO_TO_101", 36);
	check(ZERO_TO_129, "ZERO_TO_129", 37);
	check(ZERO_TO_299, "ZERO_TO_299", 38);
	check(ZERO_TO_399, "ZERO_TO_399", 39);
	check(ZERO_TO_437, "ZERO_TO_437", 40);
}
