#pragma once

#include "ATarget.hpp"

class Brickwall : public ATarget {
  public:
    Brickwall();
    virtual ~Brickwall();

    virtual ATarget *clone() const;
};
