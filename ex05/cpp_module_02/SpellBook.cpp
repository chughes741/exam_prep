#include "SpellBook.hpp"

SpellBook::SpellBook() {}

SpellBook::SpellBook(const SpellBook &other) {}

SpellBook &SpellBook::operator=(const SpellBook &rhs) {}

SpellBook::~SpellBook() {}

void SpellBook::learnSpell(ASpell *spell) {
    spells_[spell->getName()] = spell->clone();
}

void SpellBook::forgetSpell(const std::string &spell_name) {
    spells_.erase(spell_name);
}

ASpell *SpellBook::createSpell(const std::string &spell_name) {
    if (spells_.find(spell_name) == spells_.end()) {
        return NULL;
    }
    return spells_[spell_name]->clone();
}
