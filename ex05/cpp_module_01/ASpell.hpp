#pragma once

#include <string>

#include "ATarget.hpp"

class ATarget;

class ASpell {
  public:
    ASpell(const std::string &name, const std::string &effects);
    ASpell(const ASpell &other);
    ASpell &operator=(const ASpell &rhs);
    virtual ~ASpell();

    const std::string &getName() const;
    const std::string &getEffects() const;

    virtual ASpell *clone() const = 0;
    void launch(const ATarget &target) const;

  protected:
    std::string name_;
    std::string effects_;
};
