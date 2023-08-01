#pragma once

#include <map>
#include <string>

#include "ATarget.hpp"

class TargetGenerator {
  public:
    TargetGenerator();

  private:
    TargetGenerator(const TargetGenerator &other);
    TargetGenerator &operator=(const TargetGenerator &rhs);

  public:
    ~TargetGenerator();

    void learnTargetType(ATarget *target_tpye);
    void forgetTargetType(const std::string &target_type);
    ATarget *createTarget(const std::string &target_type);

  private:
    std::map<std::string, ATarget *> target_types_;
};
