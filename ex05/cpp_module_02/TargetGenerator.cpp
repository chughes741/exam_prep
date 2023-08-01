#include "TargetGenerator.hpp"

TargetGenerator::TargetGenerator() {}

TargetGenerator::TargetGenerator(const TargetGenerator &other) {}

TargetGenerator &TargetGenerator::operator=(const TargetGenerator &rhs) {}

TargetGenerator::~TargetGenerator() {}

void TargetGenerator::learnTargetType(ATarget *target) {
    targets_[target->getType()] = target->clone();
}

void TargetGenerator::forgetTargetType(const std::string &target_type) {
    targets_.erase(target_type);
}

ATarget *TargetGenerator::createTarget(const std::string &target_type) {
    if (targets_.find(target_type) == targets_.end()) {
        return NULL;
    }
    return targets_[target_type]->clone();
}
