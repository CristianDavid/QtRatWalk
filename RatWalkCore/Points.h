#ifndef POINTS_H
#define POINTS_H

#include <utility>
#include <cmath>

/*!
 * \brief euclidianDistance Calculates the euclidian distance beetween two
 *        bidimentional points.
 * \param[in] x1 First point's x coordinate.
 * \param[in] y1 First point's y coordinate.
 * \param[in] x2 First point's x coordinate.
 * \param[in] y2 First point's y coordinate.
 * \return The euclidian distance beetween (x1, y1) and (x2, y2).
 */
inline double euclidianDistance(int x1, int y1, int x2, int y2) {
   return std::sqrt(std::pow(x1-x2, 2) + std::pow(y1-y2, 2));
}

#endif // POINTS_H
