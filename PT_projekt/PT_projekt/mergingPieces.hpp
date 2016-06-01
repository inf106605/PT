#pragma once


#include <vector>
#include <limits>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include "arrangingPieces.hpp"


cv::Mat mergePieces(arrangedPieces_t &arrangedPieces);
