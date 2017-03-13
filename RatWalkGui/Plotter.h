#ifndef PLOTTER_H
#define PLOTTER_H

#include <utility>
#include <vector>
#include <QWidget>
#include <QPoint>
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
    explicit Plotter(QWidget *parent = 0);

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

    /*!
     * \brief setXRange Sets the range of the x-axis to [minX, maxX].
     * \param[in] minX
     * \param[in] maxX
     */
    void setXRange(double minX, double maxX);

    /*!
     * \brief setYRange Sets the range of the y-axis to [minY, maxY].
     * \param minY
     * \param maxY
     */
    void setYRange(double minY, double maxY);

    /*!
     * \brief realPoint2LogicPoint Converts a log point to a real point of the
     * plotter.
     *
     * The logic points are coordinates limited by the x and y ranges, meanwhile
     * the real points represent real pixels on the widget.
     *
     * \param[in] realPoint
     * \return The logicPoint equivalent of the real point.
     */
    QPointF realPoint2LogicPoint(QPointF realPoint);

    /*!
     * \brief logicXRangeLen
     * \return The distance beetween minX and maxX.
     */
    double logicXRangeLen();

    /*!
     * \brief logicYRangeLen
     * \return The distance beetween minY and maxY.
     */
    double logicYRangeLen();

    /*!
     * \brief addVerticalLine
     * \param pos Logical x position of the line.
     */
    void addVerticalLine(double pos);

    /*!
     * \brief clearVerticalLines ereases all the vertical lines.
     */
    void clearVerticalLines();

    /*!
     * \brief addVerticalRect adds a vertical rectangle
     * \param xBegin logical x position of the begining of the rectangle
     * \param xEnd logical y position of the begining of the rectangle
     */
    void addVerticalRect(double xBegin, double xEnd);

    /*!
     * \brief clearVerticalRects ereases all the rectangles.
     */
    void clearVerticalRects();
protected:
    /**
     * @brief Draws the widget including the currently added points and the axises
     * @param event
     */
    void paintEvent(QPaintEvent *event);

private:

    QPointF logicPoint2RealPoint(QPointF logicPoint);

    /**
     * @brief Translates the points in order to get them ready to be plotted.
     * @return A vector with the points ready to be drawn.
     */
    std::vector<QPointF> translatePoints();
    std::vector<QPointF> points;
    double minX, maxX;
    double minY, maxY;
    std::vector<double> verticalLines;
    std::vector<std::pair<double, double>> verticalRects;
};

} // namespace RatWalkGui

#endif // PLOTTER_H
