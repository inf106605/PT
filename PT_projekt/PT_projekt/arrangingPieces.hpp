#pragma once


#include <list>
#include <utility>
#include <vector>

#include <opencv2/core/core.hpp>


enum Rotation
{
	R0 = 0,
	R90 = 1,
	R180 = 2,
	R270 = 3
};

typedef std::pair<int, int> position_t;

struct ArrangedPiece
{
	cv::Mat &piece;
	Rotation rotation;
	position_t position;

	ArrangedPiece(const ArrangedPiece &) = default;
	ArrangedPiece& operator=(const ArrangedPiece &) = default;
};

typedef std::vector<ArrangedPiece> arrangedPieces_t;


arrangedPieces_t arrangePieces(std::list<cv::Mat> &pieces);
