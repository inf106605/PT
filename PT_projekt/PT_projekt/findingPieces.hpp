#pragma once


#include <vector>

#include <opencv2/core/core.hpp>


typedef cv::Mat piece_t;
typedef std::vector<piece_t> pieces_t;

pieces_t findPieces(const cv::Mat &inputImage);
