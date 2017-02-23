/***********************************************
 * File: StepRegister.cpp                      *
 * Namespace: RatWalkCore                      *
 * Author: Cristian David González González    *
 * Creation date: February 22nd, 2017          *
 ***********************************************/

#include "StepRegister.h"

#include <set>
#include <utility>
#include <vector>

namespace RatWalkCore {

StepRegister::StepRegister() :
    steps(stepLessThan) {
}

bool StepRegister::addStep(int begin, int end) {
    if (isValidStep(begin, end)) {
        Step newStep(begin, end);
        return steps.insert(newStep).second;
    } else {
        return false;
    }
}

bool StepRegister::isValidStep(int begin, int end) {
    return begin <= end;
}

bool StepRegister::posNotOverlapping(int pos) {
    return stepNotOverlapping(pos, pos);
}

bool StepRegister::stepNotOverlapping(int begin, int end) {
    Step step(begin, end);
    return steps.find(step) == steps.end();
}

bool StepRegister::ereaseSurroundingStep(int pos) {
    return steps.erase(Step(pos, pos)) == 1;
}

StepRegister::Step StepRegister::getSurroundingStep(int pos) {
    return getSurroundingStep(pos, pos);
}

StepRegister::Step StepRegister::getSurroundingStep(int begin, int end) {
    auto res = steps.find(Step(begin, end));
    if (res != steps.end()) {
        return *res;
    } else {
        return Step(-1, -1);
    }
}

std::vector<StepRegister::Step> StepRegister::getSteps() {
    std::vector<Step> stepVector;
    for (const Step &step : steps) {
        stepVector.push_back(step);
    }
    return stepVector;
}

bool StepRegister::stepLessThan(const Step &a, const Step &b) {
    return a.first < b.first && a.second < b.first;
}

} // namespace RatWalkCore
