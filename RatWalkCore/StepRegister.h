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

/*!
 * \brief The StepRegister class stores a set of steps captured on a
 *        video.
 *
 * Each step is represented by a pair of integers [a, b], where a represents the
 * frame number of the of the begining of the step and b represents the frame
 * number of the ending of the step.
 */
class StepRegister {
public:
    typedef std::pair<int, int> Step;

   /*!
    * \brief StepRegister creates a new step register.
    */
   StepRegister();

   /*!
    * \brief addStep adds a new step to the register.
    *
    * This function adds a new step to the register. The newly creted step
    * should not overlap with a previously added step, in this case the
    * step is not added.
    *
    * \param[in] begin Frame number of the step's begining.
    * \param[in] end Frame number of the step's ending.
    * \return True if the step has been succesfully added, false otherwise.
    */
   bool addStep(int begin, int end);


   /*!
    * \brief isValidStep Check if a step is valid.
    * \param[in] begin The step's begining position.
    * \param[in] end The step's ending position.
    * \return True if begin <= end
    */
   bool isValidStep(int begin, int end);

   /*!
    * \brief posNotOverlapping checks weather or not the given position is
    *        in beetween a step in the register.
    * \param[in] pos A frame number
    * \return True if the position is not in beetween a step in the register,
    *         false otherwise.
    */
   bool posNotOverlapping(int pos);

   /*!
    * \brief isValidStep Checks is a step is valid and therefore can be added
    *        to the register.
    * \param[in] begin The step's begining position.
    * \param[in] end   The step's ending position.
    * \return True if the step is valid and does not overlap with any other
    *         step in the register.
    */
   bool stepNotOverlapping(int begin, int end);

   /*!
    * \brief ereaseSurroundingStep Ereases the step that surrounds the given
    *        position.
    * \param[in] pos Position in the range [begin, end] of the step to erease.
    * \return True if there was a step in the register containing pos in its
    *         range and was succesfully ereased, false otherwise.
    */
   bool ereaseSurroundingStep(int pos);

   /*!
    * \brief getSurroundingStep gets the step that surrounds a position.
    * \param[in] pos A position that should be withing the range of a step
    *            in the register.
    * \return The step that surrounds pos, or [-1, -1] if no such step exists.
    */
   Step getSurroundingStep(int pos);

   /*!
    * \brief getSurroundingStep gets the step that overlaps with the given step.
    *
    * This function gets the step that overlaps with the given step. Considering
    * this step might span over many other steps this method might return any of
    * them.
    *
    * \param[in] begin
    * \param[in] end
    * \return The step overlaping [begin, end], or [-1, -1] if no such element
    *         exists in the register.
    */
   Step getSurroundingStep(int begin, int end);

   /*!
    * \brief getSteps Gets all of the steps in the register.
    * \return Vector containing all of the steps in the register.
    */
   std::vector<Step> getSteps();
private:
    static bool stepLessThan(const Step &a, const Step &b);

    std::set<Step, decltype(&stepLessThan)> steps;
};

} // namespace RatWalkCore

#endif // STEPREGISTER_H
