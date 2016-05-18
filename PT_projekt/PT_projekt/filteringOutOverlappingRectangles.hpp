#pragma once

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>


typedef std::vector<cv::Point> rectangle_t;
typedef std::vector<rectangle_t> rectangles_t;


rectangles_t filterOutOverlappingRectangles(const rectangles_t &rectangles);
