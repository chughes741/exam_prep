#pragma once

#include <iostream>
#include <string>
#include <map>

#include "ASpell.hpp"
#include "ATarget.hpp"

class Warlock {
  public:
    Warlock(std::string name, std::string title);

  private:
    Warlock(const Warlock& other) {}
    Warlock& operator=(const Warlock& rhs) {}

 public:
    ~Warlock();

    const std::string &getName() const;
    const std::string &getTitle() const;

    void setTitle(const std::string &title);

    void introduce() const;

    void learnSpell(ASpell *spell);
    void forgetSpell(const std::string &spellName);
    void launchSpell(const std::string &spellName, const ATarget &target);

  private:
    std::map<std::string, ASpell *> spells_;
    std::string name_;
    std::string title_;
};
