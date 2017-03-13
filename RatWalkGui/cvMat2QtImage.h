#ifndef CVMAT2QTIMAGE_H
#define CVMAT2QTIMAGE_H

#include <QImage>
#include <opencv2/opencv.hpp>

/*!
 * \brief cvMat2QtImage Converts an OpenCv matrix to a QImage.
 * \param[in] frame OpenCv matrix representing the frame to convert.
 * \return QImage with the frame data.
 */
inline QImage cvMat2QtImage(cv::Mat frame) {
   cv::cvtColor(frame, frame, CV_BGR2RGB);
   auto cleanup = [](void *mat) {
      delete static_cast<cv::Mat*>(mat);
   };
   return QImage(frame.data, frame.cols, frame.rows, frame.step,
                  QImage::Format_RGB888, cleanup, new cv::Mat(frame));
}

#endif // CVMAT2QTIMAGE_H
