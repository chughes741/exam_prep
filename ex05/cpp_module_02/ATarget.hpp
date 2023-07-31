#pragma once

#include <iostream>
#include <string>

#include "ASpell.hpp"

class ASpell;

class ATarget {
  public:
    ATarget(const std::string &type);
    ATarget(const ATarget &other);
    ATarget &operator=(const ATarget &rhs);
    virtual ~ATarget();

    const std::string &getType() const;

    virtual ATarget *clone() const = 0;
    void getHitBySpell(const ASpell &spell) const;

  protected:
    std::string type_;
};
