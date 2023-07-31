#include "ATarget.hpp"

ATarget::ATarget(const std::string &type): type_(type) {}

ATarget::ATarget(const ATarget &other): type_(other.type_) {}

ATarget &ATarget::operator=(const ATarget &rhs) {
    if (this != &rhs) {
        type_ = rhs.type_;
    }
    return *this;
}

ATarget::~ATarget() {}

const std::string &ATarget::getType() const {
    return type_;
}

void ATarget::getHitBySpell(const ASpell &spell) const {
    std::cout << type_ << " has been " << spell.getEffects() << "!" << std::endl;
}
