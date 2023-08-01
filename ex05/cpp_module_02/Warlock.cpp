#include "Warlock.hpp"

Warlock::Warlock(std::string name, std::string title): name_(name), title_(title) {
    std::cout << name_ << ": This looks like another boring day.\n";
}

Warlock::~Warlock() {
    std::cout << name_ << ": My job here is done!\n";
}

const std::string &Warlock::getName() const {
    return name_;
}

const std::string &Warlock::getTitle() const {
    return title_;
}

void Warlock::setTitle(const std::string &title) {
    title_ = title;
}

void Warlock::introduce() const {
    std::cout << name_ << ": I am " << name_ << ", " << title_ << "!" << std::endl;
}

void Warlock::learnSpell(ASpell *spell) {
    spells_.learnSpell(spell);
}

void Warlock::forgetSpell(const std::string &spellName) {
    spells_.forgetSpell(spellName);
}

void Warlock::launchSpell(const std::string &spellName, const ATarget &target) {
    spells_.createSpell(spellName)->launch(target);
}
