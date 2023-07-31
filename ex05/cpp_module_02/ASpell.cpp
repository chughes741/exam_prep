#include "ASpell.hpp"

ASpell::ASpell(const std::string &name, const std::string &effects)
    : name_(name), effects_(effects) {}

ASpell::ASpell(const ASpell &other)
    : name_(other.name_), effects_(other.effects_) {}

ASpell &ASpell::operator=(const ASpell &rhs) {
    if (this != &rhs) {
        name_ = rhs.name_;
        effects_ = rhs.effects_;
    }
    return *this;
}

ASpell::~ASpell() {}

const std::string &ASpell::getName() const {
    return name_;
}

const std::string &ASpell::getEffects() const {
    return effects_;
}

void ASpell::launch(const ATarget &target) const {
    target.getHitBySpell(*this);
}
