/***********************************************
 * File: StepRegister.h                        *
 * Namespace: RatWalkCore                      *
 * Author: Cristian David González González    *
 * Creation date: February 22nd, 2017          *
 ***********************************************/

#ifndef STEPREGISTER_H
#define STEPREGISTER_H

#include <set>
#include <utility>

namespace RatWalkCore {

class StepRegister {
public:
    StepRegister();

    bool addStep(int begin, int end);

    bool isValidStep(int begin, int end);

    bool posNotOverlapping(int pos);

    bool stepNotOverlapping(int begin, int end);

private:
    typedef std::pair<int, int> Step;

    static bool stepLessThan(const Step &a, const Step &b);

    std::set<Step, decltype(&stepLessThan)> steps;
};

} // namespace RatWalkCore

#endif // STEPREGISTER_H
