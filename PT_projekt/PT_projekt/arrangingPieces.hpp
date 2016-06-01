#pragma once


#include <list>
#include <utility>
#include <vector>

#include <opencv2/core/core.hpp>


enum Rotation
{
	R0,
	R90,
	R180,
	R270
};

typedef std::pair<int, int> position_t;

struct ArrangedPiece
{
	cv::Mat &piece;
	Rotation rotation;
	position_t position;
};

typedef std::vector<ArrangedPiece> arrangedPieces_t;


arrangedPieces_t arrangePieces(std::list<cv::Mat> &pieces);
