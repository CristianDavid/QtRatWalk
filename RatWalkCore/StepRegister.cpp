/***********************************************
 * File: StepRegister.cpp                      *
 * Namespace: RatWalkCore                      *
 * Author: Cristian David González González    *
 * Creation date: February 22nd, 2017          *
 ***********************************************/

#include "StepRegister.h"

#include <set>
#include <utility>

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

bool StepRegister::stepLessThan(const Step &a, const Step &b) {
    return a.first < b.first && a.second < b.first;
}

} // namespace RatWalkCore
