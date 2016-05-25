#pragma once


#include <list>
#include <vector>

#include <opencv2/core/core.hpp>


enum Rotation
{
	R0,
	R90,
	R180,
	R270
};

struct ArrangedPiece
{
	const cv::Mat * const piece;
	Rotation rotation;
	cv::Point position;
};


std::vector<ArrangedPiece> arrangePieces(const std::list<cv::Mat> &pieces);
