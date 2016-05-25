#include "arrangingPieces.hpp"


std::list<ArrangedPiece> arrangePieces(const std::list<cv::Mat> &pieces)
{
	std::list<ArrangedPiece> arrangedPieces;
	for (const cv::Mat &piece : pieces)
		arrangedPieces.push_back(ArrangedPiece{ &piece, Rotation::R0, cv::Point(0, 0) });
	//TODO
	return arrangedPieces;
}
