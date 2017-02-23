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
#include <vector>

namespace RatWalkCore {

class StepRegister {
public:
    typedef std::pair<int, int> Step;

    StepRegister();

    bool addStep(int begin, int end);

    bool isValidStep(int begin, int end);

    bool posNotOverlapping(int pos);

    bool stepNotOverlapping(int begin, int end);

    bool ereaseSurroundingStep(int pos);

    Step getSurroundingStep(int pos);

    Step getSurroundingStep(int begin, int end);

    std::vector<Step> getSteps();

private:

    static bool stepLessThan(const Step &a, const Step &b);

    std::set<Step, decltype(&stepLessThan)> steps;
};

} // namespace RatWalkCore

#endif // STEPREGISTER_H
