#pragma once


#include <list>

#include <opencv2/core/core.hpp>


std::list<cv::Mat> findPieces(const cv::Mat &inputImage);
