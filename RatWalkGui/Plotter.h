#ifndef PLOTTER_H
#define PLOTTER_H

#include <vector>
#include <QWidget>
#include <QPointF>

namespace RatWalkGui {

/**
 * @brief This class plots a sequence of points given their x and y coordinates.
 *
 * This class is intended to plot the angles determined by the points in each frame,
 * the x axis represents the number of frame and the y axis represents the angle.
 *
 * Even though the purpose of this class rigth now is to be used specifically
 * for the RatWalk system, it's designed in such a way that it allows other
 * clients to use it in other contexts.
 *
 */
class Plotter : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Creates a new Plotter widget.
     * @param[in] parent Pointer to the parent widget of this Plotter
     */
    Plotter(QWidget *parent = 0);

    /**
     * @brief Adds a new point to plot
     *
     * This method adds a new point and inmediatly draws it on the plotter.
     *
     * @param[in] x X coordinate of the new point
     * @param[in] y Y coordinate of the new point
     */
    void addPoint(double x, double y);

    /**
     * @brief Deletes all of the points from the plotter.
     */
    void clearPoints();

    /**
     * @brief Gives you the amount of points currently added to the plotter.
     * @return Amount of points currently added to the plotter
     */
    int getPointCount();

    /**
     * @brief Deletes a point from the plotter given it's index.
     *
     * Deletes a point from the plotter given it's index.
     * Indexes start at zero which represents the first added point
     * and increace up to getPointCount()-1 whick represents the newest
     * point in the plotter.
     *
     * @param[in] idx Index of the point to delete.
     * @throws std::out_of_range when ixd < 0 || idx >= getPointCount()
     */
    void deletePoint(int idx);

    /**
     * @brief Sets the length for the X axis
     *
     * After calling this method the X axis will still appear to be the same
     * length on the screen but it will change the proportion in wich the points
     * are drawn.
     *
     * @param[i] len The new length for the X axis.
     * @throws std::invalid_argument if len <= 0
     */
    void setXAxisLength(double len);

    /**
     * @brief Sets the length for the Y axis
     *
     * After calling this method the Y axis will still appear to be the same
     * length on the screen but it will change the proportion in wich the points
     * are drawn.
     *
     * @param[i] len The new length for the Y axis.
     * @throws std::invalid_argument if len <= 0 or len is NaN
     */
    void setYAxisLength(double len);

protected:
    /**
     * @brief Draws the widget including the currently added points and the axises
     * @param event
     */
    void paintEvent(QPaintEvent *event);

private:
    /**
     * @brief Translates the points in order to get them ready to be plotter.
     * @return A vector with the points ready to be drawn.
     */
    std::vector<QPointF> translatePoints();
    std::vector<QPointF> points;
    double xAxisLength,
           yAxisLength;
    double xAxisPos,
           yAxisPos;
};

} // namespace RatWalkGui

#endif // PLOTTER_H
