#pragma once

#include <map>
#include <string>

#include "ASpell.hpp"

class SpellBook {
  public:
    SpellBook();

  private:
    SpellBook(const SpellBook &other);
    SpellBook &operator=(const SpellBook &rhs);

  public:
    ~SpellBook();

    void learnSpell(ASpell *spell);
    void forgetSpell(const std::string &spellName);
    ASpell *createSpell(const std::string &spellName);

  private:
    std::map<std::string, ASpell *> spells_;
};
